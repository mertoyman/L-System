////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// texture example: How to draw triangles with textures
//
// Level: 0
//
// Demonstrates:
//   Basic framework app
//   Shaders
//   Basic Matrices
//   Textures
//

namespace octet {
	class Quartenion {

		vec4 vector;
		// here are stored points after rotaion with quartenions so the destination that should be reached via slerp
		dynarray<float> rotateDestinations;
	public:
		Quartenion () {}

		// v1 are the initial points, v2 is the destination rotation
		bool slerpRotateSmooth(dynarray<float> &v1) {

			static float t = 0.0f;
			vec3 v = vec3(v1[0], v1[1], v1[2]);

				printf ("t = %f ",t);
				for ( unsigned int j = 0; j < v1.size()/3; j++) { // for every 3 vertices in the array so every point
					// 1st argument is the start orientation point, 2nd argument is the end orientation point
					v = slerp( vec4(v1[j*3], v1[j*3+1], v1[j*3+2], 0.0f), vec4(rotateDestinations[j*3], rotateDestinations[j*3+1], rotateDestinations[j*3+2], 0.0f), t);
					// copy the output of slerp to the start orientation point as now we should interpolate between this and
					// the end orientation point which remains untouch. like that at t=1 we reach the end orientation point
					v1[j*3] = v.x();
					v1[j*3+1] = v.y();
					v1[j*3+2] = v.z();			
				}		
				t += 1.0f / 10.0f;  // as big is the denominator, the delay is becoming bigger because of lerp!

				if ( t > 1.0f) {
					t = 0.0f;
					
					return false; // rotation stops
				}
				return true;
		}


		// v1 are the initial points, v2 is the destination rotation
		bool slerpRotateSmooth(dynarray<vec3> &v1) {

			static float t = 0.0f;
			vec3 v = vec3(v[0]);

				printf ("t = %f ",t);
				for ( unsigned int j = 0; j < v1.size(); j++) { // for every point
					// 1st argument is the start orientation point, 2nd argument is the end orientation point
          // to 1 itan 0
					v = slerp( vec4( v1[j], 1.0f), vec4(rotateDestinations[j*4], rotateDestinations[j*4+1], rotateDestinations[j*4+2], rotateDestinations[j*4+3]), t);
					// copy the output of slerp to the start orientation point as now we should interpolate between this and
					// the end orientation point which remains untouch. like that at t=1 we reach the end orientation point
					v1[j] = v;		
				}		
				t += 1.0f / 8.0f;  // as big is the denominator, the delay is becoming bigger because of lerp!
				if ( t > 1.0f) {
					t = 0.0f;
					return false; // rotation stops
				}
				return true;
		}

		vec3 slerp ( vec4 &q1, vec4 &q2, float t) {
			float d, angle;
			vec4 q3 = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			vec4 v;

			float l1 = length(q1);
			float l2 = length(q2);

			// I think q1, q2 has to be normalized
			//normalize(q1);
			//normalize(q2);

			d = dot(q1, q2)/(l1*l2);
      printf("Dot product %f \n", d);
			//q1 *= l1;
			//q2 *= l2;
			
			if ( d < 0.0f ) {
				d = -d;
				q3 = -q2;
			}
			else
				q3 = q2;

			// angle = 0 cause problem!!!!
			if ( d < 0.9999f ) {
				angle = acosf(d);
				v = (q1*sinf(angle*(1-t)) + q3*sinf(angle*t))/sinf(angle);
			}
			else
				//use lerp to avoid division with zero ( if angle is close to 0.0f then we divide with aero in slerp)
				v = lerp ( q1, q3, t );
			printf("slerp result is %f %f %f %f\n", v.x(), v.y(), v.z(), v.w());
			
			return vec3(v.x(), v.y(), v.z());
		}

		vec4 &lerp ( vec4 &q1, vec4 &q2, float t ) {
			//(q1*(1-t) + q2*t).normalized();
			vec4 v = (q1*(1-t) + q2*t);
			//normalize(v);  // do I really need this??
			printf ("LERP ");
			return vec4 ( v.x(), v.y(), v.z(), v.w() );
		}

		// dot product between 2 vectors, don't calculate w value in the dot product
		float dot ( const vec4 &v1, const vec4 &v2 ) {
			return vec3(v1.x(), v1.y(), v1.z()).dot(vec3(v2.x(), v2.y(), v2.z()));
		}

