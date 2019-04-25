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

static long WAIT = DEFAULT_WAIT;
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
	if(NUM_RACERS < 2){	//  check that there are at least two racers
		fprintf(stderr, "Usage: pt-cruisers [max-speed-delay] name1 name2 [name3...]\n");
		return EXIT_FAILURE;
	}
	Racer * racers[NUM_RACERS];
	i = 0;
	while(start_names <argc){
		//  make the racer structures if the nam eis less than max len
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
		//  create the threads
		thread_return_val = pthread_create( &threads[i], NULL, run, (void *)racers[i] );

		//  if the creation failed, report it
		if (thread_return_val){
		   printf( "ERROR; pthread_create() returned %d\n", thread_return_val);
		   exit(-1);
		}
	}
	
	for(int j = 0; j < NUM_RACERS; j++){
		//  join the threads when they are done with the run method
		pthread_join( threads[j], &retval );
                destroyRacer(racers[j]);
        }
	printf("\n\n\n");	
	
	return EXIT_SUCCESS;
}
