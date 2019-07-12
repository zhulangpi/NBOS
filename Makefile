################################################

# compile

################################################
IMAGE := NBOS.elf
ROOT := $(shell pwd)


CROSS_COMPILE = aarch64-elf-

AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy


INC := $(ROOT)/include


CFLAGS = -Wall -fno-common -O0 -g \
         -nostdlib -nostartfiles -ffreestanding \
         -march=armv8-a -I$(INC)

AFLAGS = -g -I$(INC)

LDFLAGS = -nostartfiles

INIT = init/start.o init/init_task.o
KERNEL = kernel/task.o kernel/exception.o
MM = mm/mm.o
LIB = lib/lib.o


OBJS =  $(INIT) $(KERNEL) $(MM) $(LIB)

LDSCRIPT = NBOS.ld

all: $(IMAGE)

$(IMAGE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -T$(LDSCRIPT) -o $(IMAGE)
	$(OBJDUMP) -d NBOS.elf > NBOS.list
	$(OBJDUMP) -t NBOS.elf | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > NBOS.sym
#	$(OBJCOPY) -O binary -S NBOS.elf NBOS.bin

%.o : %.S
	$(AS) $(AFLAGS) $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(IMAGE) $(OBJS) *.list *.sym *.bin

.PHONY: all clean
############################################

# run qemu

############################################

qemu = qemu-system-aarch64
qgdb_host 	= 127.0.0.1
qgdb_port 	= 1234
gdb	= $(CROSS_COMPILE)gdb

cpu_config = \
        -cpu cortex-a57 \
        -smp 1 \
        -m 1G
 

qemu_cmd_args = \
        -machine virt \
        $(cpu_config) \
        -nographic \
        -serial mon:stdio \
        -kernel NBOS.elf


run: $(IMAGE)
	$(qemu) $(qemu_cmd_args)

gdb_srv:$(IMAGE)
	$(qemu) \
	-gdb tcp::$(qgdb_port) -S \
	$(qemu_cmd_args)

gdb_cli:$(IMAGE)
	$(gdb) \
		-ex="target remote $(qgdb_host):$(qgdb_port)" \
		-ex="symbol-file kernel.elf" \
		-ex="handle SIGUSR1 stop print nopass" \
		-ex="layout regs" \
		-ex="set disassemble-next-line on"

dts:
	$(qemu) -M virt,dumpdtb=virt.dtb $(cpu_config) -nographic
	dtc -I dtb -O dts virt.dtb > etc/virt.dts
	rm -f virt.dtb


.PHONY: run gdb_srv gdb_cli

