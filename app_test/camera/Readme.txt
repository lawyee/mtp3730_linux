Using it:
First,Please Specify your cross compiler,
example:
ourselec@ubuntu:$export PATH=~/arm-2009q1/bin/:$PATH

then,Please change Makefile Specify your KERNEL directory,
example:
KERNELDIR ?=~/linux-2.6.37-psp04.02.00.07.sdk/

when you specify you cross compiler and KERNEL dir,then run build.sh to build driver module and test file.


