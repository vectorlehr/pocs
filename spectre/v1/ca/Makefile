LD := gcc
AS := gcc
CC := gcc
CFLAGS := -fPIC -O0 -g


ATKOBJS := 1.o v1.o
VICOBJS := vic.o 2.o

all: attacker vicitm

attacker: $(ATKOBJS)
	$(CC) $(ATKOBJS) -O0 -g  -o v1

vicitm: $(VICOBJS)
	$(CC) $(VICOBJS) -O0 -g  -o vic

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.S
	$(AS) $(CFLAGS) -c $<

clean:
	rm -f *.o v1 vic