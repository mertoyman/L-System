////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Triangle example: The most basic OpenGL application
//
// Level: 1
//
// Demonstrates:
//   Basic framework app
//   Shaders
//   Basic Matrices
//
//
#include <time.h>

namespace octet {

  struct window {
	  float width; // width of my wall
	  float	height; // height of my wall
	  float wallsize; // define who thin is a wall 
	  float box; // box size
  } myWindow;

  class box {
    // where is our box (overkill for a ping game!)
    mat4t modelToWorld;

    // half the width of the box
    float halfWidth;

    // half the height of the box
    float halfHeight;

	//position of box
	vec4 position;

    // what color is our box
    vec4 color;
  public:
    box() { }

    void init(vec4 &_color, float x, float y, float w, float h) {
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
	  position = vec4(x, y, 0, 0); 
      halfWidth = w * 0.5f;
      halfHeight = h * 0.5f;
      color = _color;
    }
	
    // render a Box object with a color_shape_shader
	void render(color_shape_shader &shader, mat4t &cameraToWorld) {
      // build a projection matrix: model -> world -> camera -> projection
      // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

      // set up the uniforms for the shader
      //shader.render(modelToProjection, color, position);
	  shader.render(modelToProjection, color, position);
      // this is an array of the positions of the corners of the box in 3D
      // a straight "float" here means this array is being generated here at runtime.
      float vertices[] = {
        -halfWidth, -halfHeight, 0,
         halfWidth, -halfHeight, 0,
         halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };

      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
      //printf("%f\n", attribute_pos.x);
      // finally, draw the box (4 vertices)
      glDrawArrays(GL_QUADS, 0, 4); //GL_TRIANGLE_FAN
    }
	
	// render a Box object with a color_shader
	void render(color_shader &shader, mat4t &cameraToWorld) { 
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
	  shader.render(modelToProjection, color);

      float vertices[] = {
        -halfWidth, -halfHeight, 0,
         halfWidth, -halfHeight, 0,
         halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
      glDrawArrays(GL_QUADS, 0, 4); //GL_TRIANGLE_FAN
    }

	// render a Box object with a star_shader
	void render(star_shader &shader, mat4t &cameraToWorld) { 
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
	  shader.render(modelToProjection, color);

      float vertices[] = {
        -halfWidth, -halfHeight, 0,
         halfWidth, -halfHeight, 0,
         halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
      glDrawArrays(GL_QUADS, 0, 4); //GL_TRIANGLE_FAN
    }

	// render a Box object with a circle_shader
	void render(cirlce_shader &shader, mat4t &cameraToWorld) { 
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
	  //vec4 dimensions = vec4(halfWidth, halfHeight, 0.0, 0.0);
	  shader.render(modelToProjection, color, position, halfWidth);

      float vertices[] = {
        -halfWidth, -halfHeight, 0,
         halfWidth, -halfHeight, 0,
         halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);

	  // I use this to pass a color for each vertex
      static const float uvs[] = {
         1.0f,  0.0f,
         1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f,  0.0f,
      };

      // attribute_uv is position in the texture of each corner
      glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs );
      glEnableVertexAttribArray(attribute_uv);

      glDrawArrays(GL_QUADS, 0, 4); //GL_TRIANGLE_FAN
    }
	// set the color of a Box object
	void setColor (vec4 &_color) { color = _color;}
	// get the color of a Box object
	vec4 &getColor () { return color; }

    // move the object
    void translate(float x, float y) {
      modelToWorld.translate(x, y, 0);
	  // keep track of its position
	  position = vec4(position.x() + x, position.y() + y,0,0);
    }

	// return the current position of the certain box
	vec4 getPosition() { return position; }

  };

  class ping_app : public octet::app {
    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    mat4t cameraToWorld;

	 // a texture for our text
    GLuint font_texture;

    // information for our text
    bitmap_font font;

	// shader to draw a textured triangle
    texture_shader texture_shader_;

