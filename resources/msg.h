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





// #define vec3 glm::vec3
// #define vec4 glm::vec4
// #define mat3 glm::mat3
// #define mat4 glm::mat4


typedef struct rgba_t{

//16-bit values - so we can do 16-bit 3d textures
	//ranges are now 0-65535 rather than 0-255, conversion of rgb is just * 256

  unsigned short r;	//red
  unsigned short g;	//green
  unsigned short b;	//blue
  unsigned short a;	//alpha

}RGBA;


//expresses the type of the incoming message
typedef enum msg_type_t{

	JOIN,		//sent by client when the client wishes to connect
	LEAVE,			//removes the client from the list
	RESPONSE, //the type that gets sent back to the client

	STATUS,		 //tells the server to report status info
	TIME,			//tells the server to report the current time of day
	DISPLAY, //tells the server to do the command line output

	UNMASK_ALL,	//all cells mask set to false
	INVERT_MASK, //all cells go false->true and true->false
	MASK_NONZERO, //if val is nonzero, set mask to true


	NOISE,		//user wants to draw perlin noise
	POINT,		//user wants to draw a point
	SPHERE			//user wants to draw a sphere into the voraldo object


} msg_type;


//structure for the subsequent messages
typedef struct msg_t {

	int PID; //the PID value of the client

	time_t sent_at;
	msg_type type;
	char message_text[MSG_LENGTH];

	//ARGUMENTS TO DRAW FUNCTIONS

	glm::vec3 position1;
	float radius1;

	float scale;
	float threshold;

	RGBA fill1;

	bool draw;
	bool mask;

} message; //<--allows declaration with 'msg_t' rather than 'struct msg'


#endif
