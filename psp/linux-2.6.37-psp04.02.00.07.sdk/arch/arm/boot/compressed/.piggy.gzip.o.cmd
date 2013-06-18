cmd_arch/arm/boot/compressed/piggy.gzip.o := arm-none-linux-gnueabi-gcc -Wp,-MD,arch/arm/boot/compressed/.piggy.gzip.o.d  -nostdinc -isystem /home/ourselec/mtp3730_linux/psp/arm-2009q1/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3/include -I/home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-omap2/include -Iarch/arm/plat-omap/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2     -Wa,-march=all    -c -o arch/arm/boot/compressed/piggy.gzip.o arch/arm/boot/compressed/piggy.gzip.S

deps_arch/arm/boot/compressed/piggy.gzip.o := \
  arch/arm/boot/compressed/piggy.gzip.S \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

arch/arm/boot/compressed/piggy.gzip.o: $(deps_arch/arm/boot/compressed/piggy.gzip.o)

$(deps_arch/arm/boot/compressed/piggy.gzip.o):
