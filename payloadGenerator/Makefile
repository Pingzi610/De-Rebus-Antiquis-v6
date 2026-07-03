all: target/payload.bin target/nettoyeur.bin
	@gcc -o genPayload main.c -I. -Itarget -std=c23

target/payload.bin:
	@arm-none-eabi-as -march=armv7-a -mthumb -o target/payload.elf -c  target/payload.S
	@arm-none-eabi-objcopy -O binary target/payload.elf target/payload.bin

target/nettoyeur.bin:
	@arm-none-eabi-gcc -march=armv7-a -mthumb -Wno-multichar -nostdlib -fno-builtin -fpie -Os -o target/nettoyeur.elf target/nettoyeur.c
	@arm-none-eabi-objcopy -O binary target/nettoyeur.elf target/nettoyeur.bin

clean:
	@rm -rf genPayload
	@rm -rf target/*.bin
	@rm -rf target/*.elf
