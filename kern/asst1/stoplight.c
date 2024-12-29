/* 
 * stoplight.c
 *
 * 31-1-2003 : GWA : Stub functions created for CS161 Asst1.
 */


/*
 * 
 * Includes
 *
 */
#include "stoplight_framework.h"

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>


#include <synch.h> 
#include <machine/spl.h>

/*GLOBAL VARIABLES*/
//intersection locks 
struct lock *intersection_AB_Lock; 
struct lock *intersection_BC_Lock; 
struct lock *intersection_CA_Lock ; 

//Route LL locks 
struct lock *routeA_LL_Lock ; 
struct lock *routeB_LL_Lock; 
struct lock *routeC_LL_Lock; 

//node struct 
struct Node {
	long unsigned int vehicle; 
	struct Node* next; 
};

//Route LL head nodes 
struct Node *head_A = NULL; 
struct Node *head_B = NULL; 
struct Node *head_C = NULL; 

	/* data */
int vehicles_finished = 0;
struct lock *vehicle_count_lock;

//sleeping queue LL head nodes 
struct Node *sleep_head_A = NULL; 
struct Node *sleep_head_B = NULL; 
struct Node *sleep_head_C = NULL; 

//sleeping queue locks 
struct lock *sleep_A_lock; 
struct lock *sleep_B_lock; 
struct lock *sleep_C_lock; 

//sleeping queue function headers 
void enqueue(struct Node **sleep_head, unsigned long int vehiclenumber);
void dequeue(unsigned long int vehiclenumber, struct Node **sleep_head);

/*
 *
 * Function Definitions
 *
 */

/*
 * turn_left()
 *
 * Arguments:
 *      unsigned long vehiclenumber: the vehicle id number for fetching vehicle info.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a left turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

void
turn_left(unsigned long vehiclenumber)
{
	//get vehicle route 
	int route = get_vehicle_route(vehiclenumber);

	//Intersection AB - BC
	if(route == 0){
		//if route head is null, return 
		if(head_A == NULL){
			return;
		}
		//acquire AB and BC locks 
		lock_acquire(intersection_AB_Lock);
		lock_acquire(intersection_BC_Lock);

		//enter intersection 
		enter(vehiclenumber,0);

		//transfer intersections 
		transfer(vehiclenumber,0,1);


		//remvoe vehicle from head of queue and free 
		if (head_A->vehicle == vehiclenumber) {
			struct Node *temp = head_A;
			head_A = temp->next;
			kfree(temp);
		}

		//release AB lock
		lock_release(intersection_AB_Lock);
		//leave intersection 
		leave(vehiclenumber,1);
		//relaese BC lock 
		lock_release(intersection_BC_Lock);
	}

	// Intersection BC - CA
	if(route == 1){

		//if head is null return 
		if(head_B == NULL){
			return;
		}
		
		//aquire BC and CA lock 
		lock_acquire(intersection_BC_Lock);
		lock_acquire(intersection_CA_Lock);

		//enter intersection 
		enter(vehiclenumber,1);
		//transfer to new part of the intersection 
		transfer(vehiclenumber,1,2);

		//remove vehicle from head and free 
		if (head_B->vehicle == vehiclenumber){
			struct Node *temp = head_B;
			head_B = temp->next;
			kfree(temp);
		}

		//release BC lock 
		lock_release(intersection_BC_Lock);
		//leave intersection 
		leave(vehiclenumber,2);
		//release CA lock 
		lock_release(intersection_CA_Lock);
	}

	//intersection CA-AB 
	if(route == 2){
		//if head is null, return 
		if(head_C == NULL){
			return;
		}

		//acquire CA and AB locks 
		lock_acquire(intersection_CA_Lock);
		lock_acquire(intersection_AB_Lock);

		//enter intersection CA 
		enter(vehiclenumber,2);
		//transfer to AB
		transfer(vehiclenumber,2,0);

		//remove vehicle from head of list and free 
		if (head_C->vehicle == vehiclenumber){
			struct Node *temp = head_C;
			head_C = temp->next;
			kfree(temp);
		}

		//release CA lock 
		lock_release(intersection_CA_Lock);
		//leave AB intersection 
		leave(vehiclenumber,0);
		//release AB lock 
		lock_release(intersection_AB_Lock);
	}
}



/*
 * turn_right()
 *
 * Arguments:
 *      unsigned long vehiclenumber: the vehicle id number for fetching vehicle info.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      This function should implement making a right turn through the 
 *      intersection from any direction.
 *      Write and comment this function.
 */

