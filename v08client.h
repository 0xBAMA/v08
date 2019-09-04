#include <stdio.h>
#include <iostream>
#include <iomanip>
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


//from my operating systems project implementing a rudimentary version of top
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))
#define clear() printf("\033[H\033[J")



//for keeping the menu location
typedef enum menu_loc_t{
  MAIN_MENU,

  DRAW_MENU,
  NOISE_CONFIG,
  SPHERE_CONFIG,

  MASK_MENU,

  UTIL_MENU
} menu_loc;


class client{
public:
  client();
  ~client();


  bool done;

  void menu(); // calls show_current_menu(), then takes input, then parses
                    // an option with deal_with_option(char* opt)






private:


  void send_message(message m);
  message recieve_message();


  void show_current_menu();
  void present_top_menu();

  void present_draw_menu();
    void present_noise_config_menu();
    void present_sphere_config_menu();

  void present_mask_menu();

  void present_util_menu();


  void deal_with_option(std::string opt);


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

  std::string input;

  cin >> input;

  // cout << input;

  deal_with_option(input);

}

void client::deal_with_option(std::string opt)
{

  if(current_menu_location == MAIN_MENU)
  {//main menu options

    // cout << opt;

    //exit the while loop, if the user input started with "exit"
    if(!opt.compare("exit")) //EXIT OPTION
  	  done = true; //using while(!client.done) in main()





    if(!opt.compare("time")) //TIME OPTION
    { //send message of type TIME to the server
      // cout << "time option selected" << endl;

      message m;
      m.PID = client_PID;
      m.sent_at = time(NULL);
      m.type = TIME;

      int send_fd = open(send_pipe, O_WRONLY);
      write(send_fd, (char*)&m, sizeof(message));
      close(send_fd);
    }



    if(!opt.compare("display")) //DISPLAY OPTION
    {
      // cout << "display option selected" << endl;

      message m;
      m.PID = client_PID;
      m.sent_at = time(NULL);
      m.type = DISPLAY;

      int send_fd = open(send_pipe, O_WRONLY);
      write(send_fd, (char*)&m, sizeof(message));
      close(send_fd);
    }

    if(!opt.compare("draw")) //GO TO DRAW SUMENU OPTIONS
      current_menu_location = DRAW_MENU;


    if(!opt.compare("mask"))//GO TO MASK SUMENU OPTIONS
      current_menu_location = MASK_MENU;

    if(!opt.compare("util"))//GO TO UTIL SUMENU OPTIONS
      current_menu_location = UTIL_MENU;

  }



  if(current_menu_location == DRAW_MENU)
  {//draw menu options

    if(!opt.compare("noise"))
    {
      // sphere configuration menu
      current_menu_location = NOISE_CONFIG;

    }


    if(!opt.compare("sphere"))
    {
      // sphere configuration menu
      current_menu_location = SPHERE_CONFIG;

    }

    if(!opt.compare("back"))
    {
      current_menu_location = MAIN_MENU;
    }

  }



  if(current_menu_location == MASK_MENU)
  {//mask menu options

    if(!opt.compare("back"))
    {
      current_menu_location = MAIN_MENU;
    }

  }



  if(current_menu_location == UTIL_MENU)
  {//mask menu options

    // if(!opt.compare("sphere"))
    // {
    //   // sphere configuration menu
    //   current_menu_location = SPHERE_CONFIG;
    //
    // }

    if(!opt.compare("back"))
    {
      current_menu_location = MAIN_MENU;
    }

  }


}

void client::show_current_menu()
{
  clear();
  switch(current_menu_location)
  {
    case MAIN_MENU:
      present_top_menu();
      break;

    case DRAW_MENU:
      present_draw_menu();
      break;
    case NOISE_CONFIG:
      present_noise_config_menu();
      break;
    case SPHERE_CONFIG:
      present_sphere_config_menu();
      break;

    case MASK_MENU:
      present_mask_menu();
      break;

    case UTIL_MENU:
      present_util_menu();
      break;
  }
}

void client::present_top_menu()
{
  cout << endl << "╒════════════════════════════════════════════════╕";
  cout << endl << "╞MAIN═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
  cout << endl << "│Enter one of the following, then the enter key. │";
  cout << endl << "│                                                │";
  cout << endl << "│  time : server reports time                    │";
  cout << endl << "│  disp : server shows block                     │";
  cout << endl << "│                                                │";
  cout << endl << "│  draw : go to draw submenu                     │";
  cout << endl << "│  mask : go to masking submenu                  │"; //to do
  cout << endl << "│  util : go to utilities submenu                │"; //to do
  cout << endl << "│                                                │";
  cout << endl << "│  exit : end program                            │";
  cout << endl << "│                                                │";
  cout << endl << "│>                                               │";
  cout <<       "\r│>" << std::flush;  //so that this can be there  ^
                                            //at the end of the user prompt
}

