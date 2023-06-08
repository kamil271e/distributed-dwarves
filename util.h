#ifndef UTILH
#define UTILH
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <pthread.h>
#include <stddef.h>

#ifdef DEBUG
#define debug(FORMAT,...) printf("%c[%d;%dm [%ld]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, lamport_clock, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...) ;
#endif

// makro println - to samo co debug, ale wyświetla się zawsze
#define println(FORMAT,...) printf("%c[%d;%dm [%ld]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, lamport_clock, ##__VA_ARGS__, 27,0,37);

#ifndef NUM_TAVERNS
    #define NUM_TAVERNS 1
#endif

#define TRUE 1
#define FALSE 0
#define SEC_IN_STATE 1
#define STATE_CHANGE_PROB 10

/* packet_t ma trzy pola, więc NITEMS=3. Wykorzystane w inicjuj_typ_pakietu */
#define NITEMS 3
#define PORTAL_NUM 5

/* TYPY PAKIETÓW */
#define ACK     1
#define REQUEST 2
#define RELEASE 3
#define APP_PKT 4
#define FINISH  5
#define JOB 6
#define PORTAL_ACK 7
#define PORTAL_REQUEST 8
#define PORTAL_RELEASE 9

typedef struct {
    int ts;      
    int src;  
    int data; 
} packet_t;

typedef enum {
    InRun, // for dwarves
    InMonitor,
    InWant,
    InSection,
    InFinish,
    GenJob // for taverns
} state_t;

extern MPI_Datatype MPI_PACKET_T;
extern state_t state;
extern pthread_mutex_t state_mut;
extern pthread_mutex_t clock_mut;

extern int rank;
extern int size;
extern long lamport_clock;

void init_packet_type();
void sendPacket(packet_t *pkt, int destination, int tag);
void changeState( state_t );

#endif
