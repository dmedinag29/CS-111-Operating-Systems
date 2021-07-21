/* Name: Daniel Medina Garate
 * ID: 204971333
 * Email: dmedinag@g.ucla.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include "SortedList.h"


int threads = 1;
char sync_type;
char yield_type;
int iterations = 1;
SortedList_t* head = NULL;
SortedListElement_t* pool;
char** keys;
pthread_mutex_t* mutex = NULL;
long *lock = NULL;
bool syn;
long long wait_time = 0;
unsigned int list_num;

unsigned int get_key(const char* key) {
	int tempp = *key;
	return tempp % list_num;
}

void* thread_worker(void* arg){
	long long thread_time = 0;
	struct timespec start;
	struct timespec end;
	SortedListElement_t* elements = arg;
	/* Inserts pre-defined eelements into global/partitioned list */
	for(int y = 0; y < iterations; y++){
		unsigned int sub_list = get_key((elements+y)->key);
//		printf("This is the value of sub_list: %d\n",sub_list);
		if(syn == true){
			clock_gettime(CLOCK_MONOTONIC, &start);
		}
		if(sync_type == 'm'){
                	pthread_mutex_lock(mutex + sub_list);
        	}
        	else if(sync_type == 's'){
                	while(__sync_lock_test_and_set(lock + sub_list, 1));
        	}
		if(syn == true){
			clock_gettime(CLOCK_MONOTONIC, &end);	
		}
		thread_time += 1000000000 * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
		SortedList_insert(head + sub_list, (SortedListElement_t *) (elements+y));
		if(sync_type == 'm'){
                	pthread_mutex_unlock(mutex + sub_list);
        	}
        	else if(sync_type == 's'){
                	__sync_lock_release(lock + sub_list);
        	}
	} 
//	printf("MADE IT PAST INSERTION PHASE GREAT JOB HUMA \n");
	/* Get list length */
	int list_length = 0;
	for(unsigned int i = 0; i < list_num; i++){
		if(syn == true){
			clock_gettime(CLOCK_MONOTONIC, &start);
		}
		if(sync_type == 'm'){
               		 pthread_mutex_lock(mutex + i);
        	}
        	else if(sync_type == 's'){
                	while(__sync_lock_test_and_set(lock + i, 1));
        	}
		if(syn == true){
			clock_gettime(CLOCK_MONOTONIC, &end);
		}
		thread_time += 1000000000 * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
		list_length += SortedList_length(head + i);
		if(list_length == -1 ){
				fprintf(stderr, "Error obtaining list length: list may be corrupt \n");
				exit(2);
			
		}
		if(sync_type == 'm'){
                	pthread_mutex_unlock(mutex + i);
        	}
        	else if(sync_type == 's'){
               		 __sync_lock_release(lock + i);
        	}
	}
//	printf("MADE IT PAST LOOK UP PHASE GREAT JOB HUMAN \n");
	/* Look up and delete each of keys previously inserted */
	char *temp_key = malloc(sizeof(char)*256);
	for(int i = 0; i < iterations; i++){
	       unsigned int sub_list = get_key((elements+i)->key);
		if(syn == true){
			clock_gettime(CLOCK_MONOTONIC, &start);
		}
		if(sync_type == 'm'){
                	pthread_mutex_lock(mutex + sub_list);
        	}
        	else if(sync_type == 's'){
                	while(__sync_lock_test_and_set(lock + sub_list, 1));
        	}
		if(syn == true){
                	clock_gettime(CLOCK_MONOTONIC, &end);
        	}
		thread_time += 1000000000 * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
		strcpy(temp_key, (elements+i)->key);
		SortedListElement_t* prev_inserted = SortedList_lookup(head + sub_list, temp_key);
		if(prev_inserted == NULL){
			fprintf(stderr, "Failure to find element \n");
			exit(2);
		}
		if(SortedList_delete(prev_inserted) == 1){
			fprintf(stderr, "Failure to delete element \n");
			exit(2);

		}
		if(sync_type == 'm'){
                	pthread_mutex_unlock(mutex + sub_list);
        	}
        	else if(sync_type == 's'){
                	__sync_lock_release(lock + sub_list);
        	}
	}
	/* Releasing locks if neccessary */
	return (void*) thread_time;
}

void segHandler(int sig){
	fprintf(stderr, "A segmentation fault was caught: %d\n", sig);
	exit(2);
}

