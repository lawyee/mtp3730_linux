cmd_drivers/input/keyboard/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/input/keyboard/built-in.o drivers/input/keyboard/atkbd.o drivers/input/keyboard/gpio_keys.o drivers/input/keyboard/twl4030_keypad.o 
