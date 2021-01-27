lab0: lab0.c
	gcc -g -Wall -Wextra -o lab0 lab0.c

check: lab0
	./smoke_test.sh

clean: 
	rm -f lab0 lab0-204971333.tar.gz

dist: lab0.c Makefile README
	tar -cvzf lab0-204971333.tar.gz lab0.c Makefile README backtrace.png breakpoint.png smoke_test.sh
