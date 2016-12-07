#!/bin/bash
echo "Attempting hook"
rmmod myhook 2> /dev/null
insmod myhook.ko
./a.out > /dev/null
while [ $(dmesg | tail -n 5 | head -n 1 | cut -c 16-20) != "rreK" ]; do
echo "Hook failed :("
echo "Attempting hook again"
rmmod myhook
insmod myhook.ko
./a.out > /dev/null
done
