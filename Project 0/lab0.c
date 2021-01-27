#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

void segFault(){
	char *ptr = NULL;
	(*ptr) = 0;
}

void segSignal(int sig){
	fprintf(stderr, "Error, a segmentation fault was caught: %d\n", sig);
	exit(4);

}

int main(int argc, char** argv){
	char* input = NULL;
	char* output = NULL;
	bool segbool = false;
	bool catchbool = false;
	bool inbool = false;
	bool outbool = false;
	char* err = "Invalid arguemnts, use correct usage lines:\n --input=filename ... use the specified file as standard input\n --output=filename ... create specified file and use it as standard output\n --segfault ... force a segmentation fault \n --catch";
	struct option args[] = {
		{"input", 1, NULL,'i'},
		{"output", 1, NULL, 'o'},
		{"segfault", 0, NULL, 'f'},
		{"catch", 0, NULL, 'c'},
		{0, 0, 0, 0}
	};

	int i = 0;
	while((i = getopt_long(argc, argv, "", args, NULL)) >= 0){
		switch(i){
			case 'i':
				input = optarg;
				inbool = true;
				break;
			case 'o':
				output = optarg;
				outbool = true;
				break; 
			case 'f':
				segbool = true;
				break;
			case 'c':
				catchbool = true;
				break;
			case '?':
				printf(err);
				exit(1);
				break;
		
		}			
	}
	if(inbool == true){
		int ifd = open(input, O_RDONLY);	
		if(ifd < 0){
			fprintf(stderr, "Unable to open --input file: %s, %s\n", input, strerror(errno));
			exit(2);
		}
		else{
			close(0);
			dup(ifd);
			close(ifd);
		}		
	}
	if(outbool == true){
		int ofd = creat(output, 0666);
		if(ofd < 0){
			fprintf(stderr, "Unable to open or create --output file: %s, %s\n", output, strerror(errno));
			exit(3);
		}
		else{
			close(1);
			dup(ofd);
			close(ofd);
		}
	
	}
	
	if(catchbool == true)
		signal(SIGSEGV, segSignal);
	
	if(segbool == true)
		segFault();		
	
	ssize_t r, w;
	char buffer;
	while((r = read(0, &buffer, sizeof(char ))) > 0){
		w = write(1, &buffer, sizeof(unsigned char));
     		if(w < 0){
			fprintf(stderr, "There was an error while writing to output: %s\n", strerror(errno));
			exit(errno);
		}
	}
	if(r < 0){
		fprintf(stderr, "There was en error reading from input: %s\n", strerror(errno));
		exit(errno);
	}
	exit(0); 
}		