static
void
turn_right(unsigned long vehiclenumber)
{
	/*
	 * Avoid unused variable warnings.
	 */

	//get route 
	int route = get_vehicle_route(vehiclenumber); 

	//if route is A 
	if (route == 0){

		//if no vehicles in route, return 
		if (head_A == NULL) {
			return; // return early if no vehicle exists
			}

		//get intersection AB lock 
		lock_acquire(intersection_AB_Lock); 

		//enter intersection AB
		enter(vehiclenumber, 0); 

		
		//remvoe vehicle from route A queue and free 
		if (head_A->vehicle == vehiclenumber) {
			struct Node *temp = head_A;
			head_A = temp->next;
			kfree(temp);
		}
		
		//leave intersection AB
		leave(vehiclenumber, 0); 
		//release AB lock 
		lock_release(intersection_AB_Lock);
		
	}

	//if route is B
	else if (route == 1){
		//if no vehicles in queue, return 
		if (head_B == NULL) {
    		return; // Exit early if no vehicle exists
		}

		//get intersection BC lock 
		lock_acquire(intersection_BC_Lock); 
		//enter intersection BC
		enter(vehiclenumber, 1); 
		
		//remove vehicle from head of route B queue and free 
		if (head_B->vehicle == vehiclenumber){
			struct Node *temp = head_B;
			head_B = temp->next;
			kfree(temp);
		}
		
		//leave intersection BC 
		leave(vehiclenumber, 1); 
		//release BC lock 
		lock_release(intersection_BC_Lock);
		
	}

	//if route is C
	else{
		//if no vehicles in route C list, return  
		if (head_C == NULL) {
			return; // Exit early if no vehicle exists
		}

		//get intersection CA lock 
		lock_acquire(intersection_CA_Lock); 
		//enter intersection 
		enter(vehiclenumber, 2); 
		
		//remove vehicle from head of route C queue 
		if (head_C->vehicle == vehiclenumber){
			struct Node *temp = head_C;
			head_C = temp->next;
			kfree(temp);
		}
		
		//leave interesection CA 
		leave(vehiclenumber, 2); 
		//release CA lock 
		lock_release(intersection_CA_Lock);
		
	}

}


/*
 * vehicle_thread()
 *
 * Arguments: 
 *      void * unusedpointer: currently unused.
 *      unsigned long vehiclenumber: holds vehicle id number.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Change this function as necessary to implement your solution. These
 *      threads are created by stoplight_driver(). Each thread corresponds
 *      to exactly one vehicle. You can fetch information about the vehicle
 *      using its vehiclenumber, plus the get_vehicle_* functions from
 *      stoplight_framework.h.
 *
 *      This function should probably call turn_right() and turn_left(), which
 *      can then each implement their specific turns -- but you are free to
 *      complete this project however you wish.
 */

