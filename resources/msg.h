#include <time.h>
//this would all be the same in both files, so I went ahead and made it a header

#define MSG_LENGTH 100 //maximum length of message


//expresses the type of the incoming message
typedef enum msg_type_t{

	JOIN,		//sent by client when the client wishes to connect
	LEAVE,			//removes the client from the list
	RESPONSE, //the type that gets sent back to the client

	STATUS,		 //tells the server to report status info
	TIME,			//tells the server to report the current time of day
	DISPLAY, //tells the server to do the

	SPHERE			//user wants to draw a sphere into the voraldo object


} msg_type;


// typedef struct initial_msg{
//
//
// }	initial_msg_t;


//structure for the subsequent messages
typedef struct msg_t {

	int PID; //the PID value of the client

	time_t sent_at;
	msg_type type;
	char message_text[MSG_LENGTH];

	//ARGUMENTS TO DRAW FUNCTIONS

	int location1x;
	int location1y;
	int location1z;

	int radius1;

} message; //<--allows declaration with 'msg_t' rather than 'struct msg'
