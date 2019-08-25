#include <stdio.h>
#include <iostream>
using std::cout;
using std::endl;

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
  int send_fd;

  char client_recv_str[12]; //holds the name of the <PID>_recv pipe
  int recv_fd;

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

  // const char * np = "server_np";

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


   v.initialize(8,7,5);

   v.print_cli();





   //wait for the first client

   struct pollfd fds[MAX_USERS];

   mkfifo("server_np", 0600); //owner has read and write permissions, group and other have none

   fds[0].fd = open("server_np", O_RDONLY|O_NONBLOCK);
   fds[0].events = POLLIN;


   while(1)
   {
     //set up poll to watch on server_np
     if(poll(fds, 1, 500) > 0)
     {
       cout << "we have a message on server_np" << endl;
       // unlink("server_np");
       read_from_server_np();   //get the first client
       break;
     }
   }



   //



   // set up select for the first time, with the only entry being the server_np

}



server::~server()
{

  std::cout << "server exiting" << std::endl;

  // std::cout << "unlinking server_np";
  unlink("server_np"); //get rid of the fifo
  std::cout << "\rserver_np unlinked     " << std::endl;

}





int server::listen()  //call this from the glut idle function
{

  // read_from_server_np();


  // usleep(100);



  int num_clients = users.size();
  cout << "\rthere are " << num_clients << " clients... ";

// info from http://www.unixguide.net/unix/programming/2.1.2.shtml
// and also  http://man7.org/linux/man-pages/man2/open.2.html

  // open all active file descriptors
  //  this is server_np plus <PID>_recv for each client

  struct pollfd fds[MAX_USERS];

  mkfifo("server_np", 0600);

  fds[0].fd = open("server_np", O_RDONLY|O_NONBLOCK);
  fds[0].events = POLLIN; //listening for input to be ready

  if(num_clients)
    for(int i = 1; i <= num_clients; i++)
    {
      fds[i].fd = open(users[i-1].client_send_str, O_RDONLY|O_NONBLOCK);
      fds[i].events = POLLIN;
      // cout << fds[i].events << endl;
    }


  int num = poll(fds, num_clients+1, 500);
  cout << "poll returned " << num;

  if(num > 0)
  {
    if(fds[0].revents == POLLIN)
    {
      cout << "server_np has more data ready" << endl;

      close(fds[0].fd);

      read_from_server_np();
    }

    for(int i = 1; i <= num_clients; i++)
    {
      if(fds[i].revents==POLLIN)
      {
        cout << "user " << i << " has data ready" << endl;
      }
      else
      {
        cout << endl << fds[i].revents << endl;
      }
    }

    cout << "no other fds" << endl;
  }

  // unlink("server_np");
  // read_from_server_np();
  // cout << "FUUUUUUUCK" << endl;



  // set up poll()
  // returns number ready, fds is file descriptors, nfds is number of file
  //  descriptors, and timeout is the max time to wait, in ms
  //
  // int poll(struct pollfd fds[], nfds_t nfds, int timeout);



  // if (!poll(...))  //we got an error




  return 1; //do this to continue, and do this loop again
  // return 0; //do this to exit, in the real main

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

  cout << endl << "reading from server_np" << endl;


  message m;

  // unlink("server_np");
  // mkfifo("server_np", 0600); //owner has read and write permissions, group and other have none

  // cout << "unlinked" << endl;

  server_np_fd = open("server_np", O_RDONLY|O_NONBLOCK); //open the pipe in read only mode

  // cout << "and opened" << endl;




  std::cout << endl << "waiting" << endl << std::flush;
  read(server_np_fd,(char*)&m,sizeof(message)); //blocking read waits for a message

  close(server_np_fd); //close the file descriptor, now that we have a message from a client

  // unlink("server_np"); //this gets rid of a hang on a subsequent call to open (why?)

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

      mkfifo(client_send_str, 0600);
      mkfifo(client_recv_str, 0600);

      // u.recv_fd = open(client_recv_str, O_WRONLY);


      // close(u.send_fd);
      // unlink(client_send_str);

      // close(u.recv_fd);
      // unlink(client_recv_str);

      users.push_back(u);

      message r;

      r.PID = server_PID;
      r.type = RESPONSE;
      r.sent_at = time(NULL);

      sprintf(r.message_text, "you're in, number %d", client_PID);


      std::cout << "name given as: " << m.message_text << std::endl;


      std::cout << std::endl << std::endl;

      u.recv_fd = open(client_recv_str, O_WRONLY);
      write(u.recv_fd, (char*)&r, sizeof(message));
      close(u.recv_fd);
      // cout << "sent first message" << endl;

      // u.recv_fd = open(client_recv_str, O_WRONLY);
      // write(u.recv_fd, (char*)&r, sizeof(message));
      // close(u.recv_fd);
      // cout << "sent second message" << endl;

      // u.send_fd = open(client_send_str, O_RDONLY);
      // cout << "opened reading pipe";
      // close(u.send_fd);




      break;
  }



}
