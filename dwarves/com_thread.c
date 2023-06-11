#include "main.h"
#include "com_thread.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *start_com_thread(void *ptr)
{
    MPI_Status status;
    packet_t packet;

    while (state != InFinish ) {
	    // debug("Czekam na recv");
        MPI_Recv( &packet, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_lock( &clock_mut );
        lamport_clock = (lamport_clock > packet.ts ? lamport_clock : packet.ts) + 1;
        pthread_mutex_unlock( &clock_mut );
        
        switch ( status.MPI_TAG ) {
            case JOB:
                if (state == InRun){
                    debug("Skansen wysłał mi zlecenie");
                    job_id = packet.job_id; 
                    changeState(WantJob);
                }
                break;
            case REQUEST: 
                debug("Ktoś ubiega się o zlecenie!");
                if (packet.job_id != job_id){
                    sendPacket( 0, status.MPI_SOURCE, ACK );
                }
                else if ((state == WantJob || state == WaitForACK) && packet.ts < lamport_clock){
                    sendPacket( 0, status.MPI_SOURCE, ACK );
                    changeState(InRun);
                    // printf("change job id from %d to -1\n", job_id);
                    job_id = -1;
                    ack_count = 0;
                }
                break;
            case ACK: 
                ack_count++;
                debug("Dostałem ACK od %d, mam już %d, potrzebuje %d", status.MPI_SOURCE, ack_count, NUM_DWARVES-1); /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
                if ( ack_count >= NUM_DWARVES - 1 - 1){ 
					changeState(InSection);
				} 
                break;
            case PORTAL_REQUEST:
                debug("Dostałem portal request");
                if (state != InSection){
                    sendPacket(0, status.MPI_SOURCE, PORTAL_ACK);
                } else if (packet.ts < lamport_clock){
                    sendPacket(0, status.MPI_SOURCE, PORTAL_ACK);
                } else {
                    enqueue(ack_queue, packet.src); // status.MPI_SOURCE
                }
                break;
            case PORTAL_ACK:
                ack_portal_count++;
                debug("Dostałem Portal_ACK od %d, mam już %d, potrzebuje %d", status.MPI_SOURCE, ack_portal_count, NUM_DWARVES - 1 - NUM_PORTALS);
                if (ack_portal_count >= NUM_DWARVES - 1 - NUM_PORTALS - 1){
                    changeState(DoingJob);
                }
                break;
            default:
                break;
        }
    }
    return NULL;
}
