cmd_kernel/sys_ni.o := arm-none-linux-gnueabi-gcc -Wp,-MD,kernel/.sys_ni.o.d  -nostdinc -isystem /home/luoyi/arm-2009q1/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.3/include -I/home/luoyi/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-omap2/include -Iarch/arm/plat-omap/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -fno-stack-protector -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(sys_ni)"  -D"KBUILD_MODNAME=KBUILD_STR(sys_ni)" -c -o kernel/.tmp_sys_ni.o kernel/sys_ni.c

deps_kernel/sys_ni.o := \
  kernel/sys_ni.c \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/luoyi/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/linkage.h \
  include/linux/errno.h \
  /home/luoyi/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  /home/luoyi/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk/arch/arm/include/asm/unistd.h \
    $(wildcard include/config/aeabi.h) \
    $(wildcard include/config/oabi/compat.h) \

kernel/sys_ni.o: $(deps_kernel/sys_ni.o)

$(deps_kernel/sys_ni.o):
