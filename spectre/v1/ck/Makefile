LD := gcc
AS := gcc
CC := gcc
CFLAGS := -fPIC -O0 -g


ATKOBJS := vic.o client.o

all: attacker


attacker: $(ATKOBJS)
	$(CC) $(ATKOBJS) -O0 -g -no-pie  -o client

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.S
	$(AS) $(CFLAGS) -c $<

clean:
	rm -f *.o client