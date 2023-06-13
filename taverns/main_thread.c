#include "main.h"
#include "main_thread.h"

void main_loop()
{
    srandom(rank);

    while (state != InFinish) {
		switch (state) {
			case GenJob:
				if (job_sent_queue->size > 1) {changeState(ResendJob); break;}

				packet_t *pkt = malloc(sizeof(packet_t));
				if (lamport_clock == 0){
					pkt->job_id = rank;
				}else{
					pkt->job_id = lamport_clock;
				}
				println("Generuję zlecenie %d", pkt->job_id);

				for (int i = 0; i < size; i++){
					if (i != rank){
						sendPacket(pkt, i, JOB); 
					}
				}
				pthread_mutex_lock(&job_sent_queue_mut);
				enqueue(job_sent_queue, pkt->job_id);
				printQueue(job_sent_queue);
				pthread_mutex_unlock(&job_sent_queue_mut);
				break;
			case ResendJob:
				struct Queue* temp = createQueue();
                pthread_mutex_lock(&job_sent_queue_mut);
				int i = 0;
				while(!isEmpty(job_sent_queue)){
					int job_to_resend = dequeue(job_sent_queue);
					println("Przesyłam ponownie zlecenie %d", job_to_resend);
					packet_t *pkt = malloc(sizeof(packet_t));
					pkt->job_id = job_to_resend;
					for (int i = 0; i < size; i++){
						if (i != rank){
							sendPacket(pkt, i, JOB); 
						}
					}
					enqueue(temp, job_to_resend);
					i++;
				}
				job_sent_queue = temp;
                pthread_mutex_unlock(&job_sent_queue_mut);
				changeState(GenJob);
				break;
			default: 
				break;
		}
		sleep(SEC_IN_STATE);
		sleep(1);
	}
}
