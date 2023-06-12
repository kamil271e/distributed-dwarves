#include "main.h"
#include "com_thread.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *start_com_thread(void *ptr)
{
    MPI_Status status;
    packet_t packet;

    while (state != InFinish ) {
        MPI_Recv( &packet, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_lock( &clock_mut );
        lamport_clock = (lamport_clock > packet.ts ? lamport_clock : packet.ts) + 1;
        pthread_mutex_unlock( &clock_mut );

        int src_priority = packet.priority;
        int priority = rec_priority < src_priority ? 1 : 0;    // mamy priorytet, jeśli mamy mniejszą wartość
        
        switch ( status.MPI_TAG ) {
            case JOB:
                if (state == InRun){
                    debug("Skansen wysłał mi zlecenie %d", packet.job_id);
                    changeJobId(packet.job_id);
                    rec_priority = lamport_clock*1000 + rank;
                    changeState(WantJob);
                }
                break;
            case REQUEST: 
                debug("Priorytety: rec %ld, src %d", rec_priority, src_priority);
                debug("O zlecenie ubiega się %d", packet.src);
                if (packet.job_id != job_id){
                    sendPacket( 0, status.MPI_SOURCE, ACK );
                }
                else if ((state == WantJob || state == WaitForACK) && !priority){
                    debug("Rezygnuję z fuchy, %d ma wyższy priorytet", packet.src);
                    sendPacket( 0, status.MPI_SOURCE, ACK );
                    changeState(InRun);
                    changeJobId(-1);
                    changeAckCount(0);
                }
                break;
            case ACK:
                if (packet.job_id == job_id){
                    ack_count++;
                    debug("Dostałem ACK od %d, mam już %d, potrzebuje %d", status.MPI_SOURCE, ack_count, NUM_DWARVES-1); /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
                    if ( ack_count >= NUM_DWARVES - 1 - 1){ 
                        changeState(InSection);
                    } 
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
