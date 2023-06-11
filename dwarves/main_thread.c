#include "main.h"
#include "main_thread.h"

void main_loop()
{
    srandom(rank);

    while (state != InFinish) {
		switch (state) {
			case InRun: 
				// wait
				break;
			case WantJob:
				// tutaj zapewne jakiś muteks albo zmienna warunkowa
				// bo aktywne czekanie jest BUE
				println("Czekam na wejście do sekcji krytycznej")
				packet_t* pkt = malloc(sizeof(packet_t));
				pkt->job_id = job_id;
				for (int i = 0; i <= size-1; i++){ 
					sendPacket(pkt, i, REQUEST);
				} changeState(WaitForACK);
				free(pkt);
				break;
			case WaitForACK:
				// wait
				break;
			case InSection:
				// tutaj zapewne jakiś muteks albo zmienna warunkowa
				println("Jestem w sekcji krytycznej")
				for (int i = 0; i <= size-1; i++){
					sendPacket(0, i, PORTAL_REQUEST);
				} changeState(WaitForPortal);
				break;
			case WaitForPortal:
				// wait
				break;
			case DoingJob:
				println("Robię fuchę !!!!!!!!");

				sleep(5); 

				println("Wychodzę z sekcji krytycznej")
				debug("Zmieniam stan na wysyłanie");

				job_id = -1; // wysylanie ACK do krasnali z listy
				while (!isEmpty(ack_queue)){
					int dest = dequeue(ack_queue);
					sendPacket(0, dest, PORTAL_ACK);
				}
				ack_count=0;
				ack_portal_count=0;
				changeState( InRun );
				break;
			default: 
				break;
			}
		sleep(SEC_IN_STATE);
	}
}
