OBJECTS += $(patsubst %.c,%.o,$(wildcard util/*.c)) $(patsubst %.s, %.o, $(wildcard util/*.s))
