// Name: Shirley Lin
// Last Updated: 4/3/26
// Assignment: Homework 6

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Global variables
typedef struct waitinglist {
	int vehicle_id;
	int vehicle_type;
	int direction;
	struct  waitinglist *next;
} waitingvehiclelist;

typedef struct movinglist {
	int vehicle_id;
	int vehicle_type;
	int direction;
	struct  movinglist *next;
} movingvehiclelist;

typedef struct pmstr {
	int vehicle_id;
	int vehicle_type;
	int direction;
} pmstr_t;

struct waitinglist *pw; // Pointer to the waiting list
struct movinglist *pm;  // Pointer to the moving list

int waitingcarsouth, waitingcarnorth, waitingtrucksouth, waitingtrucknorth;
int movingcar, movingtruck;
int currentmovingdir, previousmovingdir; // 0: north, 1: south
pthread_mutex_t lock;
pthread_cond_t TruckNorthMovable, TruckSouthMovable, CarNorthMovable, CarSouthMovable;


void *vehicle_routine(pmstr_t *pmstrpara); // vehicle_type: 0 for truck, 1 for car
                                           // direction: 0 for north, 1 for south
void vehicle_arrival(pmstr_t *pmstrpara);
void waitinglistinsert(int vehicle_id,int vehicle_type, int direction);
void waitinglistdelete(int vehicle_id);
void movinglistinsert(int vehicle_id, int vehicle_type, int direction);
void movinglistdelete(int vehicle_id);

void printmoving();
void printwaiting();
void signal_vehicle();
int car_can_cross(int direction);
int truck_can_cross(int direction);

