#include "main.h"
#include "com_thread.h"

void *start_com_thread(void *ptr)
{
    MPI_Status status;
    packet_t packet;

    while (state != InFinish ) {
	    debug("czekam na recv");
        MPI_Recv( &packet, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_lock( &clock_mut );
        lamport_clock = (lamport_clock > packet.ts ? lamport_clock : packet.ts) + 1;
        pthread_mutex_unlock( &clock_mut );
        
        switch ( status.MPI_TAG ) {
            // TODO czy skansen odbiera info o zakonczeniu wychonywania fuchy?
            // dopiero wtedy generuje nową czy leci niezależnie?
            default:
                break;
        }
    }
    return NULL;
}
