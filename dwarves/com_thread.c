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
        int priority = rec_priority < src_priority ? 1 : 0; // mamy priorytet, jeśli mamy mniejszą wartość

        if (isEmpty(req_queue)){
            dictator = 0;
        }
        
        switch ( status.MPI_TAG ) {
            case JOB:
                if (dictator){
                    // wysyłaj robotę do kolejnych krasnoludów w req_queue
                    int worker = dequeue(req_queue);
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->job_id = packet.job_id;
                    sendDictatorPacket( pkt, worker, JOB);
                    sendPacket(pkt, packet.src, JOB_DELIVER);
                }
                else if (state == InRun && packet.src != -1){
                    debug("Skansen wysłał mi zlecenie %d", packet.job_id);
                    changeJobId(packet.job_id);
                    rec_priority = lamport_clock*1000 + rank;
                    changeState(WantJob);

                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->job_id = packet.job_id;
                    sendPacket(pkt, packet.src, JOB_DELIVER);
                }
                else if (packet.src == -1){
                    debug("Dyktator wysłał mi zlecenie %d", packet.job_id);
                    changeJobId(packet.job_id);
                    rec_priority = 0;
                    changeState(WantJob);
                }
                break;
            case REQUEST: 
                debug("Priorytety: rec %ld, src %d", rec_priority, src_priority);
                debug("O zlecenie ubiega się %d", packet.src);
                if (packet.job_id != job_id){
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->job_id = packet.job_id;
                    sendPacket( pkt, status.MPI_SOURCE, ACK );
                }
                else if ((state == WantJob || state == WaitForACK) && !priority){
                    debug("Rezygnuję z fuchy, %d ma wyższy priorytet", packet.src);
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->job_id = job_id;
                    sendPacket( pkt, status.MPI_SOURCE, ACK );
                    changeJobId(-1);
                    changeAckCount(0);
                    req_queue = createQueue();
                    changeState(InRun);
                }
                else if (state == WantJob || state == WaitForACK) {
                    enqueue(req_queue, packet.src);
                }
                break;
            case ACK:
                if (packet.job_id == job_id){
                    pthread_mutex_lock(&ack_count_mut);
                    ack_count++;
                    pthread_mutex_unlock(&ack_count_mut);

                    debug("Dostałem ACK od %d, mam już %d, potrzebuje %d", status.MPI_SOURCE, ack_count, NUM_DWARVES-1); /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
                    if ( ack_count >= NUM_DWARVES - 1){ 
                        if (!isEmpty(req_queue)){
                            dictator = 1;
                        }
                        changeState(InSection);
                    } 
                }
                break;
            case PORTAL_REQUEST:
                debug("Dostałem portal request");
                if (state != DoingJob && state != WaitForPortal){
                    sendPacket(0, status.MPI_SOURCE, PORTAL_ACK);
                } else if (!priority){
                    sendPacket(0, status.MPI_SOURCE, PORTAL_ACK);
                } else {
                    pthread_mutex_lock(&queue_mutex);
                    enqueue(ack_queue, packet.src); // status.MPI_SOURCE
                    pthread_mutex_unlock(&queue_mutex);
                }
                break;
            case PORTAL_ACK:
                if (ack_portal_count >= NUM_DWARVES - 1 - NUM_PORTALS){
                    changeState(DoingJob);
                }
                else {
                    pthread_mutex_lock(&ack_portal_count_mut);
                    ack_portal_count++;
                    pthread_mutex_unlock(&ack_portal_count_mut);
                    debug("Dostałem Portal_ACK od %d, mam już %d, potrzebuje %d", status.MPI_SOURCE, ack_portal_count, NUM_DWARVES - 1 - NUM_PORTALS);
                    if (ack_portal_count >= NUM_DWARVES - 1 - NUM_PORTALS){
                        changeState(DoingJob);
                    }
                }
                break;
            default:
                break;
        }
    }
    return NULL;
}
