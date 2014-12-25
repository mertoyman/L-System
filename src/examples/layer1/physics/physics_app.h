////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Physics example: How to use the basics of Bullet physics
//
// Level: 2
//
// Demonstrates:
//   Basic framework app
//   Shaders
//   Basic Matrices
//   Stock meshes
//   Phong shader
//

namespace octet {

	class camera {
		mat4t cameraInstance;
		//float angle;
		int preMouseX, preMouseY;
		int mouseX, mouseY;
		enum { MAX_MOVE = 2, MAX_ROTATION = 5 };
    
    static mat4t createYRotationMatrix(float angle) {
      mat4t rightRotate(1.f);
      rightRotate.rotateY(angle);
      return rightRotate;
    };

    static const mat4t& getRightRotationMatrix() {
      static mat4t rotate = createYRotationMatrix(-MAX_ROTATION);
      return rotate;
    };

    static const mat4t& getLeftRotationMatrix() {
      static mat4t rotate = createYRotationMatrix(MAX_ROTATION);
      return rotate;
    };

		void savePreviousMousePos () { preMouseX = mouseX; preMouseY = mouseY; }

	public:
		//camera () { angle = 0; }
		const mat4t& getCamera () const { return cameraInstance; }
		// if I don't return reerence then I can't see anything because it is not modified that way
	  mat4t& getCamera () { return cameraInstance; }
		//float& getCameraAngle () { return angle; }
		//void setCameraAngle ( float _angle ) { angle = _angle; }
		void translateLeft () { cameraInstance.translate(-1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateRight () { cameraInstance.translate( 1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateForward () { cameraInstance.translate( 0.0f, 0.0f, -1.0f/MAX_MOVE); }
		void translateBack () { cameraInstance.translate( 0.0f, 0.0f, 1.0f/MAX_MOVE); }
		void rotateLeft () { 
			//angle += MAX_ROTATION; 
			//cameraInstance.rotateY( MAX_ROTATION );
      cameraInstance = cameraInstance * getLeftRotationMatrix();
		}
		void rotateRight () { 
			//angle -= MAX_ROTATION; 
			//cameraInstance.rotateY( -MAX_ROTATION );
      cameraInstance = cameraInstance * getRightRotationMatrix();
		}
		void rotateUp () { cameraInstance.rotateX( MAX_ROTATION ); }
		void rotateDown () { cameraInstance.rotateX( -MAX_ROTATION ); }
		int& getMouseX () { return  mouseX; }
		int& getMouseY () { return  mouseY; }
		void detectMouse () {
			if ( preMouseX < mouseX )
				rotateRight();
			else if ( preMouseX > mouseX )
				rotateLeft();
			if ( preMouseY < mouseY )
				rotateDown();
			else if ( preMouseY > mouseY )
				rotateUp();
			savePreviousMousePos();
		}
		void reset () {
			cameraInstance.loadIdentity();
			cameraInstance.translate(0.0f, 0.0f, 5.0f);
		}
	};

  class physics_box {
    material *mat;
    mesh *box_mesh;
    int physics_index;
  public:
    physics_box() {
    }

    void init(material *_material, mesh *_box_mesh, int _physics_index) {
      mat = _material;
      box_mesh = _box_mesh;
      physics_index = _physics_index;
    }

    void render(physics_world &world, bump_shader &shader, const mat4t &worldToCamera, const mat4t &cameraToProjection, vec4 *light_uniforms, int num_light_uniforms, int num_lights) {
      mat4t modelToWorld;
      world.get_modelToWorld(modelToWorld, physics_index);
      mat4t modelToCamera = modelToWorld * worldToCamera;
      mat4t modelToProjection = modelToCamera * cameraToProjection;
      mat->render(shader, modelToProjection, modelToCamera, light_uniforms, num_light_uniforms, num_lights);
      box_mesh->render();
    }

    void render() {
     
      box_mesh->render();
    }

    void accelerate(physics_world &world, float amount) {
      mat4t modelToWorld;
      world.get_modelToWorld(modelToWorld, physics_index);
      world.apply_impulse(physics_index, modelToWorld[2] * amount);
    }

	void jump (physics_world &world, float amount) {
		mat4t modelToWorld;
        world.get_modelToWorld(modelToWorld, physics_index);
        world.apply_impulse(physics_index, vec4(0.0, amount, 0.0, 0.0));
	}

    void turn(physics_world &world, float amount) {
      mat4t modelToWorld;
      world.get_modelToWorld(modelToWorld, physics_index);
      world.apply_torque_impulse(physics_index, modelToWorld[1] * amount);
	  //world.apply_impulse(physics_index, modelToWorld[1] * amount);
    }

    void get_modelToWorld(physics_world &world, mat4t &modelToWorld) {
      world.get_modelToWorld(modelToWorld, physics_index);
    }
  };

  class physics_app : public octet::app {
    // physics world contains matrices and motion state for all objects
    physics_world world;

    // Matrix to transform points in our camera space to the world.
    // This lets us move our camera
    //mat4t cameraToWorld;
	camera cameraToWorld;
  mat4t modelToWorld;

    // shader to draw a bump mapped shaded, textured triangle
    bump_shader bump_shader_;
    color_shader color_shader_;
  
    // material for boxes
    material box_mat;
  
    // mesh for boxes
    mesh box_mesh;
  
    // material for boxes
    material floor_mat;
  
    // mesh for boxes
    mesh floor_mesh;

    // storage for boxes
    dynarray<physics_box> boxes;

	vec3 campos;

	int x, y;

  public:

    // this is called when we construct the class
    physics_app(int argc, char **argv) : app(argc, argv) {
    }

    // this is called once OpenGL is initialized
    void app_init() {
      // set up the shader
      bump_shader_.init();
      color_shader_.init();

      modelToWorld.loadIdentity();
    
      // make a mesh to share amoungst all boxes
      //box_mesh.make_cube(0.5f);
	  box_mesh.make_sphere(1.0f);
      box_mat.make_color(vec4(1, 0, 0, 1), false, false);

      float floor_size = 50.0f;
      floor_mesh.make_aa_box(floor_size, 0.5f, floor_size);
      floor_mat.make_color(vec4(0.3f, 1, 0.1f, 1), true, false);

      // make some boxes in the physics world with meshes for drawing.
      int num_boxes = 10;
      float box_spacing = 1.5f;
      boxes.resize(num_boxes+1);

      mat4t modelToWorld;
      modelToWorld.loadIdentity();
      modelToWorld.translate(-0.5f * box_spacing * (num_boxes-1), 4.0f, 0);

      for (int i = 0; i != num_boxes; ++i) {
        int body = world.add_rigid_body(modelToWorld, vec4(0.5f, 0.5f, 0.5f, 0), true, physics_world::body_box);
        boxes[i].init(&box_mat, &box_mesh, body);
        modelToWorld.translate(box_spacing, 0, 0);
      }

      {
        modelToWorld.loadIdentity();
        modelToWorld.translate(0, -2.0f, 0);
        int body = world.add_rigid_body(modelToWorld, vec4(floor_size, 0.5f, floor_size, 0), false, physics_world::body_box);
        boxes[num_boxes].init(&floor_mat, &floor_mesh, body);
      }

      //lights.add_light(vec4(10, 10, 10, 1), vec4(0, 0, 1, 0).normalize(), vec4(0.3f, 0.3f, 0.3f, 1), vec4(1, 1, 1, 1), vec4(1, 1, 1, 1));
     // boxes[0].get_modelToWorld(world, cameraToWorld);
	  boxes[0].get_modelToWorld(world, cameraToWorld.getCamera());
	  //cameraToWorld.translate(0, -2.0f, 5.0f);
	  cameraToWorld.getCamera().translate(0, -2.0f, 5.0f);
	  campos = vec3(0.0f, 0.0f, 0.0f);

	  x = y = 0;
	}

	void moveCamera () {
		// translate camera left
		if (is_key_down('A')) {
			cameraToWorld.translateLeft();
		}
		// translate camera right
		else if (is_key_down('D')) {
			cameraToWorld.translateRight();
		}
		// translate camera forward
		else if (is_key_down('W')) {
			cameraToWorld.translateForward();
		}
		// translate camra backwards
		else if (is_key_down('S')) {
			cameraToWorld.translateBack();
		}
		else if (is_key_down(key_up)) {
			cameraToWorld.rotateUp();
		}
		else if (is_key_down(key_down)) {
			cameraToWorld.rotateDown();
		}
		else if (is_key_down(key_left)) {
			cameraToWorld.rotateLeft();
		}
		else if (is_key_down(key_right)) {
			cameraToWorld.rotateRight();
		}
		else if (is_key_down(key_space)) {
			cameraToWorld.reset();
		}

	}
		
	

    // this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      // set a viewport - includes whole window area
      glViewport(x, y, w, h);

      // clear the background to gray
      glClearColor(0.5f, 0.5f, 0.5f, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // allow Z buffer depth testing (closer objects are always drawn in front of far ones)
      glEnable(GL_DEPTH_TEST);

      // improve draw speed by culling back faces - and avoid flickering edges
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CW);

      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld.getCamera());
       color_shader_.render(modelToProjection, vec4(1,0,0,1));
      // set up the matrices with a camera 5 units from the origin
      //boxes[0].get_modelToWorld(world, cameraToWorld); // that place the camera wherever the box is
      //cameraToWorld.translate(0, 2, 5); // move camera a little further so it can see the object
	  //cameraToWorld.translate(campos.x(), campos.y(), campos.z()); 
	  //cameraToWorld.rotateY(10.0f);

      // flip cameraToWorld around to transform from world to camera
      mat4t worldToCamera;
      //cameraToWorld.invertQuick(worldToCamera);
	  cameraToWorld.getCamera().invertQuick(worldToCamera);

      // build a projection matrix to add perspective
      mat4t cameraToProjection;
      cameraToProjection.loadIdentity();
      float n = 0.125f, f = 256.0f;
      cameraToProjection.frustum(-n, n, -n, n, n, f);

      // the lights are in camera space
      //lights.compute(worldToCamera);
      vec4 lights[5];
      memset(lights, 0, sizeof(lights));
      lights[0] = vec4(0.3f, 0.3f, 0.3f, 50);
	  lights[2] = vec4(0.707f, 0, 0.707f, 0) * worldToCamera;
      lights[3] = vec4(1, 1, 1, 1);
      lights[4] = vec4(1, 0, 0, 1);

      for (unsigned i = 0; i != boxes.size(); ++i) {
        //boxes[i].render(world, bump_shader_, worldToCamera, cameraToProjection, lights, 5, 1);
        boxes[i].render();
      }

      if (is_key_down('W')) {
        boxes[0].accelerate(world, -0.5f);
      }
      if (is_key_down('S')) {
        boxes[0].accelerate(world, 0.5f);
      }
      if (is_key_down('Q')) {
        boxes[0].turn(world, 0.01f);
      }
      if (is_key_down('A')) {
        boxes[0].turn(world, -0.01f);
      }
	  if (is_key_down(key_space)) {
		  boxes[0].jump(world, 0.5f);
	  }

	  moveCamera ();
      world.step(0.016f);

	  get_mouse_pos(cameraToWorld.getMouseX(), cameraToWorld.getMouseY());
	  cameraToWorld.detectMouse();
    }
  };
}

