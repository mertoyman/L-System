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

	class Catmull_Rom_Spline {
		dynarray<float> points;
		dynarray<vec2> control_points;
		dynarray<vec2> start_end_points;

		void (Catmull_Rom_Spline::*foo) ();
	public:
		Catmull_Rom_Spline() { foo = &Catmull_Rom_Spline::moveSpline1; }

		void setControlPoints (dynarray<vec2> &p) {
			//control_points = p;
			constructControlPoints();
		}

		void setStartEndPoints (dynarray<vec2> &p) {
			start_end_points = p;
		}

		void constructControlPoints() {
			for ( unsigned int i = 0; i < start_end_points.size()/2; i++ ) {
				//control_points.push_back(vec2(start_end_points[i*2+1].x(), start_end_points[i*2+1].y()));
				control_points.push_back(vec2( 5.0f, 3.0f));
				control_points.push_back(vec2( -5.0f, 0.0f));
			}
		}

		void initialize (dynarray<vec2> &p) {
			setStartEndPoints(p);
			constructControlPoints();
			
			createObjects();
		}

		void createObjects () {
			// for every 2 points in the array
			for ( unsigned int i = 0; i < control_points.size()/2; i++ ) {
				calculateSplines(i*2);
			}
		}

		void formula (float p, vec2 &p0, vec2 &p1, vec2 &p2, vec2 &p3) {
			float t = p;
			float u3 = t * t * t; 
			float u2 = t * t;

			vec2 point;
			// polynomials
			float b0 = 0.5f*(-u3 + 2*u2 - t);
			float b1 = 0.5f*(3*u3 - 5*u2 + 2);
			float b2 = 0.5f*(-3*u3 + 4*u2 + t);
			float b3 = 0.5f*(u3 - u2);
			// desired point
			point.x() = b0*p0.x() + b1*p1.x() + b2*p2.x() + b3*p3.x();
			point.y() = b0*p0.y() + b1*p1.y() + b2*p2.y() + b3*p3.y();

			points.push_back(point.x());
			points.push_back(point.y());

		}

		void calculatePoints (int num, dynarray<vec2> &vectors) {
			float t = 0.0f;
			for ( int i = 0; i <= num; i ++) {
				formula(t, vectors[0], vectors[1], vectors[2], vectors[3]);
				t += 1.0f/num;
			}
		}

		void calculateSplines (int i) {
			dynarray<vec2> four_points;
			printf("1st control point %f %f\n", control_points[i].x(), control_points[i].y());
			printf("2st control point %f %f\n\n", control_points[i+1].x(), control_points[i+1].y());
			four_points.push_back(control_points[i]); // first control point
			four_points.push_back(start_end_points[i]); // start point
			four_points.push_back(start_end_points[i+1]); // end point 
			four_points.push_back(control_points[i+1]); // second control point

			calculatePoints(5, four_points); // integer is the number of points that will be generated
			//print();
	    }

		void print () {
			for ( unsigned int i = 0; i < points.size(); i ++) {
				printf ("%f \n", points[i]);
			}
		}

		void reset () { points.reset(); }

		void moveSpline1 () {
			reset();
			for ( unsigned int i = 0; i < control_points.size()/2; i++ ) {
				// move control points depending on quadrant that the end point belongs
				if ( start_end_points[i*2+1].x() * start_end_points[i*2+1].y() < 0 ) { // if it belongs to 2nd or 4th quadrant
					control_points[i*2].x() += 1.0f;
					control_points[i*2+1].x() -= 1.0f;
					control_points[i*2].y() += 1.0f;
					control_points[i*2+1].y() -= 1.0f;
				}
				else {
					control_points[i*2].x() += 1.0f;
					control_points[i*2].y() -= 1.0f;
					control_points[i*2+1].x() -= 1.0f;
					control_points[i*2+1].y() += 1.0f;
				}
			}
			createObjects ();
		}

		void moveSpline2 () {
			reset();
			for ( unsigned int i = 0; i < control_points.size()/2; i++ ) {
				// move control points depending on quadrant that the end point belongs
				if ( start_end_points[i*2+1].x() * start_end_points[i*2+1].y() < 0 ) { // if it belongs to 2nd or 4th quadrant
					control_points[i*2].x() -= 1.0f;
					control_points[i*2+1].x() += 1.0f;
					control_points[i*2].y() -= 1.0f;
					control_points[i*2+1].y() += 1.0f;
				}
				else {
					control_points[i*2].x() -= 1.0f;
					control_points[i*2].y() += 1.0f;
					control_points[i*2+1].x() += 1.0f;
					control_points[i*2+1].y() -= 1.0f;
				}
			}
			createObjects ();
		}

		void chooseMovement ( int time ) {
			//void (*foo) () = NULL;
			//foo = &Catmull_Rom_Spline::moveSpline1;
			//foo = &Catmull_Rom_Spline::moveSpline1;
			if ( time % 10 == 0 ) {
				if ( foo == &Catmull_Rom_Spline::moveSpline1)
					foo = &Catmull_Rom_Spline::moveSpline2;
				else
					foo = &Catmull_Rom_Spline::moveSpline1;					
			}
			(this->*foo)();

		}

		

		void render () {
			 glPointSize(5.0f);
			 glVertexAttribPointer(attribute_pos, 2, GL_FLOAT, GL_FALSE, 0, (void*)&points[0] );
		     glEnableVertexAttribArray(attribute_pos);
    
			 //glDrawArrays(GL_LINE_STRIP, 0, points.size()/2);
			 glDrawArrays(GL_POINTS, 0, points.size());
		}
	};


  class Curve_app : public app {
   
    mat4t modelToWorld;
    mat4t cameraToWorld;
    color_shader color_shader_;
	float angle;

	Catmull_Rom_Spline spline;
	dynarray<vec2> points;
	dynarray<vec2> ctrlpoints;

	clock_t myclock;


  public:

    Curve_app(int argc, char **argv) : app(argc, argv) {
    }

    void app_init() {
      color_shader_.init();
      modelToWorld.loadIdentity();
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, 5);

	   myclock = clock();

	  angle = 0.0f;

	  points.push_back(vec2(0.0f,0.0f));
	  points.push_back(vec2(5.0f,3.0f));
	  points.push_back(vec2(0.0f,0.0f));
	  points.push_back(vec2(4.0f,-5.0f));
	  points.push_back(vec2(0.0f,0.0f));
	  points.push_back(vec2(-4.0f,-5.0f));
	  points.push_back(vec2(0.0f,0.0f));
	  points.push_back(vec2(-4.0f,5.0f));


	  spline.initialize(points);

	
    }

	
	void oneSecond() {
		static int temp=0;
		if ( (int)(((float)clock()- myclock)/CLOCKS_PER_SEC) == temp+1) {
			temp = (int)(((float)clock()- myclock)/CLOCKS_PER_SEC);
			spline.chooseMovement(temp);
		}
	}

    void draw_world(int x, int y, int w, int h) {
	  
      glViewport(x, y, w, h);
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
      vec4 color(0, 0, 1, 1);
      color_shader_.render(modelToProjection, color);

	  spline.render();

	  oneSecond();

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
	  else if (is_key_down('Z')) {
		  spline.moveSpline1();	
		  //spline.chooseMovement();
		  Sleep (200);
	  }
	  else if (is_key_down('X')) {
		  spline.moveSpline2();		 
		  Sleep (200);
	  }
	  
	 
    }
  };
}
