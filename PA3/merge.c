/*
    Deric Shaffer
    CS474 - Programming Assignment 3
    Due Date - Oct. 15th, 2023
    Purpose - Create Merge Sort and determine the runtime for different
        array sizes in nanoseconds
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// prototypes
void merge(int pos1, int pos2, int pos3, int pos4);
void sort(int l, int r);

// arrays
int *normal_in;
int *normal_out;

int main(int argc, char* argv[]){
    // variables
    struct timespec start_time, end_time;

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
    normal_in = (int*) malloc(sizeof(int) * arr_size);
    normal_out = (int*) malloc(sizeof(int) * arr_size);

    // fill in array
    for(int i = 0; i < arr_size; i++)
        normal_in[i] = (rand() % 1000) + 1;
    
    // test that array fill works
    // for(int i = 0; i <= arr_size; i++)
    //     printf("%d ", normal_in[i]);
    // printf("\n");
    
    // start time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // call sort
    sort(0, arr_size);

    // end_time
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // print out array just to see if it is sorted
    // for(int i = 0; i <= arr_size; i++)
    //     printf("%d ", normal_in[i]);
    // printf("\n");

    // calculate nanoseconds
    long long x = end_time.tv_sec - start_time.tv_sec;
    long long x_nano = end_time.tv_nsec - start_time.tv_nsec;
    long long res = (x * 1000000000) + x_nano;

    printf("Normal Mergesort Took: %lld ns\n", res);
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
        if(normal_in[i] > normal_in[j])
            normal_out[l++] = normal_in[j++];
        else
            normal_out[l++] = normal_in[i++];
    }

    // deal with remainders in the 2 segments
    for(int k = i; k <= pos2; k++)
        normal_out[l++] = normal_in[k];

    for(int k = j; k <= pos4; k++)
        normal_out[l++] = normal_in[k];
    
    // move merged back to original array
    for(i = pos1; i <= pos4; i++)
        normal_in[i] = normal_out[i];
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