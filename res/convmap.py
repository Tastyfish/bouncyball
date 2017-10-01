#!/usr/bin/python3
import argparse, os, math

parser = argparse.ArgumentParser(description='Convert raw map file to compressed format.')
parser.add_argument('input', metavar='FILE', type=argparse.FileType('rb'),
	help='The input rmap files')
parser.add_argument('--output', '-o', dest='output', type=argparse.FileType('wb'),
	help='Output file')
parser.add_argument('--type', '-t', dest='type', default='qle', choices=['qle','qrv','qrc','qre'],
	help='Output type')
args = parser.parse_args()

fileType = args.type
inputFile = args.input
outputFile = args.output or open(os.path.splitext(inputFile.name)[0] + '.' + fileType, 'wb')

InputWidth = 0
InputHeight = 0
SectionTableAddr = 0

SectionWidth = 16
SectionEvenHeight = 16
SectionOddHeight = 14
SectionTotalHeight = SectionEvenHeight + SectionOddHeight

def readInt():
	return int.from_bytes(inputFile.read(2), byteorder='little')

def writeInt(value):
	outputFile.write(value.to_bytes(2, byteorder='little'))

def main():
	try:
		# get dimensions at the end of input
		inputFile.seek(-4, 2)
		global InputWidth, InputHeight
		InputWidth = readInt()
		InputHeight = readInt()

		print(InputWidth, 'x', InputHeight)

		{
			'qle': processQLE,
			'qrv': processQRV,
			'qrc': processQRC,
			'qre': processQRE,
		}[fileType]()
	finally:
		inputFile.close()
		outputFile.close()

# setup section block
def setupSection(sx, sy, sectionsX):
	sectionStart = outputFile.tell()

	outputFile.seek(SectionTableAddr + (sx + sy * sectionsX) * 2)
	writeInt(sectionStart)

	outputFile.seek(sectionStart)

def getSectionCounts():
	sectionsX = math.ceil(InputWidth / SectionWidth)
	def getSectionsY():
		heightRemaining = InputHeight
		sectionsY = 0
		while heightRemaining > 0:
			sh = [SectionEvenHeight, SectionOddHeight][sectionsY % 2]
			sectionsY += 1
			heightRemaining -= sh
		return sectionsY
	return sectionsX, getSectionsY()

def writeSectionHeader(sectionsX, sectionsY):
	global SectionTableAddr

	# Output starts with header
	writeInt(sectionsX)
	writeInt(sectionsY)
	# and a list of the section ptrs, in order
	SectionTableAddr = outputFile.tell()
	outputFile.seek(sectionsX * sectionsY * 2, 1)

def getSectionYOffs(sy):
	soffs = math.floor(sy / 2) * 2 * SectionTotalHeight
	if sy % 2 == 1:
		soffs += SectionEvenHeight
	return soffs

# Yields (value, True if in attribs)
def readSection(sx, sy):
	sectionHeight = [SectionEvenHeight, SectionOddHeight][sy % 2]

	# Push out tile data
	baseX = sx * SectionWidth
	baseY = getSectionYOffs(sy)
	prevVal = 0
	# print("section", baseX, baseY)
	inputFile.seek(baseX + baseY * InputWidth)
	for y in range(sectionHeight):
		# print("y", y, inputFile.tell())

		if baseY + y < InputHeight:
			row = []
			if baseX + SectionWidth <= InputWidth:
				row = inputFile.read(SectionWidth)
			else:
				row = inputFile.read(InputWidth - baseX)
				row += [prevVal] * (SectionWidth - len(row))
			for x in range(SectionWidth):
				prevVal = row[x]
				yield prevVal, False
			inputFile.seek(InputWidth - SectionWidth, 1)
		else:
			for x in range(SectionWidth):
				yield prevVal, False

	# Now, switch over to the attrib data
	inputFile.seek(InputWidth * InputHeight + (baseX >> 2) + (baseY * InputWidth >> 4))
	for y in range(math.ceil(sectionHeight / 4)):
		# print("ay", y, inputFile.tell())

		if baseY + y * 4 < InputHeight:
			row = []
			if baseX + SectionWidth <= InputWidth:
				row = inputFile.read(SectionWidth >> 2)
			else:
				row = inputFile.read((InputWidth - baseX) >> 2)
				row += [prevVal] * ((SectionWidth >> 2) - len(row))
			for x in range(SectionWidth >> 2):
				yield row[x], True
			inputFile.seek((InputWidth - SectionWidth) >> 2, 1)
		else:
			for x in range(SectionWidth >> 2):
				yield prevVal, True

