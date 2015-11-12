#!/bin/bash

aim='const uint32_t super_start = '
block='../src/kernel/manager/FM/block.c'

read new_block_start

line=$(egrep "$aim[0-9]+" $block)
old_block_start=$(echo "$line" | egrep -o '(^[1-9][0-9]*)|( [1-9][0-9]*)' | sed 's/[^0-9]//g')

#old_block_start=$(egrep '$aim[0-9]+' $block | egrep '(^[1-9][0-9]*)|( [1-9][0-9])')

if [ "$new_block_start" != "$old_block_start" ];then
	sed -i "s/$aim$old_block_start/$aim$new_block_start/g" "$block"
	cd ..; make disk.img
	cd harddisk; 
fi
make newimg
make catimg

