cmd_drivers/regulator/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/regulator/built-in.o drivers/regulator/core.o drivers/regulator/dummy.o drivers/regulator/twl-regulator.o 
