//this would all be the same in both files, so I went ahead and made it a header

#define MSG_LENGTH 100 //maximum length of message


//expresses the type of the incoming message
typedef enum msg_type_t{
	JOIN,				//sent by client when the client wishes to connect
	RESPONSE, //the type that gets sent back to the client
	STATUS,	//tells the server to report status info
	TIME,		//tells the server to report the current time of day
	// STRING,	//tells the server to expect a plain message
	LEAVE,			//removes the client from the list
	COMMAND		//tells the server to exit
} msg_type;


// typedef struct initial_msg{
//
//
// }	initial_msg_t;


//structure for the subsequent messages
typedef struct msg_t {

	int PID; //the PID value of the client

	msg_type type;
	char message_text[MSG_LENGTH];

} message; //<--allows declaration with 'message' rather than 'struct msg'