		// first argument is the quartanion/axis of rotation
		// second argument is the vectors that we want to rotate
		void rotateWithQuartenionAndSlerp ( vec4 &quart, dynarray<float> &v ) {
			rotateDestinations.reset(); 
			for (int i = 0; i < v.size()/3.0f; i++) {
				// rotate points with quartenions
				rotate( quart, vec3(v[i*3], v[i*3+1], v[i*3+2])); // after this we have the destination rotate points
			}
			// interpolate values between the starting rotation point and the destination rotated point
			slerpRotateSmooth(v);
		}

		// first argument is the quartanion/axis of rotation
		// second argument is the vectors that we want to rotate
		void rotateWithQuartenionAndSlerp ( vec4 &quart, dynarray<vec3> &v ) {
			rotateDestinations.reset(); 
			for ( unsigned int i = 0; i < v.size(); i++) { // for all points in the v array
				// rotate points with quartenions
				rotate( quart, v[i] ); // after this we have the destination rotate points
			}
			// interpolate values between the starting rotation point and the destination rotated point
			slerpRotateSmooth(v);
		}

		// rotate point v around quartnion/axis quart
		// if I always call it like rotate(vec4(0.0f,0.0f,1.0f, 90.0f), vec3 v); then there is no problem
		// BUT if I call it rotate(vec4 &quart, vec3 v); then quart is being transformed into the function
		// and the next time it will be called it will have the transformed value which is wrong
		void rotate ( const vec4 &quart, const vec3 &v ) {
			
			vec4 quartenion = vec4 ( quart );//vec4(quart.x(), quart.y(), quart.z(), quart.w());

			normalize( quartenion );
			transfrom_Quartenion ( quartenion );

			quat q = quartenion;
			vec4 p = vec4(v, 1.0f);

			vector = q.rotate(p); // call function of quartenions that implements this: q*p*(q*)
			printf ("New point %f %f %f\n", vector.x(), vector.y(), vector.z());

			// save destination of rotation points for later use in slerp
			rotateDestinations.push_back(vector.x());
			rotateDestinations.push_back(vector.y());
			rotateDestinations.push_back(vector.z());
      rotateDestinations.push_back(vector.w()); // ayto den ypirxe edw!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			//return vec3(vector.x(), vector.y(), vector.z());
		}

		// length of vector part, dont take into account w value
		float length ( const vec4 &t ) {
			return sqrt( t.x()*t.x() + t.y()*t.y() + t.z()*t.z() );
		}

		// normalize vector without taking into account the w()
		void normalize (vec4 &t) {
			float len = sqrt( t.x()*t.x() + t.y()*t.y() + t.z()*t.z() );
			// if it is not already normalized then normalize it
			if ( len != 1.0f && len!=0.0f ) {
				t.x() = t.x() / len;
				t.y() = t.y() / len;
				t.z() = t.z() / len;
			}
		}

		// quartenion is equal to ( cos(θ/2), v*sin(θ/2) ) where v is the vector part
		void transfrom_Quartenion( vec4 &t) {
			t.x() = t.x() * sin(t.w()*3.14159f/180.0f/2.0f); // degrees
			t.y() = t.y() * sin(t.w()*3.14159f/180.0f/2.0f);
			t.z() = t.z() * sin(t.w()*3.14159f/180.0f/2.0f);
			t.w() = cos(t.w()*3.14159f/180.0f/2.0f);
		}
	};


  class texture_app : public octet::app {
    mat4t modelToWorld;              // this matrix converts from model space to world space
    mat4t cameraToWorld;             // this matrix converts from camera space to world space
    texture_shader texture_shader_; // this builds a shader to use with textures
    GLuint texture_handle_;         // this is an OpenGL texture handle which is used to draw the image.
	Quartenion temp;
	vec3 v1, v2, v3, v4, v5, v6;
	dynarray<float> vertices, endrotationvertices;
	clock_t myclock;
	int tt ;
	void (texture_app::*ptr)(dynarray<float>&,dynarray<float>&);
	bool rotation;
	dynarray<vec3> vertices2;

  public:

    // this is called when we construct the class
    texture_app(int argc, char **argv) : app(argc, argv) {
    }

