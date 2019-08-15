#include <stdio.h>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <stdbool.h>
#include <time.h>

#include "msg.h" //definitions of message types


class client{
public:
  client();
  ~client();

  void send_message(message m);
  message recieve_message();



private:
  int client_PID;
  int server_initial_pipe;
  int client_send_pipe,client_recv_pipe;

  //holds names of send and recieve pipes
  char send_pipe[12]; //pid is up to 5 digits, underscore, "send" and termination
  char recv_pipe[12]; //using recv instead of "recieve", since it's also 4 chars


};


//constructor responsible for connecting to the server
client::client()
{

  client_PID = getpid(); // who am I
  printf("v08client process started with PID %d\n", client_PID);


//before writing to the server_np, open this process's pipes, i.e. <PID>_send and <PID>_recv
//avoids a race condition, makes sure that the pipes are set up BEFORE the
//forked server process tries to used them to communicate with this process


  //create the two pipes to allow bidirectional client-server communication
  sprintf(send_pipe,"%d_send",client_PID);
  sprintf(recv_pipe,"%d_recv",client_PID);

  mkfifo(send_pipe, 0600);
  mkfifo(recv_pipe, 0600);

  //initalize the message
  message m_init;

  m_init.PID = client_PID;
  m_init.type = JOIN;

  printf("about to connect\n");

  //make sure to follow the pattern open-write-close
  //open the pipe that allows the initial contact between client and server
  server_initial_pipe = open("server_np", O_WRONLY);
  //write a message to server with the client's PID, for further communication
  write(server_initial_pipe,(char*)&m_init,sizeof(message));
  //close the initial pipe
  close(server_initial_pipe);

  printf("finished with connection to server\n");

  //we don't need server_initial_pipe again - now using two pid-specific pipes
  //with names stored in member variables send_pipe and recv_pipe

}


//destructor responsible for disconnecting from the server
client::~client()
{

  //send message with type LEAVE

  //unlink the send and recieve pipes
  std::cout << "\n...unlinking pipes";

  unlink(send_pipe);
  unlink(recv_pipe);

  std::cout << "\rpipes unlinked        " << std:: endl;

}
