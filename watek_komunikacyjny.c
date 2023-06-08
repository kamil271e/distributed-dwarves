#include "main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
	    debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        pthread_mutex_lock( &clockMut );
        lamport_clock = (lamport_clock > pakiet.ts ? lamport_clock : pakiet.ts) + 1;
        pthread_mutex_unlock( &clockMut );
        
        switch ( status.MPI_TAG ) {
            case REQUEST: 
                debug("Ktoś ubiega się o zlecenie!");
                if (pakiet.data != job_id){
                    sendPacket( 0, status.MPI_SOURCE, ACK );
                }
                else if (stan==InWant && pakiet.ts < lamport_clock){
                    sendPacket( 0, status.MPI_SOURCE, ACK );
                    changeState(InRun);
                    job_id = -1;
                }
                break;
            case ACK: 
                debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
                ackCount++; /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
                break;
            case JOB:
                debug("Skansen prosi mnie o zlecenie");
                job_id = pakiet.data; 
                changeState(InWant);
                break;
            case PORTAL_REQUEST:
                debug("Dostałem portal request");
                if (stan != InSection){
                    sendPacket(0, status.MPI_SOURCE, PORTAL_ACK);
                } else if (pakiet.ts < lamport_clock){
                    sendPacket(0, status.MPI_SOURCE, PORTAL_ACK);
                } else {
                    enqueue(ackQueue, pakiet.src); // status.MPI_SOURCE
                }
                break;
            case PORTAL_ACK:
                debug("Dostałem Portal_ACK od %d, mam już %d", status.MPI_SOURCE, ackPortalCount);
                ackPortalCount++;
                break;
            default:
                break;
        }
    }
}
