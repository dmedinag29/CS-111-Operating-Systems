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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <zlib.h>

int ret;

void sigHandle(){
	int status;
	waitpid(ret, &status, 0);
	fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", WTERMSIG(status), WEXITSTATUS(status));
	exit(0);
	

}

int server_connect(unsigned int port_num){
	int sockfd, new_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	int sin_size;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		fprintf(stderr, "There was an error creating the scoket: %s\n", strerror(errno));
		exit(1);
	}
	/* setting the address info */
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port_num);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero)); //padding zeros
	/* binds the socket to the IP address and port number */
	bind(sockfd, (struct sockaddr*) &my_addr, sizeof(struct sockaddr));
	/*listens for connections, maximum connections able to serve is 5 */
	if(listen(sockfd, 5) == -1){
		fprintf(stderr, "There was an error calling lsiten: %s\n", strerror(errno));
		exit(1);
	}
	sin_size = sizeof(struct sockaddr_in);
	/* accepts connection and saves file descriptor to it */
	if((new_fd = accept(sockfd, (struct sockaddr*) &their_addr, &sin_size)) == -1){
		fprintf(stderr, "There was an error accepting connection: %s\n", strerror(errno));
		exit(1);
	}
	return new_fd;
}

int main (int argc, char* argv[]){
	char* err = "Correct usage: --port=#### ... use the specified port number of server to establish connection";
        bool portbool = false;
        bool compbool = false;
        int portnum = 0;
        struct option args[] = {
                {"port", 1, NULL,'p'},
                {"compress", 0, NULL, 'c'},
                {0, 0, 0, 0}
        };
        int i = 0;
        while((i = getopt_long(argc, argv, "", args, NULL)) >= 0){
                switch(i){
                        case 'p':
                                portbool = true;
                                portnum = atoi(optarg);
                                break;
                        case 'c':
                                compbool = true;
                                break;
                        case '?':
                                printf(err);
                                exit(1);
                                break;
                }
        }
	if(portbool == false){
                fprintf(stderr, "Error: --port argument is mandatory");
                exit(1);
        }
 	int sock_fd = server_connect(portnum);
        int pipe_c[2];
        int pipe_p[2];
        if((pipe(pipe_c)) == -1){ 
                fprintf(stderr, "Failed to open child pipe: %s\n", strerror(errno));
                exit(1);                                                                                   
	}
        if((pipe(pipe_p)) == -1){
        	fprintf(stderr, "Failed to open parent pipe: %s\n", strerror(errno));
        	exit(1);
	}
	ret = fork();
	if(ret == 0){ //child process
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
		/* child process runs program below */
		execlp("/bin/bash", "/bin/bash", NULL);
		fprintf(stderr, "There was an error with execlp(): %s\n", strerror(errno));
		exit(1);     
	}
	else if(ret > 0){ //parent process 
		close(pipe_p[0]); //unused pipes
		close(pipe_c[1]); //unused pipes	
		struct pollfd pfd[2];
		pfd[0].fd = sock_fd;
		pfd[0].events = POLLIN + POLLHUP + POLLERR;
		pfd[1].fd = pipe_c[0];
		pfd[1].events = POLLIN + POLLHUP + POLLERR;
		signal(SIGPIPE, sigHandle);
		while(1){
			int i = poll(pfd, 2, 0);
			if(i > 0){ 
				if(pfd[0].revents & POLLIN){ //server to shell pollling
					unsigned char buffer[1024];
					int byte_count = read(sock_fd, buffer, sizeof(buffer));
					if(compbool == true){
						z_stream from_client;
                                        	unsigned char buff_in[1024];
                                        	from_client.zalloc = Z_NULL;
                                        	from_client.zfree = Z_NULL;
                                        	from_client.opaque = Z_NULL;
                                        	int ter = inflateInit(&from_client);
                                        	if(ter != Z_OK){
                                                	fprintf(stderr, "There was an error initalizing decompression: %s\n", strerror(errno));
                                                	exit(1);
                                        	}
                                        	from_client.avail_in = byte_count;
                                        	from_client.next_in = buffer;
                                        	from_client.avail_out = sizeof(buff_in);
                                        	from_client.next_out = buff_in;
                                       		do{
                                                	inflate(&from_client, Z_SYNC_FLUSH);
                                        	} while(from_client.avail_in > 0);
						int temp = 1024 - from_client.avail_out;
						//printf("This is temp: %d", temp);
						for(int j = 0; j < temp; j++){
                                                        if(buff_in[j] == 0x03){ //if Ctrl + C received
                                                                kill(ret, SIGINT);
                                                                break;
                                                        }
                                                        else if(buff_in[j] == 0x04){ //if EOF recevied
                                                                close(pipe_p[1]);
                                                                break;
                                                        }
                                                        else if(buff_in[j] == 0x0D || buff_in[j] == 0x0A){
                                                                write(pipe_p[1], "\x0A", 1);
                                                        }
                                                        else{
								write(pipe_p[1], buff_in+j, 1);
							 }

                                                }
					}
					else{
						for(int j = 0; j < byte_count; j++){
							if(buffer[j] == 0x03){ //if Ctrl + C received
								kill(ret, SIGINT);
								break;
							}
							else if(buffer[j] == 0x04){ //if EOF recevied
								close(pipe_p[1]); 
								break;
							}
							else if(buffer[j] == 0x0D || buffer[j] == 0x0A){ 
								write(pipe_p[1], "\x0A", 1);
							}
							else{
								write(pipe_p[1], buffer+j, 1);
							}	
							
						}
					}
				}
				if(pfd[1].revents & POLLIN){ //shell to server
					unsigned char buffer[256];
					int byte_count = read(pipe_c[0], buffer, sizeof(buffer));
							if(compbool == true){	
								/* Initiate zlib state for compression */
								z_stream to_ser;
                                        			unsigned char out_buff[256];
                                       				to_ser.zalloc = Z_NULL;
                                        			to_ser.zfree = Z_NULL;
                                        			to_ser.opaque = Z_NULL;
                                        			int ret = deflateInit(&to_ser, Z_DEFAULT_COMPRESSION);
                                      				if(ret != Z_OK){
                                               				fprintf(stderr, "There was an error initalizing compression: %s\n", strerror(errno));
                                                			exit(1);
                                				}
                                        			to_ser.avail_in = byte_count;
                                        			to_ser.next_in = buffer;
                                        			to_ser.avail_out = sizeof(out_buff);
                                        			to_ser.next_out = out_buff;
                                       			 	do{
                                                			deflate(&to_ser, Z_SYNC_FLUSH);
                                        			} while (to_ser.avail_in > 0);
                                        			int temp = 256 - to_ser.avail_out;
                                        			write(sock_fd, out_buff, temp);
								deflateEnd(&to_ser);
							}
							else{
								for(int j = 0; j < byte_count; j++){
									if(buffer[j] == 0x04){
										close(pipe_c[0]); 
										break;
									}
									else{
										write(sock_fd, buffer+j, 1);
									}
								}
							}
				}
				else if(pfd[1].revents & (POLLHUP | POLLERR)){
					int status = 0;
					waitpid(ret, &status, 0);
					fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", WTERMSIG(status), WEXITSTATUS(status));
					close(sock_fd);
					exit(0);
				}		
			}
			else if(i == -1){
				fprintf(stderr, "There was an error with poll(): %s\n", strerror(errno));
				exit(1);
			}
		}
	}	
	else{ 
		fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
		exit(1);
	}
	exit(0);
}
