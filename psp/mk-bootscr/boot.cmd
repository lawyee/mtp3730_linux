if fatload mmc 0 82000000 uImage
then
 echo ***** Kernel: /dev/mmcblk0p1/uImage *****
fi
echo ***** RootFS: /dev/mmcblk0p2 *****
# Pantherboard DVI output
#setenv bootargs 'console=ttyO0,115200n8 androidboot.console=ttyO2 mem=68M@0x80000000 mem=384M@0x88000000 root=/dev/mmcblk0p2 rw rootfstype=ext3 rootdelay=1 init=/init ip=off omap_vout.vid1_static_vrfb_alloc=y omapdss.def_disp=dvi omapfb.mode=dvi:800x480MR-16 vram=8M omapfb.vram=0:8M mpurate=1000'


setenv bootargs 'console=ttyO0,115200n8 androidboot.console=ttyO2 mem=78M@0x80000000 mem=130M@0x85900000 root=/dev/mmcblk0p2 rw rootfstype=ext3 rootdelay=1 init=/init ip=off omap_vout.vid1_static_vrfb_alloc=y omapdss.def_disp=dvi omapfb.mode=dvi:800x480MR-16 vram=8M omapfb.vram=0:8M mpurate=1000'




































bootm 0x82000000
