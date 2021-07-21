#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

int threads;
int iterations;
long long counter;
pthread_mutex_t mutex;
long lock = 0;
char sync_type;
bool opt_yield;
bool syn;

/* Default add, no protection */
void add(long long *pointer, long long value){
	long long sum = *pointer + value;
	if (opt_yield)
		sched_yield();
	*pointer = sum;
}

void spinlock_add(long long *pointer, long long value){
	while(__sync_lock_test_and_set(&lock,1));
	add(pointer, value);
	__sync_lock_release(&lock);
}

void mutex_add(long long *pointer, long long value){
	pthread_mutex_lock(&mutex);
	add(pointer, value);
	pthread_mutex_unlock(&mutex);
}

void cmpnswp_add(long long *pointer, long long value){
	long long prev, sum;
	do{ 
		prev = *pointer;
		if(opt_yield)
			sched_yield();
		sum = prev + value;	
	
	}while(__sync_val_compare_and_swap(pointer, prev, sum) != prev);
}
/* Work each thread will do */
void* thread_worker(void *arg){
	for(int i = 0; i < iterations; i++){	
		if(syn == true){
			if(sync_type == 'm'){
				mutex_add(&counter, 1);
				mutex_add(&counter, -1);
			}
			else if(sync_type == 's'){
				spinlock_add(&counter, 1);
				spinlock_add(&counter, -1);	
			}
			else if(sync_type == 'c'){
				cmpnswp_add(&counter, 1);
				cmpnswp_add(&counter, -1);
			}
		}
		else{
			add(&counter, 1);
			add(&counter, -1);
		}
	}
	return arg;
}

int main (int argc, char* argv[]){
	threads = 1;
	iterations = 1;
	counter = 0;
	opt_yield = false;
	syn = false;
	char err[] = "Correct usage: --threads=#### --iterations=#### --yield=#### --sync=####";
	struct option args[] = {
                {"threads", 1, NULL, 't'},
		{"iterations", 1, NULL, 'i'},
		{"yield", 0, NULL, 'y'},
		{"sync", 1, NULL, 's'},
                {0, 0, 0, 0}
        };	
	/* Processing arguemnts */
	int i = 0;
	while((i = getopt_long(argc, argv, "", args, NULL)) >= 0){
        	switch(i){
                	case 't':
                                threads = atoi(optarg);
				break;
                        case 'i':
				//iter = true;
				iterations = atoi(optarg);
				break;
			case 'y':
				opt_yield = true;
				break;
			case 's':
				syn = true;
				sync_type = optarg[0];
				if(sync_type == 'm'){
					if(pthread_mutex_init(&mutex, NULL) != 0){
                				fprintf(stderr, "Failure to intiate mutex: %s\n", strerror(errno));
                				exit(1);
        				}
				}
				break;
			case '?':
                                printf(err);
                                exit(1);
                                break;
                }
        }
	/* Create/start timer */
	struct timespec start;
 	struct timespec end;
	if(clock_gettime(CLOCK_MONOTONIC, &start) < 0){
		fprintf(stderr, "Failure to get start time: %s\n", strerror(errno));
		exit(1);
	}
	pthread_t* threadPool = malloc(threads * sizeof(pthread_t));
	if(threadPool == NULL){
		fprintf(stderr, "Failure to allocate memory for thread pool: %s\n", strerror(errno));
		exit(1);
	}
	/* Create threads */
	for(int k = 0; k < threads; k++){
		if(pthread_create(&threadPool[k], NULL, &thread_worker, NULL) != 0){
			fprintf(stderr, "Failure to create thread: %s\n", strerror(errno));
			exit(1);
		} 
	}	
	/* Join threads */
	for(int j = 0; j < threads; j++){
		if(pthread_join(threadPool[j], NULL) != 0){
			fprintf(stderr, "Failure to join thread: %s\n", strerror(errno));
			exit(1);
		}
	}
	/* Stop timer */
	if(clock_gettime(CLOCK_MONOTONIC, &end) < 0){
		fprintf(stderr, "Failure to get end time: %s\n", strerror(errno));
		exit(1);
	}
	
	/* Processing and printing output */
	long operations = threads * iterations * 2;
	long runtime = 1000000000 * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
	long avgoptime = runtime / operations;
	char output[256] = "add";
	if(opt_yield == true){
		strcat(output, "-yield");
	}
	if(syn == true){
		if(sync_type == 'm'){
			strcat(output, "-m");
                }
                else if(sync_type == 's'){
			strcat(output, "-s");
		}
		else if(sync_type == 'c'){
			strcat(output, "-c");	
		}
	}
	else{
		strcat(output, "-none");
	}
	fprintf(stdout, "%s,%d,%d,%ld,%ld,%ld,%lld\n", output, threads, iterations, operations, runtime, avgoptime, counter);
	/* Exit/clean-up */
	free(threadPool);
	if(sync_type == 'm'){
		pthread_mutex_destroy(&mutex);
	}
	exit(0);
}