    // this is called once OpenGL is initialized
    void app_init() {
      // set up the shader
      texture_shader_.init();
	   myclock = clock();
      // set up the matrices with a camera 5 units from the origin
      modelToWorld.loadIdentity();
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, 15);
	  tt = 0;
	  ptr = NULL;
	  rotation = false;
      // use helper function to generate an OpenGL texture
      texture_handle_ = resources::get_texture_handle(GL_RGB, "assets/tree.gif");  // "!bricks"

	  //temp.slerp(vec4(1.0f, 1.0f, 0.0f, 0.0f), vec4(0.0f, 1.0f, 1.0f, 1.0f), 1.0f);

	  v1 = vec3(-1.0f,  0.0f, 0.0f);
	  v2 = vec3( 1.0f,  0.0f, 0.0f);
	  v3 = vec3( 0.0f,  1.0f, 0.0f);

	 // push( v1, v2, v3);

	  vertices2.push_back(v1);
	  vertices2.push_back(v2);
	  vertices2.push_back(v3);

    //temp.rotateWithQuartenionAndSlerp(vec4(0.0f,0.0f,1.0f, 90.0f), vertices2);
	  //rotation = true;
    }


	void oneSecond() {
		static int temp=0;
		if ( (int)(((float)clock()- myclock)/CLOCKS_PER_SEC) == temp+1) {
			printf ("Clock ticks one sec\n");
			temp = (int)(((float)clock()- myclock)/CLOCKS_PER_SEC);
			
		    //slerpRotate(vertices, endrotationvertices);
		}
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

    if ( rotation == false ) {
      temp.rotateWithQuartenionAndSlerp(vec4(0.0f,0.0f,1.0f, 90.0f), vertices2);
      rotation = true;
    }

	  if ( rotation == true )
		  rotation = temp.slerpRotateSmooth(vertices2);
	  

	  if (is_key_down('Z')) {
		  //rotate();
		  temp.rotateWithQuartenionAndSlerp(vec4(0.0f,0.0f,1.0f, 5.0f), vertices2);
		  rotation = true;
		  Sleep(200);
	  }
	  else if (is_key_down('X')) {
		  //rotateS();
		  //slerpRotate(vertices, endrotationvertices);
		  tt = 1;
		  //ptr = &texture_app::slerpRotate;
		  Sleep(200);
	  }

	  //(this->*ptr)( vertices, endrotationvertices );  // (*this.*ptr) auto einai arxikopoiimeno sto NULL gi ayto petaei error

      // set up opengl to draw textured triangles using sampler 0 (GL_TEXTURE0)
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texture_handle_);
      texture_shader_.render(modelToProjection, 0);  // 0

	/*
      // this is an array of the positions of the corners of the texture in 3D
      static const float vertices[] = {
        v1.x(), v1.y(), v1.z(), 
        v2.x(), v2.y(), v2.z(),
        v3.x(), v3.y(), v3.z(),
      };
	  */
      /*
	  vertices.reset();
	  for ( unsigned int i =0; i< vertices2.size(); i++){
		  //for ( int j =0; j< 3; j++){
		  vertices.push_back( vertices2 [i].x() );
		  vertices.push_back( vertices2 [i].y() );
		  vertices.push_back( vertices2 [i].z() );
	  }
	  
    */

      // attribute_pos (=0) is position of each corner
      // each corner has 3 floats (x, y, z)
      // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)&vertices2[0] );
      glEnableVertexAttribArray(attribute_pos);
    
      // this is an array of the positions of the corners of the texture in 2D
	  /*
      static const float uvs[] = {
        -4, -4,
         4, -4,
         4,  4,
      };
	  */
	  // half of image
	  
	  static const float uvs[] = {
         0,  0,
         0,  0.5,
         0.5,  0.5,
		
      };
	  
	  /*
	  static const float uvs[] = {
         0.5,  0,
         1, 0,
         1,  1,
      };
	  */
      // attribute_uv is position in the texture of each corner
      // each corner (vertex) has 2 floats (x, y)
      // there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
      glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs );
      glEnableVertexAttribArray(attribute_uv);
    
      // finally, draw the texture (3 vertices)
      glDrawArrays(GL_TRIANGLES, 0, 3);
    }
  };
}
