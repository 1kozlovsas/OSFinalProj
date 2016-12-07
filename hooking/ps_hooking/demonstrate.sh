#!/bin/bash
echo "Attempting hook"
rmmod myhook 2> /dev/null
insmod myhook.ko
./my_secret_proc & > /dev/null
while [ -n "`ps -e | grep my_secret_proc`"  ]; do
echo "Hook failed :("
echo $(ps -e | grep my_secret_proc)
echo "Attempting hook again"
rmmod myhook
insmod myhook.ko
./my_secret_proc & > /dev/null
done
