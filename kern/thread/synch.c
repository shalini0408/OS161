/*
 * Synchronization primitives.
 * See synch.h for specifications of the functions.
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <curthread.h>
#include <machine/spl.h>

////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *namearg, int initial_count)
{
	struct semaphore *sem;

	assert(initial_count >= 0);

	sem = kmalloc(sizeof(struct semaphore));
	if (sem == NULL) {
		return NULL;
	}

	sem->name = kstrdup(namearg);
	if (sem->name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->count = initial_count;
	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	spl = splhigh();
	assert(thread_hassleepers(sem)==0);
	splx(spl);

	/*
	 * Note: while someone could theoretically start sleeping on
	 * the semaphore after the above test but before we free it,
	 * if they're going to do that, they can just as easily wait
	 * a bit and start sleeping on the semaphore after it's been
	 * freed. Consequently, there's not a whole lot of point in 
	 * including the kfrees in the splhigh block, so we don't.
	 */
	

	DEBUG(DB_VM, "Sempaphore desctroyed: %s\n", sem->name); 

	kfree(sem->name);
	kfree(sem);
}

void 
P(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	assert(in_interrupt==0);

	spl = splhigh();
	while (sem->count==0) {
		thread_sleep(sem);
	}
	assert(sem->count>0);
	sem->count--;
	splx(spl);
}

void
V(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);
	spl = splhigh();
	sem->count++;
	assert(sem->count>0);
	thread_wakeup(sem);
	splx(spl);
}

////////////////////////////////////////////////////////////
//
// Lock.

struct lock *
lock_create(const char *name)
{
	struct lock *lock;

	lock = kmalloc(sizeof(struct lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->name = kstrdup(name);
	if (lock->name == NULL) {
		kfree(lock);
		return NULL;
	}

	DEBUG(DB_VM, "Lock created: %s\n", lock->name); 
	
	// add stuff here as needed
	lock->flag = 1; 


	return lock;
}

void
lock_destroy(struct lock *lock)
{
	
	assert(lock != NULL);

	// add stuff here as needed
	
	int spl; 
	spl = splhigh(); //no interruptions 
	
	assert(thread_hassleepers(lock) == 0); //check that there are not threads waiting on the lock

	

	
	DEBUG(DB_VM, "Lock %s destroyed.\n", lock->name); 

	kfree(lock->name);
	kfree(lock->current_holder); 
	//kfree(lock->flag); 
	kfree(lock);

	splx(spl); //restore spl to previous level 
}

void
lock_acquire(struct lock *lock)
{
	int spl;
	assert(lock != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	assert(in_interrupt==0);

	spl = splhigh(); //diable interrupts 
	while (lock->flag==0) { //if lock flag is 0, lock not available, sleep thread 
		thread_sleep(lock);
	}
	assert(lock->flag == 1); //lock is available, make sure flag is 1 
	lock->flag = 0; //set flag to 0 (lock is being used)
	lock->current_holder = curthread; //set self to current_holder
	splx(spl); //reset spl level 

	//(void)lock;  // suppress warning until code gets written
}

void
lock_release(struct lock *lock)
{
	// Write this
	
	int spl; 
	
	//check that lock given is not null
	assert(lock != NULL); 
	
	spl = splhigh(); //disable interrupts 

	//decrease lock value 
	lock->flag = 1; 
	lock -> current_holder = NULL;

	//wake up next thread
	thread_wakeup(lock); 



	//set spl to previous level (enable interrupts) 
	splx(spl);


	//(void)lock;  // suppress warning until code gets written
}

int
lock_do_i_hold(struct lock *lock)
{
	// Write this
	if(lock->current_holder == curthread) return 1;
	else return 0;
	//(void)lock;  // suppress warning until code gets written

	//return 1;    // dummy until code gets written
}

////////////////////////////////////////////////////////////
//
// CV


struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(struct cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->name = kstrdup(name);
	if (cv->name==NULL) {
		kfree(cv);
		return NULL;
	}
	
	// add stuff here as needed
	
	return cv;
}

void
cv_destroy(struct cv *cv)
{
	assert(cv != NULL);

	// add stuff here as needed
	
	kfree(cv->name);
	kfree(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	// Write this
	(void)cv;    // suppress warning until code gets written
	(void)lock;  // suppress warning until code gets written
}
