#include <time.h>
//this would all be the same in both files, so I went ahead and made it a header


#ifndef MESSAGE
#define MESSAGE

#define MSG_LENGTH 100 //maximum length of message


//GLM INCLUDES

#define GLM_FORCE_SWIZZLE
#include "../resources/glm/glm.hpp" 									// general types
#include "../resources/glm/gtc/matrix_transform.hpp" // orthographic view matrix (glm::ortho( left, right, bottom, top, zNear, zFar ))
#include "../resources/glm/gtc/type_ptr.hpp" 				// allows the sending of a matrix (for glUniform)
#include "../resources/glm/gtx/transform.hpp"				// rotate()


typedef struct rgba_t{

  unsigned short red;
  unsigned short green;
  unsigned short blue;
  unsigned short alpha;

}rgba;


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

	glm::vec3 position1;

	float radius1;

	int fill1;

} message; //<--allows declaration with 'msg_t' rather than 'struct msg'


#endif
