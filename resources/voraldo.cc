#include "voraldo.h"




//default constructor
voraldo::voraldo()
{
  // std::cout << "voraldo has entered scope" << std::endl;
}

//destructor
voraldo::~voraldo()
{
  // std::cout << "voraldo has left scope" << std::endl;
}




//initialization
void voraldo::initialize(int x, int y, int z)
{
  //initialize dimensions
  cout << "voraldo initialized with dimensions";
  cout << " x: " << (this->x = x);
  cout << " y: " << (this->y = y);
  cout << " z: " << (this->z = z) << endl;

//create the vectors along the x axis (of length x)
  data.resize(this->x);

  for(int xd = 0; xd < this->x; xd++)
  {
    //create the vectors along the y axis (of length y)
    data[xd].resize(this->y);
    for(int yd = 0; yd < this->y; yd++)
    {
      //create the vectors along the z axis (of length z)
      data[xd][yd].resize(this->z);
    }
  }

  data[0][0][0].val = 1.0f;
  data[2][2][2].val = 1.0f;

}


//very simple
void voraldo::print_cli()
{
  cout << endl;

  for(int z = 0; z < this->z; z++)
  {
    for(int y = 0; y < this->y; y++)
    {
      for(int x = 0; x < this->x; x++)
      {
        cout << data[x][y][z].val;
      }
      cout << endl;
    }
    cout << endl;
  }
}
