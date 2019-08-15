CC=g++ -time

MAKE_EXE_CLIENT= -o v08c
MAKE_EXE_SERVER= -o v08s

MAKE_OBJ= -c


all: v08client v08server

#CLIENT
v08client: v08client.o
	$(CC) $(MAKE_EXE_CLIENT) v08client.o

v08client.o: v08client.cc v08client.h msg.h
	$(CC) $(MAKE_OBJ) v08client.cc

#SERVER
v08server: v08server.o
	$(CC) $(MAKE_EXE_SERVER) v08server.o

v08server.o: v08server.cc v08server.h msg.h
	$(CC) $(MAKE_OBJ) v08server.cc


clean:
	rm *.o

pipe_cleanup:
	rm *_recv
	rm *_send

server_cleanup:
	rm server_np
