#include <iostream> //terminal output
using std::cout;
using std::endl;

#include <vector>   //holding the data
using std::vector;

#include "msg.h" //definitions of message types





typedef struct vox_t
{

  float val;

}vox;


class voraldo{
public:
  voraldo();      //this don't really do anything right now
  ~voraldo();     //this doesn't really have a use right now

  void initialize(int x, int y, int z);
  void print_cli();

  void draw_sphere(glm::vec3 center, float radius, int fill);

private:

  int x,y,z;

  vector<vector<vector<vox>>> data;

};
