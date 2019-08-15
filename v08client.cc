#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <stdbool.h>
#include <time.h>

#include "msg.h" //definitions of message types


int main()
{
	int client_PID = getpid();
	int server_initial_pipe;
	int client_send_pipe,client_recv_pipe;

	initial_msg_t m_init;

	printf("This is an instance of the CS_2 client process, started with PID %d\n", client_PID);




	//before writing to the server_np, open this process's pipes, i.e. <PID>_send and <PID>_recieve
		//avoids a race condition, makes sure that the pipes are set up BEFORE the
		//forked server process tries to used them to communicate with this process

	char send_pipe[12]; //pid is up to 5 digits, plus underscore, plus "send" and termination
	char recv_pipe[12]; //using recv instead of "recieve", since it's also 4 chars


	//create the two pipes to allow bidirectional client-server communication
	sprintf(send_pipe,"%d_send",client_PID);
	sprintf(recv_pipe,"%d_recv",client_PID);

	mkfifo(send_pipe, 0600);
	mkfifo(recv_pipe, 0600);

	//initalize the message
	m_init.PID = client_PID;

	printf("about to connect\n");

	//make sure to follow the pattern open-write-close
	//open the pipe that allows the initial contact between client and server
	server_initial_pipe = open("server_np", O_WRONLY);
	//write a message to server indicating the client's PID, allowing further communication
	write(server_initial_pipe,(char*)&m_init,sizeof(msg_t));
	//close the initial pipe
	close(server_initial_pipe);

	printf("about to connect\n");


	char exit_string[5] = "exit";
	char send_string[5] = "send";
	char time_string[5] = "time";
	char stat_string[5] = "stat";

	char buf[5]; //allows for comparison

	char chlid_message_string[MSG_LENGTH]; //holds the user input

	msg_t msg; //holds the message to be sent



	//client has made contact with server - now we are using the PID specific pipes

	printf("Client process connected to server.\n");

	while(1)
	{//loop till "exit" is entered
		printf("Try one of the following options:\n");
		printf("  \'send:\' followed by a string, to have the server echo the string\n");
		printf("  \'stat\' to have the server report the current number of children\n");
		printf("  \'time\' to have the server report the current time\n");
		printf("  \'exit\' to have the server exit (also terminates client process)\n");
		printf("\n>");

		fgets(chlid_message_string,MSG_LENGTH,stdin);//get the input
		sprintf(buf,"%.4s",chlid_message_string);//grab the first four characters

		printf("\n\n");

		//----------------------------OPTIONS--------------------------------

		//SEND OPTION
		if(!strcmp(buf,send_string)) //first four letters are "send"
		{//server should report the string to the terminal

			sprintf(msg.message_text, "%s", chlid_message_string+5); //drop "send:"
			msg.type = STRING;

			client_send_pipe = open(send_pipe,O_WRONLY);				//open
			write(client_send_pipe,(char*)&msg,sizeof(msg_t));	//write
			close(client_send_pipe);														//close

		}


		//STAT OPTION
		if(!strcmp(buf,stat_string))//first four letters are "stat"
		{//server will send a response back with the number of children

			sprintf(msg.message_text, "this text doesn't matter");
			msg.type = STATUS;

			client_send_pipe = open(send_pipe,O_WRONLY);				//open
			write(client_send_pipe,(char*)&msg,sizeof(msg_t));	//write
			close(client_send_pipe);														//close

			client_recv_pipe = open(recv_pipe,O_RDONLY);				//open
			read(client_recv_pipe,(char*)&msg,sizeof(msg_t));		//read (blocking)
			close(client_send_pipe);														//close

			printf("Server says: \"%s\"\n\n",msg.message_text);			//report the number of children

		}


		//TIME OPTION
		if(!strcmp(buf,time_string))//first four letters are "time"
		{//server will send a response back with the time of day

			sprintf(msg.message_text, "this text doesn't matter");
			msg.type = TIME;

			client_send_pipe = open(send_pipe,O_WRONLY);				//open
			write(client_send_pipe,(char*)&msg,sizeof(msg_t));	//write
			close(client_send_pipe);														//close

			client_recv_pipe = open(recv_pipe,O_RDONLY);				//open
			read(client_recv_pipe,(char*)&msg,sizeof(msg_t));		//read (blocking)
			close(client_send_pipe);														//close

			printf("Server says: \"%s\"\n\n", msg.message_text); //report the time

		}


		//EXIT OPTION
		if(!strcmp(buf,exit_string))
		{//the server exits, the client also exits

			sprintf(msg.message_text, "this text doesn't matter");
			msg.type = COMMAND;

			client_send_pipe = open(send_pipe,O_WRONLY);				//open
			write(client_send_pipe,(char*)&msg,sizeof(msg_t));	//write
			close(client_send_pipe);														//close

			break;//exit the infinite while loop, if the user input started with "exit"
		}
	}

	//unlink pipes
	unlink(send_pipe);
	unlink(recv_pipe);

	return 0;
}
