cmd_.tmp_kallsyms1.o := arm-none-linux-gnueabi-gcc -Wp,-MD,./..tmp_kallsyms1.o.d -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2    -nostdinc -isystem /home/ourselec/mtp3730_linux/psp/arm-2009q1/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3/include -I/home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-omap2/include -Iarch/arm/plat-omap/include    -c -o .tmp_kallsyms1.o .tmp_kallsyms1.S

deps_.tmp_kallsyms1.o := \
  .tmp_kallsyms1.S \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
    $(wildcard include/config/64bit.h) \

.tmp_kallsyms1.o: $(deps_.tmp_kallsyms1.o)

$(deps_.tmp_kallsyms1.o):
