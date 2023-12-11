/*
    Deric Shaffer
    CS474 - PA1: Signals
    Due Date - Sept. 8th, 2023
    Purpose - Create a simple signal handler routine to demonstrate
        how to set signal handlers
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// signal handler prototypes
void sig1_handler(int sig_num);
void sig2_handler(int sig_num);

int main(){
    // error handling for SIGUSR1
    if(signal(SIGUSR1, sig1_handler) == SIG_ERR){
        printf("Error raised.\n");
        exit(1);
    }

    // error handling for SIGUSR2
    if(signal(SIGUSR2, sig2_handler) == SIG_ERR){
        printf("Error raised.\n");
        exit(1);
    }
    
    // infinite "sleep" loop
    while(1){
        sleep(1);
    }

    return(0);
}

// signal handler definitions

// pre: signal SIGUSR1
// post: print out the date and time of the final exam
void sig1_handler(int sig_num){
    printf("Final Exam will be on December 4th at 8am MST.\n");

    // reset signal
    signal(SIGINT, SIG_DFL);
}

// pre: signal SIGUSR2
// post: print out if there are any conflicts with the author & reset signal SIGUSR2
void sig2_handler(int sig_num){
    printf("Final Exam date has no conflict with the author.\n");

    // reset signal
    signal(SIGINT, SIG_DFL);
}