#include "main.h"
#include "main_thread.h"

void main_loop()
{
    srandom(rank);

    while (state != InFinish) {
		
		packet_t* pkt = malloc(sizeof(packet_t));
		
		switch (state) {			
			case InRun: 
				// wait
				break;
			case WantJob:
				debug("Czekam na wejście do sekcji krytycznej")
				
				pkt->job_id = job_id;
				pkt->priority = rec_priority;

				for (int i = 0; i < size-1; i++){
					if (i != rank){
						sendPacket(pkt, i, REQUEST);
					}
				} changeState(WaitForACK);
				free(pkt);
				break;
			case WaitForACK:
				// wait
				if (NUM_DWARVES==1) changeState(InSection);
				break;
			case InSection:
				debug("Jestem w sekcji krytycznej")
				rec_priority = lamport_clock*1000 + rank;
				pkt->priority = rec_priority;

				pkt->job_id = job_id;
				for (int i = 0; i <= size-1; i++){
					if (i != rank){
						sendPacket(pkt, i, PORTAL_REQUEST);
					}
				} changeState(WaitForPortal);
				free(pkt);
				break;
			case WaitForPortal:
				// wait
				if (NUM_DWARVES==1) changeState(DoingJob);
				break;
			case DoingJob:
				println("Robię fuchę %d !!!!!!!!", job_id);

				sleep(10); 

				println("Skończyłem robić fuchę %d !!!!!!!!", job_id);

				debug("Wychodzę z sekcji krytycznej")
				debug("Zmieniam stan na wysyłanie");

				changeJobId(-1);
				// wysylanie ACK do krasnali z listy
				pthread_mutex_lock(&queue_mutex);
				while (!isEmpty(ack_queue)){
					int dest = dequeue(ack_queue);
					sendPacket(0, dest, PORTAL_ACK);
				}
				pthread_mutex_unlock(&queue_mutex);
				changeAckCount(0);
				changeAckPortalCount(0);
				changeState( InRun );
				break;
			default: 
				break;
			}
		sleep(SEC_IN_STATE);
	}
}
