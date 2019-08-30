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

//for keeping the menu location
typedef enum menu_loc_t{
  MAIN_MENU,
  DRAW_MENU
} menu_loc;


class client{
public:
  client();
  ~client();

  void send_message(message m);
  message recieve_message();



  void menu(); // calls show_current_menu(), then takes input, then parses
                    // an option with deal_with_option(char* opt)

  void show_current_menu();
  void present_top_menu();
  void present_draw_menu();

  void deal_with_option(std::string opt);


  bool done;


private:
  int client_PID;
  int server_initial_pipe;
  int client_send_pipe,client_recv_pipe;

  //holds names of send and recieve pipes
  char send_pipe[12]; //pid is up to 5 digits, underscore, "send" and termination
  char recv_pipe[12]; //using recv instead of "recieve", since it's also 4 chars


  menu_loc current_menu_location;

};


//constructor responsible for connecting to the server
client::client()
{

  current_menu_location = MAIN_MENU;

  done = false;

  client_PID = getpid(); // who am I
  printf("╒═════════════════════════v08═Client══PID═%05d══╕", client_PID);
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


}


//destructor responsible for disconnecting from the server
client::~client()
{

  //send a message of type LEAVE
  message m;
  m.PID = client_PID;
  m.type = LEAVE;
  sprintf(m.message_text, "%05d is leaving.", client_PID);
  cout << "╞════════════════════════════════════════════════╡\n";
  cout << "│sent to server: " << m.message_text << "               │";

  //send message on <PID>_send
  int send_fd = open(send_pipe, O_WRONLY);
  write(send_fd, (char*)&m, sizeof(message));
  close(send_fd);

  //unlink the send and recieve pipes

  cout << endl << "╘═══════════════════════════════════════goodbye══╛\n";

  unlink(send_pipe);
  unlink(recv_pipe);

}



void client::menu()
{
  show_current_menu();

  // char child_message_string[MSG_LENGTH]; //holds the user input
  // char buf[11]; //allows for comparison
  //
  // fgets(child_message_string,MSG_LENGTH,stdin);//get the input
  //
  // cout << "  " << child_message_string; //echo user input
  //
  // sprintf(buf,"%.10s",child_message_string);//grab the first couple characters
  //
  // deal_with_option(buf);

  std::string input;

  cin >> input;

  cout << input;

  deal_with_option(input);

}



void client::deal_with_option(std::string opt)
{

  if(current_menu_location == MAIN_MENU)
  {//main menu options

    cout << opt;

    if(!opt.compare("exit")) //EXIT OPTION
  	{//exit the while loop, if the user input started with "exit"
      cout << "exit option selected" << endl;

  	  done = true; //using while(!client.done) in main()
  	}


    if(!opt.compare("time")) //TIME OPTION
    { //send message of type TIME to the server
      cout << "time option selected" << endl;

      message m;
      m.PID = client_PID;
      m.sent_at = time(NULL);
      m.type = TIME;

      int send_fd = open(send_pipe, O_WRONLY);
      write(send_fd, (char*)&m, sizeof(message));
      close(send_fd);
    }

    if(!opt.compare("disp")) //DISPLAY OPTION
    {
      cout << "display option selected" << endl;

      message m;
      m.PID = client_PID;
      m.sent_at = time(NULL);
      m.type = DISPLAY;

      int send_fd = open(send_pipe, O_WRONLY);
      write(send_fd, (char*)&m, sizeof(message));
      close(send_fd);
    }

    if(!opt.compare("draw")) //GO TO DRAW SUMENU OPTIONS
    {
      cout << "draw menu option selected" << endl;

      current_menu_location = DRAW_MENU;
    }
  }

  if(current_menu_location == DRAW_MENU)
  {//draw menu options

    if(!opt.compare("sphere"))
    {
      cout << "soo you want to draw a sphere, huh?" << endl;
    }

    if(!opt.compare("back"))
    {
      current_menu_location = MAIN_MENU;
    }

  }


}


void client::show_current_menu()
{
  switch(current_menu_location)
  {
    case MAIN_MENU:
      present_top_menu();
      break;
    case DRAW_MENU:
      present_draw_menu();
      break;
  }
}


void client::present_top_menu()
{
  cout << endl << "╞════════════════════════════════════════════════╡";
  cout << endl << "╞MAIN═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
  cout << endl << "│Enter one of the following, then the enter key. │";
  cout << endl << "│                                                │";
  cout << endl << "│  exit: end program                             │";
  cout << endl << "│  time: server reports time                     │";
  cout << endl << "│  disp: server shows block                      │";
  cout << endl << "│  draw: go to draw submenu                      │";
  cout << endl << "│                                                │";
  cout << endl << "│>                                               │";
  cout <<       "\r│>" << std::flush;  //so that this can be there  ^
                                            //at the end of the user prompt
}

void client::present_draw_menu()
{
  cout << endl << "╞════════════════════════════════════════════════╡";
  cout << endl << "╞DRAW═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
  cout << endl << "│Enter one of the following, then the enter key. │";
  cout << endl << "│                                                │";
  cout << endl << "│  sphere: take sphere dimensions                │";
  cout << endl << "│  back: go to main menu                         │";
  cout << endl << "│                                                │";
  cout << endl << "│>                                               │";
  cout <<       "\r│>" << std::flush;  //so that this can be there  ^
                                            //at the end of the user prompt
}





//MENU CHARACTERS - from https://en.wikipedia.org/wiki/Box-drawing_character
// │ 	┤ 	╡ 	╢ 	╖ 	╕ 	╣ 	║ 	╗ 	╝ 	╜ 	╛ 	┐
// └ 	┴ 	┬ 	├ 	─ 	┼ 	╞ 	╟ 	╚ 	╔ 	╩ 	╦ 	╠ 	═ 	╬ 	╧
// ╨ 	╤ 	╥ 	╙ 	╘ 	╒ 	╓ 	╫ 	╪ 	┘ 	┌




//COLOR FORMATTING
// BOLD       \e[1m
//
// RESET      \e[0m
// ERROR      \e[1;41;37m\a
// MENU       \e[44;37m
//
//
//TEXT COLORS
// T_BLACK    \e[30m
// T_RED      \e[31m
// T_GREEN    \e[32m
// T_YELLOW   \e[33m
// T_BLUE     \e[34m
// T_MAGENTA  \e[35m
// T_CYAN     \e[36m
// T_WHITE    \e[37m
//
//BACKGROUND COLORS
// B_BLACK    \e[40m
// B_RED      \e[41m
// B_GREEN    \e[42m
// B_YELLOW   \e[43m
// B_BLUE     \e[44m
// B_MAGENTA  \e[45m
// B_CYAN     \e[46m
// B_WHITE    \e[47m
