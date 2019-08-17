################################################

# compile

################################################
IMAGE := NBOS.elf
FILE_SYSTEM := flash1.img
ROOT := $(shell pwd)

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

ARCH = arch/aarch64.o arch/gic_v3.o arch/timer.o arch/start.o arch/exception.o arch/vector.o
INIT = init/init_task.o
KERNEL = kernel/task.o kernel/syscall.o kernel/soft_timer.o
MM = mm/mm.o mm/kmalloc.o
LIB = lib/lib.o lib/printf.o
USER = user/process1.o user/sys_user.o user/lib_user.o

OBJS = $(ARCH) $(BOOT) $(INIT) $(KERNEL) $(MM) $(LIB) $(USER)

LDSCRIPT = linker.ld

all: $(IMAGE) $(FILE_SYSTEM)

$(IMAGE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -T$(LDSCRIPT) -o $(IMAGE)
	$(OBJDUMP) -d NBOS.elf > NBOS.list
	$(OBJDUMP) -t NBOS.elf | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > NBOS.sym
	$(OBJCOPY) -O binary -S NBOS.elf NBOS.bin

%.o : %.S
	$(CC) $(CFLAGS) $< -c -o $@     # for include header file in assembly

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $@


$(FILE_SYSTEM):
	dd if=/dev/zero of=$(FILE_SYSTEM) bs=4096 count=16384
	mkfs.ext4 $(FILE_SYSTEM)
	mkdir tmp_fs
	sudo mount -o loop flash1.img tmp_fs
	sudo cp -rf rootfs/* tmp_fs
	sudo umount tmp_fs
	sudo rm -rf tmp_fs

clean:
	rm -f $(IMAGE) $(OBJS) *.list *.sym *.bin qemu.log

distclean:
	rm -f $(IMAGE) $(FILE_SYSTEM) $(OBJS) *.list *.sym *.bin qemu.log arch/virt.dts

.PHONY: all clean distclean
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
        -d in_asm,int,mmu -D ./qemu.log \
        -drive if=pflash,file=$(FILE_SYSTEM),unit=1 \
        -kernel NBOS.bin


run: $(IMAGE) $(FILE_SYSTEM)
	$(qemu) $(qemu_cmd_args)

gdb_srv:$(IMAGE) $(FILE_SYSTEM)
	$(qemu) \
	-gdb tcp::$(qgdb_port) -S \
	$(qemu_cmd_args)

gdb_cli:$(IMAGE) $(FILE_SYSTEM)
	$(gdb) \
		-ex="target remote $(qgdb_host):$(qgdb_port)" \
		-ex="symbol-file $(IMAGE)" \
		-ex="handle SIGUSR1 stop print nopass" \
		-ex="layout regs" \
		-ex="set disassemble-next-line on"

dts:
	$(qemu) -M virt,dumpdtb=virt.dtb $(cpu_config) -nographic
	dtc -I dtb -O dts virt.dtb > arch/virt.dts
	rm -f virt.dtb


.PHONY: run gdb_srv gdb_cli

