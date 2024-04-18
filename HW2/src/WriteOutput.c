#include "WriteOutput.h"
pthread_mutex_t mutexWrite = PTHREAD_MUTEX_INITIALIZER;

struct timeval startTime;

void InitWriteOutput()
{
    gettimeofday(&startTime, NULL);
}

unsigned long long GetTimestamp()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return (currentTime.tv_sec - startTime.tv_sec) * 1000000 // second
           + (currentTime.tv_usec - startTime.tv_usec); // micro second
}

void PrintThreadId()
{
    pthread_t tid = pthread_self();
    size_t i;
    printf("ThreadID: ");
    for (i=0; i<sizeof(pthread_t); ++i)
        printf("%02x", *(((unsigned char*)&tid) + i));
    printf(", ");
}


void WriteOutput(int carID, char connector_type, int connectorID, Action action) {
    unsigned long long time = GetTimestamp();
    pthread_mutex_lock(&mutexWrite);



    PrintThreadId();
    printf("CarID: %d, Object: %c%d, time stamp: %llu, AID: %d ", carID, connector_type, connectorID, time, (int)action);
        switch (action) {
            case TRAVEL:
                printf("traveling to connector.\n");
                break;
            case ARRIVE:
                printf("arrived at connector.\n");
                break;
            case START_PASSING:
                printf("started passing connector.\n");
                break;
            case FINISH_PASSING:
                printf("finished passing connector.\n");
                break;
            default:
                printf("Wrong argument format.\n");
                break;
        }
    pthread_mutex_unlock(&mutexWrite);
}