static
void
vehicle_thread(void * unusedpointer,
		unsigned long vehiclenumber)
{
	/*
	 * Avoid unused variable and function warnings.
	 */
	(void) unusedpointer;
	(void) vehiclenumber;

	//get route and type
	int route = get_vehicle_route(vehiclenumber); 
	int type = get_vehicle_type(vehiclenumber); 

	//make node for new vehicle 
	struct Node *newVehicle = kmalloc(sizeof(struct Node));
	if (newVehicle == NULL){
		panic("kmalloc failed"); 
	}
	newVehicle->vehicle = vehiclenumber; 
	newVehicle->next = NULL; 
	
	//Add vehicle to appropriate Route LL 
	if (route == 0){ //if route is A 
		//call approach for this vehicle
	
		//lock route A LL
		lock_acquire(routeA_LL_Lock); 
		
		if (head_A == NULL){ //check if head is null 
				//set this vehicle as head 
				head_A = newVehicle; 
			}
		
		else{
			//initialize curr and prev 
			struct Node *curr = head_A; 
			struct Node *prev = NULL;

			if (type == 1){ //if a car 
				
			//iterate through LL and add car
			while (curr != NULL &&  get_vehicle_type(curr->vehicle) != 0){
					prev = curr; 
					curr = curr->next; 
			}
				if (prev == NULL) {
                // No trucks found; insert at the head
                newVehicle->next = head_A; 
                head_A = newVehicle; 
            } else {
                // Insert right before the first truck
                newVehicle->next = curr; 
                prev->next = newVehicle; 
            }
			}

			else if (type == 0){ //if a truck 

				//iterate through LL and add truck

				while (curr != NULL){
					prev = curr; 
					curr = curr->next; 
				}

				//now prev is last node

				//add to end of list  
				prev->next = newVehicle; 

			}
		}
		
		//release lock 
		//approach(vehiclenumber); 
		lock_release(routeA_LL_Lock);
		

	}

	else if (route == 1){ //if route is B

		//lock route B LL
		lock_acquire(routeB_LL_Lock); 
		if (head_B == NULL){ //check if head is null 
				//set this vehicle as head 
				head_B = newVehicle; 
			}
		
		else{
			//initialize curr and prev 
			struct Node *curr = head_B; 
			struct Node *prev = NULL;

			if (type == 1){ //if a car 
				
			//iterate through LL and add car
			
					while (curr != NULL &&  get_vehicle_type(curr->vehicle) != 0){
					prev = curr; 
					curr = curr->next; 
			}
				if (prev == NULL) {
                // No trucks found; insert at the head
                newVehicle->next = head_B; 
                head_B= newVehicle; 
            } else {
                // Insert right before the first truck
                newVehicle->next = curr; 
                prev->next = newVehicle; 
            }
			}

			else if (type == 0){ //if a truck 

				//iterate through LL and add truck

				while (curr != NULL){
					prev = curr; 
					curr = curr->next; 
				}

				//now prev is last node
				//add to end of list  
				prev->next = newVehicle; 

			}
		}

		//release lock 
		 	//approach(vehiclenumber); 
		lock_release(routeB_LL_Lock);

	}

	else { //if route is C

		//lock route C LL
		lock_acquire(routeC_LL_Lock); 
		if (head_C == NULL){ //check if head is null 
				//set this vehicle as head 
				head_C = newVehicle; 
			}
		
		else{
			
			//initialize curr and prev 
			struct Node *curr = head_C; 
			struct Node *prev = NULL;

			if (type == 1){ //if a car 
				
			//iterate through LL and add car
			
					while (curr != NULL &&  get_vehicle_type(curr->vehicle) != 0){
					prev = curr; 
					curr = curr->next; 
			}
				if (prev == NULL) {
                // No trucks found; insert at the head
                newVehicle->next = head_C; 
                head_C = newVehicle; 
            } else {
                // Insert right before the first truck
                newVehicle->next = curr; 
                prev->next = newVehicle; 
            }
			}

			else if (type == 0){ //if a truck 

				//iterate through LL and add truck

				while (curr != NULL){
					prev = curr; 
					curr = curr->next; 
				}

				//now prev is last node

				//add to end of list  
				prev->next = newVehicle; 

			}
		}
		//approach(vehiclenumber); 
		//release lock 
		lock_release(routeC_LL_Lock);

	}

	approach(vehiclenumber); 
	
	//get turn direction 
	int turn_direction = get_vehicle_turn_direction(vehiclenumber);

		if(route == 0){
			
			//get route A priority queue lock 
			
			lock_acquire(routeA_LL_Lock);
			
			
			//check if this vehicle is the next one in the queue 
			if (head_A != NULL && vehiclenumber != head_A->vehicle){
				//if not next...
			
				//get sleeping queue lock 
				
				lock_acquire(sleep_A_lock); 
				//add thread to sleep queue 
				enqueue(&sleep_head_A, newVehicle->vehicle); 
				//release sleeping queue lock  
				
				lock_release(sleep_A_lock); 
				
				lock_release(routeA_LL_Lock); 

				//sleep thread 
				int spl = splhigh(); 
				
				
				thread_sleep(newVehicle); 
				splx(spl); 

				//get sleeping queue lock 
				
				lock_acquire(sleep_A_lock);
				 
				//dequeue thread from sleep queue 
				dequeue(newVehicle->vehicle, &sleep_head_A);  
				//release lock 

				
				lock_release(sleep_A_lock); 
				
				lock_acquire(routeA_LL_Lock);
				

				 
			}

			//if vehicle is next, turn 
			if(vehiclenumber ==  head_A->vehicle){
			
				//turn left 
				if (turn_direction == 1){
				turn_left(vehiclenumber); 

				}

			//otherwise turn right 
			else{
				turn_right(vehiclenumber); 
			}
			
		}
		 
		//release priority queue lock 
		
		
		
		//wake next thread 
		//if head_A thread is sleeping, wake it 
		if (sleep_head_A != NULL && head_A != NULL){
			int spl = splhigh();
			lock_acquire(sleep_A_lock); 
			thread_wakeup(head_A);
			splx(spl);

			lock_release(sleep_A_lock); 

		
			
		}
		lock_release(routeA_LL_Lock);
		

		
		}

		else if(route == 1){

						//get route A priority queue lock 
			
			lock_acquire(routeB_LL_Lock);
			
			
			//check if this vehicle is the next one in the queue 
			if (head_B != NULL && vehiclenumber != head_B->vehicle){
				//if not next...
			
				//get sleeping queue lock 
				
				lock_acquire(sleep_B_lock); 
				//add thread to sleep queue 
				enqueue(&sleep_head_B, newVehicle->vehicle); 
				//release sleeping queue lock  
				
				lock_release(sleep_B_lock); 
				
				lock_release(routeB_LL_Lock); 

				//sleep thread 
				int spl = splhigh(); 
				
				
				thread_sleep(newVehicle); 
				splx(spl); 

				//get sleeping queue lock 
				
				lock_acquire(sleep_B_lock);
				 
				//dequeue thread from sleep queue 
				dequeue(newVehicle->vehicle, &sleep_head_B);  
				//release lock 

				
				lock_release(sleep_B_lock); 
				
				lock_acquire(routeB_LL_Lock);
				

				 
			}

			//if vehicle is next, turn 
			if(vehiclenumber ==  head_B->vehicle){
			
				//turn left 
				if (turn_direction == 1){
				turn_left(vehiclenumber); 

				}

			//otherwise turn right 
			else{
				turn_right(vehiclenumber); 
			}
			
		}
		 
		//release priority queue lock 
		
		
		
		//wake next thread 
		//if head_A thread is sleeping, wake it 
		if (sleep_head_B != NULL && head_B != NULL){
			int spl = splhigh();
			lock_acquire(sleep_B_lock); 
			thread_wakeup(head_B);
			splx(spl);


			lock_release(sleep_B_lock); 

		
			
		}
		lock_release(routeB_LL_Lock);
		

		
		}

		else{
						//get route A priority queue lock 
			
			lock_acquire(routeC_LL_Lock);
			
			
			//check if this vehicle is the next one in the queue 
			if (head_C != NULL && vehiclenumber != head_C->vehicle){
				//if not next...
			
				//get sleeping queue lock 
				
				lock_acquire(sleep_C_lock); 
				//add thread to sleep queue 
				enqueue(&sleep_head_C, newVehicle->vehicle); 
				//release sleeping queue lock  
				
				lock_release(sleep_C_lock); 
				
				lock_release(routeC_LL_Lock); 

				//sleep thread 
				int spl = splhigh(); 
				
				
				thread_sleep(newVehicle); 
				splx(spl); 

				//get sleeping queue lock 
				
				lock_acquire(sleep_C_lock);
				 
				//dequeue thread from sleep queue 
				dequeue(newVehicle->vehicle, &sleep_head_C);  
				//release lock 

				
				lock_release(sleep_C_lock); 
				
				lock_acquire(routeC_LL_Lock);
				

				 
			}

			//if vehicle is next, turn 
			if(vehiclenumber ==  head_C->vehicle){
			
				//turn left 
				if (turn_direction == 1){
				turn_left(vehiclenumber); 

				}

			//otherwise turn right 
			else{
				turn_right(vehiclenumber); 
			}
			
		}
		 
		//release priority queue lock 
		
		
		
		//wake next thread 
		//if head_A thread is sleeping, wake it 
		if (sleep_head_C != NULL && head_C != NULL){
			int spl = splhigh();
			lock_acquire(sleep_C_lock); 
			thread_wakeup(head_C);
			splx(spl);


			lock_release(sleep_C_lock); 

		
			
		}
		lock_release(routeC_LL_Lock);
		

		
	}
		
		

	lock_acquire(vehicle_count_lock);
    vehicles_finished++;  // Increment the number of vehicles finished
    lock_release(vehicle_count_lock);

	

	//(void) turn_left;
	//(void) turn_right;

	/* Note: Throughout this project you will use the get_vehicle_* functions
	 * to get various information about the vehicle. For example:
	 *
	 *     int direction = get_vehicle_turn_direction(vehiclenumber);
	 *     int route = get_vehicle_route(vehiclenumber);
	 *     int type = get_vehicle_type(vehiclenumber);
	 * 
	 * See stoplight_framework.h for documentation on every function, plus various
	 * helpful constants.
	 */
}

