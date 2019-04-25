/////////////////////////////////////////////////////////////////////
/// File to process the command line and create the threads
/// @author: Julie Sojkowski jas7845@g.rit.edu
/// date: April 25, 2019
/////////////////////////////////////////////////////////////////////

#define _DEFAULT_SOURCE
#include "racer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static long WAIT = 200;
static int NUM_RACERS;

int main( int argc, char *argv[]){

	int thread_return_val;
	void *retval;
	int i = strtol(argv[1], NULL, 10);
	int start_names = 1;        
	if(i > 0){
                WAIT = i;
       		start_names = 2;
	}
	
	
	NUM_RACERS = argc-start_names;
	if(NUM_RACERS < 2){
		fprintf(stderr, "Usage: pt-cruisers [max-speed-delay] name1 name2 [name3...]\n");
		return EXIT_FAILURE;
	}
	Racer * racers[NUM_RACERS];
	i = 0;
	while(start_names <argc){
		// make the structures calling 	
		if(strlen(argv[start_names]) <= MAX_NAME_LEN){
			racers[i] = makeRacer(argv[start_names], i);
		}
		else{
			fprintf(stderr, "Error: racer names must not exceed length 6.\n");
	       		return EXIT_FAILURE;
		}	
		i++;
		start_names++;
	}
	initRacers(WAIT);
	pthread_t threads[NUM_RACERS];
	for(int i = 0; i < NUM_RACERS; i ++){

		thread_return_val = pthread_create( &threads[i], NULL, run, (void *)racers[i] );

		// if the creation failed, report it
		if (thread_return_val){
		   printf( "ERROR; pthread_create() returned %d\n", thread_return_val);
		   exit(-1);
		}
		//printf("dist %s: %i", racers[i]->graphic, racers[i]->dist);
	}
	
	
	//printf("argv[1]: %s, int: %i, name: %s\n", argv[1], WAIT, racers[0]->graphic); 
	for(int j = 0; j < NUM_RACERS; j++){
		pthread_join( threads[j], &retval );
		
		//printf( "Thread #%d returned %lu.\n", j, (unsigned long) retval );
                destroyRacer(racers[j]);
        }


	printf("\n\n\n");	
	
	return 0;
}
