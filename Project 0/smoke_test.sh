#! /bin/bash

# Name: Daniel Medina Garate
# Email: dmedinag@g.ucla.edu
# ID: 204971333

# Test 1: Make sure copying is successful returning exit code 0 using --input=joe.txt --output=bruin.txt
echo "Input/Output file check" > joe.txt
./lab0 --input=joe.txt --output=bruin.txt
if [ $? -eq 0 ]
then
	echo "Test 1: Succefully exited with exit code 0"
else
	echo "Test 1: Failed to exit with exit code 0"
fi

# Test 2: Make sure unrecognized argument returns exit code 1
./lab0 --joe &> /dev/null
if [ $? -eq 1 ]
then
	echo "Test 2: Succesfully exited with exit code 1"
else
	echo "Test 2: Failed to exit with exit code 1 due to unrecognized argument"
fi

# Test 3: Make sure that exit code 2 is returned when not able to open --input=file
touch josie.txt
chmod a-r josie.txt
./lab0 --input=josie.txt &> /dev/null
if [ $? -eq 2 ]
then 
	echo "Test 3: Succesfully exited with exit code 2"
else
	echo "Test 3: Failed to exit with exit code 2 due to not being able to open input file"
fi
rm -f josie.txt

# Test 4: Make sure that exit code 3 is returned when not able to open --output=file
touch josie.txt
chmod a-w josie.txt
./lab0 --input=joe.txt --output=josie.txt &> /dev/null
if [ $? -eq 3 ]
then
	echo "Test 4: Succesfully exited with exit code 3"
else
	echo "Test 4: Failed to exit with exit code 3 due to not being able to open output file"
fi
rm -f josie.txt

#Test 5: Make sure that exit code 4 is returned segment fault is caused, caught, and received SIGSEG
./lab0 --catch --segfault &> /dev/null
if [ $? -eq 4 ]
then
	echo "Test 5: Succesfully exited with exit code 4"
else
	echo "Test 5: Failed to exit with exit code 4 due to not being able to catch segment fault and receiving SIGSEG"
fi

# Test 6: Make sure copying is correct from stdin/stdout
echo "This is a test" > in.txt
echo "This is a test" | ./lab0 > out.txt
cmp in.txt out.txt
if [ $? -eq 0 ]
then
        echo "Test 6: Succesfully copied from stdin to stdout"
else
        echo "Test 6: Failed to correctly copy from stdin to std out"
fi
rm -f in.txt out.txt


# Test 7: Make sure copying is correct using --input=joe.txt --output=bruin.txt
cmp joe.txt bruin.txt
if [ $? -eq 0 ]
then
        echo "Test 7: Succesfully copied --input=joe.txt to --output=bruin.txt"
else
        echo "Test 7: Failed to correctly copy --input=joe.txt"
        cat joe.txt
        echo "to --output=bruin.txt"
        cat bruin.txt
fi
rm -f joe.txt bruin.txt

