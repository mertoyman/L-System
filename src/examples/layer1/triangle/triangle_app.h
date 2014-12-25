////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Triangle example: The most basic OpenGL application
//
// Level: 0
//
// Demonstrates:
//   Basic framework app
//   Shaders
//   Basic Matrices
//
//

// namespace help us to group classes and avoid clashes
namespace octet {

  class Box {
    //struct dimensions { int x; int y; } dim;
    struct position { float x; float y;} pos;
    enum {dimension = 1};
    enum state { DEAD, ALIVE } type;
  public:
    Box () {}
    Box(float x_, float y_) { pos.x = x_; pos.y = y_; type = DEAD; }

    void setState ( int n ) {
      n==0?type=DEAD:type=ALIVE;
    }

    

    int getState () const  {
      return type;
    }
  };

  class Grid {
    dynarray< dynarray<Box> > grid;
  public:
    Grid(){}

    void init () {
      grid[0][5].setState(1);
      grid[1][5].setState(1);
      grid[0][6].setState(1);
      grid[1][6].setState(1);
      grid[2][7].setState(1);
    }

    // create 2D grid nxn dimension
    void insert ( int n ) {          
      for ( int i = 0; i < n; i++ ) {
        grid.push_back( dynarray<Box>() );
        for ( int j = 0; j < n; j++ ) {
          grid.back().push_back(Box(static_cast<float>(i), static_cast<float>(j)));
        }
      }
    }

    int checkLife ( int x, int y ) {
      int count = -1; // because it will count itself
      for ( int i = (x-1); i <= (x+1); i++ ) {
        for ( int j = (y-1); j <= (y+1); j++ ) {
          if ( grid[i][j].getState() == 1 )
            count++;
        }
      }
      return count;
    }

    void setLife ( int x, int y ) {
      int temp = checkLife ( x, y );
      if ( grid[x][y].getState() == 1 && temp >= 2 && temp <= 4 )
        grid[x][y].setState(1);

    }

    void checkState () {
      for ( unsigned int i = 0; i < grid.size(); i++ ) {
        for ( unsigned int j = 0; j < grid.size(); j++ ) {
          checkLife(i,j);

        }
      }
    
    }

    void getSize () {
      printf ("%d %d", grid.size(), grid[0].size());
    }
  };

  class Cellular {
    Grid cg;
    
  public:
    Cellular () {}

    void create ( int n ) {    
        cg.insert( n );
        cg.getSize();
    }

    
    
  };

  // this is an app to draw a triangle, it takes some of its workings from the class "app"
  class triangle_app : public app {
    // Matrix to transform points on our triangle to the world space
    // This allows us to move and rotate our triangle
    mat4t modelToWorld;

    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    mat4t cameraToWorld;

    // shader to draw a solid color
    color_shader color_shader_;

    Cellular cel;

	float angle;

  public:

    // this is called when we construct the class
    triangle_app(int argc, char **argv) : app(argc, argv) {
    }

    // this is called once OpenGL is initialized
    void app_init() {
      // initialize the shader
      color_shader_.init();

      // put the triangle at the center of the world
      modelToWorld.loadIdentity();

      // put the camera a short distance from the center, looking towards the triangle
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, 5);

	    angle = 0.0f;

      cel.create(10);
    }

    // this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      // set a viewport - includes whole window area
      glViewport(x, y, w, h);

      // clear the background to black
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // allow Z buffer depth testing (closer objects are always drawn in front of far ones)
      glEnable(GL_DEPTH_TEST);

      // build a projection matrix: model -> world -> camera -> projection
      // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

      // spin the triangle by rotating about Z (the view direction)
      //modelToWorld.rotateZ(1);

      // set up opengl to draw flat shaded triangles of a fixed color
      vec4 color(0, 0, 1, 1);
      color_shader_.render(modelToProjection, color);

      // this is an array of the positions of the corners of the triangle in 3D
      // static const means that it is created at compile time
      static const float vertices[] = {
        -2, -2, 0,
         2, -2, 0,
         2,  2, 0,
      };

      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
    
      // finally, draw the triangle (3 vertices)
      glDrawArrays(GL_TRIANGLES, 0, 3);

	  if (is_key_down(key_left)) {
		  cameraToWorld.rotateX(-angle);
		  cameraToWorld.rotateY(10.0f);
		  cameraToWorld.rotateX(angle);
      }
	  else if (is_key_down(key_right)) {
		  cameraToWorld.rotateX(-angle);
		  cameraToWorld.rotateY(-10.0f);
		  cameraToWorld.rotateX(angle);
      }
	  else if (is_key_down(key_up)) {
		  angle += 10.0f;
		  cameraToWorld.rotateX(10.0f);
      }
	  else if (is_key_down(key_down)) {
		  angle -= 10.0f;
		  cameraToWorld.rotateX(-10.0f);
      }
	  else if (is_key_down('W'))
		  cameraToWorld.translate(0.0f, 0.0f, -1.0f);
	  else if (is_key_down('S'))
		  cameraToWorld.translate(0.0f, 0.0f, 1.0f);
	  else if (is_key_down('A'))
		  cameraToWorld.translate(-1.0f, 0.0f, 0.0f);
	  else if (is_key_down('D'))
		  cameraToWorld.translate(1.0f, 0.0f, 0.0f);

    }
  };
}
