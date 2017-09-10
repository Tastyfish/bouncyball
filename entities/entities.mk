OBJECTS += $(patsubst %.c, %.o, $(wildcard entities/gent_*.c) $(wildcard entities/ent_*.c))
