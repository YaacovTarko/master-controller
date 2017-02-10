#include <errno.h>  //for perror
#include <stdio.h>  //for perror and other i/o
#include <string>
#include <unistd.h>     //for fork, pipe, execvp, dup2

void launch_process(std::string path, std::string args, int *input_pipe,
                    int *output_pipe) {
  pid_t process_id = fork();
  if (process_id == -1) {
    perror("Fork failed in create_process");
  }
  if (process_id == 0) { // if this is the child process
  	//Set the pipes through which the child process will communicate with this program as stdio
  	dup2(*input_pipe, 0);
  	dup2(*output_pipe, 1); 

    if (execvp(path, args) == -1) {
      perror("Exec failed in create_process");
    }
  }
  return;
}

// creates pipe for interprocess communication. Won't work with execvp
int *create_pipe() {
  int *file_designators = new int[2];
  if (!pipe(file_designators)) {
    perror("Pipe failed in create_pipe");
  }
  return file_designators;
}

int main(int argc, char *argv[]) {
  // create pipes to pass data between this process and its children
  int *pipe_to_mission_planner = create_pipe();
  int *pipe_from_mission_planner = create_pipe();
  int *pipe_to_interop_system = create_pipe();
  int *pipe_from_interop_system = create_pipe();
  // these can be used to pass args to the client
  std::string interop_client_args;
  std::string mission_planner_args;

  // create new processes for the interop client system and mission planner
  // launch_process("pathname_of_interop_client", interop_client_args, pipe_to_mission_planner[0], pipe_from_mission_planner[1]);
  // launch_process("pathname_of_mission_planner", mission_planner_args, pipe_to_interop_system[0], pipe_from_interop_system[1]);

  // clean up and exit
  delete pipe_to_interop_system;
  delete pipe_from_interop_system;
  delete pipe_to_mission_planner;
  delete pipe_from_mission_planner;
  return 0;
}