# All QLE files are split into a quarter of a normal nametable, and RLE compressed
# This way, it can be compressed while reasonably being blittable during scroll
def processQLE():
	sectionsX, sectionsY = getSectionCounts()
	print(sectionsX, "x", sectionsY, "sections")

	print("= Header =")
	writeSectionHeader(sectionsX, sectionsY)

	def writeRLEChunk(b, count):
		if count < 4:
			outputFile.write(bytes([b]*count))
		elif count <= 256:
			outputFile.write(bytes([b, rleByte, count - 1]))
		else:
			writeRLEChunk(b, 256)
			count -= 256
			while count > 255:
				outputFile.write(bytes([rleByte, 255]))
				count -= 255
			outputFile.write(bytes([rleByte, count]))

	print("= QLE Sections =")
	for sy in range(sectionsY):
		for sx in range(sectionsX):
			setupSection(sx, sy, sectionsX)

			# figure out the magic RLE byte for section first
			rleByte = 0
			for b in readSection(sx, sy, True):
				if b == rleByte:
					rleByte += 1
			assert(rleByte < 256)
			outputFile.write(bytes([rleByte]))

			# now compress
			oldB = rleByte
			repCount = 0
			for b in readSection(sx, sy, True):
				# check ahead if bytes different
				if b == oldB:
					repCount += 1
				else:
					writeRLEChunk(oldB, repCount)

					oldB = b
					repCount = 1
			writeRLEChunk(oldB, repCount)
			# end of RLE
			outputFile.write(bytes([rleByte, 0]))

			# print("Section", sx, ",", sy, ["(tall) complete", "(short) complete"][sy % 2])

# The QRx formats use the same 16/14 heights as QLE
# notably, they use MSB 1 to indicate rle stretch rather than magic byte
# in the rare case of a full byte eg the attributes, encoded as 0x80 [value]
# end of section is 0x80 0x00 or for binary types, just 0x00
def processQRV():
	processQRX('v')
def processQRC():
	processQRX('c')
def processQRE():
	processQRX('e')
def processQRX(t):
	valueRange = {
		'v': [0x00, 0x7F],
		'c': [0x40, 0x7F],
		'e': [0x80, 0xFF]
	}[t]
	binaryData = t == 'c'
	useAttribs = t == 'v'

	sectionsX, sectionsY = getSectionCounts()
	print(sectionsX, "x", sectionsY, "sections")

	print("= Header =")
	writeSectionHeader(sectionsX, sectionsY)

	# for non-binary eg QRV, QRE data
	def writeRLEChunk(b, count):
		if count < 3:
			if b <= 127:
				outputFile.write(bytes([b]*count))
			else:
				for i in range(count):
					outputFile.write(bytes([0x80, b]))
		elif count <= 128:
			outputFile.write(bytes([b, 0x80 | count - 1]))
		else:
			writeRLEChunk(b, 128)
			count -= 128
			while count > 127:
				outputFile.write(bytes([255]))
				count -= 127
			outputFile.write(bytes([0x80 | count]))

	# A more efficient format for data that is 0 or 1 per cell
	def writeBinaryRLEChunk(b, count):
		if count <= 127:
			outputFile.write(bytes([b | count]))
		else:
			while count > 127:
				writeBinaryRLEChunk(b, 127)
				count -= 127
			writeBinaryRLEChunk(b, count)

	print("= qr"+t, "Sections =")
	for sy in range(sectionsY):
		for sx in range(sectionsX):
			setupSection(sx, sy, sectionsX)

			# now compress
			oldB = -1000
			repCount = 0
			for b, attrib in readSection(sx, sy):
				if attrib and not useAttribs:
					break

				if not attrib:
					if b < valueRange[0] or b > valueRange[1]:
						b = 0; # data out of range is always 0 (blank background visually)
					elif binaryData:
						b = 128; # the mask to write into the data
					else:
						b -= valueRange[0] # normal data is 0-based to the range bottom

				# check ahead if bytes different
				if b == oldB:
					repCount += 1
				else:
					if oldB >= 0 and binaryData:
						writeBinaryRLEChunk(oldB, repCount)
					elif oldB >= 0:
						writeRLEChunk(oldB, repCount)

					oldB = b
					repCount = 1
			writeRLEChunk(oldB, repCount)
			# end of RLE
			if binaryData:
				outputFile.write(bytes([0x00]))
			else:
				outputFile.write(bytes([0x80, 0x00]))

main()
