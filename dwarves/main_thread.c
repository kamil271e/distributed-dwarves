#include "main.h"
#include "main_thread.h"

void main_loop()
{
    srandom(rank);

    while (state != InFinish) {
		switch (state) {
			case InRun: 
				break;
			case InWant:
				println("Czekam na wejście do sekcji krytycznej")
				// tutaj zapewne jakiś muteks albo zmienna warunkowa
				// bo aktywne czekanie jest BUE
				if ( ack_count == size - 1) 
					changeState(InSection);
				break;
			case InSection:
				// tutaj zapewne jakiś muteks albo zmienna warunkowa
				println("Jestem w sekcji krytycznej")
				packet_t *pkt = malloc(sizeof(packet_t));
				for (int i=0;i<=size-1;i++){
					if (i!=rank){
						sendPacket( pkt, (rank+1)%size, PORTAL_REQUEST);
					}
				}
				while(ack_portal_count < size-1-PORTAL_NUM){} // TODO aktywne czekanie

				sleep(5); // robimy fuche

				println("Wychodzę z sekcji krytyczneh")
				debug("Zmieniam stan na wysyłanie");

				job_id = -1;
				for (int i = 0; i <= size-1; i++){
					if (i != rank){
						sendPacket( pkt, (rank+1)%size, RELEASE);
					}
				}
				changeState( InRun );
				free(pkt);
				break;
			default: 
				break;
			}
		sleep(SEC_IN_STATE);
	}
}