/*SLEEPING THREAD QUEUE FUNCTIONS*/
/**
 * enqueue() 
 * 
 * Arguments: 
 * 			struct Node **sleep_head: double pointer for head of queue list 
 * 			unsigned long int vehiclenumber: holds vehicle id number 
 * 
 * Returns: 
 * 			nothing 
 * 
 * Notes: 
 * 			Used to add a sleeping thread to the sleep queue that corresponds with that route. 
 */
void enqueue(struct Node **sleep_head, unsigned long int vehiclenumber){
	//get vehicle type 
	int type = get_vehicle_type(vehiclenumber); 

	//if head of queue is NULL...
	if (*sleep_head == NULL){ 
				
					//create new node that is the vehicle number and make head of the queue 
					*sleep_head = kmalloc(sizeof(struct Node)); 
					if (*sleep_head == NULL){
						panic("kmalloc failed"); 
				
					}

					//set fields 
					(*sleep_head) ->vehicle = vehiclenumber; 
					(*sleep_head) ->next = NULL; 
				
		
			}
		
		//otherwise, add to queue 
		else{
			//create new node that is vehicle number 
			struct Node *newNode = kmalloc(sizeof(struct Node)); 
			if (newNode == NULL){
				panic("kmalloc failed"); 
			}
			//set fields 
			newNode->next = NULL; 
			newNode -> vehicle = vehiclenumber;


			//initialize curr and prev 
			struct Node *curr = *sleep_head; 
			struct Node *prev = NULL;

			if (type == 1){ //if a car 
				
			//iterate through LL and add car
			while (curr != NULL &&  get_vehicle_type(curr->vehicle) != 0){
					prev = curr; 
					curr = curr->next; 
			}
				//if no trucks present, insert at head of queue 
				if (prev == NULL) {
                newNode->next = *sleep_head;
				*sleep_head = newNode; 
			}
				//otherwise there is at least one truck
				//iterate to end of cars and insert before first truck 
             else {
                newNode->next = curr; 
                prev->next = newNode; 
            }
			}

			//if vehicle is a truck...
			else if (type == 0){ 

				//iterate through LL and add truck to end of queue 
				while (curr != NULL){
					prev = curr; 
					curr = curr->next; 
				}

				prev->next = newNode; 

			}
		}

	
}


