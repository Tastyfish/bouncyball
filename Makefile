include util/util.mk
include engine/engine.mk
include entities/entities.mk

OUTPUT = bouncyball.nes
OBJECTS += main.o states/splash.o states/ingame.o states/testgame.o res/res.o
SOUNDS += res/theme.s res/sfx.s

MAKEFLAGS += --no-builtin-rules
CC := cc65
AS := ca65
LD := ld65
GFLAGS = -tnes-mmc5
AFLAGS = -Iasminc
CFLAGS = -Iinclude -Iinclude/stuff -Osir
LFLAGS = -mnes.map --lib nesmmc5.lib

.PHONY: all clean

all: $(OUTPUT)

clean:
	find . -name "*.nes" -type f -delete
	rm -f "*.map"
	find . -name "*.o" -type f -delete
	find . -name "*.si" -type f -delete
	find . -name "*.qle" -type f -delete
	rm -f $(SOUNDS)

$(OUTPUT): $(OBJECTS) $(LIBOVERRIDES)
	$(LD) $(OBJECTS) -o $@ $(GFLAGS) $(LFLAGS)

%.o: %.s
	$(AS) $< -o $@ $(GFLAGS) $(AFLAGS)

%.o: %.si
	$(AS) $< -o $@ $(GFLAGS) $(AFLAGS)

%.si: %.c
	$(CC) $< -o $@ $(GFLAGS) $(CFLAGS)

res/res.o: res/res.s $(wildcard res/*.chr) $(wildcard res/*.rle) $(patsubst %.map,%.qrv,$(wildcard res/*.map)) $(wildcard res/*.pal) $(SOUNDS)
	$(AS) $< -o $@ $(GFLAGS) $(AFLAGS)

%.qle: %.map
	./res/convmap.py $< -tqle

%.qrv %.qre %.qrc: %.map
	./res/convmap.py $< -tqrv
	./res/convmap.py $< -tqre
	./res/convmap.py $< -tqrc

%.s: %.txt
	../famitone/tools/text2data $< -ca65

%.s: %.nsf
	../famitone/tools/nsf2data $< -ca65 -ntsc
