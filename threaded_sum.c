#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <errno.h> 
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>



typedef struct _thread_data_t {
    const int *data;
    int startInd;
    int endInd;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;

int readFile(char[], int []);
void* arraySum(void* input);


int main(int argc, char* argv[]) {
    //char array1[MAX_Words];
    //int array2[maxInput];
    //char filename = argv[1];
    

    if(argc != 3){
        printf("There are not enough parameters please try again");
        return -1;
    }
    int threadcount = atoi(argv[2]); //change to atoi to make it from a string to an integer

    int data[1000000];
    char filename[100];
    strcpy(filename, argv[1]); //had to use this to get the file name
    printf("printing filename: %s\n", filename);
    int numNumbers = readFile(filename, data);
    if(numNumbers == -1){
        printf("Too many threads requested\n");
        return -1;
    }
    else if(threadcount > numNumbers){
        printf("Too many threads requested\n");
        return -1;
    }

    long long int totalSum = 0;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);


    thread_data_t threadData[threadcount];
    pthread_t threads[threadcount];
    int chunkSize = numNumbers / threadcount;

    for(int i = 0; i < threadcount; i++){
        threadData[i].data = data;
        threadData[i].startInd = i * chunkSize; //if thread 0 starts then thread 1 starts where thread 0 ends
        if( i == threadcount-1){
            threadData[i].endInd = numNumbers;
        }else{
            threadData[i].endInd = (1+i)*chunkSize;
        }
        threadData[i].lock = &lock;
        threadData[i].totalSum = &totalSum;
    }

    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL); //gets start time for the threads

    for(int i = 0; i < threadcount; i++){
        pthread_create(&threads[i], NULL, arraySum, &threadData[i]);
    }
    for(int i = 0; i < threadcount; i++){
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&endTime, NULL); //gets end time after threads are done

    double finishedTime= (endTime.tv_sec - startTime.tv_sec) *1000.0 + (endTime.tv_usec - startTime.tv_usec) / 1000.0;
    printf("Total Sum: %lld", totalSum);
    printf("\n Total Time: %f ms\n", finishedTime);

    return 0;
}

int readFile(char filename[], int array[]) {
    //printf("printing filename again!!: %s\n", filename);
    FILE *file;
    file = fopen(filename, "r");
    if(file == NULL){
        perror("File could not be opened\n");
        return -1;
    }
    int nums = 0;
    while( fscanf(file, "%d", &array[nums]) == 1){
        nums++;
    }
    fclose(file);
    return nums;
}

void* arraySum(void* input){
    thread_data_t* data = (thread_data_t*)input;
    long long int threadSum = 0;
    for (int i = data->startInd; i < data->endInd; i++){
        threadSum += data->data[i];
    }
    pthread_mutex_lock(data->lock);
    *(data->totalSum) += threadSum; //pointer makes it work dont remove
    pthread_mutex_unlock(data->lock);
    return NULL; 
}

// to initialize this code do ./threaded_sum data.txt 4
// data.txt is the txt file and 4 is the num of threads