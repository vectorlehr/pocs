LD := gcc
AS := gcc
CC := gcc
CFLAGS := -fPIC -O0 -g


OBJS := 1.o v2.o


all: $(OBJS)
	$(LD) $(OBJS) -O0 -g -no-pie -o v2

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.S
	$(AS) $(CFLAGS) -c $<

clean:
	rm -f *.o v2