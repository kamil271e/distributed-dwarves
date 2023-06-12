#ifndef MAINH
#define MAINH
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "../util.h"
#include "../queue.h"

extern int rank;
extern int size;
extern long lamport_clock;

extern pthread_t com_thread;
extern struct Queue* job_sent_queue; // kolejka z wysłanymi zleceniami
extern pthread_mutex_t job_sent_queue_mut;

#endif
