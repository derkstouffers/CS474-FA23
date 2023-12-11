/*
    Deric Shaffer
    CS474 - Programming Assignment 3
    Due Date - Oct. 15th, 2023
    Purpose - Create Merge Sort and determine the runtime for different
        array sizes in nanoseconds by using threads
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// struct definition
typedef struct {int val1, val2, val3;} input;

// prototypes
void merge(int pos1, int pos2, int pos3, int pos4);
void sort(int l, int r);
void split(input* args);

// arrays
int *thread_in;
int *thread_out;

int main(int argc, char* argv[]){
    // variables
    struct timespec start_time, end_time;
    input params;

    // check if user inputted an array size
    if(argc < 2){
        printf("Missing an argument for array size\n");
        exit(1);
    }

    // get array size from terminal
    int arr_size = atoi(argv[1]);

    // random number generator
    srand(time(NULL));

    // allocate memory for array
    thread_in = (int*) malloc(sizeof(int) * arr_size);
    thread_out = (int*) malloc(sizeof(int) * arr_size);

    // fill in array
    for(int i = 0; i < arr_size; i++)
        thread_in[i] = (rand() % 1000) + 1;

    // test that array fill works
    // for(int i = 0; i <= arr_size; i++)
    //     printf("%d ", thread_in[i]);
    // printf("\n");

    // create input for function call
    params.val1 = 0;
    params.val2 = arr_size;
    params.val3 = 3;

    // start time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // call split
    split(&params);

    // end time
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // test that array actually sorted
    // for(int i = 0; i <= arr_size; i++)
    //     printf("%d ", thread_in[i]);
    // printf("\n");

    // calculate nanoseconds
    long long x = end_time.tv_sec - start_time.tv_sec;
    long long x_nano = end_time.tv_nsec - start_time.tv_nsec;
    long long res = (x * 1000000000) + x_nano;

    printf("Threaded Mergesort Took: %lld ns\n", res);
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
        if(thread_in[i] > thread_in[j])
            thread_out[l++] = thread_in[j++];
        else
            thread_out[l++] = thread_in[i++];
    }

    // deal with remainders in the 2 segments
    for(int k = i; k <= pos2; k++)
        thread_out[l++] = thread_in[k];

    for(int k = j; k <= pos4; k++)
        thread_out[l++] = thread_in[k];
    
    // move merged back to original array
    for(i = pos1; i <= pos4; i++)
        thread_in[i] = thread_out[i];
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
void split(input* args){
    // variables
    int status, x;
    int l = args->val1;
    int r = args->val2;
    int level = args->val3;
    int mid = (l + r) / 2;


    pthread_t t;

    // create input structures for left and right-hand side
    input lhs, rhs;
    
    lhs.val1 = l;
    lhs.val2 = mid;
    lhs.val3 = level - 1;

    rhs.val1 = mid + 1;
    rhs.val2 = r;
    rhs.val3 = level - 1;

    // parent does left side, child does right side
    if(level <= 0){
        sort(l, r);
        return; 
    }

    // create thread
    if((x = pthread_create(&t, NULL, (void *) split, &rhs)) != 0){
        printf("Can't Create Thread\n");
        exit(1);
    }

    split(&lhs);

    pthread_join(t, (void**) &status);

    merge(l, mid, mid + 1, r);
}