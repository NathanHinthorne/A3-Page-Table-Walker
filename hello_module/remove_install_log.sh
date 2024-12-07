#!/bin/bash
echo "MAKING GREATNESS."
make
echo "Removing helloModule.ko to show changes"
sudo rmmod ./helloModule.ko
echo "Installing helloModule.ko"
sudo insmod ./helloModule.ko
./log.sh $1