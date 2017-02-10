#include <errno.h>      //for perror
#include <stdio.h>      //for perror and other i/o
#include <stdlib.h>     //for memory management (malloc, free...)
#include <string.h>     //for strcpy, strlen, etc...
#include <sys/socket.h> //for socket
#include <sys/types.h>  //for getpid, connect, ...
#include <sys/stat.h> //for open
#include <unistd.h> //for close, and other stuff too but I'm not sure what
#include <fcntl.h> //for open
#include <string> 


void launch_process(char *path, char *const args[]) {
  pid_t process_id = fork();
  if (process_id == -1) {
    perror("Fork failed in create_process");
  }
  if (process_id == 0) { // if this is the child process
    if (execvp(path, args) == -1) {
      perror("Exec failed in create_process");
      return;
    }
  }
  return;
}

// creates pipe for interprocess communication. Won't work with execvp
int *create_pipe() {
  int* file_designators = new int[2];
  if (!pipe(file_designators)) {
    perror("Pipe failed in create_pipe");
  }
  return file_designators;
}


int main(int argc, char *argv[]) {
	//create pipes to pass data between this process and its children
	int* pipe_to_mission_planner = create_pipe();  
	int* pipe_from_mission_planner = create_pipe();
	int* pipe_to_interop_system = create_pipe();
	int* pipe_from_interop_system = create_pipe(); 
  // these can be used to pass args to the client
  char * interop_client_args;
  char * mission_planner_args;

  // create new processes for the interop client system and mission planner
  // launch_process("pathname_of_interop_client", interop_client_args);
  // launch_process("pathname_of_mission_planner", mission_planner_args);



//clean up and exit
  delete pipe_to_interop_system;
  delete pipe_from_interop_system;
  delete pipe_to_mission_planner;
  delete pipe_from_mission_planner; 
  return 0;
}
