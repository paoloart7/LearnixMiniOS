.PHONY: all clean run

all:
	@echo "Building kernel..."
	@$(MAKE) -C src/kernel
	@echo "Building bootloader..."
	@mkdir -p bin
	@nasm -f bin src/boot/boot.asm -o bin/boot.bin
	@echo "Creating OS image..."
	@cat bin/boot.bin build/kernel.bin > bin/os.bin
	@# Padding à 32KB minimum pour s'assurer que tous les secteurs existent
	@truncate -s 32768 bin/os.bin 2>/dev/null || dd if=/dev/zero bs=1 count=0 seek=32768 of=bin/os.bin 2>/dev/null
	@echo "=== OS Image Info ==="
	@ls -la bin/os.bin
	@echo "✅ Build complete: bin/os.bin"

clean:
	@echo "Cleaning..."
	@$(MAKE) -C src/kernel clean
	@rm -f bin/*.bin
	@echo "✅ Clean complete"

run: all
	@echo "Starting QEMU..."
	@qemu-system-i386 -drive format=raw,file=bin/os.bin,if=floppy -boot a