#include "v08client.h"

int main()
{
	client c;

	while(!c.done)
		c.menu();

	return 0;
}
