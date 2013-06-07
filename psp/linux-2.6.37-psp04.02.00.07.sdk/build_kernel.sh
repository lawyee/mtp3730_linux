#!/bin/sh
export MTP3730_LINUX=~/mtp3730_linux/psp/
export UBOOT=$MTP3730_LINUX/u-boot-2010.06-psp04.02.00.07.sdk
export PATH=~/arm-2009q1/bin:$PATH
export CROSS_COMPILE=arm-none-linux-gnueabi-                                                                                               
export ARCH=arm
export PATH=$UBOOT/tools:$PATH
#make clean && \
#make omap3_beagle_defconfig && \
#make menuconfig && \
make uImage -j4 2>&1 |tee kernel_make.out
#make modules -j4 2>&1 |tee kernel_module.out
