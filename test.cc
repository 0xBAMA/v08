#include "resources/msg.h"
#include <iostream>

void func(glm::vec3 in);

int main(int argc, char const *argv[]) {

  glm::vec3 b;

  b = glm::vec3(0,0,0);

  func(b);

  return 0;
}

void func(glm::vec3 in)
{
  std::cout << in.x << " " << in.y << " " << in.z << std::endl;
}

///... so what's going on?
