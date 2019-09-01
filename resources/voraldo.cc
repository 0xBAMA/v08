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

  srand(time(NULL)); // by default seeded with 1, here seeded with the time

  //fill the block with random values 1-10
  for(int loopx = 0; loopx < x; loopx++)
    for(int loopy = 0; loopy < y; loopy++)
      for(int loopz = 0; loopz < z; loopz++)
        data[loopx][loopy][loopz].val = ((int)std::rand()) % 10;


  data[0][0][0].val = 1.0f;
  data[2][2][2].val = 1.0f;

}


//very simple
void voraldo::print_cli()
{
  cout << endl;

  for(int z = 0; z < this->z; z++)
  {

    cout << "slice " << std::hex << z << endl;

    for(int y = 0; y < this->y; y++)
    {
      for(int x = 0; x < this->x; x++)
      { //colors
        if((int)data[x][y][z].val %2 == 0)
          cout << "\e[32m";
        if((int)data[x][y][z].val %3 == 0)
          cout << "\e[34m";
        if((int)data[x][y][z].val %4 == 0)
          cout << "\e[35m";

        cout << data[x][y][z].val << "\e[0m"; //reset colors


      }
      cout << "  " << std::hex << y << endl;
    }
    cout << endl;
  }

  for(int x = 0; x < this->x; x++)
    if(x/16 == 0)
      cout << " ";
    else
      cout << x/16;

  cout << endl;

  for(int x = 0; x < this->x; x++)
    cout << std::hex << x%16;

  cout << std::dec << endl;
  

}

void voraldo::draw_sphere(glm::vec3 center, float radius, int fill)
{
  for(int z = 0; z < this->z; z++)
  {
    for(int y = 0; y < this->y; y++)
    {
      for(int x = 0; x < this->x; x++)
      {
        if(glm::distance(glm::vec3(x,y,z), center) <= radius)
          data[x][y][z].val = fill;
      }
    }
  }
}