void client::present_draw_menu()
{
  cout << endl << "╒════════════════════════════════════════════════╕";
  cout << endl << "╞MAIN═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
  cout << endl << "╒════════════════════════════════════════════════╕";
  cout << endl << "╞DRAW═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
  cout << endl << "│Enter one of the following, then the enter key. │";
  cout << endl << "│                                                │";
  cout << endl << "│  noise: go to noise config menu                │";
  cout << endl << "│  sphere: go to sphere config menu              │";
  cout << endl << "│  back: go to main menu                         │";
  cout << endl << "│                                                │";
  cout << endl << "│>                                               │";
  cout <<       "\r│>" << std::flush;  //so that this can be there  ^
                                            //at the end of the user prompt
}

void client::present_mask_menu()
{
  cout << endl << "╒════════════════════════════════════════════════╕";
  cout << endl << "╞MAIN═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
  cout << endl << "╒════════════════════════════════════════════════╕";
  cout << endl << "╞MASK═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
  cout << endl << "│Enter one of the following, then the enter key. │";
  cout << endl << "│                                                │";
  cout << endl << "│  invert: send message to invert mask           │";
  cout << endl << "│  unmask: send message to unmask all cells      │";

  cout << endl << "│  back: go to main menu                         │";
  cout << endl << "│                                                │";
  cout << endl << "│>                                               │";
  cout <<       "\r│>" << std::flush;  //so that this can be there  ^
                                            //at the end of the user prompt
}

void client::present_util_menu()
{
  cout << endl << "╒════════════════════════════════════════════════╕";
  cout << endl << "╞MAIN═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
  cout << endl << "╒════════════════════════════════════════════════╕";
  cout << endl << "╞UTILITY═MENU══ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
  cout << endl << "│Enter one of the following, then the enter key. │";
  cout << endl << "│                                                │";
  // cout << endl << "│  sphere: go to sphere config menu              │";
  cout << endl << "│  back: go to main menu                         │";
  cout << endl << "│                                                │";
  cout << endl << "│>                                               │";
  cout <<       "\r│>" << std::flush;  //so that this can be there  ^
                                            //at the end of the user prompt
}

void client::present_noise_config_menu()
{

  std::string input = "nut";

  // cout << input;

  float scale, threshold;

  scale = 0.5f;
  threshold = 0.5f;

  bool draw, mask;

  draw = true;
  mask = false;

  // float radius = 0.0f;

  int fill = 0;

  while(1)
  {
    clear();
    cout << endl << "╒════════════════════════════════════════════════╕";
    cout << endl << "╞MAIN═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
    cout << endl << "╒════════════════════════════════════════════════╕";
    cout << endl << "╞DRAW═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
    cout << endl << "╒════════════════════════════════════════════════╕";
    cout << endl << "╞NOISE═CONFIG═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
    cout << endl << "│Enter one of the following, then the enter key. │";
    cout << endl << "│                                                │";
    cout << endl << "│    reasonable values for scale are ~0.1-1      │";
    cout << endl << "│    try negative values for thresh to invert    │";
    cout << endl << "│                                                │";

    cout << endl << "│        fill :  " <<  std::setw(5) << std::setprecision(5) << fill << "                           │";
    cout << endl << "│        scale :  " <<  std::setw(5) << std::setprecision(5) << scale << "                          │";
    cout << endl << "│        thresh :  " <<  std::setw(5) << std::setprecision(5) << threshold << "                         │";

    cout << endl << "│                                                │";
    cout << endl << "│         draw : " << draw << "   mask:  " << mask << "                     │";

    cout << endl << "│                                                │";
    cout << endl << "│  s : set the scale value                       │";
    cout << endl << "│  t : set the threshold value                   │";
    cout << endl << "│  f : set the fill value                        │";
    cout << endl << "│                                                │";
    cout << endl << "│  m : toggle value of mask                      │";
    cout << endl << "│  d : toggle value of draw                      │";
    cout << endl << "│                                                │";
    cout << endl << "│  display: server shows block                   │";
    cout << endl << "│  send : server will draw with current values   │";
    cout << endl << "│                                                │";
    cout << endl << "│  back: go to draw menu                         │";
    cout << endl << "│                                                │";
    cout << endl << "│>                                               │";
    cout <<       "\r│>" << std::flush;  //so that this can be there  ^
                                              //at the end of the user prompt

    cin >> input;

    if(!input.compare("s"))
    {
      cout << "What is it?" << endl << " >";
      cin >> scale;
    }

    if(!input.compare("t"))
    {
      cout << "What is it?" << endl << " >";
      cin >> threshold;
    }

    if(!input.compare("f"))
    {
      cout << "What is it? (0 to 9)" << endl << " >";
      cin >> fill;
    }

    if(!input.compare("d"))
    {
      draw = draw ? false : true;
    }

    if(!input.compare("m"))
    {
      mask = mask ? false : true;
    }

    if(!input.compare("send"))
    {
      cout << "sending...";

      message m;



      m.PID = client_PID;
      m.sent_at = time(NULL);
      m.type = NOISE;

      // m.position1 = glm::vec3(center_x,center_y,center_z);
      // m.radius1 = radius;
      m.fill1 = fill;

      m.threshold = threshold;
      m.scale = scale;

      m.mask = mask;
      m.draw = draw;

      int send_fd = open(send_pipe, O_WRONLY);
      write(send_fd, (char*)&m, sizeof(message));
      close(send_fd);


      cout << "\rsent        " << endl;
    }

    if(!input.compare("display"))
    {
      message m;
      m.PID = client_PID;
      m.sent_at = time(NULL);
      m.type = DISPLAY;

      int send_fd = open(send_pipe, O_WRONLY);
      write(send_fd, (char*)&m, sizeof(message));
      close(send_fd);
    }

    if(!input.compare("back"))
      break;

  }

  current_menu_location = DRAW_MENU;
  show_current_menu(); //see present_sphere_config_menu() for details
}

