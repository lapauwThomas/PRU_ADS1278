#!/bin/bash

# start logging
echo -e "Project EMS program starting\n" > /root/ProjectEMSLog

# Load cape
echo cape-universala > /sys/devices/platform/bone_capemgr/slots

# Configure PinMux
# pru0 r30 15 SCLK
config-pin P8.11 pruout
# pru0 r30 14 heartbeat
config-pin P8.12 pruout
# pru0 r31 14 !DRDY
config-pin P8.16 pruin

# pru0 r31  0-7
config-pin P9.31 pruin
config-pin P9.29 pruin
config-pin P9.30 pruin
config-pin P9.28 pruin
config-pin P9.92 pruin
config-pin P9.27 pruin
config-pin P9.91 pruin
config-pin P9.25 pruin

# pru0 r31 16
config-pin P9.24 pruin

# copy lastest  version of program to right directory
cp -rf /root/PRU_ADS1278.out /lib/firmware/am335x-pru0-fw


# remove and start rproc kernel module
echo -e "pru_rproc kernel module restarted\n" > /root/ProjectEMSLog

rmmod -f pru_rproc
modprobe pru_rproc

# send 1 character to start the pogram
#echo "a"> /dev/rpmsg_pru30

