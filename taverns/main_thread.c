#include "main.h"
#include "main_thread.h"

void main_loop()
{
    srandom(rank);

    while (state != InFinish) {
		switch (state) {
			// TODO stany dla skansanu
			case GenJob:
				packet_t *pkt = malloc(sizeof(packet_t));
				pkt->jobId = lamport_clock; // ???
				for (int i = 0; i < size; i++){
					if (i != rank){
						sendPacket(pkt, i, JOB); 
					}
				}
			default: 
				break;
		}
		sleep(SEC_IN_STATE);
		sleep(5);
	}
}
