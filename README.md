# v08
Reimplementation of v07 as a client-server arrangement, also moving forward with the OpenGL ideas we started with in VIVS






## TO DO:

  implementation of voraldo functions (figure out threads)
  
  user menu prompt and input interpretation client-side
  
  glut functions server-side







# Usage

Open at least two terminals, navigate to the root directory of the repo - in one of them, type make - doesn't matter which one. Then type ./v08s to run the server process. In the other terminal(s), run ./v08c to run instances of the client process. Each client process expects that the server process is already running when it starts.



[note: this time, I'm going to multithread the voraldo draw functions, since every voxel can be evaluated independently, I want to be able to speed that up as much as possible to avoid delays - i.e. when a message is recieved, so that there aren't excessive delays between frames]

thread pooling-

https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

overhead associated with thread creation - 

https://stackoverflow.com/questions/3929774/how-much-overhead-is-there-when-creating-a-thread

c++ std::thread reference

http://www.cplusplus.com/reference/thread/thread/


At some point, I'm going to need to learn to set up Vulkan, so I can multithread things like sending textures to the gpu and stuff like that.



