# 获取当前系统的处理器架构
ARCH := $(shell uname -m)

# 根据处理器架构设置相应的宏定义
ifeq ($(ARCH), x86_64)
    CFLAGS += -DUSE_X86_64_INSTRUCTIONS
	SHAREOBJ += share_x86.o
endif

ifeq ($(ARCH), aarch64)
    CFLAGS += -DUSE_ARMV8_INSTRUCTIONS
	SHAREOBJ += share_aarch64.o
endif


CC                   = gcc
AS                   = gcc
LD                   = gcc
CFLAGS              += -g -O0 -fPIC -D_GNU_SOURCE
LDFLAGS             += $(SUBDIRS:%=-L %) -lpthread -pthread

ATKOBJS = attacker.o share.o atk.o
ATTACKER = attacker

VICOBJS = victim.o vic.o share.o
VICTIM = victim

BUILDDIRS = $(SUBDIRS:%=build-%)
CLEANDIRS = $(SUBDIRS:%=clean-%)

.SILENT:
all:$(ATTACKER) $(VICTIM)

$(ATTACKER) : $(BUILDDIRS) $(ATKOBJS) 
	$(LD)  -no-pie $(LDFLAGS) -o $(ATTACKER) $(ATKOBJS)

$(VICTIM) : $(BUILDDIRS) $(VICOBJS) 
	$(LD) $(VICOBJS) -no-pie $(LDFLAGS) -o $(VICTIM)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

%.o: %.S
	$(AS) $(CFLAGS) $(INCLUDE) -c -w $< -o $@

clean: $(CLEANDIRS)
	rm -f $(ATKOBJS) $(ATTACKER) $(VICTIM) $(VICOBJS)
