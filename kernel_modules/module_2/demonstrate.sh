#!/bin/bash
echo "Attempting hook"
rmmod myhook 2> /dev/null
insmod myhook.ko
./my_proc > /dev/null &
sleep 1
while [ $(dmesg | tail -n 1 | cut -c 16-22) != "my_proc" ]; do
echo "Hook failed :("
echo "Attempting hook again"
rmmod myhook
insmod myhook.ko
./my_proc > /dev/null &
done