void client::present_sphere_config_menu()
{

  std::string input = "nut";

  // cout << input;

  float center_x, center_y, center_z;

  center_x = 0.0f;
  center_y = 0.0f;
  center_z = 0.0f;

  bool draw, mask;

  draw = true;
  mask = false;

  float radius = 0.0f;

  int fill = 0;

  while(1)
  {
    clear();
    cout << endl << "╒════════════════════════════════════════════════╕";
    cout << endl << "╞MAIN═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
    cout << endl << "╒════════════════════════════════════════════════╕";
    cout << endl << "╞DRAW═MENU═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
    cout << endl << "╒════════════════════════════════════════════════╕";
    cout << endl << "╞SPHERE═CONFIG═MENU══ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═╡";
    cout << endl << "│Enter one of the following, then the enter key. │";
    cout << endl << "│                                                │";
    cout << endl << "│      center x: " << std::setw(5) << std::setprecision(5) << center_x
    << "    y: " << std::setw(5) << std::setprecision(5)<< center_y
    << "    z: " << std::setw(5) << std::setprecision(5)<< center_z
                              << "   │";
    cout << endl << "│        radius :" <<  std::setw(5) << std::setprecision(5) << radius << "                           │";
    cout << endl << "│        fill :  " <<  std::setw(5) << std::setprecision(5) << fill << "                           │";
    cout << endl << "│                                                │";
    cout << endl << "│         draw : " << draw << "   mask:  " << mask << "                       │";

    cout << endl << "│                                                │";
    cout << endl << "│  x : set the center x value                    │";
    cout << endl << "│  y : set the center y value                    │";
    cout << endl << "│  z : set the center z value                    │";
    cout << endl << "│  r : set the radius value                      │";
    cout << endl << "│  f : set the fill value                        │";
    cout << endl << "│                                                │";
    cout << endl << "│  m : toggle value of mask                      │";
    cout << endl << "│  d : toggle value of draw                      │";
    cout << endl << "│                                                │";
    cout << endl << "│  display: server shows block                   │";
    cout << endl << "│  send : server will draw with current values   │";
    cout << endl << "│                                                │";
    cout << endl << "│  back: go to draw menu                         │";
    cout << endl << "│                                                │";
    cout << endl << "│>                                               │";
    cout <<       "\r│>" << std::flush;  //so that this can be there  ^
                                              //at the end of the user prompt

    cin >> input;

    if(!input.compare("x"))
    {
      cout << "What is it?" << endl << " >";
      cin >> center_x;
    }

    if(!input.compare("y"))
    {
      cout << "What is it?" << endl << " >";
      cin >> center_y;
    }

    if(!input.compare("z"))
    {
      cout << "What is it?" << endl << " >";
      cin >> center_z;
    }

    if(!input.compare("r"))
    {
      cout << "What is it?" << endl << " >";
      cin >> radius;
    }

    if(!input.compare("f"))
    {
      cout << "What is it? (0 to 9)" << endl << " >";
      cin >> fill;
    }

    if(!input.compare("d"))
    {
      draw = draw ? false : true;
    }

    if(!input.compare("m"))
    {
      mask = mask ? false : true;
    }

    if(!input.compare("send"))
    {
      cout << "sending...";

      message m;



      m.PID = client_PID;
      m.sent_at = time(NULL);
      m.type = SPHERE;

      m.position1 = glm::vec3(center_x,center_y,center_z);
      m.radius1 = radius;
      m.fill1 = fill;

      m.mask = mask;
      m.draw = draw;

      int send_fd = open(send_pipe, O_WRONLY);
      write(send_fd, (char*)&m, sizeof(message));
      close(send_fd);


      cout << "\rsent        " << endl;
    }

    if(!input.compare("display"))
    {
      message m;
      m.PID = client_PID;
      m.sent_at = time(NULL);
      m.type = DISPLAY;

      int send_fd = open(send_pipe, O_WRONLY);
      write(send_fd, (char*)&m, sizeof(message));
      close(send_fd);
    }

    if(!input.compare("back"))
      break;

  }

  current_menu_location = DRAW_MENU;
  show_current_menu();  //the combination of these two lines puts you back
    //at a prompt for the draw menu, which outputs a menu then this config
    //function returns to menu() for cin to take in the option (from that
    //draw menu) and then calls deal_with_option() with whatever was cin.
    //In any event - it works. I can do similar stuff for drawing other shapes,
    //since they don't really have anything like sub-menus themselves

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
