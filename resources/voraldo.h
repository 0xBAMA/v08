#include <iostream> //terminal output
using std::cout;
using std::endl;

#include <vector>   //holding the data
using std::vector;

#include "msg.h" //definitions of message types
  //also includes glm types, since the messages need those

#include "lodepng.h"
// Good, simple png library


#ifndef VORALDO
#define VORALDO



typedef struct vox_t
{

  RGBA val;
  bool mask;

}vox;


class voraldo{
public:
  voraldo();      //this don't really do anything right now
  ~voraldo();     //this doesn't really have a use right now


  //SHAPES - right now, using int fill
  //common to all, draw sets wether or not to try to change the contents
  // of data - it can be blocked by masking, and mask sets wether or not
  //that cell should end up masked once the function returns.

  //scale increases the scale factor on acquisition of noise samples (lower number, larger contiguous shapes)
  // the noise function generates values between 0 and 1 -
  // a positive threshold value tells it to draw if the sampled noise value is less than the threshold
  // a negative threshold value tells it to draw if the sampled noise value is greater than the absolute value of the threshold
  void draw_perlin_noise(float scale, float threshold, RGBA fill, bool draw=true, bool mask=false);


  //self explanatory, uses floor() to get nearest index
  void draw_point(glm::vec3 position, RGBA fill, bool draw=true, bool mask=false)
  {
    //do the checking for mask value here - abstracts this away
    //so that other draw functions do not have to do any checking

    int x,y,z;

    x = std::floor(position.x);
    y = std::floor(position.y);
    z = std::floor(position.z);


    if(!data[x][y][z].mask)
    {//not masked, so it's ok to manipulate this data

      if(draw)  //user wants to draw
      {
        data[x][y][z].val = fill;
        data[x][y][z].mask = mask;
      }
      else
      {
        data[x][y][z].mask = mask;
      }

    }

    // else, it is masked, and should not be drawn to

  }

  //draws a triangle of the given thickness between points v0,v1 and v2
  void draw_triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float thickness, RGBA fill, bool draw=true, bool mask=false);


  //draws in all cells within a radius of a center point
  void draw_sphere(glm::vec3 center_point, float radius, RGBA fill, bool draw=true, bool mask=false)
  {
    for(int z = 0; z < this->z; z++)
    {
      for(int y = 0; y < this->y; y++)
      {
        for(int x = 0; x < this->x; x++)
        {

          if(glm::distance(glm::vec3(x,y,z), center_point) <= radius)
            draw_point(glm::vec3(x,y,z), fill, draw, mask);

        }
      }
    }
  }

  //draws an ellipsoid, which is a sphere that has three separate radii
  //this needs to be extended so that this shape can rotate. As we sit right now
  //this shape can change position but the axes can be only along the cardinal
  //axes of the block. It should be a relatively simple thing to use a rotation
  //matrix, but I will need to think about the architecture of the function.
  void draw_ellipsoid(glm::vec3 center, glm::vec3 radii, RGBA fill, bool draw=true, bool mask=false);

  //draws a cylinder with circular faces centered at bvec and tvec, with the
  //radius specified by the input argument.
  void draw_cylinder(glm::vec3 bvec, glm::vec3 tvec, double radius, RGBA fill, bool draw=true, bool mask=false);

  //draws a cylinder that has a smaller cyilnder taken out of the center
  //	>>	make sure to add validation code for if inner >= outer
  void draw_tube(glm::vec3 bvec, glm::vec3 tvec, double inner_radius, double outer_radius, RGBA fill, bool draw=true, bool mask=false);

  //draws a block shape that can be stretched, skewed, twisted and rotated.
  //the main issue comes in when the four points making up a face do not lie in
  //the same plane - there the algorithm has to choose between two ambigous
  //cases and the results will be less than predictable i.e. a default is used.

  // note, re: arrangement of verticies on quadrilaterial hexahedron:

  // 	a : -x, +y, +z
  // 	b : -x, -y, +z
  // 	c : +x, +y, +z
  // 	d : +x, -y, +z
  // 	e : -x, +y, -z
  // 	f : -x, -y, -z
  // 	g : +x, +y, -z
  // 	h : +x, -y, -z

  // point location reference - it can be rotated, but the faces must be
  //  ACEG, CGDH, EGFH, ABEF, ABCD and BDFH - axes just for reference

  // 	   e-------g    +y
  // 	  /|      /|		 |
  // 	 / |     / |     |___+x
  // 	a-------c  |    /
  // 	|  f----|--h   +z
  // 	| /     | /
  //  |/      |/
  // 	b-------d
  void draw_quadrilateral_hexahedron(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 e, glm::vec3 f, glm::vec3 g, glm::vec3 h, RGBA fill, bool draw=true, bool mask=false);


  //too many arguments? allows for the scaling, rotation and then placement of the icosahedron.
  //in addition, a material can be specified to use for the vertexes, the material for the edges, and that for the faces.
  //the edge width (0 -> don't draw, 0-1 -> draw a line or >1 -> draw a cylinder) and the radius of the spheres for the verticies can be specified.
  //right now there's no way to manipulate the faces, beyond the material.
  void draw_regular_icosahedron(double x_rot, double y_rot, double z_rot, double scale, glm::vec3 center_point, RGBA vertex_material, double verticies_radius, RGBA edge_material, double edge_thickness, RGBA face_material, bool draw_faces = true, bool draw=true, bool mask=false);


  //hard coded filename
  void draw_heightmap(/*std::string filename, std::vector<Vox> materials,*/ bool draw=true, bool mask=false);

  void blur( int radius, bool change_alpha=true);




  //MASKING

  void mask_unmask_all();
  void mask_invert_mask();
  void mask_all_nonzero(); //mask if r,g,b, or a are nonzero
  // void mask_by_state

  // void mask_over_red_val(float thresh)
  // void mask_under_red_val(float thresh) ... maybe - r,g,b,a, too?






  //BASIC UTILITIES

  void initialize(int x, int y, int z);
  void print_cli();

  void save(std::string filename); //output block to png image (slices)
  void load(std::string filename, bool overwrite=true, short thresh=0);
  //if overwrite is set to false, areas in the loaded image with alpha
  // less than thresh/(max value of short) will not write their values
  // note that masking will achieve a similar effect, but based on the
  // local data rather than the loaded data.



  int x,y,z;

private:

  bool get_mask(glm::vec3 index);
  void set_mask(glm::vec3 index, bool in);

  RGBA get_val(glm::vec3 index);
  void set_val(glm::vec3 index, RGBA in);

  //this is temporarily a float
  // float get_data_by_vector_index(glm::vec3 index);
  // void set_data_by_vector_index(glm::vec3 index, vox set, bool draw=true,
  //   bool mask=false, bool force=false);

  bool planetest(glm::vec3 plane_point, glm::vec3 plane_normal, glm::vec3 test_point);
    // returns false if the point is above the plane, true if it is above

  bool intersect_ray_bbox(glm::vec3 bbox_min, glm::vec3 bbox_max, glm::vec3 ray_org, glm::vec3 ray_dir, float &tmin, float &tmax, float t0=0, float t1=9999);
    // returns true if the vector starting at ray_org doing in direction ray_dir hits the bounding box
    // tmin and tmax let the user know the far and near points of intersection


  vector<vector<vector<vox> > > data;

};

#endif
