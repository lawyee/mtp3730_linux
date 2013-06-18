cmd_arch/arm/vfp/vfphw.o := arm-none-linux-gnueabi-gcc -Wp,-MD,arch/arm/vfp/.vfphw.o.d  -nostdinc -isystem /home/ourselec/mtp3730_linux/psp/arm-2009q1/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3/include -I/home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-omap2/include -Iarch/arm/plat-omap/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -include asm/unified.h -Wa,-mfpu=softvfp+vfp -gdwarf-2        -c -o arch/arm/vfp/vfphw.o arch/arm/vfp/vfphw.S

deps_arch/arm/vfp/vfphw.o := \
  arch/arm/vfp/vfphw.S \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/thumb2/kernel.h) \
    $(wildcard include/config/vfpv3.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/iwmmxt.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/vfpmacros.h \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/hwcap.h \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/vfp.h \
  arch/arm/vfp/../kernel/entry-header.S \
    $(wildcard include/config/frame/pointer.h) \
    $(wildcard include/config/alignment/trap.h) \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/cpu/v6.h) \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/linkage.h \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/linkage.h \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/trace/irqflags.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \

arch/arm/vfp/vfphw.o: $(deps_arch/arm/vfp/vfphw.o)

$(deps_arch/arm/vfp/vfphw.o):
