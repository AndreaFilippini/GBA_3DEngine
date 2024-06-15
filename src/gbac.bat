arm-none-eabi-gcc -mthumb -mthumb-interwork -c GBA3D.c
arm-none-eabi-gcc -mthumb -mthumb-interwork -S -o GBA3D.asm GBA3D.c
arm-none-eabi-gcc -specs=gba.specs -mthumb -mthumb-interwork GBA3D.o -o GBA3D.elf
arm-none-eabi-objcopy -O binary GBA3D.elf GBA3D.gba
gbafix GBA3D.gba