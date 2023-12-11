/*
    Deric Shaffer
    CS474 - PA2
    Due Date - Oct. 1st, 2023
    Purpose - Create a simple shell that takes in commands
        from standard input, and execute commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/stat.h>

// prototype(s)
void parse(char* line, char* args[]);

int main(){
    // variables
    char comm[200]; 
    char* args[100];
    char c;

    int status;
    int fp;
    int mypipe[2];

    pid_t child;

    while (1){       
        // print out prompt
        printf("Enter Command: ");
        fflush(stdout);

        // read in a line
        scanf("%199[^\n]", comm);
        c = getchar();

        // remove newline at end
        if(comm > 0 && comm[strlen(comm) - 1] == '\n')
            comm[strlen(comm) - 1] = '\0';

        // parse the input line
        parse(comm, args);

        child = fork();

        // parent
        if(child != 0){
            // echo
            if(strcmp(args[0], "echo") == 0){
                // check if token after echo is NULL or not
                for(int i = 1; args[i] != NULL; i++)
                    printf("%s ", args[i]);
                
                printf("\n");
            
            // cd
            }else if(strcmp(args[0], "cd") == 0){
                chdir(args[1]);
            
                if (chdir(args[1]) == -1)
                    perror("chdir");
            // exit
            }else if(strcmp(args[0], "exit") == 0)
                exit(0);

                
            waitpid(child, &status, 0);

        // child
        }else{
            // I/O redirection
            for(int i = 0; args[i] != NULL; i++){
                // stdin
                if(strcmp(args[i], "<") == 0){
                    fp = open(args[i+1], O_RDONLY);
                    
                    if(fp == -1)
                        perror("open");

                    if(dup2(fp, STDIN_FILENO) == -1)
                        perror("dup2 for stdin");
                        
                    // set < token to NULL
                    args[i] = NULL;

                    close(fp);

                // stdout
                }else if(strcmp(args[i], ">") == 0){
                    fp = open(args[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0644);

                    if(fp == -1)
                        perror("open");

                    if(dup2(fp, STDOUT_FILENO) == -1)
                        perror("dup2 for stdout");

                    // set > token to NULL
                    args[i] = NULL;

                    close(fp);
                }
            }

            // pwd
            if(strcmp(args[0], "pwd") == 0){
                // size of buffer array
                char buffer[1024];

                if(getcwd(buffer, sizeof(buffer)) != NULL)
                    printf("%s\n", buffer);
                else
                    perror("getcwd");

            // mkdir
            }else if(strcmp(args[0], "mkdir") == 0){
                mkdir(args[1], 0755);

                if(mkdir(args[1], 0755) != 0)
                    perror("mkdir");
            
            // other commands
            }else if(strcmp(args[0], "echo") != 0 && strcmp(args[0], "pwd") != 0)
                execvp(args[0], args);

            // exit child
            exit(0);
        }
    }
}

// pre: line entered during "enter command" prompt and array that houses all of the tokens
// post: args array has tokens
void parse(char* line, char* args[]){
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