#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <lwip_test_ta.h>
#include "semaphore.h"

/*
typedef struct {
	uint32_t count;
	struct mutex mtx;
	struct condvar cv;
} semaphore_t;
*/
void semaphore_init(semaphore_t *sem, int init_count) {
	sem->count = init_count;
	mutex_init(&sem->mtx);
	condvar_init(&sem->cv);
}

void semaphore_wait(semaphore_t *sem) {
	mutex_lock(&sem->mtx);
	while( sem->count == 0 ) {
		condvar_wait(&sem->cv, &sem->mtx);
	}
	sem->count--;
	mutex_unlock(&sem->mtx);
}

void semaphore_signal(semaphore_t *sem) {
	mutex_lock(&sem->mtx);
	sem->count ++;
	if( sem->count <= 0 )  {
		condvar_signal(&sem->cv);
	}
	mutex_unlock(&sem->mtx);
}
