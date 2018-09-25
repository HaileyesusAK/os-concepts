#!/bin/bash

INACTIVE_TIME_S=10 # parent process's inactive time in seconds
CHECK_INTERVAL=0.1 # wait time before checking child process's status
PROGRAM_NAME=$1

./"$PROGRAM_NAME" &

ppid=$!

#wait until the child process is created
chld_pid=$(ps --ppid $ppid -o pid --no-headers)
while [ "$chld_pid" == "" ]; do
	sleep $CHECK_INTERVAL
    chld_pid=$(ps --ppid $ppid -o pid --no-headers)
done

chld_status=$(ps -q $chld_pid -o state --no-headers)

# wait till the child becomes zombie
while [ "$chld_status" != "" ] && [ "$chld_status" != "Z" ]; do
	sleep $CHECK_INTERVAL
	chld_status=$(ps -q $chld_pid -o state --no-headers)
done

sleep $INACTIVE_TIME_S

# check the child's state
chld_status=$(ps -q $chld_pid -o state --no-headers)
if [ "$chld_status" == "Z" ]; then
    echo "PASS: Child status: '$chld_status'"
else
    echo "FAIL: Child status: '$chld_status'"
fi