	int score;
	// stores how many boxes are aligned in each row
	bvec4 matches; 

    // shaders to draw 
    color_shader color_shader_;
	color_shape_shader myShader;
	star_shader starShader;
	cirlce_shader circleShader;

	clock_t myclock;

	enum game_state {
		left,
		right,
		up,
		down
	};
	game_state mystate;

	ALuint bang;
	ALuint get_sound_source() { return bang; }
    // counters for scores
    int scores[2];

    // game objects
    box court[4];
	box lines[5];
	box boxes[12];
  
    // move the right boxes
    void simulate() {
	  // check if the move is valid (in borders) and then find the box that should alter its position with the moveable Box
	  // then play a sound and sleep because otherwise the buttons are so sensitive that to box makes lots of translations in one button hit
	  // alignedLines() checks how many aligned boxes there are and updates the score
      if (is_key_down(key_up) && checkBorders(0.0, 2.5)) {
		  mystate = up;
		  findBox().translate(0, -2.5f);
		  boxes[0].translate( 0, 2.5f);
		  playMusic();
		  Sleep(100);
		  alignedLines();
      } 
	  else if (is_key_down(key_down) && checkBorders(0.0, -2.5)) {
		  mystate = down;
		  findBox().translate(0, 2.5f);
		  boxes[0].translate( 0, -2.5f);
		  playMusic();
		  Sleep(100);
		  alignedLines();
      }
	  else if (is_key_down(key_right) && checkBorders(2.5, 0.0)) {		  
		  mystate = right;
		  findBox().translate( -2.5, 0);
		  boxes[0].translate( 2.5f, 0);
		  playMusic();
		  Sleep(100);
		  alignedLines();
	  }
	  else if (is_key_down(key_left) && checkBorders(-2.5, 0.0)) {
		  mystate = left;
		  findBox().translate( 2.5, 0);
		  boxes[0].translate( -2.5f, 0);
		  playMusic();
		  Sleep(100);
		  alignedLines();
	  }
    }

	 void draw_text(texture_shader &shader, float x, float y, float scale, const char *text) {
      mat4t modelToWorld;
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      modelToWorld.scale(scale, scale, 1);
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

      /*mat4t tmp;
      glLoadIdentity();
      glTranslatef(x, y, 0);
      glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);
      glScalef(scale, scale, 1);
      glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);*/

      enum { max_quads = 32 };
      bitmap_font::vertex vertices[max_quads*4];
      uint32_t indices[max_quads*6];
      aabb bb(vec3(0, 0, 0), vec3(256, 256, 0));

      unsigned num_quads = font.build_mesh(bb, vertices, indices, max_quads, text, 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, font_texture);

      shader.render(modelToProjection, 0);

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].x );
      glEnableVertexAttribArray(attribute_pos);
      glVertexAttribPointer(attribute_uv, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].u );
      glEnableVertexAttribArray(attribute_uv);

      glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_INT, indices);
    }

  public:

    // this is called when we construct the class
    ping_app(int argc, char **argv) : app(argc, argv), font(512, 256, "assets/big.fnt") {
    }

    // this is called once OpenGL is initialized
    void app_init() {

		// set up the shader
      texture_shader_.init();
	  font_texture = resources::get_texture_handle(GL_RGBA, "assets/big_0.gif");
	  score=0;
	  matches = bvec4( 0, 0, 0, 0);
	  // initialize the shaders
      color_shader_.init();
	  myShader.init();
	  starShader.init();
	  circleShader.init();
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, 5);
	  // width and height of my game's walls
	  myWindow.width = 10.0f;
	  myWindow.height = 7.5f;
	  myWindow.wallsize = 0.1f;
	  myWindow.box = 2.5f;
	  // load the sound file
	  bang = resources::get_sound_handle(AL_FORMAT_MONO16, "assets/beep.wav");
	  alGenSources(1, &bang);
	  // initialize all the objects of the scene
	  court[0].init(vec4(1, 1, 1, 1), 0, -myWindow.height * 0.5f, myWindow.width, myWindow.wallsize);
      court[1].init(vec4(1, 1, 1, 1), 0,  myWindow.height * 0.5f, myWindow.width, myWindow.wallsize);
	  court[2].init(vec4(1, 1, 1, 1), -myWindow.width * 0.5f, 0, myWindow.wallsize, myWindow.height);
      court[3].init(vec4(1, 1, 1, 1), myWindow.width * 0.5f,  0, myWindow.wallsize, myWindow.height);

	  lines[0].init(vec4(1,1,1,1), -myWindow.width * 0.25f, 0, myWindow.wallsize, myWindow.height); // I multiple with 0.25 == /4 to divide width in 4 pieces
	  lines[1].init(vec4(1,1,1,1), 0, 0, myWindow.wallsize, myWindow.height);
	  lines[2].init(vec4(1,1,1,1), myWindow.width * 0.25f, 0, myWindow.wallsize, myWindow.height);
	  lines[3].init(vec4(1,1,1,1), 0, myWindow.height * 0.166f, 10, myWindow.wallsize);  // I multiple by 0.166 == myWindow.width/3/2 to divide height in 3 pieces
	  lines[4].init(vec4(1,1,1,1), 0, -myWindow.height * 0.166f, 10, myWindow.wallsize);
	  
	  boxes[0].init(vec4(0.0,0.0,0.0,0.0), -myWindow.height * 0.5f, -myWindow.width * 0.25f, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[1].init(vec4(0.0,0.0,0.0,0.0), -1.25f, -myWindow.width * 0.25f, myWindow.width * 0.25f, myWindow.width * 0.25f); // myWindow.height * 0.166f == myWindow.height/3/2 
	  boxes[2].init(vec4(0.0,0.0,0.0,0.0),  1.25f, -myWindow.width * 0.25f, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[3].init(vec4(0.0,0.0,0.0,0.0),  myWindow.height * 0.5f, -myWindow.width * 0.25f, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[4].init(vec4(0.0,0.0,0.0,0.0), -myWindow.height * 0.5f,  0, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[5].init(vec4(0.0,0.0,0.0,0.0), -1.25f,  0, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[6].init(vec4(0.0,0.0,0.0,0.0), 1.25f,  0, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[7].init(vec4(0.0,0.0,0.0,0.0), myWindow.height * 0.5f,  0, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[8].init(vec4(0.0,0.0,0.0,0.0), -myWindow.height * 0.5f,  myWindow.width * 0.25f, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[9].init(vec4(0.0,0.0,0.0,0.0), -1.25f, myWindow.width * 0.25f, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[10].init(vec4(0.0,0.0,0.0,0.0), 1.25f,  myWindow.width * 0.25f, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  boxes[11].init(vec4(0.0,0.0,0.0,0.0), myWindow.height * 0.5f,  myWindow.width * 0.25f, myWindow.width * 0.25f, myWindow.width * 0.25f);
	  
	  // randomly give a color in each box
	  for (int i = 0; i != 1; ++i) {
		  int temp;
		  for (int j = 0; j != 4; ++j) {
			  temp = randomColor();
			  //to problima me tin boxes[temp].getColor() == vec4(0,0,0,0) itan oti den uparxei ilopoimeni tetoia == 
			  //prepei na elegxw ena ena ta stoixeia
			  if (boxes[temp].getColor().w() == 0.0f) // if w == 0 then its color is black so set a new value for color
			  //if ( boxes[temp].getColor().xyz0() == vec4(0.0,0.0,0.0,0.0) )
				  boxes[temp].setColor(vec4(1.0,0.0,0.0,1.0));
			  else
				  --j;
		  }
		  for (int j = 0; j != 4; ++j) {
			  temp = randomColor();
			  if (boxes[temp].getColor().w() == 0.0f) // if w == 0 then its color is black so set a new value for color
				  boxes[temp].setColor(vec4(0.0,1.0,0.0,1));
			  else
				  --j;
		  }
		  for (int j = 0; j != 4; ++j) {
			  temp = randomColor();
			  if (boxes[temp].getColor().w() == 0.0f) // if w == 0 then its color is black so set a new value for color
				  boxes[temp].setColor(vec4(0.0,0.0,1.0,1.0));
			  else
				  --j;
		  }
	  }
	  boxes[0].setColor(vec4(0.0,0.0,0.0,1.0)); // set the moveable's box color as black 

	  myclock = clock();
    }

	void playMusic () {
		ALuint source = get_sound_source();
        alSourcei(source, AL_BUFFER, bang);
        alSourcePlay(source);
	}
	// give randomly a number between 0 - 12
	int randomColor () {
		srand (time(NULL)); // without it, it chooses the same numbers every time
		int t;
		t = rand()%12; // % 12 to be up to 12
		return t;
	}
	// alternative for Sleep() function
	void Wait(int seconds)
	{
	  clock_t endwait;
	  endwait = clock () + 1 * CLK_TCK ;
	  while (clock() < endwait) {}
	}

	// find which is the box to alternate its position with the moving box
	box &findBox() { 
		// if the moving box is going left then search which box's position mathes that position
		if (mystate == left) {
			for (int i=0; i != 12; ++i) {
				if ( ((boxes[0].getPosition().x() - 2.5f) == boxes[i].getPosition().x()) && (boxes[0].getPosition().y() == boxes[i].getPosition().y())){
					return boxes[i];
				}
			}
		}
		// if the moving box is going right then search which box's position mathes that position
		else if (mystate == right) {
			for (int i=0; i != 12; ++i) {
				if ( ((boxes[0].getPosition().x() + 2.5f) == boxes[i].getPosition().x()) && (boxes[0].getPosition().y() == boxes[i].getPosition().y())){
					return boxes[i];
				}
			}
		}
		// if the moving box is going up then search which box's position mathes that position
		else if (mystate == up) {
			for (int i=0; i != 12; ++i) {
				if ( ((boxes[0].getPosition().y() + 2.5f) == boxes[i].getPosition().y()) && (boxes[0].getPosition().x() == boxes[i].getPosition().x())){
					return boxes[i];
				}
			}
		}
		// if the moving box is going down then search which box's position mathes that position
		else if (mystate == down) {
			for (int i=0; i != 12; ++i) {
				if ( ((boxes[0].getPosition().y() - 2.5f) == boxes[i].getPosition().y()) && (boxes[0].getPosition().x() == boxes[i].getPosition().x())){
					return boxes[i];
				}
			}
		}
		return boxes[0]; // if none is true then return the moving box
	}

	// check if the moving box is out of borders
	bool checkBorders(float x, float y) {
		if (boxes[0].getPosition().x() + x > court[3].getPosition().x()) {   //right border
			return false;
		}
		else if (boxes[0].getPosition().x() + x < court[2].getPosition().x()) {   //left border
			return false;
		}
		else if (boxes[0].getPosition().y() + y > court[1].getPosition().y()) {   //up border
			return false;
		}
		else if (boxes[0].getPosition().y() + y < court[0].getPosition().y()) {   //bottom border
			return false;
		}
		return true;
	}
	// check if all boxes are right aligned
	// two rows should have 4 boxes aligned and one row 3 boxes aligned
	bool winning () {
		// 3 matches is when there are 3 right boxes in a row. with 4 boxes it is 6 matches
		// 2 right boxes result in 1 match	
		// so in winning state there are 15 matches ans score = matches * 10 so 150
		if ( score >= 150 )
			return true;
		else
			return false;
	}
	// gives the matched boxes in each row
	void alignedLines () {
		vec2 temp = vec2( -3.75f, -2.5f); // position of boxes[0] at left corner
		score = 0;
		for ( int i = 0; i < 3; i ++ ) {
			getMatchAtRow(i) = 0;
			// for each one of 3 rows check all the boxes if their color match
			for ( int j = 0; j < 3; j++) {
				for ( int z = 0; z < 3; z++) {
					if ( z >= j) {
						// check the first left box with all the others in the row
						// then the second left with the boxes at the right so don't calculate again the match with the left one
						// then check the third with the forth
						// in each of these comparisons update the x position of boxes
						// to change row and check the others update the y position once.
						if ( checkColors( getBoxAtPosition(temp + vec2( myWindow.box, 0.0f)*j + vec2( 0.0f, myWindow.box)*i), getBoxAtPosition( temp + vec2( myWindow.box, 0.0f)*(z+1) + vec2( 0.0f, myWindow.box)*i) ) ) { 
							getMatchAtRow(i) += 1;
						}
					}					
				}
			}		
			score += getMatchAtRow(i);		
		}
		score *= 10;
		winning(); // check if this is the winning state
	}
	// take the specified row of the bvec that the "score" is stored
	int &getMatchAtRow ( int t) {
		if ( t == 0)
			return matches.x();
		else if ( t == 1)
			return matches.y();
		else if ( t == 2)
			return matches.z();
	}
	// return the box at the specified position
	box getBoxAtPosition ( vec2 &pos ) {
		for ( int i = 0; i < sizeof(boxes)/sizeof(box); i++) {
			if ( boxes[i].getPosition().x() == pos.x() && boxes[i].getPosition().y() == pos.y() ) 
				return boxes[i];
		}
		// if for some reason nothing is returned, then return the box at position 0 which is the black box but this is WRONG!!!!
		return boxes[0]; // wrong!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
	/*
	// check if this is the black/moveable box
	bool isBlack ( box &a ) {
		if ( a.getColor().x() == 0.0 && a.getColor().y() == 0.0 && a.getColor().z() == 0.0)
			return true;
		else
			return false;
	}
	*/
	// check if 2 boxes have the same color
	bool checkColors ( box &a, box &b) {
		if ( a.getColor().x() == b.getColor().x() && a.getColor().y() == b.getColor().y() && a.getColor().z() == b.getColor().z())
			return true;
		else
			return false;
	}

    // this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
	  //myclock = clock() - myclock;
		//if (((int)clock()- myclock)/CLOCKS_PER_SEC)
	  //printf ("%d clicks and %d seconds\n", clock()- myclock, (int)(((float)clock()- myclock)/CLOCKS_PER_SEC));   //AYTO
	  //printf ("%d clicks and %f seconds\n",  myclock, ((float) myclock)/CLOCKS_PER_SEC);
  
      simulate();
	  
      // set a viewport - includes whole window area
      glViewport(x, y, w, h);

      // clear the background to black
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // allow Z buffer depth testing (closer objects are always drawn in front of far ones)
      glEnable(GL_DEPTH_TEST);

	  // allow alpha blend (transparency when alpha channel is 0)
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
      // draw the court
      for (int i = 0; i != 4; ++i) {
         court[i].render(color_shader_, cameraToWorld);
      }

	  //draw the inside lines
	  for (int i = 0; i != 5; ++i) {
		  lines[i].render(color_shader_, cameraToWorld);
	  }

	  //draw boxes. each one with the right shader
	  for (int i = 0; i!= 12; ++i) {
		  if (boxes[i].getColor().x() == 1.0)
			boxes[i].render(myShader, cameraToWorld);
		  else if (boxes[i].getColor().y() == 1.0)
			boxes[i].render(starShader, cameraToWorld);
		  else if (boxes[i].getColor().z() == 1.0)
			boxes[i].render(circleShader, cameraToWorld);
		  else
			  boxes[i].render(color_shader_, cameraToWorld);
	  }

	  char score_text[32];
      sprintf(score_text, "SCORE : %d\n", score);
      draw_text(texture_shader_, 0, 3, 1.0f/128, score_text);
    }
  };
}
