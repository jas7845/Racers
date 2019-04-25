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
///
void initRacers( long milliseconds ){

	WAIT = milliseconds;
	if (pthread_mutex_init(&lock, NULL) != 0){
        	fprintf(stderr, "\n mutex init failed\n");
    	}
	clear();
}

/// makeRacer - Create a new racer.
///
/// @param name the string name to show on the display for this racer
/// @param position the row in which to race
/// @return Racer pointer a dynamically allocated Racer object
/// @pre strlen( name ) <= MAX_NAME_LEN, for display reasons.
///
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
///
/// @param racer the object to be de-allocated
///
void destroyRacer( Racer *racer ){
	free(racer->graphic);
	free(racer);
}

/// Run one racer in the race.
/// Initialize the display of the racer*:
///   The racer starts at the start position, column 0.
///   The racer's graphic (a string) is displayed.
///
/// These actions happen repetitively, until its position is at FINISH_LINE:
///
///  Calculate a random waiting period between 0 and
///    the initial delay value given to initRacers at the start of the race.
///  Sleep for that length of time.
///  If sleep value is less than or equal 3, racer gets flat and can't finish.
///    A flat tire is displayed by 'X' in the second character of the graphic.
///    Display the car with the flat tire and stop further racing.
///  Change the display position of this racer by +1 column this way:
///    Erase the racer's car and name from the display.
///    Update the racer's dist field by +1.
///    Display the racer's graphic (car and name) at the new position.
///
/// The intention is to execute this function many times simultaneously,
/// each in its own thread.
///
/// Note: Be sure to keep the update of the display by one racer "atomic".
///
/// @pre racer cannot be NULL.
/// @param racer Racer object declared as void* for pthread compatibility
/// @return void pointer to status. A NULL represents success.
///
void *run( void *racer ){
	
	Racer * r;
	r = (Racer *) racer;
	int row = r->row + 1;
	int col = 0;
	char name[MAX_CAR_LEN+1];
	strcpy(name, r->graphic);
	pthread_mutex_lock(&lock);
        	for(int i = 0; i< MAX_CAR_LEN; i++){
			set_cur_pos(row, col);
			put(r->graphic[i]);
			col++;
		}
        pthread_mutex_unlock(&lock);
	while(r->dist < FINISH_LINE){
		int waiting_time = rand() % (WAIT + 1);
		if(waiting_time <= 3){
			r->graphic[1] = 'X'; //car has a flat
        	        // display this and keep it on the screen
        	        // cancel the thread	
			pthread_mutex_lock(&lock);
			col = 0;
			for(int i = 0; i <FINISH_LINE; i++){ //clear the line
				set_cur_pos(row, col);
				put(' ');
				col++;
			}
			col = r->dist;
			for(int i = 0; i< MAX_CAR_LEN; i++){
				set_cur_pos(row, col);
				put(r->graphic[i]);
				col++;
			}
			pthread_mutex_unlock(&lock);
			pthread_exit(racer);
			break;
		}
		else{
			//usleep(waiting_time);		//something is wrong with this should be faster
			usleep(waiting_time * 1000);
		}
		r->dist = r->dist + 5; // should be + 1
		//update display
		pthread_mutex_lock(&lock);
                        set_cur_pos(row, 0);
			col = 0;
			for(int i = 0; i <FINISH_LINE+MAX_CAR_LEN; i++){ //clear the line
                                set_cur_pos(row, col);
				put(' ');
				col++;
                        }
			col = r->dist;	
                        for(int i = 0; i< MAX_CAR_LEN; i++){
                                set_cur_pos(row, col);
                                put(r->graphic[i]);
                                col++;
                        }
        	pthread_mutex_unlock(&lock);
	}
	//set_cur_pos(20, 20); //debug
	//printf("%s finished the race", name); //debug
	//exit(1);
	return NULL;
}
