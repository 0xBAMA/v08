#include "v08server.h"

int current_client_count;

//want this accessible in global scope
server * s;

int main(int argc, char const *argv[])
{
	int x,y,z;

	if(argc == 4)
	{
		x = atoi(argv[1]);
		y = atoi(argv[2]);
		z = atoi(argv[3]);
		cout << "input dimensions used (" << x << "x" << y << "x" << z << ")\n";

	}
	else
	{
		x = 256;
		y = 256;
		z = 256;
		cout << "standard dimensions used (256x256x256)" << endl;
	}

	s = new server(x,y,z);


	//open a window, set up all the callbacks - should this be internal to the
	// server class?
	while(s->listen()); // should happen in the timer function?

	return 0;

}
