/////////////////////////////////////////////////////////////////////
/// File to implement the methods in the racer.h file
/// @author: Julie Sojkowski jas7845@g.rit.edu
/// date: April 25, 2019
/////////////////////////////////////////////////////////////////////

#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "racer.h"
#include "display.h"

long WAIT;
pthread_mutex_t lock;

/// initRacers - Do setup work for all racers at the start of the program.
/// @param milliseconds length of pause between steps in animation
void initRacers( long milliseconds ){

	WAIT = milliseconds;
	if (pthread_mutex_init(&lock, NULL) != 0){
        	fprintf(stderr, "\n mutex init failed\n");
    	}
	clear();
}

/// makeRacer - Create a new racer.
/// @param name the string name to show on the display for this racer
/// @param position the row in which to race
/// @return Racer pointer a dynamically allocated Racer object
Racer *makeRacer( char *name, int position ){
	
	Racer * r = malloc(sizeof(Racer));
	r->row = position;
	char g[25];
        strcpy(g,"~O=");
	int j = strlen(g);
	for ( unsigned int i = 0; i<= MAX_NAME_LEN; i++){
		if(i < strlen(name)){
			g[j] = name[i];
		}
		else{
			g[j] =  '-';
		}
		j++;
	}
	g[j] = 'o';
        g[j+1] = '>';
	g[j+2] = '\0';
	
	r->graphic = strdup(g);
	r->dist = 0;
	return r;
	
}

/// destroyRacer - Destroy all dynamically allocated storage for a racer.
/// @param racer the object to be de-allocated
void destroyRacer( Racer *racer ){
	free(racer->graphic);
	free(racer);
}

/// run - Run the race for a thread
/// @param racer Racer object declared as void* for pthread compatibility
/// @return void pointer to status. A NULL represents success.
void *run( void *racer ){
	
	Racer * r;
	r = (Racer *) racer;
	srand(time(NULL)); 
	int row = r->row + 1;
	int col = 0;
	pthread_mutex_lock(&lock);
        	for(int i = 0; i< MAX_CAR_LEN; i++){
			set_cur_pos(row, col);
			put(r->graphic[i]);
			col++;
		}
        pthread_mutex_unlock(&lock);
	while(r->dist < FINISH_LINE){
		int waiting_time = rand() % (WAIT + 1);
		if(waiting_time <= 3){ 				//  car has a flat
			r->graphic[1] = 'X'; 
        	        //  display flat tire graphic and keep it on the screen
			pthread_mutex_lock(&lock);
			col = 0;
			for(int i = 0; i <FINISH_LINE+MAX_CAR_LEN; i++){  	// clear the line
				set_cur_pos(row, col);
				put(' ');
				col++;
			}
			col = r->dist;
			for(int i = 0; i< MAX_CAR_LEN; i++){	// display flat tire
				set_cur_pos(row, col);
				put(r->graphic[i]);
				col++;
			}
			pthread_mutex_unlock(&lock);
			//  cancel the thread
			pthread_exit(racer);
			break;
		}
		else{
			usleep(waiting_time * 1000);
		}
		r->dist = r->dist + 1;
		//  update display
		pthread_mutex_lock(&lock);
                        set_cur_pos(row, 0);
			col = 0;
			for(int i = 0; i <FINISH_LINE+MAX_CAR_LEN; i++){ 	//  clear the line
                                set_cur_pos(row, col);
				put(' ');
				col++;
                        }
			col = r->dist;	
                        for(int i = 0; i< MAX_CAR_LEN; i++){			//  display car
                                set_cur_pos(row, col);
                                put(r->graphic[i]);
                                col++;
                        }
        	pthread_mutex_unlock(&lock);
	}
	//  racer has finished
	return NULL;
}
