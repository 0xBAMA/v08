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

#include <poll.h> //for poll()

#include <time.h>


#include "resources/msg.h" //definitions of message types
#include "resources/voraldo.h"


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

  // remove users that haven't done anything in a while (60 minutes?)
  void remove_inactive_users(); //time since last operation > 60 min


  // not really compatible with how glut is handled
  // int server_main(int argc, char const *argv[]) {return listen();}


  int listen();   //to use glut, I think this goes in the timer function (?)

  void read_from_server_np(); //listen found activity on server_np

  void read_from_pid(int pid); //listen found activity related to
  void send_to_pid(int pid);  //for replies

  void list_users();  //echo all active users in users




private:


  void process_message(message m); //do something with
  // a message recieved from listen - i.e. manipulate 'v'

  const char * np = "server_np";

  voraldo v; //holds the data

  int server_np_fd;
  int server_PID;

  std::vector<user> users;

};

#define MAX_USERS 10




server::server()
{
   server_PID = getpid();



   std::cout << "v08server process started with PID " << server_PID << std::endl;


//HOW TO OUTPUT TIME - manipulating time_t, is in seconds since Jan 1, 1970
   time_t temp = time(NULL);
   printf ( "The current date/time is: %s", asctime ( localtime ( &temp )) );


   v.initialize(15,7,5);

   v.print_cli();



   // set up select for the first time, with the only entry being the server_np

}



server::~server()
{

  std::cout << "server exiting" << std::endl;

  // std::cout << "unlinking server_np";
  unlink(np); //get rid of the fifo
  std::cout << "\rserver_np unlinked     " << std::endl;

}





int server::listen()  //call this from the glut idle function
{
  // // set up select() (or poll())
  //
  // // std::cout << "listening" << std::endl;
  //
  // // make a decision based on what triggered the return from select()
  //
  // // if there's activity on server_np (new client connection)
  read_from_server_np();

  //
  // list_users();
  //
  // return 0;

  int num_clients = users.size();
  cout << "there are " << num_clients << " clients"<< endl;

// info from http://www.unixguide.net/unix/programming/2.1.2.shtml

  // open all active file descriptors
  //  this is server_np plus <PID>_recv for each client

  struct pollfd fds[MAX_USERS];

  server_np_fd = open(np, O_RDONLY);

  cout << "returned from read" << endl;

  fds[0].fd = server_np_fd;
  fds[0].events = POLLIN; //listening for input to be ready

  // close(server_np_fd);

  read_from_server_np();

  cout << "FUUUUUUUCK" << endl;



  // set up poll()
  // returns number ready, fds is file descriptors, nfds is number of file
  //  descriptors, and timeout is the max time to wait, in ms
  //
  // int poll(struct pollfd fds[], nfds_t nfds, int timeout);



  // if (!poll(...))  //we got an error



  //close all the file descriptors [0] is server_np, then num_clients other fds
  for(int i = 0; i <= num_clients; i++)
    close(fds[i].fd);



  // return 1; //do this to continue, and do this loop again
  return 0; //do this to exit, in the real main

}



void server::list_users()
{
  // cout << "listing listing listing" << endl;

  cout << endl;


  // generate another user to confirm that the listing works

  // user u;
  // u.PID = 420;
  // sprintf(u.initial_message_text, "INITIAL USERNAME");
  // sprintf(u.client_send_str, "butt %s", "ssss");
  // sprintf(u.client_recv_str, "nugg %s", "SSSS");
  // u.time_of_connection = time(NULL);
  // u.time_of_last_operation = time(NULL);
  // users.push_back(u);



  cout << "List of all present users:" << endl << endl << endl;
  int count = 1;

  for(user n : users)
  {
    cout << "-" << count << "-" << endl;
    cout << "user with username " << n.initial_message_text << " has PID: "<< n.PID << endl;
    cout << "\nconnected at "<< asctime(localtime(&n.time_of_connection));
    cout << "\nlast active at "<< asctime(localtime(&n.time_of_last_operation));

    cout << "\nuses " << n.client_send_str<<" and "<< n.client_recv_str << endl;
    cout << endl << endl;
    count++;
  }

  cout << endl;
}






void server::read_from_server_np()
{


  char client_send_str[12];//holds the name of the <PID>_send pipe
  char client_recv_str[12];//holds the name of the <PID>_recv pipe


  message m;

  mkfifo(np, 0600); //owner has read and write permissions, group and other have none

  server_np_fd = open(np, O_RDONLY); //open the pipe in read only mode


  std::cout << "waiting";
  read(server_np_fd,(char*)&m,sizeof(message)); //blocking read waits for a message
  close(server_np_fd); //close the file descriptor, now that we have a message from a client
  unlink(np); //get rid of the fifo

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

  sprintf(r.message_text, "you're in, number %d", client_PID);


  std::cout << "name given as: " << m.message_text << std::endl;


  std::cout << std::endl << std::endl;

  // printf("the string is \n");
  // std::cout << "\"" << client_recv_str << "\""<< std::endl;

  int response = open(client_recv_str, O_WRONLY);
  write(response, (char*)&r, sizeof(message));
  close(response);

}
