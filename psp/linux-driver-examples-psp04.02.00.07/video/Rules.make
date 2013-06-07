#
# Rules.make
#
# Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
#
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#    Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#    Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the
#    distribution.
#
#    Neither the name of Texas Instruments Incorporated nor the names of
#    its contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#This is the path to the Linux Kernel Directory. Change this path
#to the one where Linux kernel is kept.
KERNEL_DIR = /home/luoyi/mtp3730_linux/psp/linux-2.6.37-psp04.02.00.07.sdk
#KERNEL_DIR = /projects/git/users/a0393758/omap-kernel

#This is root directory where Source is available. Change this path
#to the one where source files are kept
INSTALL_DIR = ./

#this is the path to the executable where all the executables are kept
EXE_DIR = ./bin

#This is the path to the Directory containing Library
LIB_DIR = ./

#This is path to the Include files of Library
#LIB_INC = $(INSTALL_DIR)/User_Library

#This is the path the include folder of the kernel directory. The include
#folder should contain all the necessary header files
INCLUDE_DIR = $(KERNEL_DIR)/include

#This is the prefix applied to the gcc when compiling applications and library
CROSS_COMPILE = arm-none-linux-gnueabi-
#CROSS_COMPILE = arm-arago-linux-gnueabi-

