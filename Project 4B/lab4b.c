/* Name: Daniel Medina Garate
 * Email: dmedinag@g.ucla.edu
 * ID: 204971333
 */
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <getopt.h>
#include <poll.h>
#include <string.h>
#include <time.h>
#include <mraa.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>
#include <math.h>

#define B 4275
#define R0 100000.0
bool dum_flag = false;
int status = 1;
char scale = 'F';
int period = 1;
struct timeval cur_time; 
mraa_gpio_context button;
mraa_aio_context temp;
time_t run_time = 0; 
bool log_flag = false;
FILE *file_f = NULL;

void print_current_temp(){
	int temp_reading = mraa_aio_read(temp);
	float R = 1023.0/((float)temp_reading) - 1.0;
	R = R0*R;
	float final_temp;
	float convert = 1.0/(log(R/R0)/B + 1/298.15) - 273.15;
	if(scale == 'C'){
		final_temp = convert;
	}
	else if(scale == 'F'){
		final_temp = (convert * 9)/5 + 32; 
	}
	printf("%.1f\n", final_temp);
	if(log_flag){
		fprintf(file_f,"%.1f\n",final_temp);
	} 	
}

void print_current_time(){
	struct timespec time;
	struct tm *tm;
	clock_gettime(CLOCK_REALTIME, &time);
	tm = localtime(&(time.tv_sec));
	printf("%02d:%02d:%02d ",tm->tm_hour,tm->tm_min,tm->tm_sec);
	if(log_flag == true){
		fprintf(file_f,"%02d:%02d:%02d ",tm->tm_hour,tm->tm_min,tm->tm_sec);
	}
}

void process_shutdown(){
	print_current_time();
	printf("SHUTDOWN\n");
	if(log_flag){
		fprintf(file_f,"SHUTDOWN\n");
		fclose(file_f);
	}
	mraa_gpio_close(button);
	mraa_aio_close(temp);
	exit(0);
}

void process_input(char* command){
	if(strcmp(command,"SCALE=F") == 0){
		scale = 'F';
		if(log_flag){
			fprintf(file_f,"%s\n",command);
		}
	}
	else if(strcmp(command,"SCALE=C") == 0){
		scale = 'C';
		if(log_flag){
                        fprintf(file_f,"%s\n",command);
                }
	}
	else if(strstr(command,"PERIOD=") != NULL){
		int new_period = atoi(command+7);
		period = new_period;
                if(log_flag){
                        fprintf(file_f,"%s\n",command);
                }
	}
	else if(strcmp(command,"STOP") == 0){
		status = 0;
                if(log_flag){
                        fprintf(file_f,"%s\n",command);
                }
	}
	else if(strcmp(command,"START") == 0){
		status = 1;	
                if(log_flag){
                        fprintf(file_f,"%s\n",command);
                }
	}
	else if(strstr(command,"LOG") != NULL){
		if(log_flag){
			fprintf(file_f,"%s\n",command);
		}
	}
	else if(strcmp(command,"OFF") == 0){
		if(log_flag){
			fprintf(file_f,"%s\n",command);
		}
		process_shutdown();
	}
}

void read_input(){
	char input[256];
	char command[256];
	int num = read(STDIN_FILENO, &input, 256);
	if(num < 0){
		fprintf(stderr, "Error reading input from terminal \n");
	}
	int i;
	int k = 0;
	for (i = 0; i < num; i++) {
		if(input[i] == '\n'){
			command[k] = '\0';
			k = 0;
			process_input(command);
		} 
		else{
			command[k] = input[i];
			k++;
		}
	}
}

int main(int argc, char* argv[]) {
        struct option options[] = {
                {"period", required_argument, NULL, 'p'},
                {"scale", required_argument, NULL, 's'},
                {"log", required_argument, NULL, 'l'},
               	{"DUMMY", 0, NULL, 'd'},
		{0, 0, 0, 0}
        };
	int opt;
	char* file_name = NULL;
	/* Processes arguements */	
	while ((opt = getopt_long(argc, argv, "", options, NULL)) != -1) {
                switch (opt) {
                        case 'p':
                                period = atoi(optarg);
                                break;

                        case 's':
                                if(optarg[0] == 'F' || optarg[0] == 'C'){
                                	scale = optarg[0];
                                }
				else{
					fprintf(stderr, "Incorrect usage of --scale arguemnt\n");
					exit(1);
				}
                                break;

                        case 'l':
                        	log_flag = true;
				file_name = optarg;  
				break;
			
			case 'd':
				dum_flag = true;
				break;
                        
			default:
                                fprintf(stderr, "Incorrect usage of arguments.\n");
                                exit(1);
                                break;
                }
        }
	/* If specefied, opens log file to write to */
	if(log_flag){
		file_f = fopen(file_name, "w+");
		if(file_f == NULL){
			fprintf(stderr, "Error opening --log file\n");
			exit(1);
		}	
	}
	/* Initialize polling */
	struct pollfd fds;
	fds.fd = STDIN_FILENO;
	fds.events = POLLIN;
	/* Initialize sensors */
  	button = mraa_gpio_init(73);
	temp = mraa_aio_init(1);
	mraa_gpio_dir(button, MRAA_GPIO_IN);
	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &process_shutdown, NULL);
	while(1){
		gettimeofday(&cur_time, 0);
		if(status && (cur_time.tv_sec >= run_time)){
			print_current_time();
			print_current_temp();
			run_time = cur_time.tv_sec + period;
		}
		int ret = poll(&fds, 1, 0);
		if(ret >= 1){
			if(fds.revents & POLLIN){
				read_input();
			}
		}
		else if(ret < 0){
			fprintf(stderr, "There was en error with polling\n");
			exit(1);
		}
	}
	exit(0);
}
