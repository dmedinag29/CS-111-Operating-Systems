#! /bin/bash

printf "SCALE=F\nSCALE=C\nSTOP\nSTART\nLOG this is a test\nOFF\n" | ./lab4b --log=test.txt
for i in SCALE=F SCALE=C STOP START "LOG this is a test" OFF "SHUTDOWN"
do
    grep "$i" test.txt > /dev/null
    if [ $? -ne 0 ]
    then
        echo "The command $i was not logged"
    else
        echo "The command $i was logged correctly"
    fi
done

rm -f test.txt
