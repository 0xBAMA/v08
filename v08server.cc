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
		x = 16;
		y = 16;
		z = 16;
		cout << "standard dimensions used (16x16x16)" << endl;
	}

	s = new server(x,y,z);

	while(s->listen());

	return 0;

}
