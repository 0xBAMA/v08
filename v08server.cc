#include <stdio.h>
#include <string>
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

int current_client_count;

//function to decrement child count
void decrement_clients(int sig){
	current_client_count--;
	// printf("A child process has terminated - leaving %d clients connected\n", current_client_count);
	signal(SIGCHLD, decrement_clients); //make sure the handler is registered
}


int main()
{
	int server_PID = getpid();
	int fd, client_PID;

	int client_send_pipe, client_recv_pipe;

	current_client_count = 0; //initially, no clients have connected
	signal(SIGCHLD, decrement_clients); //set up signal handler to decrement child count

	initial_msg_t m; //used as a buffer for the incoming initial message
	msg_t msg;

	const char * np = std::string("server_np").c_str(); //name of the named pipe


	// printf("This is the CS_2 server process, started with PID %d and %d chlidren\n", server_PID, current_client_count);


	//create the initial FIFO
	mkfifo(np, 0600); //owner has read and write permissions, group and other have none



	char client_send_str[12];//holds the name of the <PID>_send pipe
	char client_recv_str[12];//holds the name of the <PID>_recv pipe



	while(1){

		fd = open(np, O_RDONLY); //open the pipe in read only mode
		read(fd,(char*)&m,sizeof(initial_msg_t)); //blocking read waits for a message
		close(fd); //close the file descriptor, now that we have a message from a client
		//make sure to follow the pattern open-read-close to avoid complications

		client_PID = m.PID; //get the PID from the message

		sprintf(client_send_str,"%d_send",client_PID);
		sprintf(client_recv_str,"%d_recv",client_PID);

		current_client_count++; //parent and child will need this information
		//As mentioned in the instructions, this will only be correct for the
		//moment at which the child process is created - it will not be updated.

		//fork
		if(!fork()){ //here we enter the child function

			printf("Client process has connected, with PID %d\n",client_PID);

			while(1){

				client_send_pipe = open(client_send_str,O_RDONLY);				//open
				read(client_send_pipe,(char*)&msg,sizeof(msg_t));					//read
				close(client_send_pipe);																	//close

				if(msg.type == STATUS){
				//response will include the number of children

						printf("Client process %d wants to know how many children have been created.\n",client_PID);

						msg.type = RESPONSE;
						sprintf(msg.message_text,"I have %d children.",current_client_count);

						client_recv_pipe = open(client_recv_str,O_WRONLY);
						write(client_recv_pipe,(char*)&msg,sizeof(msg_t));
						close(client_recv_pipe);

				}else if(msg.type == TIME){
				//response will tell the time to the client

						printf("Client process %d wants to know the time.\n",client_PID);

						//from: http://www.cplusplus.com/reference/ctime/localtime/
						time_t rawtime;
						struct tm * timeinfo;

						time (&rawtime);
						timeinfo = localtime(&rawtime);

						msg.type = RESPONSE;
						sprintf(msg.message_text, "The time is %s", asctime(timeinfo));
						sprintf(msg.message_text, strtok(msg.message_text, "\n"));
						//for some reason this was adding a trailing newline I don't want included

						client_recv_pipe = open(client_recv_str,O_WRONLY);
						write(client_recv_pipe,(char*)&msg,sizeof(msg_t));
						close(client_recv_pipe);

				}else if(msg.type == STRING){
				//does not require a response

						printf("Client process %d wants me to echo a string.\n", client_PID);
						printf("Here it is \"%s\"\n",strtok(msg.message_text, "\n"));
						//getting weird newlines here, too

				}else if(msg.type == COMMAND){
				//does not require a response

						printf("Client process %d is done.\n", client_PID);
						break;//exit the infinite loop - "break" doesn't seem to exit the loop properly on P2

				}else{
						printf("Server recieved message of unknown type - exiting\n");
						break; //exit the infinite loop - "break" doesn't seem to exit the loop properly on P2
				}
			}

			printf("Child process handling client number %d has finished\n\n", client_PID);

			return 0; //once you have exited infinite while loop, exit this child process

		}//parent does not execute any code, no 'else' branch required
	}

	unlink(np); //get rid of the fifo
	return 0;
}
