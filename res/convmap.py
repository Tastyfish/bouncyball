#!/usr/bin/python3
import argparse, os, math

parser = argparse.ArgumentParser(description='Convert raw map file to compressed format.')
parser.add_argument('input', metavar='FILE', type=argparse.FileType('rb'),
	help='The input rmap files')
parser.add_argument('--output', '-o', dest='output', type=argparse.FileType('wb'),
	help='Output file')
parser.add_argument('--type', '-t', dest='type', default='qle', choices=['qle'],
	help='Output type')
args = parser.parse_args()

fileType = args.type
inputFile = args.input
outputFile = args.output or open(os.path.splitext(inputFile.name)[0] + '.' + fileType, 'wb')

InputWidth = 0
InputHeight = 0

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
			'qle': processQLE
		}[fileType]()
	finally:
		inputFile.close()
		outputFile.close()

# All QLE files are split into a quarter of a normal nametable, and RLE compressed
# This way, it can be compressed while reasonably being blittable during scroll

def processQLE():
	SectionWidth = 16
	SectionEvenHeight = 16
	SectionOddHeight = 14
	SectionTotalHeight = SectionEvenHeight + SectionOddHeight
	#if InputWidth % SectionWidth != 0 or (InputHeight % SectionTotalHeight != 0 and InputHeight % SectionTotalHeight != SectionEvenHeight):
	#	raise Exception("Input dimensions must be multiple of 16x30, optionally + 16 more on the height");

	print("= Header =")
	sectionsX = math.ceil(InputWidth / SectionWidth)
	def getSectionsY():
		heightRemaining = InputHeight
		sectionsY = 0
		while heightRemaining > 0:
			sh = [SectionEvenHeight, SectionOddHeight][sectionsY % 2]
			sectionsY += 1
			heightRemaining -= sh
		return sectionsY
	sectionsY = getSectionsY()
	print(sectionsX, "x", sectionsY, "sections")

	# Output starts with header
	writeInt(sectionsX)
	writeInt(sectionsY)
	# and a list of the section ptrs, in order
	sectionTableAddr = outputFile.tell()
	outputFile.seek(sectionsX * sectionsY * 2, 1)

	# setup section file
	def setupSection(sx, sy):
		sectionStart = outputFile.tell()

		outputFile.seek(sectionTableAddr + (sx + sy * sectionsX) * 2)
		writeInt(sectionStart)

		outputFile.seek(sectionStart)

	def getSectionYOffs(sy):
		soffs = math.floor(sy / 2) * 2 * SectionTotalHeight
		if sy % 2 == 1:
			soffs += SectionEvenHeight
		return soffs

	def readSection(sx, sy):
		sectionHeight = [SectionEvenHeight, SectionOddHeight][sy % 2]

		# Push out tile data
		baseX = sx * SectionWidth
		baseY = getSectionYOffs(sy)
		prevVal = 0
		print("section", baseX, baseY)
		inputFile.seek(baseX + baseY * InputWidth)
		for y in range(sectionHeight):
			print("y", y, inputFile.tell())

			if baseY + y < InputHeight:
				row = []
				if baseX + SectionWidth <= InputWidth:
					row = inputFile.read(SectionWidth)
				else:
					row = inputFile.read(InputWidth - baseX)
					row += [prevVal] * (SectionWidth - len(row))
				for x in range(SectionWidth):
					prevVal = row[x]
					yield prevVal
				inputFile.seek(InputWidth - SectionWidth, 1)
			else:
				for x in range(SectionWidth):
					yield prevVal

		# Now, switch over to the attrib data
		inputFile.seek(InputWidth * InputHeight + (baseX >> 2) + (baseY * InputWidth >> 4))
		for y in range(math.ceil(sectionHeight / 4)):
			print("ay", y, inputFile.tell())

			if baseY + y * 4 < InputHeight:
				row = []
				if baseX + SectionWidth <= InputWidth:
					row = inputFile.read(SectionWidth >> 2)
				else:
					row = inputFile.read((InputWidth - baseX) >> 2)
					row += [prevVal] * ((SectionWidth >> 2) - len(row))
				for x in range(SectionWidth >> 2):
					yield row[x]
				inputFile.seek((InputWidth - SectionWidth) >> 2, 1)
			else:
				for x in range(SectionWidth >> 2):
					yield prevVal

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

	print("= Sections =")
	for sy in range(sectionsY):
		for sx in range(sectionsX):
			setupSection(sx, sy)

			# figure out the magic RLE byte for section first
			rleByte = 0
			for b in readSection(sx, sy):
				if b == rleByte:
					rleByte += 1
			assert(rleByte < 256)
			outputFile.write(bytes([rleByte]))

			# now compress
			oldB = rleByte
			repCount = 0
			for b in readSection(sx, sy):
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

			print("Section", sx, ",", sy, ["(tall) complete", "(short) complete"][sy % 2])
main()
