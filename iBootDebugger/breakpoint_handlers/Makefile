%.bin: %.c
	@arm-none-eabi-gcc -march=armv7-a -mthumb -nostdlib -fno-builtin -fomit-frame-pointer -fpie -I../target -o tmp.elf $(<)
	@arm-none-eabi-objcopy -O binary tmp.elf $(@)

all: readp_handler.bin memalign_start_handler.bin memalign_loop_handler.bin memalign_end_handler.bin generic_handler.bin stack_dump_handler.bin

clean:
	@rm -rf *.elf
	@rm -rf *.bin