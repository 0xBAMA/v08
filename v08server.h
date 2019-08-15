#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <stdbool.h> //didn't realize bools required a header

#include <time.h>


#include "msg.h" //definitions of message types


typedef struct user_t
{
  int PID;

  // time type here - from before

  //  time of connection;
  //  time of last operation;

  char initial_message_text[MSG_LENGTH]; //taken as name

  char client_send_str[12]; //holds the name of the <PID>_send pipe
  char client_recv_str[12]; //holds the name of the <PID>_recv pipe

}user;




class server{
public:

  server();
  ~server();

  void add_a_client(int PID);
  void delete_client(int PID);

  // remove users that haven't done anything in a while (60 minutes?)
  void remove_inactive_users(); //time since last operation > 60 min


  int server_main(int argc, char const *argv[]);


  int listen();

  void read_from_server_np(); //listen found activity on server_np

  void read_from_pid(int pid); //listen found activity related to
  void send_to_pid(int pid);  //for replies



private:

  const char * np = "server_np";

  int fd;
  int server_PID;

  std::vector<user> users;

};




server::server()
{
   server_PID = getpid();



   std::cout << "v08server process started with PID " << server_PID << std::endl;



   // set up select for the first time, with the only entry being the server_np




}



server::~server()
{

  std::cout << "server exiting" << std::endl;

  std::cout << " unlinking server_np";
  unlink(np); //get rid of the fifo
  std::cout << "\r server_np unlinked     " << std::endl;

}


int server::listen()
{
  std::cout << "listening" << std::endl;

  read_from_server_np();

  return 0;
}




void server::read_from_server_np()
{


  char client_send_str[12];//holds the name of the <PID>_send pipe
  char client_recv_str[12];//holds the name of the <PID>_recv pipe

  message m;


  mkfifo(np, 0600); //owner has read and write permissions, group and other have none

  fd = open(np, O_RDONLY); //open the pipe in read only mode


  std::cout << "waiting";
  read(fd,(char*)&m,sizeof(message)); //blocking read waits for a message
  close(fd); //close the file descriptor, now that we have a message from a client
  //make sure to follow the pattern open-read-close to avoid complications

  int client_PID = m.PID;
  std::cout << "\rrecieved message from PID " << m.PID; //get the PID from the message

  switch(m.type)
  {
    case JOIN:
      std::cout << " asking to join" << std::endl;
      break;
  }

  sprintf(client_send_str,"%d_send",client_PID);
  sprintf(client_recv_str,"%d_recv",client_PID);



  message r;



  sprintf(r.message_text, "you are good, number %d", client_PID);

  std::cout << r.message_text << std::endl;


  int response = open(client_recv_str, O_WRONLY);


  write(response, (char*)&r, sizeof(message));
  close(response);



}





int server::server_main(int argc, char const *argv[])
{
  return listen();
  return 1;
}
