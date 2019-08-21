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
#include "voraldo.h"


typedef struct user_t
{
  int PID;


  time_t time_of_connection;
  time_t time_of_last_operation;

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


  int listen();   //to use glut, I think this goes in the timer function

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


//HOW TO OUTPUT TIME - manipulating time_t, is in seconds since Jan 1, 1970
   time_t temp = time(NULL);
   printf ( "The current date/time is: %s", asctime ( localtime ( &temp )) );



   // set up select for the first time, with the only entry being the server_np

}



server::~server()
{

  std::cout << "server exiting" << std::endl;

  std::cout << "unlinking server_np";
  unlink(np); //get rid of the fifo
  std::cout << "\rserver_np unlinked     " << std::endl;

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



  sprintf(client_send_str,"%d_send",client_PID);
  sprintf(client_recv_str,"%d_recv",client_PID);


  user u;

  switch(m.type)
  {
    case JOIN:
      std::cout << " asking to join" << std::endl;


      u.PID = client_PID; //record client's pid

      sprintf(u.initial_message_text, "%s", m.message_text); //copy the string



      // these two are the same initially
      //get the time for u.time_of_connection
      u.time_of_connection = time(NULL);
      //get the time for u.time_of_last_operation
      u.time_of_last_operation = time(NULL);

      sprintf(u.client_send_str, "%s", client_send_str);
      sprintf(u.client_recv_str, "%s", client_recv_str);


      users.push_back(u);
      break;
  }




  message r;

  r.PID = server_PID;
  r.type = RESPONSE;
  r.sent_at = time(NULL);

  sprintf(r.message_text, "you are good, number %d", client_PID);



  std::cout << r.message_text << std::endl;
  std::cout << "name given as: " << m.message_text << std::endl;


  std::cout << std::endl << std::endl;
  printf("the string is \n");
  std::cout << "\"" << client_recv_str << "\""<< std::endl;
  int response = open(client_recv_str, O_WRONLY);
  write(response, (char*)&r, sizeof(message));
  close(response);



}





int server::server_main(int argc, char const *argv[])
{
  return listen();
  return 1;
}
