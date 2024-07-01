#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#endif

#include <stdint.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <lwip_test_ta.h>


struct mutex {
	unsigned spin_lock;
	struct wait_queue wq;
	short state;
};

struct condvar {
        unsigned spin_lock;
        struct mutex *m;
};

typedef struct {
    int count;
    struct mutex mtx;
    struct condvar cv;
} semaphore_t;

void semaphore_init(semaphore_t *sem, int init_count);
void semaphore_wait(semaphore_t *sem);
void semaphore_signal(semaphore_t *sem);

