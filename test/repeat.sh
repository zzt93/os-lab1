#!/bin/bash

logfile=test/logs/output.log
maxrun=1	# Run qemu for $maxrun times. When this value is 0, qemu will run forever (use ctrl+C to stop running).
timeinterval=10	# Each run lasts for $timeinterval second.
i=1

function die {
    pkill qemu
    echo -e $1 1>&2
    stty echo
    exit $2
}

make disk.img
if [ $? != 0 ]
then
    exit
fi

while true
do
    echo -e "\nstart the ${i}th test..."

    make run 2>&1 | tee $logfile & #overwrite log file every run
    sleep $timeinterval

    if grep -q 'system panic' $logfile
    then
        die '\n\033[1;31mSystem panic is triggered!\033[0m' 1
    elif grep -q 'qemu: fatal:' $logfile
    then
        die '\n\033[1;31mqemu has crashed!\033[0m' 1
    elif [ `grep -c 'Hello, OS World!' $logfile` -ge 2 ]
    then
        echo -e "\n" `grep -c 'Hello, OS World!' $logfile`
        die '\n\033[1;31mMysterious reboot is detected!\033[0m' 1
    elif [ $i == $maxrun ]
    then
        die '\n\033[1;31mReach maximal run!\033[0m' 0
    elif [ $(grep -c 'timer pid: #1, device id: #0' $logfile) -ge 2 ]
    then
        die '\n\033[1;31mMysterious repetition is detected!\033[0m' 1
    fi

    pkill qemu
    i=$((i + 1))
done
