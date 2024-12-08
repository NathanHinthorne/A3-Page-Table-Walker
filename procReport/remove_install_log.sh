#!/bin/bash
clear
echo "CLEARED"
echo "MAKING GREATNESS."
make
echo "Removing procReport.ko to show changes"
sudo rmmod ./procReport.ko
echo "Installing procReport.ko"
sudo insmod ./procReport.ko
./log.sh $1