# simpleshell
Simple Shell (Multi-Processing)
Lab 1
problem statement:
It is required to implement a Unix shell program that supports the following commands:
1. "exit" command to terminate the shell using the exit() system call.
2. Commands with no arguments, which block until the command completes and print an error
message if the return code is abnormal. This requires forking a child process, waiting for it
to complete, and synchronous execution using the fork(), execvp(), exit(), and waitpid()
system calls.
3. Commands with arguments where the first argument is the name of the command.
4. Commands, with or without arguments, executed in the background using the & operator.
This requires background execution, signal handling, and asynchronous execution. Child
process termination should be logged in a basic text file, and an interrupt handler should be
implemented to handle the SIGCHLD signal.
5. Shell builtin commands for cd and echo. cd must handle all cases (no spaces in path) for
changing directories, and echo must evaluate all expressions within double quotes.
6. Expression evaluation using the export command to set and print variable values. Variables
can be input as a string without spaces or as a full string inside double quotes.
------------------------------------------------------------------
description of the overall organization of the code and the major functions:
This is a basic implementation of a shell program in C language, which allows users to
execute commands and perform basic shell functionalities. The simple shell code is
typically organized into several modules:
1- main():
- This is the main entry point of the program. It initializes the shell environment and starts an
infinite loop that reads user input and executes the corresponding command.

2- setup_environment():
- It is responsible for initializing the shell environment. It is typically called from the main()
function before the shell starts its main loop.

3- shell():
- Shell function is used to check if the command is (cd, echo, export, exit) then execute the
shell built in functions, else execute the command function with background parameter, and
also used to evaluate expression if the command has the dollar sign $.

4- parse_command():
- This function takes a command entered by the user as input and splits it into arguments. It
returns a boolean value indicating whether the command contains the "$" symbol or not.

5- evaluate_expression():
- This function is used to evaluate the expression when the argument has $ sign to replace it
and its followed variable with another saved value when export is used without quotes.

6- execute_shell_builtin():
- It takes an argument array and executes built-in shell commands such as cd, echo, and
export. The cd command changes the current working directory, while the echo command
prints its argument to the console. The export command sets an environment variable with 
a given name and value.

7- execute_command():
- The function begins by registering a signal handler for SIGCHLD (child process exited)
using the signal() function.
- This signal will be used to notify the parent process when the child process exits.
- The function forks a new child process using the fork() function.
- If the current process is the parent process, the function checks if the background
parameter is false to make the parent waits for the child process to finish executing
using waitpid().
Finally, the function checks the status of the child process using the WIFEXITED() and
WIFSIGNALED() If the child process exited normally, the function prints a message
indicating the exit status. If the child process was terminated by a signal, the function
prints a message indicating the signal number.

8- proc_exit():
- that is registered to handle the SIGCHLD (child process exited) signal. When a child process
terminates, the operating system sends this signal to the parent process, allowing it to handle the
child's exit status. It uses the wait3() system call to wait for any child processes to exit. The
WNOHANG option tells wait3() to return immediately if no child processes have exited yet.
wait3() returns the process ID of the child process that exited or 0 if no child processes have exited
yet. If wait3() returns -1, an error occurred, else then this child is zombie.
------------------------------------------------------------------
Sample run:

exit with statue 1 means => error

exit with statue 0 means => normal

![image](https://user-images.githubusercontent.com/96303049/225118362-d752bb90-dea3-44b5-a90d-db8d26ced038.png)
![image](https://user-images.githubusercontent.com/96303049/225118401-6a84414b-ce4d-4690-a8a5-ceff29c941c8.png)
![image](https://user-images.githubusercontent.com/96303049/225118425-1bc7cd63-3ad1-4478-ae42-b3714f677145.png)
![image](https://user-images.githubusercontent.com/96303049/225118446-5da65df5-0a62-4209-b980-4e1d5584c099.png)
![image](https://user-images.githubusercontent.com/96303049/225118462-509d38c8-9068-42e7-aaf2-56231804f9e9.png)
![image](https://user-images.githubusercontent.com/96303049/225118481-03634e6f-7089-4c7c-b147-22e2f97e3eae.png)
![image](https://user-images.githubusercontent.com/96303049/225118500-ee907782-48d0-4b8e-a01b-d78871970b2d.png)

After terminating the Firefox

![image](https://user-images.githubusercontent.com/96303049/225118528-8dab4f14-fc53-418b-a222-ba179f2683f5.png)
![image](https://user-images.githubusercontent.com/96303049/225118553-a0ebb9bf-574f-4109-8a17-0eb384777e3e.png)
![image](https://user-images.githubusercontent.com/96303049/225118573-b541490c-620e-4595-8dc7-9464be239de2.png)

After terminating the Firefox

![image](https://user-images.githubusercontent.com/96303049/225118645-6da407b0-323e-413c-96cb-577673ce86f9.png)
![image](https://user-images.githubusercontent.com/96303049/225118672-ed05c820-fb16-4ff7-8135-61d403a87ce7.png)
![image](https://user-images.githubusercontent.com/96303049/225118695-e79c781d-98ac-41e1-86bd-462b5c1f0103.png)
