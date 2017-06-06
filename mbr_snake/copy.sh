#!/bin/sh
if [ "$#" -ne 1 ] ; then
    echo "Please specify an output file/device!"
    exit 1
fi
sudo dd bs=512 if=bin/mbr_snake.bin of=$1
