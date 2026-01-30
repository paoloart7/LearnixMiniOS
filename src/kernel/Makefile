PREFIX = $(HOME)/opt/cross
TARGET = i686-elf
CC = $(PREFIX)/bin/$(TARGET)-gcc
LD = $(PREFIX)/bin/$(TARGET)-ld
OBJCOPY = $(PREFIX)/bin/$(TARGET)-objcopy
NASM = nasm

CFLAGS = -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -I../../include

BUILD_DIR = ../../build

.PHONY: all clean

all: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel_asm.o: arch/kernel.asm
	@mkdir -p $(BUILD_DIR)
	$(NASM) -f elf32 $< -o $@

$(BUILD_DIR)/kernel.o: kernel.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# IMPORTANT: kernel_asm.o DOIT être en premier !
$(BUILD_DIR)/kernel.elf: $(BUILD_DIR)/kernel_asm.o $(BUILD_DIR)/kernel.o linker.ld
	$(LD) -T linker.ld -o $@ $(BUILD_DIR)/kernel_asm.o $(BUILD_DIR)/kernel.o

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@
	@echo "=== Vérification: premiers octets ==="
	@hexdump -C $@ | head -3

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.elf $(BUILD_DIR)/*.bin