#include "main.h"
#include "watek_glowny.h"

void mainLoop()
{
    srandom(rank);
    int tag;
    int perc;

    while (stan != InFinish) {
	switch (stan) {
	    case InRun: 
			sleep(1);
			break;
	    case InWant:
			println("Czekam na wejście do sekcji krytycznej")
			// tutaj zapewne jakiś muteks albo zmienna warunkowa
			// bo aktywne czekanie jest BUE
			if ( ackCount == ceil(size/2) - 1) 
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
			while(ackPortalCount < ceil(size/2)-1-1-PORTAL_NUM){} // TODO aktywne czekanie

			sleep(5); // robimy fuche

			println("Wychodzę z sekcji krytyczneh")
			debug("Zmieniam stan na wysyłanie");
			// packet_t *pkt = malloc(sizeof(packet_t));
			pkt->data = perc;
			job_id = -1;
			for (int i=0;i<=size-1;i++){
				if (i!=rank){
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

void jobFactory(){
	packet_t *pkt = malloc(sizeof(packet_t));
	pkt->data = lamport_clock; // ???
	for (int i = 0; i < size; i+=2){
		sendPacket(pkt, i, JOB); 
		sleep(1);
	}
}
