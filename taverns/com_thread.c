#include "main.h"
#include "com_thread.h"

void *start_com_thread(void *ptr)
{
    MPI_Status status;
    packet_t packet;

    while (state != InFinish ) {
        MPI_Recv( &packet, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_lock( &clock_mut );
        lamport_clock = (lamport_clock > packet.ts ? lamport_clock : packet.ts) + 1;
        pthread_mutex_unlock( &clock_mut );
        
        switch ( status.MPI_TAG ) {
            case JOB_DELIVER:
                // usun dostarczone zlecenie z kolejki
                pthread_mutex_lock(&job_sent_queue_mut);
                deleteNode(job_sent_queue, packet.job_id);
                pthread_mutex_unlock(&job_sent_queue_mut);
                break; 
            default:
                break;
        }
    }
    return NULL;
}
