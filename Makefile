include engine/engine.mk
include entities/entities.mk

OUTPUT = balls.nes
OBJECTS += main.o res/res.o

MAKEFLAGS += --no-builtin-rules
CC := cc65
AS := ca65
LD := ld65
GFLAGS = -tnes-mmc5
AFLAGS = -Iasminc
CFLAGS = -Iinclude -Osir
LFLAGS = -mnes.map --lib nesmmc5.lib

.PHONY: all clean

all: $(OUTPUT)

clean:
	find . -name "*.nes" -type f -delete
	find . -name "*.map" -type f -delete
	find . -name "*.o" -type f -delete
	find . -name "*.si" -type f -delete

$(OUTPUT): $(OBJECTS) $(LIBOVERRIDES)
	$(LD) $(OBJECTS) -o $@ $(GFLAGS) $(LFLAGS)

%.o: %.s
	$(AS) $^ -o $@ $(GFLAGS) $(AFLAGS)

%.o: %.si
	$(AS) $^ -o $@ $(GFLAGS) $(AFLAGS)

%.si: %.c
	$(CC) $^ -o $@ $(GFLAGS) $(CFLAGS)

res/res.o: res/res.s res/*.chr res/*.rle
	$(AS) res/res.s -o $@ $(GFLAGS) $(AFLAGS)
