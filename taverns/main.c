#include "main.h"
#include "main_thread.h"
#include "com_thread.h"
#include "../queue.h"
#include "../util.h"

/*
 * W main.h extern int rank (zapowiedź) w main.c int rank (definicja)
 * Zwróćcie uwagę, że każdy proces ma osobą pamięć, ale w ramach jednego
 * procesu wątki współdzielą zmienne - więc dostęp do nich powinien
 * być obwarowany muteksami. Rank i size akurat są write-once, więc nie trzeba,
 * ale zob util.c oraz util.h - zmienną state_t state i funkcję changeState
 *
 */
int job_id; // size - ilosc procesow - krasnali
int ack_count = 0;
int ack_portal_count = 0;
struct Queue* ack_queue = NULL;

pthread_t com_thread;
pthread_mutex_t clock_mut = PTHREAD_MUTEX_INITIALIZER;

void finalize()
{
    pthread_mutex_destroy( &state_mut);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n" );
    pthread_join(com_thread,NULL);
    MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
}

void check_thread_support(int provided)
{
    // printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
	    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	    MPI_Finalize();
	    exit(-1);
	    break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	    break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	    break;
        case MPI_THREAD_MULTIPLE: // printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}

int main(int argc, char **argv)
{
    ack_queue = createQueue();
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);
    srand(rank);
    /* zob. util.c oraz util.h */
    init_packet_type(); // tworzy typ pakietu
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    changeState(GenJob);

    // komunikacja (przy tawernie nieużywana)
    pthread_create( &com_thread, NULL, start_com_thread , 0);
    // główna pętla (wysyłanie zleceń)
    main_loop();

    finalize();
    return 0;
}

