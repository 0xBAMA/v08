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
  void remove_inactive_users(); //time since time of last operation > 60 min



private:
  int server_PID;

  std::vector<user> users;

};




server::server()
{
   server_PID = getpid();

   std::cout << "v08server process started with PID " << server_PID << std::endl;
}



server::~server()
{
  std::cout << "server exiting" << std::endl;
}
