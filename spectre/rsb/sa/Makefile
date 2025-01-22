LD := gcc
AS := gcc
CC := gcc
CFLAGS := -fPIC -O0 -g


# RSB_OBJS := 1.o rsb.o
VIC_OBJS := 2.o vic.o


all: $(RSB_OBJS) $(VIC_OBJS)
	# $(LD) $(RSB_OBJS) -O0 -g -no-pie -o rsb
	$(LD) $(VIC_OBJS) -O0 -g -no-pie -o vic

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.S
	$(AS) $(CFLAGS) -c $<

clean:
	rm -f *.o vic