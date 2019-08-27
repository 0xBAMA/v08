CC=g++ -time

MAKE_EXE_CLIENT= -o v08c
MAKE_EXE_SERVER= -o v08s

MAKE_OBJ= -c


#a note on my usage of echo -
#  @ makes it silent
#  -e enables escape sequences, i.e. \whatever

all: v08client v08server
	@echo
	@echo -e "\e[31mCLEANUP\e[0m"
	@echo
	rm -f *.o
	rm -f *_recv
	rm -f *_send
	rm -f server_np
	@echo


#CLIENT
v08client: v08client.o
	$(CC) $(MAKE_EXE_CLIENT) v08client.o

v08client.o: v08client.cc v08client.h resources/msg.h
	@echo
	@echo -e "\e[31mBUILDING CLIENT EXECUTABLE\e[0m"
	@echo
	$(CC) $(MAKE_OBJ) v08client.cc


#SERVER
v08server: v08server.o
	$(CC) $(MAKE_EXE_SERVER) v08server.o voraldo.o

v08server.o: v08server.cc v08server.h resources/msg.h voraldo.o
	$(CC) $(MAKE_OBJ) v08server.cc

voraldo.o: resources/voraldo.h resources/voraldo.cc
	@echo
	@echo -e "\e[31mBUILDING SERVER EXECUTABLE\e[0m"
	@echo
	$(CC) $(MAKE_OBJ) resources/voraldo.cc

clean:
	@echo
	@echo -e "\e[31mCLEANUP\e[0m"
	@echo
	rm -f *.o
	rm -f *_recv
	rm -f *_send
	rm -f server_np
	@echo