int main(int argc, char* argv[]){
	list_num = 1;
	opt_yield = 0;
	char err[] = "Correct usage: --threads=#### --iterations=#### --yield=#### --sync=####";
	struct option args[] = {
                {"threads", 1, NULL, 't'},
                {"iterations", 1, NULL, 'i'},
                {"yield", 1, NULL, 'y'},
                {"sync", 1, NULL, 's'},
                {"lists", 1, NULL, 'l'},
		{0, 0, 0, 0}
        };
        int i = 0;
        while((i = getopt_long(argc, argv, "", args, NULL)) >= 0){
                switch(i){
                        case 't':
                                threads = atoi(optarg);
                                break;
                        case 'i':
                              //  iter = true;
                                iterations = atoi(optarg);
                                break;
                        case 'y':
                                for(size_t z = 0; z < strlen(optarg); z++){
					if(optarg[z] == 'l'){
						opt_yield |= LOOKUP_YIELD;
					}
					else if(optarg[z] == 'd'){
						opt_yield |= DELETE_YIELD;
					}
					else if(optarg[z] == 'i'){
						opt_yield |= INSERT_YIELD;
					}
				}
                                break;
                        case 's':
                                syn = true;
                                sync_type = optarg[0];
				break;
                        case 'l':
				list_num = atoi(optarg);
				break;
			case '?':
                                printf(err);
                                exit(1);
                                break;
                }
        }
	mutex = malloc(list_num * sizeof(pthread_mutex_t));
	lock = malloc(list_num * sizeof(long));
	head = malloc(list_num * sizeof(SortedList_t));
	for(unsigned int i = 0; i < list_num; i++){
		pthread_mutex_init(mutex + i, NULL);
		lock[i] = 0;
		head[i].next = NULL;
		head[i].prev = NULL;
	}
	signal(SIGSEGV, segHandler);
	pool = malloc(iterations * threads * sizeof(SortedListElement_t));
	if(pool == NULL){
		fprintf(stderr, "Failure to allocate memory: %s\n", strerror(errno));
		exit(1);
	}
	keys = (char**)malloc(sizeof(char) * 256);
	if(keys == NULL ){
		fprintf(stderr, "Failure to allocate memory: %s\n", strerror(errno));
		exit(1);
	}
	for(int i = 0; i < (iterations * threads); i++){
		keys[i] = (char*)malloc(sizeof(char)*256);
		if(keys[i] == NULL){
			fprintf(stderr, "Failure to allocate memory: %s\n", strerror(errno));
			exit(1);
		}	
		for(int j = 0; j < 256; j++){
			keys[i][j] = rand() % 94 + 33;
		}
		pool[i].key = keys[i];
	}
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
                if(pthread_create(&threadPool[k], NULL, &thread_worker, (void *) (pool + iterations * k)) != 0){
                        fprintf(stderr, "Failure to create thread: %s\n", strerror(errno));
                        exit(1);
                }
        }
        /* Join threads */
	void **time_per_thread = malloc(sizeof(void*));  
	for(int j = 0; j < threads; j++){
                if(pthread_join(threadPool[j], time_per_thread) != 0){
                        fprintf(stderr, "Failure to join thread: %s\n", strerror(errno));
                        exit(1);
                }
        	wait_time += (long long) *time_per_thread;
	}
        /* Stop timer */
        if(clock_gettime(CLOCK_MONOTONIC, &end) < 0){
                fprintf(stderr, "Failure to get end time: %s\n", strerror(errno));
                exit(1);
        }
	/* Check the length of list is zero */
        int list_length = 0;
	for(unsigned int i = 0; i < list_num; i++){
		list_length += SortedList_length(head + i);
	}
	if(list_length != 0){
		fprintf(stderr, "The length of final list is not zero \n");
		exit(2);
	}
	/* Processing and printing output */
        long long operations = threads * iterations * 3;
        long long runtime = 1000000000 * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
        long long avgoptime = runtime / operations;
        char output[256] = "list"; 
	long long wait_time_over_op = wait_time/operations;       
	switch(opt_yield){
		case 0:
			strcat(output, "-none");
			break;
		case 1:
			strcat(output, "-i");
			break;
		case 2: 
			strcat(output, "-d");
			break;
		case 3:
			strcat(output, "-id");
			break;
		case 4:
			strcat(output, "-l");
			break;
		case 5:
			strcat(output, "-il");
			break;
		case 6:
			strcat(output, "-dl");
			break;
		case 7:
			strcat(output, "-idl");
			break;
		default:
			break;
    	}
        if(syn == true){
                if(sync_type == 'm'){
                        strcat(output, "-m");
                }
                else if(sync_type == 's'){
                        strcat(output, "-s");
                }
        }
        else{
                strcat(output, "-none");
        }
        fprintf(stdout, "%s,%d,%d,%d,%lld,%lld,%lld,%lld\n", output, threads, iterations, list_num, operations, runtime, avgoptime, wait_time_over_op);
	free(mutex);
	free(lock);
	free(time_per_thread);
	free(threadPool);
	exit(0);
	
}
