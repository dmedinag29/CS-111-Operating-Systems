/* Name: Daniel Medina Garate
 * Email: dmedinag@g.ucla.edu
 * ID: 204971333
 */
#include "SortedList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>

int opt_yield = 0;

void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
	if(list == NULL || element == NULL){
		return;
	}

	if (list->next == NULL){
        	/* Critical section */
        	if(opt_yield & INSERT_YIELD){
          		sched_yield();
        	}
		list->next = element;
		element->prev = list;
		element->next = NULL;
        	return;
	}
	SortedListElement_t* temp = list->next;
	SortedListElement_t* prv = list;
   	while(temp != NULL && strcmp(element->key, temp->key) >= 0 ){
        	prv = temp;
        	temp = temp->next;
    	} 
    	if(opt_yield & INSERT_YIELD){
        	sched_yield();
	}
    	if(temp == NULL){
		prv->next = element;
		element->prev = prv;
		element->next = NULL;
   	}else{
		prv->next = element;
		temp->prev = element;
		element->next = temp;
		element->prev = prv;
    	}	
}

int SortedList_delete(SortedListElement_t *element){
	if(element == NULL){
		return 1;
	}
	/* Critical section */	
	if(opt_yield & DELETE_YIELD){
		sched_yield();
	}
	if(element->next != NULL){
		if(element->next->prev != element){
			return 1;
		} else{
			element->next->prev = element->prev;
		}
	}	
	if(element->prev != NULL){
		if(element->prev->next != element){
			return 1;
		} else{
			element->prev->next = element->next;
		}
	}
	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
	if(list == NULL){
		return NULL;
	}
	SortedListElement_t* temp = list->next;
	while(temp != list){
		if(opt_yield & LOOKUP_YIELD){
			sched_yield();
		}
		if(strcmp(temp->key, key) == 0){
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

int SortedList_length(SortedList_t *list){
	if(list == NULL){
		return -1;
	}
	SortedListElement_t* temp = list->next;
	int count = 0;
	while(temp != NULL){
		if(opt_yield & LOOKUP_YIELD){
			sched_yield();
		}
		count++;
		temp = temp->next;
	}
	return count;	
}
