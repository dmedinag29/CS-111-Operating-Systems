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
SortedList_t head;
SortedListElement_t* pool;
char** keys;
pthread_mutex_t mutex;
long lock = 0;
bool syn;

void* thread_worker(void* arg){
	/* Initializing locks if neccesary */
	if(sync_type == 'm'){
		pthread_mutex_lock(&mutex);
	}
	else if(sync_type == 's'){
		while(__sync_lock_test_and_set(&lock, 1));	
	}
	SortedListElement_t* elements = arg;
	/* Inserts pre-defined eelements into global list */
	for(int y = 0; y < iterations; y++){
		SortedList_insert(&head, (SortedListElement_t *) (elements+y));
	} 
	/* Get list length */
	if(SortedList_length(&head) == -1 ){
		fprintf(stderr, "Error obtaining list length: list may be corrupt \n");
		exit(2);
	}
	/* Look up and delete each of keys previously inserted */
	char *temp_key = malloc(sizeof(char)*256);
	for(int i = 0; i < iterations; i++){
		strcpy(temp_key, (elements+i)->key);
		SortedListElement_t* prev_inserted = SortedList_lookup(&head, temp_key);
		if(prev_inserted == NULL){
			fprintf(stderr, "Failure to find element \n");
			exit(2);
		}
		if(SortedList_delete(prev_inserted) == 1){
			fprintf(stderr, "Failure to delete element \n");
			exit(2);

		}
	}
	/* Releasing locks if neccessary */
	if(sync_type == 'm'){
		pthread_mutex_unlock(&mutex);
	}
	else if(sync_type == 's'){
		__sync_lock_release(&lock);	
	}
	return NULL;
}

void segHandler(int sig){
	fprintf(stderr, "A segmentation fault was caught: %d\n", sig);
	exit(2);
}

int main(int argc, char* argv[]){
	opt_yield = 0;
	char err[] = "Correct usage: --threads=#### --iterations=#### --yield=#### --sync=####";
	struct option args[] = {
                {"threads", 1, NULL, 't'},
                {"iterations", 1, NULL, 'i'},
                {"yield", 1, NULL, 'y'},
                {"sync", 1, NULL, 's'},
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
	/* Check the length of list is zero */
        if(SortedList_length(&head) != 0){
		fprintf(stderr, "The length of final list is not zero \n");
	}
	/* Processing and printing output */
        long operations = threads * iterations * 3;
        long runtime = 1000000000 * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
        long avgoptime = runtime / operations;
        char output[256] = "list";
	long list_num = 1; 
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
        fprintf(stdout, "%s,%d,%d,%ld,%ld,%ld,%ld\n", output, threads, iterations, list_num, operations, runtime, avgoptime);
	free(threadPool);
	/* Exit routine/clean-up */
	if(sync_type == 'm'){
		pthread_mutex_destroy(&mutex);
	}
	exit(0);
	
}
