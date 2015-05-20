#!/bin/sh

logfile=log
initinterval=1	# wait $initinterval second after qemu starts before sending keys
keyinterval=0.1	# send a key after for $keyinterval second.
maxrun=0		# Run qemu for $maxrun times. When this value is 0, qemu will run forever (use ctrl+C to stop running).
fifo=fifo #monitor-fifo
cmdfile=cmd	#file containing strings to send to qemu
i=1

function sendkey {
echo "sendkey $1" >> $fifo
sleep $keyinterval
}

function sendstr() {
str=$@
length=`expr length "$str"`
local i
for ((i=1; i<=$length; i++)); do
	char=`expr substr "$str" $i 1`
	case $char in
	'-') char='minus';;
	'.') char='dot';;
	'/') char='slash';;
	' ') char='spc';;
	esac
	sendkey "$char"
done
sendkey "ret"
}

function exec_cmd() {
str=$@
char=`expr substr "$str" 1 1`
if [ "$char" = "#" ];then
	sleep `expr substr "$str" 2 99999`
else
	sendstr $str
fi
}

function exec_cmdfile() {
repeat=1	# modify this value if you want to execute commands in a file several times
while [ $repeat != 0 ]; do
	while read line; do
		exec_cmd $line
	done < $@
	repeat=$(($repeat - 1))
done
}

function die {
pkill -9 tail
pkill -9 qemu
echo -e $1 1>&2
stty echo
exit
}

trap 'die "kill by user"' 2

make disk.img
if [ $? != 0 ]; then
	exit
fi

>> $cmdfile

while true; do
	echo -e "\nstart the ${i}th test..."

	> $fifo
	tail -f $fifo | qemu-system-i386 -serial file:$logfile -monitor stdio disk.img 2>> $logfile &
	sleep $initinterval

	exec_cmdfile $cmdfile


	if grep -q 'system panic' $logfile; then
		die
	elif grep -q 'qemu: fatal:' $logfile; then
		die '\n\033[1;31mqemu has crashed!\033[0m'
	elif [ `grep -c 'Hello, OS World!' $logfile` != 1 ]; then
		die '\n\033[1;31mMysterious reboot is detected!\033[0m'
	elif [ $i == $maxrun ]; then
		die
	fi

	pkill -9 tail
	pkill -9 qemu

	i=$((i + 1));
done