/**
 * dequeue() 
 * 
 * Arguments: 
 * 			unsigned long int vehiclenumber: holds vehicle id number 			
 * 			struct Node **sleep_head: double pointer for head of queue list 
 * 			
 * Returns: 
 * 			nothing 
 * 
 * Notes: 
 * 			Used to remove a sleeping thread from the sleep queue that corresponds with that route. 
 */
void dequeue(unsigned long int vehiclenumber, struct Node **sleep_head){

	//head of queue 
	struct Node *curr = *sleep_head; 

	//if head is null, return 
	if (curr == NULL){
		return; 
	}

	//remove vehicle from head of queue and free 
	if (curr->vehicle == vehiclenumber){
		*sleep_head = curr->next; 
		kfree(curr); 
		return; 
	}
	

	
	
	


}



/*
 * stoplight_driver()
 *
 * Arguments:
 *      int vehicle_count: The number of vehicles for this problem.
 *                         See the test code in stoplight_framework.c for
 *                         more info.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up the approachintersection() threads.  You are
 *      free to modify this code as necessary for your solution.
 * 
 *      Be sure that the function remains able to create one thread per vehicle
 *      (a requirement of the project), and also be sure not to delete the call
 *      to finish().
 */

int stoplight_driver(int vehicle_count) {
    int index, error;

    // Initialize intersection, route, and sleeping queue locks
    intersection_AB_Lock = lock_create("intersection_AB_Lock");
    intersection_BC_Lock = lock_create("intersection_BC_Lock");
    intersection_CA_Lock = lock_create("intersection_CA_Lock");

    routeA_LL_Lock = lock_create("routeA_LL_Lock");
    routeB_LL_Lock = lock_create("routeB_LL_Lock");
    routeC_LL_Lock = lock_create("routeC_LL_Lock");

	sleep_A_lock = lock_create("sleep_A_lock");
	sleep_B_lock = lock_create("sleep_B_lock");
	sleep_C_lock = lock_create("sleep_B_lock");

	//initialize vehicle count lock 
	vehicle_count_lock = lock_create("vehicle_count_lock");


	//create vehicle threads 
	for (index = 0; index < vehicle_count; index++) {

		/* This creates a thread for each vehicle, with unique numbers. */
		error = thread_fork("vehicle thread",
				NULL,
				index,
				vehicle_thread,
				NULL
				);

		/*
		 * panic() on error.
		 */

		if (error) {
			panic("approachintersection: thread_fork failed: %s\n",
					strerror(error)
				 );
		}
	}

	/* This function indicates the end of the stoplight problem.
	 * This kind of thing is important in a synchronization problem, as
	 * without a specific indicator that the problem has been concluded,
	 * it is impossible to know whether the program has finished successfully
	 * or whether it deadlocked and stopped making progress.
	 * 
	 * Part of your task will be to add synchronization code so that finish()
	 * is not called until every vehicle has made its way through the intersection.
	 * 
	 * Note that deleting the call to finish() will not score you any points. You
	 * must correctly synchronize it to get credit for that part of the project.
	 */
	
		//wait until all vehicles have finished 
        while (vehicles_finished != vehicle_count) { //not done 
			continue; 
        }  

		//call finish once all vehicles are done 
		finish(); 
	
		//clean up 
		lock_destroy(vehicle_count_lock);

		lock_destroy(intersection_AB_Lock);
		lock_destroy(intersection_BC_Lock);
		lock_destroy(intersection_CA_Lock);
		
		lock_destroy(routeA_LL_Lock); 
		lock_destroy(routeB_LL_Lock); 
		lock_destroy(routeC_LL_Lock); 

		lock_destroy(sleep_A_lock); 
		lock_destroy(sleep_B_lock); 
		lock_destroy(sleep_C_lock); 

	return 0;

}

