#!/bin/bash/

echo "setting up internet"
sudo /sbin/route add default gw 192.168.7.1
echo "nameserver 8.8.8.8" >> /etc/resolv.conf
echo "done"
