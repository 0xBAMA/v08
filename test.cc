#include "resources/msg.h"
#include <iostream>

void func(glm::vec3 in);

class bar{
public:
  void func(glm::vec3 in)
  {
    std::cout << in.x << in.y << in.z << std::endl;
  }

};

int main(int argc, char const *argv[]) {

  glm::vec3 b;
  bar c;


  b = glm::vec3(0,0,0);

  c.func(b);

  func(b);

  return 0;
}

void func(glm::vec3 in)
{
  std::cout << in.x << in.y << in.z << std::endl;
}
