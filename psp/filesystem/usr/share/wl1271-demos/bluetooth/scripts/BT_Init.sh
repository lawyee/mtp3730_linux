#!/bin/sh
#/***************************************************************************
# * \file    BT_Init.sh
# *
# * \brief   Script to Initialize the BT
# *
# * \author  Sinoj@Mistral
# *
# * \version 01a,17Nov09, created
# * \version 02a,27Sep10, modified
# * \version 03a,25Oct10, modified
# * \version 02b,09Dec10, reverted, modified
# * \version 03a,09Dec10, modified
# ***************************************************************************/

FTP_STORE_PATH=$GALLERY
echo "
/*
 * #################################################
 *                  Initialize BT
 * #################################################
 */
 "
rm   $EXE_PATH/var/run/messagebus.pid &> /dev/null
rm   $EXE_PATH/var/run/dbus/pid &> /dev/null
sleep 1
dbus-daemon --system &> /dev/null &
sleep 2
bluetoothd -n &> /dev/null &
insmod `find /lib/modules/ -name "gpio_en.ko"`
sleep 2
hciattach /dev/ttyO1 texas 3000000 & 
sleep 5
hciconfig hci0 piscan &> /dev/null
agent --path /org/bluez/agent 0000 &> /dev/null &
sleep 2
sdptool add OPUSH &> /dev/null
obexftpd -c $FTP_STORE_PATH -b &> /dev/null &
