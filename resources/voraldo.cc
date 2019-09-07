#include "voraldo.h"
#include "perlin.h"
//simple to use implementation of 3d perlin noise

#include "lodepng.h"
// Good, simple png library


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
      {
        //initialize with zero values
        data[loopx][loopy][loopz].color.r = 0;
        data[loopx][loopy][loopz].color.g = 0;
        data[loopx][loopy][loopz].color.b = 0;
        data[loopx][loopy][loopz].color.a = 0;

        data[loopx][loopy][loopz].mask = false;
      }
}

bool voraldo::get_mask(glm::vec3 index)
{
  int x = floor(index.x);
  int y = floor(index.y);
  int z = floor(index.z);

  return data[x][y][z].mask;
}

void voraldo::set_mask(glm::vec3 index, bool in)
{
  int x = floor(index.x);
  int y = floor(index.y);
  int z = floor(index.z);

  data[x][y][z].mask = in;
}

//this is easier than messing with passing around things of type vox
//eventually it will deal with straight 16-bit RGBA values, so they can
//be passed directly to the GPU
RGBA voraldo::get_color(glm::vec3 index)
{
  int x = floor(index.x);
  int y = floor(index.y);
  int z = floor(index.z);

  return data[x][y][z].color;
}

void voraldo::set_color(glm::vec3 index, RGBA in)
{
  int x = floor(index.x);
  int y = floor(index.y);
  int z = floor(index.z);

  data[x][y][z].color = in;
}

void voraldo::mask_unmask_all()
{
  for(int z = 0; z < this->z; z++)
    for(int y = 0; y < this->y; y++)
      for(int x = 0; x < this->x; x++)
        set_mask(glm::vec3(x,y,z), false);
}

void voraldo::mask_invert_mask()
{
  for(int z = 0; z < this->z; z++)
    for(int y = 0; y < this->y; y++)
      for(int x = 0; x < this->x; x++)
        set_mask(glm::vec3(x,y,z), get_mask(glm::vec3(x,y,z)) ? false : true);
}

void voraldo::mask_all_nonzero()
{
  RGBA empty_black;

  empty_black.r = empty_black.g = empty_black.b = empty_black.a = 0;

  for(int z = 0; z < this->z; z++)
    for(int y = 0; y < this->y; y++)
      for(int x = 0; x < this->x; x++)
        if(compare_rgba(get_color(glm::vec3(x,y,z)), empty_black)) //in the style of string compare - return true for a non match, false for a match
          set_mask(glm::vec3(x,y,z), true);
}

//very simple placeholder output
//(NOTE THIS WILL BE PRETTY BROKEN AFTER 9/7/2019)
// void voraldo::print_cli()
// {
//   cout << endl;
//
//   for(int z = 0; z < this->z; z++)
//   {
//
//     cout << "slice " << std::hex << z << endl;
//
//     for(int y = 0; y < this->y; y++)
//     {
//       for(int x = 0; x < this->x; x++)
//       { //colors
//         if((int)data[x][y][z].val %2 == 0)
//           cout << "\e[32m";
//         if((int)data[x][y][z].val %3 == 0)
//           cout << "\e[34m";
//         if((int)data[x][y][z].val %4 == 0)
//           cout << "\e[35m";
//         if((int)data[x][y][z].val == 7)
//           cout << "\e[31m";
//
//         cout << data[x][y][z].val << "\e[0m"; //reset colors
//
//
//       }
//       cout << "  " << std::hex << y << endl;
//     }
//     cout << endl;
//   }
//
//   for(int x = 0; x < this->x; x++)
//     if(x/16 == 0)
//       cout << " ";
//     else
//       cout << x/16;
//
//   cout << endl;
//
//   for(int x = 0; x < this->x; x++)
//     cout << std::hex << x%16;
//
//   cout << std::dec << endl;
//
//
// }


void voraldo::draw_perlin_noise(float scale, float threshold, RGBA fill, bool draw, bool mask)
{
  PerlinNoise p;

  if(threshold >= 0)
  {
    for( int x = 0; x < this->x; x++ )
    {
      for( int y = 0; y < this->y; y++ )
      {
        for( int z = 0; z < this->z; z++ )
        {//per voxel

          if( p.noise( scale*x, scale*y, scale*z ) < threshold )
          {
            draw_point( glm::vec3( x, y, z ), fill, draw, mask );
          }
        }
      }
    }
  }
  else
  {
    threshold = std::abs(threshold);

    for( int x = 0; x < this->x; x++ )
    {
      for( int y = 0; y < this->y; y++ )
      {
        for( int z = 0; z < this->z; z++ )
        {//per voxel

          if( p.noise( scale*x, scale*y, scale*z ) > threshold )
          {
            draw_point( glm::vec3( x, y, z ), fill, draw, mask );
          }
        }
      }
    }
  }
}

