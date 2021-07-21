default:
	gcc -Wall -Wextra -lz lab1b-client.c -o lab1b-client
	gcc -Wall -Wextra -lz lab1b-server.c -o lab1b-server

client:
	gcc -Wall -Wextra -lz lab1b-client.c -o lab1b-client

server:
	gcc -Wall -Wextra -lz lab1b-server.c -o lab1b-server

clean: 
	rm -f lab1b-204971333.tar.gz lab1b-client lab1b-server

dist:
	tar -cvzf lab1b-204971333.tar.gz lab1b-client.c lab1b-server.c Makefile README
