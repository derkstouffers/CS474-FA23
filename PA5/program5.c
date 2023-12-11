/*
    Deric Shaffer
    CS474 - Programming Assignment 5
    Due Date - Nov. 12th, 2023
    Purpose - Mimic process management within physical memory
*/

//      Garbage Collection Explanation
/*
    The strategy consists of compacting free bocks with the use of the compact()
    function whenever memory is freed during the execution of a STOP command. And
    then we coalesce those free blocks via the free_up() function.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 10

// matrix that acts as the process table
// [process][size]
int process_table[MAX_PROCESSES][2];

// prototype(s)
void parse(char* line, char* args[]);
void manage(char* args[], int* arr);
void free_up(int pid);
void compact();
void dump();
void dump_free();

int allocate(int size);

// global variables
int* arr;
int total, time, free_pointer;

int main(){
    // variables
    char line[200];
    char temp[100]; // house RAM command, isn't used afterward
    char* args[100]; // args[0] = command, args[1] = process, args[2] = size/address

    int ram_size;
    
    // ask for RAM size
    scanf("%s %d", temp, &ram_size);

    // create memory array
    arr = (int*) malloc(ram_size * sizeof(int));

    arr[0] = ram_size;
    arr[1] = -1;

    free_pointer = 0;
    total = ram_size;

    // set processes to inactive
    for(int i = 0; i < MAX_PROCESSES; i++)
        process_table[i][0] = -1;

    while(1){
        // get time
        scanf("%d", &time);

        // read in a line
        scanf("%199[^\n]", line);

        // parse input line
        parse(line, args);

        // run command entered
        manage(args, arr);
    }

    // free allocated memory
    free(arr);

    // implicit return
    return 0;
}



// pre: line entered during prompt and array that will house all of the tokens
// post: args array has tokens inside args[0] = command, args[1] = process, args[2] = size/location
void parse(char* line, char* args[]){
    // variables
    int i = 0;
    char* token;

    token = strtok(line, " ");

    while(token != NULL){
        // append token into args array
        args[i] = token;
        
        i++;

        // move onto next token
        token = strtok(NULL, " ");
    }

    // make last token in args array NULL
    args[i] = NULL;
}



// pre: command that was inputted (args[]) and memory that is to be managed 
// post: command entered is executed and depending on what command it is,
//      will be done to memory
void manage(char* args[], int* arr){
    if(strcmp(args[0], "START") == 0){
        char* process = args[1];
        int pid = process[1] - '0' - 1;
        int size = atoi(args[2]);

        if(process_table[pid][0] != -1)
            printf("Process %s is already running.\n", process);
        
        if(total < size){
            printf("Can't start Process %s with size %d with only %d available.\n", process, size, total);
            compact();
        }
        
        // start process
        process_table[pid][0] = allocate(size);
        process_table[pid][1] = size;

        printf("Process %s started at time %d with size %d at location %d.\n", process, time, size, process_table[pid][0]);
    
    }else if(strcmp(args[0], "LOC") == 0){
        // give physical location of process a virtual address
        char* process = args[1];
        int pid = process[1] - '0' - 1;
        int location = atoi(args[2]);

        if(process_table[pid][0] == -1)
            printf("Process %s is inactive.\n", process);
        else{
            if(location >= process_table[pid][1])
                printf("Location %d for Process %s is Out of Bounds.\n", location, process);
            else
                printf("Process %s Location is %d.\n", process, (process_table[pid][0] + location));
        }
        
    }else if(strcmp(args[0], "DUMP") == 0){
        // give base/bounds of all active processes
        dump();

    }else if(strcmp(args[0], "DUMP_FREE") == 0){
        // gives list of blocks that are free
        dump_free();

    }else if(strcmp(args[0], "STOP") == 0){
        // stop process
        char* process = args[1];
        int pid = process[1] - '0' - 1;

        if(process_table[pid][0] == -1)
            printf("Process %s is inactive.\n", process);
        
        // free up memory and stop process
        free_up(pid);

    }else if(strcmp(args[0], "END") == 0){
        printf("End of Simulation.\n");

        // show final base/bounds of all active processes
        dump();

        printf("\n");

        // show final list of free blocks
        dump_free();

        exit(0);
    }
}



// pre: uses the global memory array and assumes free_pointer points to the beginning of the list
// post: merges contiguous free blocks
void compact(){
    // variables
    int active[MAX_PROCESSES];
    int num_active = 0;
    int current = free_pointer;
    int offset = 0;

    // populate active array
    for(int i = 0; i < MAX_PROCESSES; i++)
        if(process_table[i][0] != -1)
            active[num_active++] = i;
    
    // move active processes to close gaps left by freed memory
    for(int i = 0; i < num_active; i++){
        int pid = active[i];
        int old = process_table[pid][0];
        int new = free_pointer;

        // move process to new location
        for(int j = 0; j < process_table[pid][1]; j++)
            arr[new + j] = arr[old + j];
        
        // update process table w/ new location
        process_table[pid][0] = new;
    }

    // update base addresses of free blocks
    while(current != -1){
        arr[current] -= offset;
        current = arr[current + 1];
        offset += arr[current - 1];
    }

    // update free pointer
    free_pointer = arr[free_pointer];
}



// pre: free up process, place onto free list
// post: process table & total are updated, array gets coalesced if possible
void free_up(int pid){
    if(process_table[pid][0] == -1){
        printf("Can't free up Process %d because it is inactive.\n", pid + 1);
        return;
    }

    // variables
    int start = process_table[pid][0];
    int size = process_table[pid][1];
    int current = free_pointer;
    int prev = -1;
    int done = 0;

    // increase total by size of freed memory
    total += size;

    // set process to inactive
    process_table[pid][0] = -1;
    process_table[pid][1] = -1;

    // set current to be where the freed block should be inserted
    while (current != -1 && current < start) {
        prev = current;
        current = arr[current + 1];
    }

    // insert the freed block
    arr[start] = size;

    if (prev == -1){
        // freed block is to be inserted at the start
        arr[start + 1] = free_pointer;
        free_pointer = start;
    }else{
        arr[start + 1] = arr[prev + 1];
        arr[prev + 1] = start;
    }

    // coalesce free blocks
    while(!done){
        int prev_c = -1;
        int look = free_pointer;

        while(look != -1){
            if(arr[look + 1] == -1){
                done = 1;
                break;
            }

            // non contiguous
            if((look + arr[look]) != arr[look + 1]){
                prev_c = look;
                look = arr[look + 1];
            }else{
                arr[look] += arr[look + 1];
                arr[look + 1] = arr[arr[look + 1] + 1];

                if(prev_c == -1)
                    free_pointer = look;
            }
        }
    }

    printf("Process %d freed at time %d. Memory allocated at location %d.\n", pid + 1, time, start);
}



// pre: size of memory array, access to memory array
// post: memory block of specified sizeis allocated and it returns the start index of the block
int allocate(int size){
    // variables
    int current = free_pointer;
    int prev = -1;

    while(current != -1){
        if(arr[current] >= size){
            // found a free block
            if(arr[current] == size){
                // entire block gets allocated
                if(prev == -1)
                    free_pointer = arr[current + 1];
                else
                    arr[prev + 1] = arr[current + 1];
            }else{
                // allocate a portion of the block and update free block
                int leftover = arr[current] - size;
                int new_block_start = current + size;

                if(prev == -1)
                    free_pointer = new_block_start;
                else
                    arr[prev + 1] = new_block_start;

                arr[new_block_start] = leftover;
                arr[new_block_start + 1] = arr[current + 1];
            }

            total -= size;
            return current;
        }
        
        prev = current;
        current = arr[current + 1];
    }

    // couldn't find a suitable free block, so compact and try again
    compact();
    return allocate(size);
}



// pre: n/a
// post: prints out the base and bounds of every active process in the process table
void dump(){
    printf("%20s %10s %10s\n", "PROCESS", "START", "END");

    for(int i = 0; i < MAX_PROCESSES; i++){
        if(process_table[i][0] != -1){
            printf("%20d %10d %10d\n", i + 1, process_table[i][0], process_table[i][0] + process_table[i][1]);
        }
    }
}



// pre: n/a
// post: displace the start and size of all free memory blocks in memory array
void dump_free(){
    // variables
    int current = free_pointer;

    printf("%20s %10s\n", "START", "SIZE");

    // run through memory array until free block is found
    while(current != -1){
        printf("%20d %10d\n", current, arr[current]);
        current = arr[current + 1];
    }
}