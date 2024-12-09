#!/bin/bash
echo "Remember to print out around 40+ logs to show the print output for column type."
echo "Removing old logs..."
echo "Output:"
echo "Outputting $1 lines of new logs."
sudo dmesg --notime | tail -n $1
echo "Removing old logs."
sudo dmesg -C
