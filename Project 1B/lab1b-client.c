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



struct termios origt;
struct termios newt;
void terminal_setup(void){
	tcgetattr(0, &origt);
	tcgetattr(0, &newt);
	newt.c_iflag = ISTRIP;
	newt.c_oflag = 0;
	newt.c_lflag = 0;
	tcsetattr(0, TCSANOW, &newt);
}

void terminal_restore(void){
	tcsetattr(0, TCSANOW, &origt);
}

int client_connect(char* hostname, unsigned int port){
	int sockfd;
	struct sockaddr_in serv_addr; //stores server address and port info
	struct hostent* server;
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		fprintf(stderr, "There was an error creating socket: %s", strerror(errno));
		terminal_restore();
		exit(1);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	server = gethostbyname(hostname); //convert host_name to IP addr
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length); //copy IP address from server to serv_addr
	memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero); //padding zeros
	if(connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1){
		fprintf(stderr, "There was an error connecting to socket: %s", strerror(errno));
		terminal_restore();
		exit(1);
	}
	return sockfd;
}




int main(int argc, char* argv[]){
	int portnum = 0;
        char* logfile = NULL;
	int logfd = 0;
	bool portbool = false;
        bool logbool = false;
        bool compbool = false;
	char* hostname = "localhost";
        char* err = "Invalid arguemnts, use correct usage lines:\n --port=#### ... use the specified port number of server to establish connection\n --log=filename ... create specified file and use it to record data sent over socket\n --compress ... compress data that is communicated \n";
 	char sent[] = "SENT ";
	char bytes[] = " bytes: ";
	char rec[] = "RECEIVED ";       
	struct option args[] = {
                {"port", 1, NULL,'p'},
                {"log", 1, NULL, 'l'},
                {"compress", 0, NULL, 'c'},
                {0, 0, 0, 0}
        };

        int i = 0;
        while((i = getopt_long(argc, argv, "", args, NULL)) >= 0){
                switch(i){
                        case 'p':
                                portnum = atoi(optarg);
                                portbool = true;
                                break;
                        case 'l':
                                logfile = optarg;
                                logbool = true;
                                break;
                        case 'c':
                                compbool = true;
                                break;
                        case '?':
                                fprintf(stderr,err);
                                exit(1);
                                break;

                }
        }
	
	terminal_setup(); //set terminal to non-canonical mode
	//process arguments
	if(portbool == false){
		fprintf(stderr, "Error: --port argument is mandatory");
		terminal_restore();
		exit(1);
	}
	if(logbool == true){
		logfd = creat(logfile, 0666);
		if(logfd < 0){
			terminal_restore();
			fprintf(stderr, "There was an error creating log file: %s\n", strerror(errno));
			exit(1);
		}	
	}
	int sock_fd = client_connect(hostname, portnum); //conect client to server	
	struct pollfd pfd[2];
	pfd[0].fd = 0;
	pfd[0].events = POLLIN + POLLHUP + POLLERR;
	pfd[1].fd = sock_fd;
	pfd[1].events = POLLIN + POLLHUP + POLLERR;
	while(1){
		int i = poll(pfd, 2, 0);
		if(i > 0){ //terminal to socket
			if(pfd[0].revents & POLLIN){
				unsigned char buffer[256];
				int byte_count = read(0, buffer, sizeof(buffer));
				for(int j = 0; j < byte_count; j++){
					if(buffer[j] == 0x0D || buffer[j] == 0x0A){ //if \n or \r detected
						write(1,"\x0D\x0A", 2);
					}
					else{
						write(1, buffer+j, 1);
					}	
				}
				if(compbool == true){
					// Initiate zlib state for compression 
					z_stream to_shell;
					unsigned char out_buff[256];
					to_shell.zalloc = Z_NULL;
					to_shell.zfree = Z_NULL;
					to_shell.opaque = Z_NULL;
					int ret = deflateInit(&to_shell, Z_DEFAULT_COMPRESSION);
					if(ret != Z_OK){
						terminal_restore();
						fprintf(stderr, "There was an error initalizing compression: %s\n", strerror(errno));
						exit(1);
					
					} 
					to_shell.avail_in = byte_count;
					to_shell.next_in = buffer;
					to_shell.avail_out = sizeof(out_buff);
					to_shell.next_out = out_buff;
					do{
						deflate(&to_shell, Z_SYNC_FLUSH);
					} while (to_shell.avail_in > 0);
					int temp = 256 - to_shell.avail_out;
					write(sock_fd, out_buff, temp);
					if(logbool == true){
						char sizeb[20];
						sprintf(sizeb, "%d",temp);
						write(logfd, sent, strlen(sent));
						write(logfd, sizeb, strlen(sizeb));
						write(logfd, bytes, strlen(bytes));
						write(logfd, out_buff, temp);
						write(logfd, "\x0A", 1);
					}
					deflateEnd(&to_shell);
				}
				else{ 
					write(sock_fd, buffer, byte_count);
					if(logbool == true){
						char sizeb[20];
                                                sprintf(sizeb, "%d", byte_count);
                                                write(logfd, sent, strlen(sent));
                                                write(logfd, sizeb, strlen(sizeb));
                                                write(logfd, bytes, strlen(bytes));
                                                write(logfd, buffer, byte_count);
                                                write(logfd, "\x0A", 1);
					}
				}
			}
			else if(pfd[0].revents & (POLLHUP | POLLERR)){
                                close(logfd);
                                terminal_restore();
				exit(0);
                        }
			if(pfd[1].revents & POLLIN){ //socket to terminal
				unsigned char buffer[256];
				int byte_count = read(sock_fd, buffer, sizeof(buffer));
				if(byte_count == 0){
					break;
				}
				if(logbool == true){
						char siz_eb[20];
                                                sprintf(siz_eb, "%d", byte_count);
                                                write(logfd, rec, strlen(rec));
                                                write(logfd, siz_eb, strlen(siz_eb));
                                                write(logfd, bytes, strlen(bytes));
                                                write(logfd, buffer, byte_count);
                                                write(logfd, "\x0A", 1);		
				}
				if(compbool == true){
					z_stream from_shell;
					unsigned char buff_in[256];
					from_shell.zalloc = Z_NULL;
					from_shell.zfree = Z_NULL;
					from_shell.opaque = Z_NULL;
					int ter = inflateInit(&from_shell);
					if(ter != Z_OK){
						terminal_restore();
						fprintf(stderr, "There was an error initalizing decompression: %s\n", strerror(errno));
						exit(1);
					}
					from_shell.avail_in = byte_count;
					from_shell.next_in = buffer;
					from_shell.avail_out = sizeof(buff_in);
					from_shell.next_out = buff_in;
					do{
						inflate(&from_shell, Z_SYNC_FLUSH);
					} while(from_shell.avail_in > 0);
					int temp = 256 - from_shell.avail_out; 
					for(int k = 0; k < temp; k++ ){
						if(buff_in[k] == 0x0A){
							write(1, "\x0D\x0A", 2);
						}
						else{
							write(1, buff_in+k, 1);
						}

					}
					inflateEnd(&from_shell); 
				}
				else{ 
					for(int j = 0; j < byte_count; j++){
						if(buffer[j] == 0x0A){
							write(1, "\x0D\x0A", 2);
						}
						else{
							write(1, buffer+j, 1);
						}
					}
				}
			}
			else if(pfd[1].revents & (POLLHUP | POLLERR)){
				close(logfd);
				terminal_restore();
				exit(0);
			}		
		}
		else if(i == -1){
			terminal_restore();
			fprintf(stderr, "There was an error with poll(): %s\n", strerror(errno));
			exit(1);
		}
	}
	terminal_restore();
	exit(0);
}
