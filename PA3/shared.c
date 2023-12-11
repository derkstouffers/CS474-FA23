/*
    Deric Shaffer
    CS474 - Programming Assignment 3
    Due Date - Oct. 15th, 2023
    Purpose - Create Merge Sort and determine the runtime for different
        array sizes in nanoseconds by creating 8 processes instead of 2
*/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

// prototypes
void merge(int pos1, int pos2, int pos3, int pos4);
void sort(int l, int r);
void split(int l, int r, int level);

// arrays & global var.
int *shared_in;
int *shared_out;
int share_key_in;

int main(int argc, char* argv[]){
    // variables
    struct timespec start_time, end_time;
    struct shmid_ds item;
    
    // check if user inputted an array size
    if(argc < 2){
        printf("Missing an argument for array size\n");
        exit(0);
    }

    // get array size from terminal
    int arr_size = atoi(argv[1]);

    // random number generator
    srand(time(NULL));

    // allocate memory for temporary array
    shared_out = (int*) malloc(sizeof(int) * arr_size);

    // define shared memory
    if((share_key_in = shmget(IPC_PRIVATE, arr_size * sizeof(int), IPC_CREAT|0666)) < 0)
        perror("Can't Get Shared Memory");

    // attach shared memory
    if((shared_in = shmat(share_key_in, (void*) 0, SHM_RND)) == (void*) -1)
        perror("Can't Attach To Shared Memory");

    // fill in array
    for(int i = 0; i < arr_size; i++)
        shared_in[i] = (rand() % 1000) + 1;

    // test that array fill works
    // for(int i = 0; i <= arr_size; i++)
    //     printf("%d ", shared_in[i]);
    // printf("\n");

    // start time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // no need to split if array is <= 8
    if(arr_size <= 8){
        sort(0,arr_size);
        shmctl(share_key_in, IPC_RMID, &item);
    }else{
        // level = 3 --> 2^3 processes = 8
        split(0, arr_size, 3);
        shmctl(share_key_in, IPC_RMID, &item);
    }

    // end time
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // test that array actually sorted
    // for(int i = 0; i <= arr_size; i++)
    //     printf("%d ", shared_in[i]);
    // printf("\n");

    // calculate nanoseconds
    long long x = end_time.tv_sec - start_time.tv_sec;
    long long x_nano = end_time.tv_nsec - start_time.tv_nsec;
    long long res = (x * 1000000000) + x_nano;

    printf("Shared Mergesort Took: %lld ns\n", res);
}

// pre: start and end positions of the 2 segments of the array that need to be sorted
// post: combines the segments that were sorted in the sort function
void merge(int pos1, int pos2, int pos3, int pos4){
    int i, j, l;

    l = pos1;
    i = pos1;
    j = pos3;

    // merge sublists
    while((i <= pos2) && (j <= pos4)){
        if(shared_in[i] > shared_in[j])
            shared_out[l++] = shared_in[j++];
        else
            shared_out[l++] = shared_in[i++];
    }

    // deal with remainders in the 2 segments
    for(int k = i; k <= pos2; k++)
        shared_out[l++] = shared_in[k];

    for(int k = j; k <= pos4; k++)
        shared_out[l++] = shared_in[k];
    
    // move merged back to original array
    for(i = pos1; i <= pos4; i++)
        shared_in[i] = shared_out[i];
}

// pre: start and end position of array
// post: array gets sorted
void sort(int l, int r){
    int mid;

    // base case
    if(l >= r)
        return;
    
    else{
        // update middle
        mid = (l + r) / 2;

        // sort left
        sort(l, mid);

        // sort right
        sort(mid + 1, r);

        // merge together
        merge(l, mid, mid + 1, r);
    }
}

// pre: provided the start and end to the array and level, which signifys the number of processes you want (2^level)
// post: creates those processes and splits the array to segments to be sorted
void split(int l, int r, int level){
    int status;
    int mid = (l + r) / 2;
    pid_t child;


    if(level <= 0){
        sort(l, r);
        return; 
    }

    child = fork();

    if(child < 0){
        perror("Fork Failed");
        exit(0);
    }

    if(child == 0){
        // attach to shared memory, for some reason having this causes perror to show up multiple times on arr_size = 2
        if((shared_in = shmat(share_key_in, (void*) 0, SHM_RND)) == (void*) -1)
            perror("Can't Attach To Shared Memory");
        
        // child does right-hand side of array
        split(mid + 1, r, level - 1);

        exit(0);
    }else{
        // parent does left-hand side of array
        split(l, mid, level - 1);
        waitpid(child, &status, 0);
        merge(l, mid, mid + 1, r);
    }
}