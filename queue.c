#include "queue.h"
#include "elev.h"
#include "fsm.h"
#include <stdio.h>

//En matrise som representerer køen. Rader tilsvarer etasje og kolonner tilsvarer type knapp. Når er bestilling legges til i køen
//blir riktig posisjon i matrisen satt til 1. Når en bestilling slettes blir posisjonen satt til 0.
static int queue[4][3] = {{0}};


void queue_add_to_queue(elev_button_type_t button, int floor) {
	if (!((button == BUTTON_CALL_UP && floor == 3) || (button == BUTTON_CALL_DOWN && floor == 0))){
		queue[floor][button] = 1;
		elev_set_button_lamp(button, floor, 1);
	}
}

//Forsikre om at ikke funksjoner får inn knapper som ikke eksisterer.
int queue_not_a_real_button() {

	if(queue[0][1] == 1){
		return 1;
	}
	else if(queue[3][0] == 1) {
		return 1;
	}
	
	return 0;
}

//Sletter bestillinger fra kø og skrur av lys til knapper.
void queue_remove_from_queue(int floor, int dir) {

	if(dir == 1 && floor == 3 && queue_not_a_real_button() == 0) {
		queue[floor][1] = 0;
		queue[floor][2] = 0;
		elev_set_button_lamp(BUTTON_CALL_DOWN, floor, 0);
		elev_set_button_lamp(BUTTON_COMMAND, floor, 0);
	}
	else if(dir == -1 && floor == 0 && queue_not_a_real_button() == 0) {
		queue[floor][0] = 0;
		queue[floor][2] = 0;
		elev_set_button_lamp(BUTTON_CALL_UP, floor, 0);
		elev_set_button_lamp(BUTTON_COMMAND, floor, 0);
	}
	else if(!((dir == 1 && floor == 3) || (dir == -1 && floor == 0))) {
		queue[floor][2] = 0;
		elev_set_button_lamp(BUTTON_COMMAND, floor, 0);

		if(dir == 1 && queue_is_order_below(floor, dir) == 0) {
			queue[floor][0] = 0;
			queue[floor][1] = 0;
			elev_set_button_lamp(BUTTON_CALL_UP, floor, 0);
			elev_set_button_lamp(BUTTON_CALL_DOWN, floor, 0);
		}
		else if(dir == -1 && queue_is_order_below(floor, dir) == 0) {
			queue[floor][0] = 0;
			queue[floor][1] = 0;
			elev_set_button_lamp(BUTTON_CALL_UP, floor, 0);
			elev_set_button_lamp(BUTTON_CALL_DOWN, floor, 0);
		}
		else if(dir == 1 && queue_is_order_below(floor, dir) == 1) {
			queue[floor][0] = 0;
			elev_set_button_lamp(BUTTON_CALL_UP, floor, 0);
		}
		else if(dir == -1 && queue_is_order_below(floor, dir) == 1) {
			queue[floor][1] = 0;
			elev_set_button_lamp(BUTTON_CALL_DOWN, floor, 0);
		}
	}

}



void queue_remove_all_orders() {

	for(int floor = 0; floor < 4; floor++) {
		for(int button = 0; button < 3; button++) {
			queue[floor][button] = 0;
		}
	}

	for(int floor = 0; floor < 4; floor++) {
		for(elev_button_type_t button = 0; button < 3; button++) {
			if(!((floor == 0 && button == 1) || (floor == 3 && button == 0))) {
				elev_set_button_lamp(button, floor, 0);
			}
		}
	}
}


int queue_is_order(elev_button_type_t button, int floor) {

	if(queue[floor][button] == 1) {
		return 1;
	}

	return 0;
}

//Avgjør om heisen skal stopp ved en etasje eller ikke.
int queue_elev_stop(int floor, int dir){

	if(queue_is_order_below(floor, dir) == 0){
		return 1;
	}

	if ((queue_is_order(BUTTON_CALL_UP, 3) == 0) && (queue_is_order(BUTTON_CALL_UP, floor) == 1) && dir == 1){
		return 1;
	} 
	else if ((queue_is_order(BUTTON_CALL_DOWN, 0) == 0) && (queue_is_order(BUTTON_CALL_DOWN, floor) == 1) && dir == -1){
		return 1;
	}
	else if (queue_is_order(BUTTON_COMMAND, floor) == 1){
		return 1;
	}

	return 0;
}


int queue_is_empty(){

	for(int floor = 0; floor < 4; floor++) {
		for(int button = 0; button < 3; button++) {
			if(queue[floor][button] == 1) {
				return 0;
			}
		}
	}
	return 1;
}	

void queue_print (){

	for(int floor = 0; floor < 4; floor++) {
		for(int button = 0; button < 3; button++) {
			printf("%d", queue[floor][button]);
		}
		printf("\n");
	}
}

//Avgjør hvilken vei heisen skal kjøre etter en bestilling er betjent.
int queue_which_direction(int dir, int floor) {



	if(dir == 1 && floor != 3){
		for(int level = 0; level < 4; level++){
			for(elev_button_type_t button = 0; button < 3; button++) {
				if((queue_is_order(button, level) == 1) && level > floor) {
					return 1;
				}
				else if ((queue_is_order(button, level) == 1) && level < floor) {
					return -1;
				}
			}
		}
	}
	
	else if(dir == -1 && floor != 0) {
		

		for(int level = 0; level < 4; level++){
			for(elev_button_type_t button = 0; button < 3; button++) {
				if((queue_is_order(button, level) == 1) && level < floor) {


					return -1;
				}
				else if((queue_is_order(button, level) == 1) && level > floor) {
					printf("34567\n");
					return 1;
				}
			}
		}
	}
	
	else if(dir == 1 && floor == 3) {
		return -1;
	}

	else if(dir == -1 && floor == 0) {
		return 1;
	}

	return 0;
}

//Sjekker om det er noen bestillinger over etasjen hvis dir = opp eller om det er bestillinger under etasjen hvis dir = ned.
int queue_is_order_below(int floor, int dir){

	if(dir == 1) {
		for(int level = floor + 1; level < 4; level++){
			for(int button = 0; button < 3; button++){
				if(queue[level][button] == 1){
					return 1;
				}
			}
		}
	}
	else if(dir == -1) {
		for(int level = 0; level < floor; level++){
			for(int button = 0; button < 3; button++){
				if(queue[level][button] == 1){
					return 1;
				}
			}
		}
	}
	return 0;
}




