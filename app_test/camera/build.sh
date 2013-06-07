#!/bin/sh
#this shell to build driver module and test file.
#please Specify your cross compiler.
#export PATH=/home/luoyi/arm-2009q1/bin:$PATH
export PATH=/home/ourselec/ti-dvsdk_dm3730-evm_04_03_00_06/linux-devkit/bin:$PATH
arm-arago-linux-gnueabi-gcc hello.c -o hello
#arm-arago-linux-gnueabi-gcc ov5640_Linux_Testapp.c -o ov5640_Linux_Testapp

#copy test files to test directory
