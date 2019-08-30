#include "v08server.h"

int current_client_count;



int main(int argc, char const *argv[]) {

	server s(9,5,5);

	while(s.listen());








	// 	//fork
	// 	if(!fork()){ //here we enter the child function
	//
	// 		printf("Client process has connected, with PID %d\n",client_PID);
	//
	// 		while(1){
	//
	// 			client_send_pipe = open(client_send_str,O_RDONLY);				//open
	// 			read(client_send_pipe,(char*)&m,sizeof(message));					//read
	// 			close(client_send_pipe);																	//close
	//
	// 			if(m.type == STATUS){
	// 			//response will include the number of children
	//
	// 					printf("Client process %d wants to know how many children have been created.\n",client_PID);
	//
	// 					m.type = RESPONSE;
	// 					sprintf(m.message_text,"I have %d children.",current_client_count);
	//
	// 					client_recv_pipe = open(client_recv_str,O_WRONLY);
	// 					write(client_recv_pipe,(char*)&m,sizeof(message));
	// 					close(client_recv_pipe);
	//
	// 			}else if(m.type == TIME){
	// 			//response will tell the time to the client
	//
	// 					printf("Client process %d wants to know the time.\n",client_PID);
	//
	// 					//from: http://www.cplusplus.com/reference/ctime/localtime/
	// 					time_t rawtime;
	// 					struct tm * timeinfo;
	//
	// 					time (&rawtime);
	// 					timeinfo = localtime(&rawtime);
	//
	// 					m.type = RESPONSE;
	// 					sprintf(m.message_text, "The time is %s", asctime(timeinfo));
	// 					sprintf(m.message_text, strtok(m.message_text, "\n"));
	// 					//for some reason this was adding a trailing newline I don't want included
	//
	// 					client_recv_pipe = open(client_recv_str,O_WRONLY);
	// 					write(client_recv_pipe,(char*)&m,sizeof(message));
	// 					close(client_recv_pipe);
	//
	// 			// }else if(m.type == STRING){
	// 			// //does not require a response
	// 			//
	// 			// 		printf("Client process %d wants me to echo a string.\n", client_PID);
	// 			// 		printf("Here it is \"%s\"\n",strtok(m.message_text, "\n"));
	// 			// 		//getting weird newlines here, too
	//
	// 			}else if(m.type == COMMAND){
	// 			//does not require a response
	//
	// 					printf("Client process %d is done.\n", client_PID);
	// 					break;//exit the infinite loop - "break" doesn't seem to exit the loop properly on P2
	//
	// 			}else{
	// 					printf("Server recieved message of unknown type - exiting\n");
	// 					break; //exit the infinite loop - "break" doesn't seem to exit the loop properly on P2
	// 			}
	// 		}
	//
	// 		printf("Child process handling client number %d has finished\n\n", client_PID);
	//
	// 		return 0; //once you have exited infinite while loop, exit this child process
	//
	// 	}//parent does not execute any code, no 'else' branch required

	return 0;
}
