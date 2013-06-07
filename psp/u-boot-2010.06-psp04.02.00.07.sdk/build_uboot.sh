#!/bin/sh
export PATH=~/arm-2009q1/bin:$PATH
export CROSS_COMPILE=arm-none-linux-gnueabi-                                                                                               
export ARCH=arm
export TARGET_PRODUCT=beagleboard 
#make distclean && \
#make omap3_beagle_config && \
make 2>&1 |tee u-boot_make.out
