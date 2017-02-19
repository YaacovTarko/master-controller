//master_controller.cpp
//this program creates a process for each program that needs to run on the ground control station (MAVProxy, qGroundControl and our interop client).
//it creates pipes to allow those processes to communicate with this process, and passes data back and forth between the process

#include <errno.h>  //for perror
#include <stdio.h>  //for perror and other i/o
#include <string>
#include <unistd.h>     //for fork, pipe, execvp, dup2

//PROGRAMNAME_IN/OUT - file descriptor passed to new process
//TO/FROM_PROGRAMNAME - file descriptor used in this process to communicate with new process

#define INTEROPCLIENT_IN 0
#define TO_INTEROPCLIENT 1
#define FROM_INTEROPCLIENT 2
#define INTEROPCLIENT_OUT 3

#define MAVPROXY_IN 4
#define TO_MAVPROXY 5
#define FROM_MAVPROXY 6
#define MAVPROXY_OUT 7

#define QGROUNDCONTROL_IN 8
#define TO_QGROUNDCONTROL 9
#define FROM_QGROUNDCONTROL 10
#define QGROUNDCONTROL_OUT 11


void launch_process(std::string path, std::string args, int* pipe_fd_array, int pipe_in_index, int pipe_out_index) {
  pid_t process_id = fork();
  if (process_id == -1) {
    perror("Fork failed in launch_process");
  }
  if (process_id == 0) { // if this is the child process
  	//Set the pipes through which the child process will communicate with this program as stdin/stdout
  	if(dup2(pipe_fd_array[pipe_in_index], 0)==-1) perror(path.c_str());
  	if(dup2(pipe_fd_array[pipe_out_index], 1)==-1) perror("Dup2 failed in launch_process"); 

  	//close all other file descriptors to avoid hanging on open pipes
  	int i=0;
  	while(pipe_fd_array[i]>=0) close(pipe_fd_array[i]);

  	//execvp takes a c string as the path of the program to execute, and an array of c strings as the arguments passed to the program that will be executed
    if (execl(path.c_str(), args.c_str() ) == -1) {
      perror("Exec failed in launch_process");
    }

  }
  return;
}

// creates pipe for interprocess communication. Won't work with execvp
int *create_pipe() {
  int *file_designators = new int[2];
  if (pipe(file_designators)) {
    perror("Pipe failed in create_pipe");
  }
  return file_desi
int main(int argc, char *argv[]) {
  // create pipes to pass data between this process and its children
	const int num_pipes=6; 
	int pipe_file_descriptors[num_pipes*2+1];	
	for(int i=0; i<num_pipes; i++){
		int* new_fds = create_pipe();
		pipe_file_descriptors[2*i]= new_fds[0];
		pipe_file_descriptors[2*i+1]= new_fds[1]; 
		delete new_fds; 
	}	
	//set final index in the array to -1, so that the launch_process function can tell that it's finished

	//add the paths to the programs that will run. These should be changed obviously 
	std::string interop_client_path = "client";
	std::string qgroundcontrol_path = "qgc";
	std::string mavproxy_path = "mp"; 

  // these can be used to pass arguments to the programs that will run
  std::string interop_client_args;
  std::string qgroundcontrol_args;
  std::string mavproxy_args; 

  // create new processes for the interop client system and mission planner
  launch_process(interop_client_path, interop_client_args, pipe_file_descriptors, INTEROPCLIENT_IN, INTEROPCLIENT_OUT);
  launch_process(qgroundcontrol_path, qgroundcontrol_args, pipe_file_descriptors, QGROUNDCONTROL_IN, QGROUNDCONTROL_OUT);
  launch_process(mavproxy_path, mavproxy_args, pipe_file_descriptors, MAVPROXY_IN, MAVPROXY_OUT); 

  // clean up and exit
  return 0;
}
