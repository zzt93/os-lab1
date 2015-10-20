#!/bin/bash

aim='const uint32_t super_start = '
block='./src/kernel/manager/FM/block.c'

new_block_start=$(read)

old_block_start=$(echo $block | egrep '$aim[0-9]+' | sed 's/[^0-9]*//g')

if [ "$new_block_start" != "$old_block_start" ];then
	sed 's/$aim$old_block_start/$aim$new_block_start/g' $block
	make run
fi
