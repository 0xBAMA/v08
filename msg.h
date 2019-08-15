//this would all be the same in both files, so I went ahead and made it a header

#define MSG_LENGTH 100 //maximum length of message


//expresses the type of the incoming message
typedef enum {
	RESPONSE, //the type that gets sent back to the client
	STATUS,	//tells the server to report how many children it has
	TIME,		//tells the server to report the current time of day
	STRING,	//tells the server to expect a plain message
	COMMAND		//tells the server to exit
} msg_type_t;

typedef struct initial_msg{

	int PID; //all that is needed initially is the PID value of the client

}	initial_msg_t;


//structure for the subsequent messages
typedef struct msg {

	msg_type_t type;
	char message_text[MSG_LENGTH];

} msg_t; //<--allows declaration with 'msg_t' rather than 'struct msg'
