/* Name: Daniel Medina Garate
 * Email: dmedinag@g.ucla.edu
 * ID: 204971333
 */

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <stdbool.h>

bool shellbool = false;
char err[] = "Correct usage: --shell";
struct termios origt;
int ret;
void terminal_setup(void){
	struct termios newt;
	tcgetattr(0, &origt);
	newt = origt;
	newt.c_iflag = ISTRIP;
	newt.c_oflag = 0;
	newt.c_lflag = 0;
	tcsetattr(0, TCSANOW, &newt);
}

void terminal_restore(void){
	tcsetattr(0, TCSANOW, &origt);
}

void sigHandle(){
	int status;
	waitpid(ret, &status, 0);
	printf("SHELL EXIT SIGNAL=%d STATUS=%d", WTERMSIG(status), WEXITSTATUS(status));
	exit(0);
	

}

int main (int argc, char* argv[]){
	//Checks arguments
	struct option args[] = {
                {"shell", 0, NULL,'s'},
                {0, 0, 0, 0}
        };	
	int i = 0;
	while((i = getopt_long(argc, argv, "", args, NULL)) >= 0){
        	switch(i){
                	case 's':
                                shellbool = true;
				break;
                        case '?':
                                printf(err);
                                exit(1);
                                break;
                }
        }
        //Part A: Set the stdin into no echo mode, non-canonical input
    	terminal_setup();
	//Process any argument(s)
	if(shellbool == true){
               	int pipe_c[2];
		int pipe_p[2];
		if((pipe(pipe_c)) == -1){ //checks if pipes created correctly
			fprintf(stderr, "Failed to open child pipe: %s\n", strerror(errno));
			terminal_restore();
			exit(1);
		}
		if((pipe(pipe_p)) == -1){
			fprintf(stderr, "Failed to open parent pipe: %s\n", strerror(errno));
			terminal_restore();
			exit(1);
		}
		ret = fork();	
                
                if(ret == 0 ){ //child process
 			close(pipe_p[1]); //unused pipes
			close(pipe_c[0]); //unused pipes 
			close(0);
			dup(pipe_p[0]);
			close(pipe_p[0]);
			close(1);
			dup(pipe_c[1]);
			close(2);
			dup(pipe_c[1]);
			close(pipe_c[1]);
			//child process runs program below
			execlp("/bin/bash", "/bin/bash", NULL);
			//will not fun unless error w/ execlp()
			fprintf(stderr, "There was an error with execlp(): %s\n", strerror(errno));
			terminal_restore();
			exit(1);     
                }
		else if(ret > 0){ //parent prcess
			close(pipe_p[0]); //unused pipes
			close(pipe_c[1]); //unused pipes
			
			struct pollfd pfd[2];
			pfd[0].fd = 0;
			pfd[0].events = POLLIN + POLLHUP + POLLERR;
			pfd[1].fd = pipe_c[0];
			pfd[1].events = POLLIN + POLLHUP + POLLERR;
			signal(SIGPIPE, sigHandle);
			while(1){
				int i = poll(pfd, 2, 0);
					if(i > 0){ //terminal to shell
						if(pfd[0].revents & POLLIN){
							char buffer[256];
							int byte_count = read(0, buffer, sizeof(buffer));
							for(int j = 0; j < byte_count; j++){
								if(buffer[j] == 0x03){ //if Ctrl + C detected
									write(1, "^C", 2);
									kill(ret, SIGINT);
									break;
								}
								else if(buffer[j] == 0x04){ //if Ctrl + D detected
									write(1, "^D", 2);
									close(pipe_p[1]); //sends EOF-->Shell-->pipe_c
									break;
								}
								else if(buffer[j] == 0x0D || buffer[j] == 0x0A){ //if \n or \r detected
									write(1,"\x0D\x0A", 2);
									write(pipe_p[1], "\x0A", 1);
								}
								else{
									write(0, buffer+j, 1);
									write(pipe_p[1], buffer+j, 1);
								}	
							
							}
						}
						if(pfd[1].revents & POLLIN){ //shell to terminal
							char buffer[256];
							int byte_count = read(pipe_c[0], buffer, sizeof(buffer));
							for(int j = 0; j < byte_count; j++){
								if(buffer[j] == 0x04){
									close(pipe_c[0]); //will trigger POLLHUP
									break;
								}
								else if(buffer[j] == 0x0A){
									write(0, "\x0D\x0A", 2);
								}
								else{
									write(0, buffer+j, 1);
								}
							}
						}
						else if(pfd[1].revents & POLLHUP){ //only exectues if shell is no longegr running 
							int status = 0;
							waitpid(ret, &status, 0);
							terminal_restore();
							printf("SHELL EXIT SIGNAL=%d STATUS=%d", WTERMSIG(status), WEXITSTATUS(status));
							exit(0);
						}
						else if(pfd[1].revents & POLLERR){
                                                        fprintf(stderr, "There was an error with Poll: %s", strerror(errno));
                                                        exit(1);
						}		
					}
					else if(i == -1){
						fprintf(stderr, "There was an error with poll(): %s\n", strerror(errno));
						terminal_restore();
						exit(1);
					}
			}
		}
		else{ //error when calling fork()
                        fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
                        terminal_restore();
                        exit(1);
                }
	}
	else{ //runs if shell argument is not provided 
	char buffer[256];
	bool eof = true;
	int byte_count = 0;
	while((byte_count = read(0, buffer, 256)) && eof){
		for(int i = 0; i < byte_count; i++){
			if(buffer[i] == 0x04){
				eof = false;	
				break;
			}
			else if(buffer[i] == 0x0D || buffer[i] == 0x0A){
				write(1, "\x0D\x0A", 2);
			}
			else{
				write(1, buffer+i, 1);
			}
		}
		if(eof == false)
			break;
	}
	terminal_restore();
	exit(0);
	}
}
