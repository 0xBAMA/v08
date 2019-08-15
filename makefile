CC=g++

MAKE_EXE_CLIENT= -o v08client
MAKE_EXE_SERVER= -o v08server

MAKE_OBJ= -c


all: v08client v08server


v08client: v08client.o
	$(CC) $(MAKE_EXE_CLIENT) v08client.o

	#           client finished compiling

v08client.o: v08client.cc msg.h
	$(CC) $(MAKE_OBJ) v08client.cc


v08server: v08server.o
	$(CC) $(MAKE_EXE_SERVER) v08server.o

	#           server finished compiling


v08server.o: v08server.cc msg.h
	$(CC) $(MAKE_OBJ) v08server.cc


clean:
	rm *.o
