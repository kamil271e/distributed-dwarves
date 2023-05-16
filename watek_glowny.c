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
			perc = random()%100;
			if ( perc < 25 ) {
				debug("Perc: %d", perc);
				println("Ubiegam się o sekcję krytyczną")
				debug("Zmieniam stan na wysyłanie");
				packet_t *pkt = malloc(sizeof(packet_t));
				pkt->data = perc;
				ackCount = 0;
				for (int i=0;i<=size-1;i++)
				if (i!=rank)
					sendPacket( pkt, i, REQUEST);
				changeState( InWant );
				free(pkt);
			}
			debug("Skończyłem myśleć");
			break;
	    case InWant:
			println("Czekam na wejście do sekcji krytycznej")
			// tutaj zapewne jakiś muteks albo zmienna warunkowa
			// bo aktywne czekanie jest BUE
			if ( ackCount == size - 1) 
				changeState(InSection);
			break;
			case InSection:
			// tutaj zapewne jakiś muteks albo zmienna warunkowa
			println("Jestem w sekcji krytycznej")
				sleep(5);
			//if ( perc < 25 ) {
				debug("Perc: %d", perc);
				println("Wychodzę z sekcji krytyczneh")
				debug("Zmieniam stan na wysyłanie");
				packet_t *pkt = malloc(sizeof(packet_t));
				pkt->data = perc;
				for (int i=0;i<=size-1;i++)
				if (i!=rank)
					sendPacket( pkt, (rank+1)%size, RELEASE);
				changeState( InRun );
				free(pkt);
			//}
			break;
	    default: 
			break;
		}
        sleep(SEC_IN_STATE);
    }
}

void jobFactory(){
	packet_t *pkt = malloc(sizeof(packet_t));
	for (int i = 0; i < size; i+=2){
		sendPacket(pkt, i, ); // TODO: request type - e.g. JOB
		sleep(1);
	}
}
