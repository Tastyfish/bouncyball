include engine/engine.mk
include entities/entities.mk

OUTPUT = balls.nes
OBJECTS += main.o res/res.o

MAKEFLAGS += --no-builtin-rules
CC := cc65
AS := ca65
LD := ld65
GFLAGS = -tnes
AFLAGS = -Iasminc
CFLAGS = -Iinclude -Oi
LFLAGS = -mnes.map nes.lib

.PHONY: all clean clean-res reres

all: $(OUTPUT)

clean:
	find . -name "*.nes" -type f -delete
	find . -name "*.map" -type f -delete
	find . -name "*.o" -type f -delete
	find . -name "*.is" -type f -delete

clean-res:
	rm -f res/*.o

reres: | clean-res all

$(OUTPUT): $(OBJECTS)
	$(LD) $^ -o $@ $(GFLAGS) $(LFLAGS)

%.o: %.s
	$(AS) $^ -o $@ $(GFLAGS) $(AFLAGS)

%.o: %.si
	$(AS) $^ -o $@ $(GFLAGS) $(AFLAGS)

%.si: %.c
	$(CC) $^ -o $@ $(GFLAGS) $(CFLAGS)
