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
				if (!sent_req){
					for (int i = 0; i <= size-1; i++){ // nie wysylaj jak juz to zrobiles
						if (i != rank){
							sendPacket(0, i, REQUEST);
						}
					} sent_req=1;
				} // TODO dodac nowy state zamiast uzywania sent_req
				break;
			case InSection:
				// tutaj zapewne jakiś muteks albo zmienna warunkowa
				println("Jestem w sekcji krytycznej")
				for (int i = 0; i <= size-1; i++){
					if (i != rank){
						sendPacket(0, i, PORTAL_REQUEST);
					}
				}
				while(ack_portal_count < size-1-PORTAL_NUM){} // TODO aktywne czekanie

				println("Robię fuchę :)");

				sleep(5); // robimy fuche

				println("Wychodzę z sekcji krytycznej")
				debug("Zmieniam stan na wysyłanie");

				job_id = -1;
				for (int i = 0; i <= size-1; i++){
					if (i != rank){
						sendPacket(0, i, RELEASE);
					}
				}
				changeState( InRun );
				break;
			default: 
				break;
			}
		sleep(SEC_IN_STATE);
	}
}
