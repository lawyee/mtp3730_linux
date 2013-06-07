#This is the path to the Linux Kernel Directory. Change this path
#to the one where Linux kernel is kept.
KERNEL_DIR =

#This is root directory where Source is available. Change this path
#to the one where source files are kept
INSTALL_DIR = ./

#this is the path to the executable where all the executables are kept
EXE_DIR = ./bin

#This is the path to the directory containing required libraries
LIB_DIR =

#This is path to the Include files of Library
LIB_INC =

#This is the path the include folder of the kernel directory. The include
#folder should contain all the necessary header files
INCLUDE_DIR = $(KERNEL_DIR)/include -I$(KERNEL_DIR)/arch/arm/plat-omap/include/

#This is the prefix applied to the gcc when compiling applications and library
CROSS_COMPILE = arm-arago-linux-gnueabi-
