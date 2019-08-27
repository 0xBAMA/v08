#include <stdio.h>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <stdbool.h>
#include <time.h>

#include "resources/msg.h" //definitions of message types


class client{
public:
  client();
  ~client();

  void send_message(message m);
  message recieve_message();

  void present_top_menu();




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
  printf("╒════════════════════════════════════PID═%05d══╕", client_PID);
  // printf("│"); fflush(stdout); //need to manually flush buffered output

//before writing to the server_np, open this process's pipes
//  i.e. <PID>_send and <PID>_recv
// avoids a race condition, makes sure that the pipes are set up BEFORE the
// server process tries to used them to communicate with this process


  //create the two pipes to allow bidirectional client-server communication
  sprintf(send_pipe,"%d_send",client_PID);
  sprintf(recv_pipe,"%d_recv",client_PID);

  mkfifo(send_pipe, 0600);
  mkfifo(recv_pipe, 0600);

  //initalize the message
  message m_init;

  sprintf(m_init.message_text, "USERNAME %d", client_PID);

  // std::cout << m_init.message_text << std::endl;

  m_init.PID = client_PID;
  m_init.type = JOIN;


  //make sure to follow the pattern open-write-close
  //open the pipe that allows the initial contact between client and server
  server_initial_pipe = open("server_np", O_WRONLY);
  //write a message to server with the client's PID, for further communication
  write(server_initial_pipe,(char*)&m_init,sizeof(message));

  //close the initial pipe
  //note: we don't need server_np again - now using two pid-specific pipes
  //  with names stored in member variables send_pipe and recv_pipe
  close(server_initial_pipe);



  //read from the pid's recieve pipe
  int recv_fd = open(recv_pipe, O_RDONLY);

  message m;

  read(recv_fd, (char*)&m, sizeof(message));
  close(recv_fd);

  // std::cout << "│server says: " << m.message_text << std::endl;



  // //wait 1 second
  // usleep(1000000);


  while(1)
  {
    present_top_menu();

    char temp;
    cin >> temp;

    if(temp == 'n')
    {
      break;
    }
    else
    {
      cout << "│unrecognized input                             │";
    }
  }

}


//destructor responsible for disconnecting from the server
client::~client()
{

  //send a message of type LEAVE
  message m;
  m.PID = client_PID;
  m.type = LEAVE;
  sprintf(m.message_text, "%05d is leaving.              │", client_PID);
  cout << "╞═══════════════════════════════════════════════╡\n";
  cout << "│sent to server: " << m.message_text;

  //send message on <PID>_send
  int send_fd = open(send_pipe, O_WRONLY);
  write(send_fd, (char*)&m, sizeof(message));
  close(send_fd);

  //unlink the send and recieve pipes

  cout << endl << "╘══════════════════════════════════════goodbye══╛\n";

  unlink(send_pipe);
  unlink(recv_pipe);

}


void client::present_top_menu()
{
  cout << endl << "╞═MAIN═MENU═════════════════════════════════════╡";
  cout << endl << "│enter 'n', followed by the enter key, to exit. │";
  cout << endl << "│>                                              │";
  cout <<       "\r│>" << std::flush; //so that this can be there  ^
                                            //at the end of the user prompt
}






//MENU CHARACTERS - from https://en.wikipedia.org/wiki/Box-drawing_character
// │ 	┤ 	╡ 	╢ 	╖ 	╕ 	╣ 	║ 	╗ 	╝ 	╜ 	╛ 	┐
// └ 	┴ 	┬ 	├ 	─ 	┼ 	╞ 	╟ 	╚ 	╔ 	╩ 	╦ 	╠ 	═ 	╬ 	╧
// ╨ 	╤ 	╥ 	╙ 	╘ 	╒ 	╓ 	╫ 	╪ 	┘ 	┌
