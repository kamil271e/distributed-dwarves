#include "main.h"
#include "com_thread.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
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
            case REQUEST: 
                debug("Ktoś ubiega się o zlecenie!");
                if (packet.data != job_id){
                    sendPacket( 0, status.MPI_SOURCE, ACK );
                }
                else if (state == InWant && packet.ts < lamport_clock){
                    sendPacket( 0, status.MPI_SOURCE, ACK );
                    changeState(InRun);
                    job_id = -1;
                }
                break;
            case ACK: 
                ack_count++;
                debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ack_count); /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
                break;
            case JOB:
                debug("Skansen prosi mnie o zlecenie");
                job_id = packet.data; 
                changeState(InWant);
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
                debug("Dostałem Portal_ACK od %d, mam już %d", status.MPI_SOURCE, ack_portal_count);
                break;
            default:
                break;
        }
    }
    return NULL;
}
