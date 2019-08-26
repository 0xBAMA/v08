# v08
Reimplementation of v07 as a client-server arrangement, also moving forward with the OpenGL ideas we started with in VIVS


note: this time, I'm going to multithread the voraldo draw functions, since every voxel can be evaluated independently, I want to be able to speed that up as much as possible to avoid delays - i.e. when a message is recieved, so that there aren't excessive delays between frames

https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

https://stackoverflow.com/questions/3929774/how-much-overhead-is-there-when-creating-a-thread

http://www.cplusplus.com/reference/thread/thread/




TO DO:

  implementation of voraldo functions (figure out threads)
  
  user menu prompt and input interpretation client-side
  
  glut functions server-side
