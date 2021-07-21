default:
	gcc -g -lm -lmraa -Wall -Wextra lab4b.c -o lab4b

check: default
	bash lab4b_test.sh

dist:
	tar -czvf lab4b-204971333.tar.gz lab4b.c README Makefile lab4b_test.sh

clean:
	rm -f lab4b-204971333.tar.gz lab4b
