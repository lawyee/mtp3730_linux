cmd_arch/arm/mach-omap2/sleep34xx.o := arm-none-linux-gnueabi-gcc -Wp,-MD,arch/arm/mach-omap2/.sleep34xx.o.d  -nostdinc -isystem /home/ourselec/mtp3730_linux/psp/arm-2009q1/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3/include -I/home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-omap2/include -Iarch/arm/plat-omap/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float -gdwarf-2     -Wa,-march=armv7-a   -c -o arch/arm/mach-omap2/sleep34xx.o arch/arm/mach-omap2/sleep34xx.S

deps_arch/arm/mach-omap2/sleep34xx.o := \
  arch/arm/mach-omap2/sleep34xx.S \
    $(wildcard include/config/p.h) \
    $(wildcard include/config/omap3/l2/aux/secure/save/restore.h) \
    $(wildcard include/config/omap3/l2/aux/secure/service/set/id.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/linkage.h \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/hwcap.h \
  arch/arm/plat-omap/include/plat/sram.h \
    $(wildcard include/config/pm.h) \
  arch/arm/mach-omap2/include/mach/io.h \
  arch/arm/plat-omap/include/plat/io.h \
    $(wildcard include/config/arch/omap2420.h) \
    $(wildcard include/config/arch/omap2430.h) \
    $(wildcard include/config/arch/omap3.h) \
    $(wildcard include/config/arch/omap4.h) \
    $(wildcard include/config/arch/ti81xx.h) \
  arch/arm/mach-omap2/include/mach/hardware.h \
  arch/arm/plat-omap/include/plat/hardware.h \
    $(wildcard include/config/reg/base.h) \
    $(wildcard include/config/arch/omap1.h) \
  /home/ourselec/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/sizes.h \
  arch/arm/plat-omap/include/plat/serial.h \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  arch/arm/plat-omap/include/plat/omap7xx.h \
    $(wildcard include/config/base.h) \
  arch/arm/plat-omap/include/plat/omap1510.h \
  arch/arm/plat-omap/include/plat/omap16xx.h \
  arch/arm/plat-omap/include/plat/omap24xx.h \
  arch/arm/plat-omap/include/plat/omap34xx.h \
  arch/arm/plat-omap/include/plat/omap44xx.h \
  arch/arm/plat-omap/include/plat/ti81xx.h \
  arch/arm/mach-omap2/include/mach/pci.h \
  arch/arm/mach-omap2/cm2xxx_3xxx.h \
  arch/arm/mach-omap2/prcm-common.h \
  arch/arm/mach-omap2/prm2xxx_3xxx.h \
    $(wildcard include/config/offset.h) \
    $(wildcard include/config/arch/omap2.h) \
    $(wildcard include/config/arch/ti816x.h) \
  arch/arm/mach-omap2/prm.h \
  arch/arm/mach-omap2/sdrc.h \
  arch/arm/plat-omap/include/plat/sdrc.h \
  arch/arm/mach-omap2/control.h \
    $(wildcard include/config/arch/omap2plus.h) \
  arch/arm/mach-omap2/include/mach/ctrl_module_core_44xx.h \
    $(wildcard include/config/idlemode/shift.h) \
    $(wildcard include/config/idlemode/mask.h) \
  arch/arm/mach-omap2/include/mach/ctrl_module_wkup_44xx.h \
  arch/arm/mach-omap2/include/mach/ctrl_module_pad_core_44xx.h \
  arch/arm/mach-omap2/include/mach/ctrl_module_pad_wkup_44xx.h \

arch/arm/mach-omap2/sleep34xx.o: $(deps_arch/arm/mach-omap2/sleep34xx.o)

$(deps_arch/arm/mach-omap2/sleep34xx.o):
