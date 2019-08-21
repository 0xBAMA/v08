CC=g++ -time

MAKE_EXE_CLIENT= -o v08c
MAKE_EXE_SERVER= -o v08s

MAKE_OBJ= -c


all: v08client v08server


#CLIENT
v08client: v08client.o
	$(CC) $(MAKE_EXE_CLIENT) v08client.o

v08client.o: v08client.cc v08client.h msg.h
	@echo
	@echo "BUILDING CLIENT EXECUTABLE"
	@echo
	$(CC) $(MAKE_OBJ) v08client.cc


#SERVER
v08server: v08server.o
	$(CC) $(MAKE_EXE_SERVER) v08server.o

v08server.o: v08server.cc v08server.h msg.h
	@echo
	@echo "BUILDING SERVER EXECUTABLE"
	@echo
	$(CC) $(MAKE_OBJ) v08server.cc


clean:
	@echo
	@echo "CLEANUP"
	@echo
	rm -f *.o
	rm -f *_recv
	rm -f *_send
	rm -f server_np
