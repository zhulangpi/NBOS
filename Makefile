################################################

# compile

################################################
IMAGE := kernel.elf

CROSS_COMPILE = aarch64-elf-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -Wall -fno-common -O0 -g \
         -nostdlib -nostartfiles -ffreestanding \
         -march=armv8-a

AFLAGS = -g

OBJS =  start.o main.o

all: $(IMAGE)

$(IMAGE): kernel.ld $(OBJS)
	$(LD) $(OBJS) -T kernel.ld -o $(IMAGE)
	$(OBJDUMP) -d kernel.elf > kernel.list
	$(OBJDUMP) -t kernel.elf | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > kernel.sym
	$(OBJCOPY) -O binary -S kernel.elf kernel.bin

%.o : %.S %.h 
	$(AS) $(AFLAGS) $< -o $@

%.o : %.c %.h 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(IMAGE) *.o *.list *.sym *.bin

.PHONY: all clean
############################################

# run qemu

############################################

qemu = qemu-system-aarch64
qgdb_host 	= 127.0.0.1
qgdb_port 	= 1234
gdb	= $(CROSS_COMPILE)gdb

qemu_cmd_args = \
        -machine virt \
        -cpu cortex-a57 \
        -smp 1 -m 4096M \
        -nographic \
        -serial mon:stdio \
        -kernel kernel.elf


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
	$(qemu) -M virt,dumpdtb=virt.dtb -cpu cortex-a57 -nographic
	dtc -I dtb -O dts virt.dtb > etc/virt.dts
	rm -f virt.dtb


.PHONY: run gdb_srv gdb_cli