int main(void) {
	int option;
	int i, j;
	float carprob;
	int vehicle_type, direction, vehicle_id;

	pthread_t vehicle[30];
	pmstr_t *pmthread;

	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&TruckNorthMovable, NULL);
	pthread_cond_init(&TruckSouthMovable, NULL);
	pthread_cond_init(&CarNorthMovable, NULL);
	pthread_cond_init(&CarSouthMovable, NULL);

	waitingcarnorth = 0;
	waitingcarsouth = 0;
	waitingtrucknorth = 0;
	waitingtrucksouth = 0;
	movingcar = 0;
	movingtruck = 0;
	option = -1;
	pw = NULL; // Waiting list
	pm = NULL; // Moving list

	fprintf(stderr,"***************************************************************\n");
	fprintf(stderr,"Please select one Schedules from the following six options:\n");
	fprintf(stderr,"1. 10 : DELAY(10) : 10\n");
	fprintf(stderr,"   car/truck probability: [1.0, 0.0]\n");
	fprintf(stderr,"2. 10 : DELAY(10) : 10\n");
	fprintf(stderr,"   car/truck probability: [0.0, 1.0]\n");
	fprintf(stderr,"3. 20\n");
	fprintf(stderr,"   car/truck probability: [0.65, 0.35]\n");
	fprintf(stderr,"4. 10 : DELAY(25) : 10 : DELAY(25) : 10\n");
	fprintf(stderr,"   car/truck probability: [0.5, 0.5]\n");
	fprintf(stderr,"5. 10 : DELAY(3) : 10 : DELAY(10): 10\n");
	fprintf(stderr,"   car/truck probability: [0.65, 0.35]\n");
	fprintf(stderr,"6. 20 : DELAY(15) : 10\n");
	fprintf(stderr,"   car/truck probability: [0.75, 0.25]\n");

	do {
		fprintf(stderr,"\nPlease select [1-6]:");
		scanf("%d", &option);
	} while((option < 0) || (option > 6));

	fprintf(stderr,"***************************************************************\n");

	switch(option) {
		case 1: carprob = 1;    break;
		case 2: carprob = 0;    break;
		case 3: carprob = 0.65; break;
		case 4: carprob = 0.5;  break;
		case 5: carprob = 0.65; break;
		case 6: carprob = 0.75; break;
		default: carprob = 0.5;
	}

	srand((unsigned int)time((time_t *)NULL));

	// 20 vehicles - 10 : DELAY(10) : 10
	if(option == 1) {
        pthread_mutex_lock(&lock);

		for(j = 0; j <= 9; j++) {
			// Initialize properties of vehicle
			vehicle_type = 1;
			direction = rand() % 2;
			vehicle_id = j;

            // Generate pmstr_t struct to save the vehicle type, direction, and id
			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

            // Call vehicle_arrival()
			vehicle_arrival(pmthread);
            // Create a pthread to represent the vehicle, vehicle_routine() is the start function of a pthread
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
	    sleep(10);
		pthread_mutex_lock(&lock);

		for(j = 10; j <= 19; j++) {
			vehicle_type = 1;
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		for(j = 0; j <= 19; j++)
			pthread_join(vehicle[j], NULL);

	} // End of option 1

	// 20 vehicles - 10 : DELAY(10) : 10
	else if(option == 2) {
		pthread_mutex_lock(&lock);

		for(j = 0; j <= 9; j++) {
			vehicle_type = 0;
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
	    sleep(10);
		pthread_mutex_lock(&lock);

		for(j = 10; j <= 19; j++) {
			vehicle_type = 0;
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		for(j = 0; j <= 19; j++)
			pthread_join(vehicle[j], NULL);
	} // End of option 2

	// 20 vehicles
	else if(option == 3) {
        pthread_mutex_lock(&lock);

		for(j = 0; j <= 19; j++) {
			vehicle_type = (rand() % 100) < (carprob * 100);
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		for(j = 0; j <= 19; j++)
			pthread_join(vehicle[j], NULL);
	} // End of option 3

	// 30 vehicles - 10 : DELAY(25) : 10 : DELAY(25) : 10
	else if(option == 4) {
		pthread_mutex_lock(&lock);

		for(j = 0; j <= 9; j++) {
			vehicle_type = (rand() % 100) < (carprob * 100);
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		sleep(25);
		pthread_mutex_lock(&lock);

		for(j = 10; j <= 19; j++) {
			vehicle_type = (rand() % 100) < (carprob * 100);
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		sleep(25);
		pthread_mutex_lock(&lock);

		for(j = 20; j <= 29; j++) {
			vehicle_type = (rand() % 100) < (carprob * 100);
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		for(j = 0; j <= 29; j++)
			pthread_join(vehicle[j], NULL);
	} // End of option 4

	// 30 vehicles - 10 : DELAY(3) : 10 : DELAY(10) : 10
	else if(option == 5) {
        pthread_mutex_lock(&lock);

		for(j = 0; j <= 9; j++) {
			vehicle_type = (rand() % 100) < (carprob * 100);
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		sleep(3);
		pthread_mutex_lock(&lock);

		for(j = 10; j <= 19; j++) {
			vehicle_type = (rand() % 100) < (carprob * 100);
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		sleep(10);
		pthread_mutex_lock(&lock);

		for(j = 20; j <= 29; j++) {
			vehicle_type = (rand() % 100) < (carprob * 100);
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		for(j = 0; j <= 29; j++)
			pthread_join(vehicle[j], NULL);
	} // End of option 5

	// 30 vehicles - 20 : DELAY(15) : 10
	else {
    	pthread_mutex_lock(&lock);

		for(j = 0; j <= 19; j++) {
			vehicle_type = (rand() % 100) < (carprob * 100);
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		sleep(15);
		pthread_mutex_lock(&lock);

		for(j = 20; j <= 29; j++) {
			vehicle_type = (rand() % 100) < (carprob * 100);
			direction = rand() % 2;
			vehicle_id = j;

			pmthread = (pmstr_t *)malloc(sizeof(pmstr_t));
			pmthread->vehicle_id = vehicle_id;
			pmthread->vehicle_type = vehicle_type;
			pmthread->direction = direction;

			vehicle_arrival(pmthread);
			pthread_create(&vehicle[j], NULL, (void *(*)(void *)) vehicle_routine, (void *) pmthread);
		}

		pthread_mutex_unlock(&lock);
		for(j = 0; j <= 29; j++)
			pthread_join(vehicle[j], NULL);
	} // End of option 6

	fprintf(stderr,"\nFinished execution.\n");
} // End of main function


void *vehicle_routine(pmstr_t *pmstrpara) {
	if(pmstrpara->vehicle_type) {
		// Car arrives and attempts to cross
		pthread_mutex_lock(&lock);
		while(!car_can_cross(pmstrpara->direction)) {
			if(!pmstrpara->direction)
				pthread_cond_wait(&CarNorthMovable, &lock);
			else
				pthread_cond_wait(&CarSouthMovable, &lock);
		}

		// Car begins crossing
		// Update global variables
		waitinglistdelete(pmstrpara->vehicle_id);
		if(!pmstrpara->direction)
			waitingcarnorth--;
		else
			waitingcarsouth--;
		movinglistinsert(pmstrpara->vehicle_id, pmstrpara->vehicle_type, pmstrpara->direction);
		movingcar++;
		currentmovingdir = pmstrpara->direction;

		// Print out proper message
		fprintf(stderr, "\nCar #%d is now crossing the bridge. \n", pmstrpara->vehicle_id);
		printmoving();
		printwaiting();

		// Signal cars moving in the same direction if there is still room on the bridge
		// Makes sure bridge is completely filled
		if(movingcar < 3) {
			if(!pmstrpara->direction && waitingcarnorth > 0)
				pthread_cond_signal(&CarNorthMovable);
			else if(pmstrpara->direction && waitingcarsouth > 0)
				pthread_cond_signal(&CarSouthMovable);
		}

		pthread_mutex_unlock(&lock);
		sleep(2);
		pthread_mutex_lock(&lock);

		// Car leaves
		// Update global variables
		movingcar--;
		movinglistdelete(pmstrpara->vehicle_id);

		// Send out signals to wake up vehicle(s) accordingly
    	fprintf(stderr,"\nCar #%d exited the bridge.\n", pmstrpara->vehicle_id);
		printmoving();
		printwaiting();

		signal_vehicle();
		pthread_mutex_unlock(&lock);
	}
	else {
		// Truck arrives and attemps to cross
		pthread_mutex_lock(&lock);
		while(!truck_can_cross(pmstrpara->direction)) {
			if(!pmstrpara->direction)
				pthread_cond_wait(&TruckNorthMovable, &lock);
			else
				pthread_cond_wait(&TruckSouthMovable, &lock);
		}

		// Truck begins crossing
		// Update global variables
		waitinglistdelete(pmstrpara->vehicle_id);
		if(!pmstrpara->direction)
			waitingtrucknorth--;
		else
			waitingtrucksouth--;
		movinglistinsert(pmstrpara->vehicle_id, pmstrpara->vehicle_type, pmstrpara->direction);
		movingtruck++;
		currentmovingdir = pmstrpara->direction;
		previousmovingdir = currentmovingdir;

		// Print out proper message
		fprintf(stderr, "\nTruck #%d is now crossing the bridge.\n", pmstrpara->vehicle_id);
		printmoving();
		printwaiting();

		pthread_mutex_unlock(&lock);
		sleep(2);
		pthread_mutex_lock(&lock);
		
		// Truck leaves
		// Update global variables
		movingtruck--;
		movinglistdelete(pmstrpara->vehicle_id);

		// Send out signals to wake up vehicle(s) accordingly
		signal_vehicle();
		fprintf(stderr,"\nTruck #%d exited the bridge.\n", pmstrpara->vehicle_id);

		pthread_mutex_unlock(&lock);
	}
	
	free(pmstrpara);
} // End of thread routine

int car_can_cross(int direction) {
	if(waitingtrucksouth > 0 || waitingtrucknorth > 0) // There is a truck waiting
		return 0;
	if(movingtruck > 0) // There is a truck on the bridge
		return 0;
	if(movingcar >= 3) // There are 3 cars on the bridge
		return 0;
	if(currentmovingdir != direction && movingcar > 0) // Traffic is flowing in the opposite direction, and there are still cars on the bridge
		return 0;
	return 1;
}

int truck_can_cross(int direction) {
	if(movingtruck > 0 || movingcar > 0) // There are vehicles still on the bridge
		return 0;
	if(waitingtrucknorth > 0 && waitingtrucksouth > 0) { // There are trucks waiting on either side
		// Do not let truck cross if the last truck came from the same direction
		if(direction == previousmovingdir)
			return 0;
	}
	return 1;
}

void signal_vehicle() {
	// Any waiting trucks have priority
	if(waitingtrucknorth > 0 || waitingtrucksouth > 0) {
		// Wait until the bridge has completely cleared
		if(movingcar <= 0 && movingtruck <= 0) {
			// Alternate directions when both sides have waiting trucks
			// Otherwise signal whichever side has a waiting truck
			if(waitingtrucknorth > 0 && waitingtrucksouth > 0) {
				if(!previousmovingdir)
					pthread_cond_signal(&TruckSouthMovable);
				else
					pthread_cond_signal(&TruckNorthMovable);
			} else if(waitingtrucknorth > 0)
				pthread_cond_signal(&TruckNorthMovable);
			else
				pthread_cond_signal(&TruckSouthMovable);
		}
	} else { // Any waiting cars have next priority
		// Prioritize cars going in the same direction
		if(!currentmovingdir && waitingcarnorth > 0 && movingcar < 3)
			pthread_cond_signal(&CarNorthMovable);
		else if(currentmovingdir && waitingcarsouth > 0 && movingcar < 3)
			pthread_cond_signal(&CarSouthMovable);
		else if(movingcar <= 0) { // Signal car from either direction when no cars are on the bridge
			if(waitingcarnorth > 0)
				pthread_cond_signal(&CarNorthMovable);
			else if(waitingcarsouth > 0)
				pthread_cond_signal(&CarSouthMovable);
		}
	}
}


void vehicle_arrival(pmstr_t *pmstrpara) {
	if(pmstrpara->vehicle_type) {
		if(pmstrpara->direction)
			waitingcarsouth++;
		else
			waitingcarnorth++;
	} else {
		if(pmstrpara->direction)
			waitingtrucksouth++;
		else
			waitingtrucknorth++;
	}
	waitinglistinsert(pmstrpara->vehicle_id,pmstrpara->vehicle_type,pmstrpara->direction);

	if(pmstrpara->vehicle_type) {
		if (pmstrpara->direction)
			fprintf(stderr,"\nCar #%d (southbound) arrived.\n",pmstrpara->vehicle_id);
		else
			fprintf(stderr,"\nCar #%d (northbound) arrived.\n",pmstrpara->vehicle_id);
	} else {
		if (pmstrpara->direction)
			fprintf(stderr,"\nTruck #%d (southbound) arrived.\n",pmstrpara->vehicle_id);
		else
			fprintf(stderr,"\nTruck #%d (northbound) arrived.\n",pmstrpara->vehicle_id);
	}
} // End of vehicle_arrival


void waitinglistinsert(int vehicle_id,int vehicle_type, int direction) {
	struct waitinglist *p;
	p = (struct waitinglist *)malloc(sizeof(struct waitinglist));
	p->vehicle_id = vehicle_id;
	p->vehicle_type = vehicle_type;
	p->direction = direction;
	p->next = pw;
	pw = p;
}


void waitinglistdelete(int vehicle_id) {
	struct waitinglist *p, *pprevious;
	p = pw;
	pprevious = p;
	while(p) {
		if((p->vehicle_id) == (vehicle_id))
			break;
		else {
			pprevious = p;
			p = p->next;
		}
	}

	if(p == pw)
		pw = p->next;
	else
		pprevious->next = p->next;

	free(p);
}


void movinglistinsert(int vehicle_id, int vehicle_type, int direction) {
	struct movinglist *p;
	p = (struct movinglist *)malloc(sizeof(struct movinglist));
	p->vehicle_id = vehicle_id;
	p->vehicle_type = vehicle_type;
	p->direction = direction;
	p->next = pm;
	pm = p;
}


void movinglistdelete(int vehicle_id) {
	struct movinglist *p, *pprevious;
	p = pm;
	pprevious = p;

	while(p) {
		if((p->vehicle_id) == (vehicle_id))
			break;
		else {
			pprevious = p;
			p = p->next;
		}
	}

	if(p == pm)
		pm = p->next;
	else
		pprevious->next=p->next;

	free(p);
}


void printmoving() {
	struct movinglist *p;
	p = pm;
	fprintf(stderr,"Vehicles on the bridge: [");

	while(p) {
		if(p->vehicle_type)
			fprintf(stderr,"Car #%d,",p->vehicle_id);
		else
			fprintf(stderr,"Truck #%d,",p->vehicle_id);
		p = p->next;
	}

	fprintf(stderr,"]\n");
	fprintf(stderr,"Now %d cars are moving.\n", movingcar);
	fprintf(stderr,"Now %d trucks are moving.\n", movingtruck);
	// fprintf(stderr,"Current moving direction: %d.\n", currentmovingdir);
}


void printwaiting() {
	struct waitinglist *p;
	p = pw;
	fprintf(stderr,"Waiting Vehicles (northbound): [");

	while(p) {
		if(p->direction == 0)
		{
			if (p->vehicle_type)
		 		fprintf(stderr,"Car #%d,",p->vehicle_id);
			else
				fprintf(stderr,"Truck #%d,",p->vehicle_id);
		}
		p = p->next;
	}

	fprintf(stderr,"]\n");

	p = pw;
	fprintf(stderr,"Waiting Vehicles (Southbound): [");

	while(p) {
		if(p->direction)
		{
			if(p->vehicle_type)
		 		fprintf(stderr,"Car #%d,",p->vehicle_id);
			else
				fprintf(stderr,"Truck #%d,",p->vehicle_id);
		}
		p = p->next;
	}

	fprintf(stderr,"]\n");

	// fprintf(stderr,"Now %d cars (south) are waiting.\n", waitingcarsouth);
	// fprintf(stderr,"Now %d cars (north) are waiting.\n", waitingcarnorth);
	// fprintf(stderr,"Now %d trucks (south) are waiting.\n", waitingtrucksouth);
	// fprintf(stderr,"Now %d trucks (north) are waiting.\n", waitingtrucknorth);
}