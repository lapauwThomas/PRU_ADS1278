#!/bin/bash

echo "Removing pru_rproc.ko"
rmmod -f /lib/modules/4.4.54-ti-r93//kernel/drivers/remoteproc/pru_rproc.ko

echo  "Reenabling pru_rproc"
modprobe pru_rproc

echo "done"
