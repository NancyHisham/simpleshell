#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> // for errno variable
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>
#define MAXARGS 100
FILE *fp;
int parse_command(char* command, char *args[]){
    int numOfArgsC = 0;
    bool dollarsign=0;
    for(int i =0 ;i <strlen(command);i++){ //to check the command containing $ sign
        if(command[i]=='$'){
            dollarsign=1;
            break;
        }
    }
    char *token = strtok(command," ");
    while(token != NULL){ //store the split command in argument array
        args[numOfArgsC] = token;
        if(args[numOfArgsC][strlen(args[numOfArgsC])-1] =='\n') //check if the last char is reached
            args[numOfArgsC][strlen(args[numOfArgsC])-1] ='\0'; //then change it with null char
        numOfArgsC++;
        token = strtok(NULL," "); //to continue split the args..
    }
    args[numOfArgsC] = NULL;//null in the last of argument array
    return dollarsign;
}
void execute_shell_builtin(char *args[]) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || strcmp(args[1], "~") == 0) {
            chdir(getenv("HOME")); //change the current directory to home path
        } else if (strcmp(args[1], "..") == 0) {
            chdir("..");
        } else if (chdir(args[1]) == -1) {
            printf("cd: %s: %s\n", args[1], strerror(errno));
        } else {
            chdir(args[1]);
        }
    } else if (strcmp(args[0], "echo") == 0) {
        int i = 0, k = 0,j=1;
        char var[1024];
        while(args[j]!=NULL){ //get the variable next to the echo command
            //and store it in var array without double quotes
            if(args[j][i]=='"'&&args[j][i+1]=='\0') break;
            else if (args[j][i]=='"'&&args[j][i+1]!='\0') i++;
            var[k]=args[j][i]; i++,k++;
            if(args[j][i]=='\0') {//if it was the last char
                if(args[j+1]!=NULL){ //and it wasn't the last word
                    var[k] = ' ' ; k++; j++; i=0; //then it must be a space
                } else break;
            }
        }
        var[k]='\0'; //null at the end of the array
        int l=0;
        while(var[l]!='\0'){//print the variable as echo command is used to do
            printf("%c",var[l]); l++;
        }
        printf("\n");
    } else if (strcmp(args[0], "export") == 0) {
        char value[1024], name[1024];
        int k=0,l=0,i=0;
        if(args[1][0]=='"') i++;
        while(args[1][i]!='='){ //store the variable after export command
            //till the char before the = operator in name char array
            name[l] = args[1][i];
            l++; i++;
        }i++;
        name[l]='\0';
        int j=1;
        while(args[j]!=NULL){//starting from the char after = operator
            //till the end of arguments
            if(args[j][i]=='"'&&args[j][i+1]=='\0')break;//to not store " at the end
            else if (args[j][i]=='"'&&args[j][i+1]!='\0') i++;//" at the start of the variable
            //store the characters next to it
            value[k]=args[j][i];
            i++,k++;
            if(args[j][i]=='\0') {//if it is the last char
              if(args[j+1]!=NULL){//and not the last argument
                  value[k] = ' ' ; //it must be a space
                  k++; j++; i=0;
              }else break;
            }
        }value[k]='\0';
        if(setenv(name,value,1)!=0){//set the value to the name
            perror("error");
            exit(1);
        }
    } else if(strcmp(args[0], "exit") == 0){
        printf("terminate the program");
        exit(0);
    }
}
void proc_exit(){
    int wstat;
    pid_t pid;
    while (true) {
        pid = wait3 (&wstat, WNOHANG, (struct rusage *)NULL );
        if (pid == 0||pid==-1) return;
        else{
            fp = fopen("output.txt", "a");
            fprintf(fp, "%s", "child is zombie\n");
            fclose(fp);
        }
    }
}
void execute_command(char *args[], bool background) {
    signal (SIGCHLD, proc_exit); //signal to tell the parent the child exit
    pid_t pid = fork(); // create a child process
    if (pid == -1) { // fork failed
        perror("fork");
        exit(1);
    } if (pid == 0) { // child process
        if (execvp(args[0], args) == -1) { // execute the command
            exit(1); //if execvp return 1 then error
        }
    } else { // parent process
        if (!background) { // wait for child process to finish
            int status;
            waitpid(pid, &status, 0);
            fp = fopen("output.txt", "a");
            fprintf(fp, "child ");
            fprintf(fp, "%d", pid);
            fprintf(fp, " is terminated\n");
            fclose(fp);
            if (WIFEXITED(status)) {//for the correct command
                printf("Child process exited with status %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {//for false command
                printf("Child process terminated by signal %d\n", WTERMSIG(status));
            }
        }
    }
}
void evaluate_expression(char *args[]){
    char var[1024]; int k = 0, i=0, j=0;
    while(args[i]!=NULL){
        if(args[i][j]=='$'){ //check if the command has the $ value is this index
            j++; //reach the first index of the variable after $ sign
            while(args[i][j]!='\0'){ //loop till the last char of the variable
                if(args[i][j]!='"'){ //don't store "
                    var[k]=args[i][j]; //store the variable char in var char array till the end of variable
                    j++;k++;
                }
                else {j++;} //move to the next char
            }var[k]='\0'; //store the null char at the end of the array
            args[i]=getenv(var);//replace the $ and variable followed,by the value of the var
            i++;k=0;j=0;
        }else if(args[i][j+1]=='\0'){ //if the last char in the argument is reached then move to the next argument
            i++;j=0;
        }else j++;//move the next char
    }
    char* str = args[1];
    if(strcmp(args[0],"echo")!=0){//handle if the argument have space in all commands except echo
        char *token = strtok(str," ");//separate the str by the space
        int h=1;
        while(token != NULL){
            args[h] = token; // distribute the argument in the next indexes
            if(args[h][strlen(args[h])-1] =='\n')//if the last char in last argument is reached
                args[h][strlen(args[h])-1] ='\0'; //then put the null char
            h++;
            token = strtok(NULL," "); //to continue split the args..
        }
        args[h] = NULL; //null in the last of the argument array
    }
}
void shell(char** args, char command[]){
    int dollarsign = parse_command(command, args);//dollarsign if it is > 0 then there is a $ in the command o.w no $ in the command
    if(dollarsign)
        evaluate_expression(args); //if the & in the command then evaluate the expression and return the accurate argument.
    if ((strcmp(args[0], "cd") == 0|| (strcmp(args[0], "echo") == 0)|| (strcmp(args[0], "export") == 0)) || (strcmp(args[0], "exit") == 0)){
        //if the command was cd, echo, export, exit: then execute the shell builtin functions
        execute_shell_builtin(args);
    }
    else {
        //if the command was ls, cp, rm …etc
        bool background=0;
        if(args[1]!=NULL) {
            if (args[1][0] == '&') background = true;// if the last of the command is & ,then background = true
            else background = false;
        }
        //execute this command
        execute_command(args,background);
    }
}
void setup_environment(){
    char Current_Working_Dirctory[MAXARGS];
    chdir(getcwd(Current_Working_Dirctory,strlen(Current_Working_Dirctory)));
}
int main() {
    setup_environment(); //initializing the shell environment
    while(true){
        char command[1024];
        char *args[MAXARGS];
        printf("Enter a command: ");
        fgets(command, sizeof(command), stdin);
        shell(args, command);
    }
}