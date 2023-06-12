#include "util.h"

int rank, size;
int job_id = -1;
int ack_count, ack_portal_count = 0;
int dictator = 0;
long lamport_clock = 0;
long rec_priority = 0;
MPI_Datatype MPI_PACKET_T;
state_t state = InRun;

/* zamek wokół zmiennej współdzielonej między wątkami. 
 * Zwróćcie uwagę, że każdy proces ma osobą pamięć, ale w ramach jednego
 * procesu wątki współdzielą zmienne - więc dostęp do nich powinien
 * być obwarowany muteksami
 */
pthread_mutex_t state_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clock_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t job_id_mut= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ack_count_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ack_portal_count_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

struct tagNames_t{
    const char *name;
    int tag;
} tagNames[] = { { "pakiet aplikacyjny", APP_PKT }, { "finish", FINISH}, 
                { "potwierdzenie", ACK}, {"prośbę o sekcję krytyczną", REQUEST}, {"zwolnienie sekcji krytycznej", RELEASE},
                {"zlecenie na fuchę", JOB }, {"prośbę o portal", PORTAL_REQUEST}, {"potwierdzenie odn. portalu", PORTAL_ACK}};

const char *const tag2string( int tag )
{
    for (int i=0; i <sizeof(tagNames)/sizeof(struct tagNames_t);i++) {
	if ( tagNames[i].tag == tag )  return tagNames[i].name;
    }
    return "<unknown>";
}

void init_packet_type()
{
    int blocklengths[NITEMS] = {1,1,1,1,1};
    MPI_Datatype types[NITEMS] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[NITEMS]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);
    offsets[2] = offsetof(packet_t, job_id);
    offsets[3] = offsetof(packet_t, priority);
    offsets[4] = offsetof(packet_t, is_tavern);
    

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, types, &MPI_PACKET_T);

    MPI_Type_commit(&MPI_PACKET_T);
}

void sendPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt=0;
    if (pkt==0) { pkt = malloc(sizeof(packet_t)); freepkt=1;}

    pkt->src = rank;

    pthread_mutex_lock( &clock_mut );
    lamport_clock++;
    pkt->ts = lamport_clock;
    pthread_mutex_unlock( &clock_mut );

    MPI_Send( pkt, 1, MPI_PACKET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d", tag2string(tag), destination);
    if (freepkt) free(pkt);
}

void sendDictatorPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt=0;
    if (pkt==0) { pkt = malloc(sizeof(packet_t)); freepkt=1;}

    pkt->src = -1;
    
    pthread_mutex_lock( &clock_mut );
    lamport_clock++;
    pkt->ts = lamport_clock;
    pthread_mutex_unlock( &clock_mut );

    MPI_Send( pkt, 1, MPI_PACKET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d", tag2string(tag), destination);
    if (freepkt) free(pkt);
}

void changeState( state_t new_state )
{
    pthread_mutex_lock( &state_mut );
    if (state==InFinish) { 
	    pthread_mutex_unlock( &state_mut );
        return;
    }
    state = new_state;
    pthread_mutex_unlock( &state_mut );
}

void changeJobId(int value){
    pthread_mutex_lock(&job_id_mut);
    job_id = value;
    pthread_mutex_unlock(&job_id_mut);
}

void changeAckCount(int value){
    pthread_mutex_lock(&ack_count_mut);
    ack_count = value;
    pthread_mutex_unlock(&ack_count_mut);
}

void changeAckPortalCount(int value){
    pthread_mutex_lock(&ack_portal_count_mut);
    ack_portal_count = value;
    pthread_mutex_unlock(&ack_portal_count_mut);
}