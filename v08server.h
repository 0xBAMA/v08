#include <stdio.h>
#include <iostream>
using std::cout;
using std::cin;
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

#define MAX_USERS 10 //arbitrary

typedef struct user_t
{
  int PID;

  time_t time_of_connection;
  time_t time_of_last_operation;

  char initial_message_text[MSG_LENGTH]; //taken as name

  char client_send_str[12]; //holds the name of the <PID>_send pipe
  // int send_fd;

  char client_recv_str[12]; //holds the name of the <PID>_recv pipe
  // int recv_fd;

}user;




class server{
public:

  server(int x, int y, int z);
  ~server();

  // remove users that haven't done anything in a while (60 minutes?)
  void remove_inactive_users(); //time since last operation > 60 min


  // not really compatible with how glut is handled
  // int server_main(int argc, char const *argv[]) {return listen();}


  int listen();   //to use glut, I think this goes in the timer function (?)
                 //there's some likelyhood that a separate thread could be used

  void read_from_server_np(); //listen found activity on server_np
  void take_input_from_user(int i); //listen found activity on a <PID>_send

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

  time_t server_last_operation;   // used to shut down the server if inactive
  // where inactive is defined as having zero users for some period of time

  std::vector<user> users;

};

server::server(int x, int y, int z)
{
   server_PID = getpid();
   mkfifo("server_np", 0600);


   std::cout << "v08server process started with PID " << server_PID << std::endl;


//HOW TO OUTPUT TIME - manipulating time_t, is in seconds since Jan 1, 1970
   time_t temp = time(NULL);
   printf ( "The current date/time is: %s\n", asctime ( localtime ( &temp )) );


   v.initialize(x,y,z);

   v.print_cli();





   //wait for the first client

   struct pollfd fds[MAX_USERS];

   mkfifo("server_np", 0600); //owner has read and write permissions, group and other have none

   fds[0].fd = open("server_np", O_RDONLY|O_NONBLOCK);
   fds[0].events = POLLIN;


   while(1)
   {//set up poll to watch on server_np, until the first message is recieved
     if(poll(fds, 1, 500) > 0)
     {
       cout << "first message recieved on server_np: " << endl;
       read_from_server_np();   //get the first client
       break;
     }
   }
}

server::~server()
{

  std::cout << "server exiting" << std::endl;

  // std::cout << "unlinking server_np";
  unlink("server_np"); //get rid of the fifo
  std::cout << "\rserver_np unlinked     " << std::endl;

}

int server::listen()  //call this from the glut idle function?
{

  // read_from_server_np();


  // usleep(100);



  int num_clients = users.size();
  cout << "\rthere are " << num_clients << " clients... ";

  // cout << endl;

  // list_users();

// info from http://www.unixguide.net/unix/programming/2.1.2.shtml
// and also  http://man7.org/linux/man-pages/man2/open.2.html

  // open all active file descriptors
  //  this is server_np plus <PID>_recv for each client

  struct pollfd fds[MAX_USERS];

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
  cout << "poll returned " << num << std::flush;

  if(num > 0)
  {
    if(fds[0].revents == POLLIN)
    {
      cout << "\rserver_np has more data ready:            " << endl;

      close(fds[0].fd);

      read_from_server_np();
    }

    for(int i = 1; i <= num_clients; i++)
    {
      if(fds[i].revents==POLLIN)
      {
        cout << "\ruser " << i << " has data ready:                " << endl;
    //then deal with the message on user[i]'s <PID>_send pipe
        take_input_from_user(i-1);  //to array index
      }
      close(fds[i].fd);
    }
  }
  else
  {
    close(fds[0].fd);

    for(int i = 1; i <= num_clients; i++)
      close(fds[i].fd);
  }



  //***NEED TO CLOSE ALL FDs BEFORE THIS FUNCTION RETURNS***
  //  otherwise, it keeps allocating, and runs out of fds

  // for(int i = 0; i <= num_clients; i++)
  //   close(fds[i].fd);


  return 1; //do this to continue, and do this loop again
  // return 0; //do this to exit, in the real main

}

