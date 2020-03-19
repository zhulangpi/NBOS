################################################

# compile

################################################
IMAGE := user_code.elf
BIN   := user_code.bin
ROOT  := $(shell pwd)

CROSS_COMPILE = aarch64-elf-

AS = $(CROSS_COMPILE)gcc
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

INC := $(ROOT)/include

CFLAGS = -Wall -fpic -fno-common -O0 \
         -nostdlib -nostartfiles -ffreestanding \
         -march=armv8-a -I$(INC) -mgeneral-regs-only

AFLAGS = -I$(INC)

LDFLAGS = -nostartfiles

OBJS = sys_user.o lib_user.o main.o

LDSCRIPT = linker.ld

all: $(IMAGE)

$(IMAGE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -T$(LDSCRIPT) -o $(IMAGE)
	$(OBJDUMP) -d $(IMAGE) > user_code.list
	$(OBJDUMP) -t $(IMAGE) | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > user_code.sym
	$(OBJCOPY) -O binary -S $(IMAGE) $(BIN)

%.o : %.S
	$(CC) $(CFLAGS) $< -c -o $@     # for include header file in assembly

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $@


clean:
	rm -f $(IMAGE) $(BIN) $(OBJS) *.list *.sym


.PHONY: all clean


