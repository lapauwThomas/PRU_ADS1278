#!/bin/bash/
#  The following are a series of bash commands
#  to set environment variables useful for
#  PRU code development.

export PRU_CGT=/usr/share/ti/cgt-pru
export PRU=/home/debian/pru
export SLOTS=/sys/devices/platform/bone_capemgr/slots

#  The following use the "Universal IO" to configure
#  the Beaglebone pins to required PRU modes.
#  https://github.com/cdsteinkuehler/beaglebone-universal-io

echo cape-universala > $SLOTS

#pru0 r30 15 SCLK
config-pin P8.11 pruout
#pru0 r30 14 heartbeat
config-pin P8.12 pruout
#pru0 r31 14 !DRDY
config-pin P8.16 pruin

#pru0 r31  0-7
config-pin P9.31 pruin
config-pin P9.29 pruin
config-pin P9.30 pruin
config-pin P9.28 pruin
config-pin P9.42.1 pruin
config-pin P9.27 pruin
config-pin P9.41.1 pruin
config-pin P9.25 pruin