void server::list_users()
{
  cout << endl;

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
  message m;

  int recv_fd = 0; //used to point to <PID>_recv

  //open the pipe in read only mode
  server_np_fd = open("server_np", O_RDONLY|O_NONBLOCK);



  //wait for a message
  //  it's blocking but it's not an issue, because we know there's data there
  read(server_np_fd,(char*)&m,sizeof(message));
  int client_PID = m.PID; // who connected?

  //close the file descriptor, we have a message from a client
  close(server_np_fd);






  switch(m.type)
  {
    case JOIN:  //server_np only expects messages of type JOIN

      user u;
      u.PID = client_PID; //record client's pid

      sprintf(u.initial_message_text, "%s", m.message_text); //copy the string

      // these two are the same initially
      //get the time for u.time_of_connection
      u.time_of_connection = time(NULL);
      //get the time for u.time_of_last_operation - this updates, each operation
      u.time_of_last_operation = time(NULL);

      sprintf(u.client_send_str,"%d_send",client_PID);
      sprintf(u.client_recv_str,"%d_recv",client_PID);

      mkfifo(u.client_send_str, 0600);
      mkfifo(u.client_recv_str, 0600);

      users.push_back(u);

      message r;

      r.PID = server_PID;
      r.type = RESPONSE;
      r.sent_at = time(NULL);

      sprintf(r.message_text, "you're in, number %d", client_PID);

      cout << "\r  recieved message from PID " << m.PID; //get the PID from the message

      cout << " asking to join, with name given as: ";
      cout << m.message_text << std::endl;


      std::cout << std::endl << std::endl;

      recv_fd = open(u.client_recv_str, O_WRONLY);
      write(recv_fd, (char*)&r, sizeof(message));
      close(recv_fd);

      break;

    default:
      cout << endl << "recieved message of unknown type" << endl;
      break;
  }
}

void server::take_input_from_user(int i)
{
  glm::vec3 center;

  time_t temp_time;
  message m,r;

  //open the pipe in read only mode
  int user_send_fd = open(users[i].client_send_str, O_RDONLY|O_NONBLOCK);
  int user_recv_fd; //uninitialized

  //wait for a message
  //  it's blocking but it's not an issue, because we know there's data there
  read(user_send_fd,(char*)&m,sizeof(message));

  //close the file descriptor, we have a message from a client
  close(user_send_fd);

  // int client_PID = m.PID;
  // who connected is not relevant, we know this before the function is called

  //update the time of the last operation with the current time


  switch(m.type)
  {
    case LEAVE: //the client represented by users[i] wants to leave

      cout << "  user with PID " << m.PID << " is leaving" << endl << endl;

      //send the user a message, "goodbye" or whatever
      user_recv_fd = open(users[i].client_send_str, O_WRONLY|O_NONBLOCK);

      r.PID = server_PID;
      r.type = RESPONSE;
      sprintf(r.message_text, "goodbye %d", m.PID);

      write(user_recv_fd, (char*)&m, sizeof(message));
      close(user_recv_fd);

      //remove the user from the list of active users
      users.erase(users.begin() + i);
      //i = 0 erases users[0], etc

      break;

    case TIME:
      cout << "  " << m.PID << " wants to know the time" << endl;
      temp_time = time(NULL);
      cout << "  here it is: " << asctime(localtime(&temp_time)) << endl;
      break;

    case DISPLAY:
      cout << "  " << m.PID << " wants to see the block" << endl;
      cout << "  here it is:" << endl;
      v.print_cli();
      break;

    case NOISE:
      cout << "  " << m.PID << " wants to draw noise with ";
      cout << "scale = " << m.scale << " and threshold = " << m.threshold;

      cout << endl << "...";

      v.draw_perlin_noise(m.scale, m.threshold,m.fill1,m.draw,m.mask);

      cout << "\rdone";
      break;

    case SPHERE:
    //client wants to draw a sphere into the voraldo object
      //I want to make this at least multithreaded, this time, since every
      //voxel can be evaluated independently
      cout << "  " << m.PID << " wants to draw a sphere at ";
      cout << m.position1.x << " " << m.position1.y << " " << m.position1.z;


      cout << " with radius " << m.radius1 << endl;

      cout << endl << "...";


      center = glm::vec3(m.position1.x, m.position1.y, m.position1.z);

      // cout << center.x << " " << center.y << " " << center.z << endl;

      v.draw_sphere(glm::vec3(center.x,center.y,center.z), m.radius1, m.fill1, m.draw, m.mask);


      cout << "\rdone";

      break;

    default:
      cout << "unrecognized command from " << users[i].client_send_str << endl;
      break;
  }

}