void voraldo::save(std::string filename)
{
  int width = this->x;
  int height = this->y;
  int depth = this->z;

  filename += ".png";

  cout << "saving block of " << width << " " << height << " " << depth << endl;
  cout << " with name " << filename << endl;

  RGBA temporary_color;

  std::vector<unsigned char> image;

	unsigned image_width = width;
  unsigned image_height = height * depth;

  //2 bytes per channel, 4 channels, then number of pixels.
  int image_size_bytes = 2 * 4 * width * height * depth;

  image.resize( image_size_bytes ); // byte array has same number of bytes
                                                //as the image it reperesents

  int image_index = 0;

  for(int z = 0; z < depth; z++)
    for(int y = 0; y < height; y++)
      for(int x = 0; x < width; x++)
      {

        temporary_color = get_color(glm::vec3( x, y, z ));

        //red
				image[image_index] = (unsigned char) (temporary_color.r / 256); //high bits
  			image_index++;

  			image[image_index] = (unsigned char) (temporary_color.r % 256); //low bits
  			image_index++;

        //green
        image[image_index] = (unsigned char) (temporary_color.g / 256); //high bits
        image_index++;

        image[image_index] = (unsigned char) (temporary_color.g % 256); //low bits
        image_index++;

        //blue
        image[image_index] = (unsigned char) (temporary_color.b / 256); //high bits
        image_index++;

        image[image_index] = (unsigned char) (temporary_color.b % 256); //low bits
        image_index++;

        //alpha
        image[image_index] = (unsigned char) (temporary_color.a / 256); //high bits
        image_index++;

        image[image_index] = (unsigned char) (temporary_color.a % 256); //low bits
        image_index++;

      }


  //z * height * width  +  y * width  +  x

	unsigned error = lodepng::encode("resources/models/" + filename, image, image_width, image_height,  LodePNGColorType::LCT_RGBA, 16 );

	if(error) std::cout << "encoder error on the color image step " << error << ": "<< lodepng_error_text(error) << std::endl;

}



bool voraldo::planetest(glm::vec3 plane_point, glm::vec3 plane_normal, glm::vec3 test_point)
{
  //return false if the point is above the plane
	//return true if the point is below the plane

	double result = 0.0;

	//equation of plane

	// a (x-x1) + b (y-y1) + c (z-z1) = 0

	double a = plane_normal[0];
	double b = plane_normal[1];
	double c = plane_normal[2];

	double x1 = plane_point[0];
	double y1 = plane_point[1];
	double z1 = plane_point[2];

	double x = test_point[0];
	double y = test_point[1];
	double z = test_point[2];

	result = a * (x - x1) + b * (y - y1) + c * (z - z1);

	return (result < 0)?true:false;
}

bool voraldo::intersect_ray_bbox(glm::vec3 bbox_min, glm::vec3 bbox_max, glm::vec3 ray_org, glm::vec3 ray_dir, float &tmin, float &tmax, float t0, float t1)
{
/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

//I pulled this code after three attempts at my own implementation didn't work
  glm::vec3 bbox[2];
	int sign[3];

	glm::vec3 inv_direction = glm::vec3(1/ray_dir[0],1/ray_dir[1],1/ray_dir[2]);

	sign[0] = (inv_direction[0] < 0);
	sign[1] = (inv_direction[1] < 0);
	sign[2] = (inv_direction[2] < 0);

	bbox[0] = bbox_min;
	bbox[1] = bbox_max;


	//already declared (passed in by reference so that they can be used)
  tmin = (bbox[sign[0]][0] - ray_org[0]) * inv_direction[0];
  tmax = (bbox[1-sign[0]][0] - ray_org[0]) * inv_direction[0];

  float tymin = (bbox[sign[1]][1] - ray_org[1]) * inv_direction[1];
  float tymax = (bbox[1-sign[1]][1] - ray_org[1]) * inv_direction[1];

  if ( (tmin > tymax) || (tymin > tmax) )
    return false;
  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;

  float tzmin = (bbox[sign[2]][2] - ray_org[2]) * inv_direction[2];
  float tzmax = (bbox[1-sign[2]][2] - ray_org[2]) * inv_direction[2];

  if ( (tmin > tzmax) || (tzmin > tmax) )
    return false;
  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;
  return ( (tmin < t1) && (tmax > t0) );

}
