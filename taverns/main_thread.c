#include "main.h"
#include "main_thread.h"

void main_loop()
{
    srandom(rank);

    while (state != InFinish) {
		switch (state) {
			case GenJob:
				debug("Generuję fuchę %d", lamport_clock);
				packet_t *pkt = malloc(sizeof(packet_t));
				pkt->job_id = lamport_clock;
				for (int i = 0; i < size; i++){
					if (i != rank){
						sendPacket(pkt, i, JOB); 
					}
				}
			default: 
				break;
		}
		sleep(SEC_IN_STATE);
		sleep(1);
	}
}
