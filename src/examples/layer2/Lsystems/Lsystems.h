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


namespace octet {

	class camera {
		mat4t cameraInstance;
		float angle, angleX;
		int preMouseX, preMouseY;
		int mouseX, mouseY;
		enum { MAX_MOVE = 2, MAX_ROTATION = 5 };

	public:
		camera () { angle = 0.0f; angleX = 0.0f; cameraInstance.loadIdentity(); cameraInstance.translate(0.0f, 0.0f, 5.0f); }
		// if I don't return reerence then I can't see anything because it is not modified that way
		mat4t& getCamera () { return cameraInstance; }
		float& getCameraAngle () { return angle; }
		void setCameraAngle ( float _angle ) { angle = _angle; }
		void translateLeft () { cameraInstance.translate(-1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateRight () { cameraInstance.translate( 1.0f/MAX_MOVE, 0.0f, 0.0f); }
		void translateForward () { cameraInstance.translate( 0.0f, 0.0f, -1.0f/MAX_MOVE); }
		void translateBack () { cameraInstance.translate( 0.0f, 0.0f, 1.0f/MAX_MOVE); }
		void translateUp ( float t ) { cameraInstance.translate( 0.0f, t, 0.0f); }
		void rotateLeft () { 
			cameraInstance.rotateX( -angleX );
			cameraInstance.rotateY( MAX_ROTATION );
			cameraInstance.rotateX( angleX );
		}
		void rotateRight () { 
			cameraInstance.rotateX( -angleX );
			cameraInstance.rotateY( -MAX_ROTATION );
			cameraInstance.rotateX( angleX );
		}
		void rotateUp () { 
			angleX += MAX_ROTATION;
			cameraInstance.rotateX( MAX_ROTATION ); 
		}
		void rotateDown () { 
			angleX -= MAX_ROTATION;
			cameraInstance.rotateX( -MAX_ROTATION );
		}
		int& getMouseX () { return  mouseX; }
		int& getMouseY () { return  mouseY; }
		void savePreviousMousePos () { preMouseX = mouseX; preMouseY = mouseY; }
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

	// READ FILE
  class L_Systems  {
    std::FILE *file;
    string axiom;
	string constants;
	int iterations;
	float angle;
	dynarray<char> var;
	dynarray<string> rule;
	dynarray<int> positions;
	dynarray<float> probabilities;

	enum { MAX_LINE = 30 };

  public:
    L_Systems () {	}

	//void init () { }

	void openFiles ( const char *filename) {

		file = fopen(filename, "r"); 
	    if ( file != NULL ) {
		  printf("file opened\n");
		  readData(file);
		  fclose(file);
	    }
	    else
		  printf("file error\n");
	}
	
	void readData ( FILE *in ) {
		// ayto to pragma einai stin stack!!!!!
		// static pio apotelesmatiko dioti den 8a dhmiourgei olo tin wra thn metabliti
		// kai den exw problima uperfortosis dioti panta ua grafei sti 8esi 0
		// dioti ekei deixnei o deikths
		static char stackBuf[MAX_LINE];
		char *buf = &(stackBuf[0]); //without new it is not initialized and throws error
		while ( fgets( buf, MAX_LINE, in) != NULL ) {
			if ( strncmp(buf, "axiom", 5) == 0) {
				axiom = fgets( buf, MAX_LINE, in); //(buf+5);
			}
			else if ( strncmp(buf, "constants", 9) == 0) {
				constants = fgets( buf, MAX_LINE, in);//buf;//(buf+strlen("constants "));
			}
			else if ( strncmp(buf, "variables", 9) == 0) {
				fgets( buf, MAX_LINE, in);
				var.push_back(buf[0]);   // this thing has lots of trush inside!!!!!!!!!!!
			}
			else if ( strncmp(buf, "rules", 5) == 0) {
				rule.push_back(fgets( buf, MAX_LINE, in));
			}
			else if ( strncmp(buf, "iterations", 10) == 0) {
				iterations = atoi(fgets( buf, MAX_LINE, in));
			}
			else if ( strncmp(buf, "angle", 5) == 0) {
				angle = (float)atof(fgets( buf, MAX_LINE, in));
			}
			else if (strncmp(buf, "positions", 8) == 0){
				positions.push_back( atoi(fgets( buf, MAX_LINE, in)) );
				positions.push_back( atoi(fgets( buf, MAX_LINE, in)) );
			}
			else if ( strncmp(buf, "probability", 5) == 0) {
				probabilities.push_back((float)atof(fgets( buf, MAX_LINE, in)));
			}
		}
	}


	void reset () {		
		var.reset();
		rule.reset();
		positions.reset();
		probabilities.reset();
	}

	string selectRule ( char c, int random ) {
		float temp = 0.0f;

		for ( unsigned int i = 0; i < var.size(); i++ ){
			if (c == var[i]){
				// check the probability value in that position ( i )
				if ( i < probabilities.size() ) {
					temp += probabilities[i]*100.0f;
					if ( random <= temp)
						return rule[i];
				}
			}
				
		}
		// if no rule was selected by probabilities then return the first one
		for ( unsigned int i = 0; i < var.size(); i++ ) {
			if (c == var[i])
				return rule[i];
		}
		printf("Maybe there is an error, no rule is selected\n");
		return "";
	}

	bool isStohastic () {
		char temp = var[0];
		for ( unsigned int i = 0; i < var.size()-1; i++) {
			for( unsigned int j = i; j < var.size()-1; j++) {
				if ( var[i] == var[j+1] )
					return true;
			}
		}
		return false;
	}

	int getPosition ( int i ) { return positions[i]; }
	string getAxiom () { return axiom; }
	float getAngle () { return angle; }
	string getConstants () { return constants; }
	char getVariables ( int temp ) { return var[temp]; }
	string getRules ( int temp ) { return rule[temp]; }
	int sizeOfVariables () { return var.size(); }
	int getIterations () { return iterations; }
	int& setIterations ( ) { return iterations; }
	float& setAngle () { return angle; }
	bool existInVariables ( char temp ) {
		for ( int j = 0; j < sizeOfVariables(); j++){
			if ( temp == getVariables(j))
				return 1;
		}
		return 0;
	}
  };

	// mesh holds indices, vertices, uv coordinates
	class Mesh {
		dynarray<float> vertices;
		dynarray<GLuint> indices;
		dynarray<float> uvcoord;
		int index_number;
		GLuint vertexbuffer;
		GLuint indexbuffer;
		GLuint uvbuffer;
	public :
    Mesh ():index_number(0) { }

		void init () {
			index_number = 0;
		}

		void reset () {
			index_number = 0;
			vertices.reset();
			indices.reset();
			uvcoord.reset();
		}

		void pushVertex ( vec3 &vector) {
			vertices.push_back(vector.x());
			vertices.push_back(vector.y());
			vertices.push_back(vector.z());
		}

		void pushIndex ( ) {
			indices.push_back( index_number );
			index_number++;
		}

		void pushIndex ( int n ) {
			indices.push_back( n );
		}
		
		void pushUV ( float u_, float v_ ) { 
			uvcoord.push_back( u_ ); 
			uvcoord.push_back( v_ );
		}

		dynarray<float> &getVertices () { return vertices; }

		dynarray<GLuint> &getIndices () { return indices; }

		//void increaceIndexNumber () { index_number++; }

		int &getIndexNumber () { return index_number; }

		void setBuffers () {
			glEnableVertexAttribArray(attribute_pos);
		    glGenBuffers(1, &vertexbuffer);			
		    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);  // 3 korifes * sizeof(float)
			glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

			// http://stackoverflow.com/questions/15041120/crash-at-draw-call-in-nvoglv32-dll-on-new-video-card
			if(!uvcoord.is_empty()){
			glEnableVertexAttribArray(attribute_uv);
			glGenBuffers(1, &uvbuffer);	
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
			glBufferData(GL_ARRAY_BUFFER, uvcoord.size()*sizeof(float), &uvcoord[0], GL_STATIC_DRAW);  // 3 korifes * sizeof(float)
			glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
			}

		    //glEnableVertexAttribArray(attribute_pos);
			//glVertexPointer(3, GL_FLOAT, 0, 0); //sizeof(GLfloat)*3
			glGenBuffers(1, &indexbuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
		}

		void render() {
			/*
			// one attribute buffer, bind vertices to buffer
			//glEnableClientState(GL_VERTEX_ARRAY);
			glEnableVertexAttribArray(attribute_pos);
		    glGenBuffers(1, &vertexbuffer);			
		    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);  // 3 korifes * sizeof(float)

		    glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
		    //glEnableVertexAttribArray(attribute_pos);
			//glVertexPointer(3, GL_FLOAT, 0, 0); //sizeof(GLfloat)*3
			glGenBuffers(1, &indexbuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
			*/

			//setBuffers ();
			
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
			//glDrawArrays(GL_QUADS, 0, vertices.size()); 
			//glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, (void*)0);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
			//glDisableClientState(GL_VERTEX_ARRAY);
		}
		/*
		void renderQuads () {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			setBuffers ();			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
			//glDrawArrays(GL_QUADS, 0, vertices.size()); 
			glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, (void*)0);
		}
		*/
	};

	class OctetMesh {
		mat4t modelToWorld;
		unsigned int index_number;
		mesh_builder mb;
		mesh m;
		float halfWidth;
	    float halfHeight;
	public:
		OctetMesh ( ) { }
		 void init (float w, float h) {
			 modelToWorld.loadIdentity();
			 index_number = 0;
			 halfWidth = w * 0.5f;
			 halfHeight = h * 0.5f;
			 mb.init ();
			 /*
			 mb.add_vertex(vec4(0.0f,0.0f,0.0f,1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0.0f);
		  mb.add_index(0);
		  mb.add_vertex(vec4(1.0f,0.0f,0.0f,1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0.0f);
		  mb.add_index(1);
		  mb.add_vertex(vec4(1.0f,1.0f,0.0f,1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0.0f);
		  mb.add_index(2);
		  mb.add_index(2);
		  mb.add_vertex(vec4(0.0f,1.0f,0.0f,1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0.0f);
		  mb.add_index(3);
		  mb.add_index(0);
		  */
		 }

		 void bindMesh () { mb.get_mesh(m); }

		 void addQuad () {
			 vec4 v1 =  vec4 ( -halfWidth, 0.0f, 0.0f, 1.0f) * modelToWorld;
			 vec4 v2 = vec4 (  halfWidth, 0.0f, 0.0f, 1.0f) * modelToWorld;
			 vec4 v3 =  vec4 (  halfWidth, 2*halfHeight, 0.0f, 1.0f ) * modelToWorld;
			 vec4 v4 =  vec4 ( -halfWidth, 2*halfHeight, 0.0f, 1.0f) * modelToWorld;

			 mb.add_vertex(v1, vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0.0f );
			 mb.add_vertex(v2, vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0.0f );
			 mb.add_vertex(v3, vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0.0f );
			 mb.add_vertex(v4, vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f, 0.0f );

			 addIndicesQuad ();
		 }

		 void addIndicesQuad () {
			mb.add_index(index_number);
			index_number++;
			mb.add_index(index_number);
			index_number++;
			mb.add_index(index_number);
			mb.add_index(index_number);
			index_number++;
			mb.add_index(index_number);
			mb.add_index(index_number - 3);
			index_number++;
		}

		 void reset () {
			 modelToWorld.loadIdentity();
			 index_number = 0;

			 m.get_indices()->reset();
			m.get_vertices()->reset();
			m.set_indices(0);
			m.set_vertices(0);
			m.set_num_indices(0);
			m.set_num_vertices(0);
			m = NULL;
			mb.init();
			//m.~mesh();
		 }

		 void render ( color_shader &shader, mat4t &cameraToWorld ) {
			 mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
		     shader.render(modelToProjection, vec4(0.0f, 1.0f, 0.0f, 1.0f));
			 m.render();
		 }
	};

	class HandleMesh {
		mat4t modelToWorld;
		float halfWidth;
	    float halfHeight;

		vec4 color;

		//GLuint vertexbuffer;
		//GLuint uvbuffer;
		//GLuint indexbuffer;
		
		//GLubyte index_number;

		Mesh mesh_;
		dynarray<mat4t> modelToWorldMatrices;

    	GLuint texture_handle_;         // this is an OpenGL texture handle which is used to draw the image.

		color_shader color_shader_;
		texture_shader texture_shader_;

	public:
		HandleMesh () { }

		void init() {
		  //index_number = 0;
			// There is a problem here with the initialization. Has to be done once!
		  mesh_.init();
		  //color_shader_.init();
		  //texture_shader_.init();
		  //texture_handle_ = resources::get_texture_handle(GL_RGB, "assets/tree.gif");  
		}

		float getHeight() { return halfHeight*2; }
		void rotateY ( float angle ) { modelToWorld.rotateY(angle); }
		void rotateX ( float angle ) { modelToWorld.rotateX(angle); }
		void translate(float x, float y, float z) { modelToWorld.translate(x, y, z); }
		void rotateZ(float angle) { modelToWorld.rotateZ(angle); }

		// to load new tree we need the world to be the identity matrix and we translate it in the right position (x,y,z)
		// before we add the vertices into the mesh
		void loadNewObject (float x, float y, float z, float w, float h) {
			modelToWorld.loadIdentity();
		    modelToWorld.translate(x, y, z);
		    halfWidth = w*0.5f;
		    halfHeight = h*0.5f;
		    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

		mat4t &getLastModelToWorldmatrix () { return modelToWorldMatrices.back(); }
		mat4t &getModelToWorldMatrix () { return modelToWorld; };

		void pushVertex (vec3 &v) {
			mesh_.pushVertex(v);
		}

    void pushIndex () {
      mesh_.pushIndex();
    }

		void pushUV ( float u, float v ) { mesh_.pushUV( u , v ); }

		void pushIndexForQuad() {
			mesh_.pushIndex();
			mesh_.pushIndex();
			mesh_.pushIndex();

			mesh_.pushIndex(mesh_.getIndexNumber()-2);
			mesh_.pushIndex();
			mesh_.pushIndex(mesh_.getIndexNumber()-2);
		}

		// create a cube for each branch. cube has 8 corners/vertices
		void puchCube () {
			float n = 1.0f/4.0f;  // 4 is the number of photos in .gif

			vec4 v1 =  vec4 ( -halfWidth, 0.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v2 = vec4 (  halfWidth, 0.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v3 =  vec4 (  halfWidth, 2*halfHeight, 0.0f, 1.0f ) * modelToWorld;
			vec4 v4 =  vec4 ( -halfWidth, 2*halfHeight, 0.0f, 1.0f) * modelToWorld;
			vec4 v5 = vec4 ( halfWidth, 0.0f, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v6 = vec4 ( halfWidth, 2*halfHeight, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v7 = vec4 ( -halfWidth, 0.0f, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v8 = vec4 ( -halfWidth, 2*halfHeight, -2*halfWidth, 1.0f) * modelToWorld;
			
			// push vertices and indices into the mesh according to the 8 vertices
			pushIntoMesh( v1, v2, v3, v4, v5, v6, v7, v8);

			// 6 calls, each for each face of cube
			for ( int i = 0; i < 6; i++) {
				mesh_.pushUV( 0.0f, 0.0f);
				mesh_.pushUV( n, 0.0f);
				mesh_.pushUV( n, 1.0f);
				mesh_.pushUV( 0.0f, 1.0f);
			}
			/*
			// 6 calls, each for each face of cube
			// each call has uv coordinates for 4 vertices
			mesh_.pushUVCoordinates1();
			mesh_.pushUVCoordinates1();
			mesh_.pushUVCoordinates1();
			mesh_.pushUVCoordinates1();
			mesh_.pushUVCoordinates1();
			mesh_.pushUVCoordinates1();
			*/
		}
		// push vertices and indices that form a cube into the mesh
		void pushIntoMesh ( vec4 &v1, vec4 &v2, vec4 &v3, vec4 &v4, vec4 &v5, vec4 &v6, vec4 &v7, vec4 &v8) {
			// push front face vertices into the mesh			
			mesh_.pushVertex(vec3 (v4.x(), v4.y(), v4.z()));
			mesh_.pushVertex(vec3 (v3.x(), v3.y(), v3.z()));
			mesh_.pushVertex(vec3 (v1.x(), v1.y(), v1.z()));
			mesh_.pushVertex(vec3 (v2.x(), v2.y(), v2.z()));
			// push indices for the front face
			pushIndexForQuad();

			// right face
			mesh_.pushVertex(vec3 (v3.x(), v3.y(), v3.z()));
			mesh_.pushVertex(vec3(v6.x(), v6.y(), v6.z()));
			mesh_.pushVertex(vec3(v2.x(), v2.y(), v2.z()));
			mesh_.pushVertex(vec3(v5.x(), v5.y(), v5.z()));

			pushIndexForQuad();	

			// back face
			// maybe not in the right order!!!!!		
			mesh_.pushVertex(vec3(v6.x(), v6.y(), v6.z()));
			mesh_.pushVertex(vec3(v8.x(), v8.y(), v8.z()));
			mesh_.pushVertex(vec3(v5.x(), v5.y(), v5.z()));
			mesh_.pushVertex(vec3(v7.x(), v7.y(), v7.z()));

			pushIndexForQuad();	

			// left face	
			mesh_.pushVertex(vec3(v8.x(), v8.y(), v8.z()));
			mesh_.pushVertex(vec3 (v4.x(), v4.y(), v4.z()));
			mesh_.pushVertex(vec3(v7.x(), v7.y(), v7.z()));
			mesh_.pushVertex(vec3 (v1.x(), v1.y(), v1.z()));

			pushIndexForQuad();

			// up face	
			mesh_.pushVertex(vec3(v8.x(), v8.y(), v8.z()));
			mesh_.pushVertex(vec3 (v6.x(), v6.y(), v6.z()));
			mesh_.pushVertex(vec3(v4.x(), v4.y(), v4.z()));
			mesh_.pushVertex(vec3 (v3.x(), v3.y(), v3.z()));

			pushIndexForQuad();
			
			// bottom face
			// maybe its clockwise instead of counterclockwise
			mesh_.pushVertex(vec3(v7.x(), v7.y(), v7.z()));
			mesh_.pushVertex(vec3 (v5.x(), v5.y(), v5.z()));
			mesh_.pushVertex(vec3(v1.x(), v1.y(), v1.z()));
			mesh_.pushVertex(vec3 (v2.x(), v2.y(), v2.z()));

			pushIndexForQuad();
		}

		// create leaf shapes
		void pushLeafs () {
			float n = 1.0f/4.0f;  // 4 is the number of photos in .gif
			//modelToWorld.rotateZ(90.0f);
			vec4 v1 =  vec4 ( 0.0f, 0.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v2 = vec4 (  halfHeight/2, halfHeight/2.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v3 =  vec4 (  0.0f, halfHeight, 0.0f, 1.0f ) * modelToWorld;
			vec4 v4 =  vec4 ( -halfHeight/2, halfHeight/2.0f, 0.0f, 1.0f) * modelToWorld;
			vec4 v5 = vec4 ( halfHeight/2, halfHeight/2.0f, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v6 = vec4 ( 0.0f, halfHeight, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v7 = vec4 ( 0.0f, 0.0f, -2*halfWidth, 1.0f) * modelToWorld;
			vec4 v8 = vec4 ( -halfHeight/2, halfHeight/2.0f, -2*halfWidth, 1.0f) * modelToWorld;
			//modelToWorld.rotateZ(-90.0f);
			pushIntoMesh( v1, v2, v3, v4, v5, v6, v7, v8);

			// 6 calls, each for each face of cube
			for ( int i = 0; i < 6; i++) {
				mesh_.pushUV( n, 0.0f);
				mesh_.pushUV( n + n, 0.0f);
				mesh_.pushUV( n + n, 1.0f);
				mesh_.pushUV( n, 1.0f);
			}
			/*
			// 6 calls, each for each face of cube
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			mesh_.pushUVCoordinates2();
			*/

		}

		void meshReady () { mesh_.setBuffers(); }		

		void pushModelMatrix () { modelToWorldMatrices.push_back(modelToWorld); }
		void popModelMatrix () { modelToWorldMatrices.pop_back(); }
		//dynarray<mat4t> &getModelToWorldMatrices () { return modelToWorldMatrices; }
		void setIdentity(mat4t &matrix) { matrix.loadIdentity(); }

		// render mesh
		void render(color_shader &shader, mat4t &cameraToWorld) { 
		  modelToWorld.loadIdentity();
		 // modelToWorld.rotateX(45.0f);
		  mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

		  //glActiveTexture(GL_TEXTURE0);
		  //glBindTexture(GL_TEXTURE_2D, texture_handle_);
		  //texture_shader_.render(modelToProjection, 0);  
		  //color_shader_.render(modelToProjection, color);
		  shader.render(modelToProjection, color);
		  mesh_.render();
		}

		// render mesh with texture shader
		void render(texture_shader &shader, mat4t &cameraToWorld) { 
		  modelToWorld.loadIdentity();
		 // modelToWorld.rotateX(45.0f);
		  mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
		  texture_handle_ = resources::get_texture_handle(GL_RGB, "assets/terrain2.gif");  
		  glActiveTexture(GL_TEXTURE0);
		  glBindTexture(GL_TEXTURE_2D, texture_handle_);
		  // http://www.codeproject.com/Articles/236394/Bi-Cubic-and-Bi-Linear-Interpolation-with-GLSL
		  //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		  shader.render(modelToProjection, 0);  		
		  mesh_.render();
		}
		/*
		void renderQuads(texture_shader &shader, mat4t &cameraToWorld) { 
		  modelToWorld.loadIdentity();
		 // modelToWorld.rotateX(45.0f);
		  mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
		  texture_handle_ = resources::get_texture_handle(GL_RGB, "assets/terrain.gif");  
		  glActiveTexture(GL_TEXTURE0);
		  glBindTexture(GL_TEXTURE_2D, texture_handle_);
		  shader.render(modelToProjection, 0);  		
		  mesh_.renderQuads();
		}
		*/
		  /*
		  // one attribute buffer, bind vertices to buffer
		  glGenBuffers(1, &vertexbuffer);
		  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		  glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), &vertices[0], GL_STATIC_DRAW);  // 3 korifes * sizeof(float)
		  glVertexAttribPointer(
				attribute_pos,                  // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride   itan 0 !!!!!!!!!!!!!
				(void*)0            // array buffer offset
			);
*/
		 // glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
		  //glEnableVertexAttribArray(attribute_pos);

		  /*
		  glGenBuffers(1, &uvbuffer);
		  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		  glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(vec2), &uv[0], GL_STATIC_DRAW);
		  glVertexAttribPointer(
			attribute_uv,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		*/
		  
		  // attribute_uv is position in the texture of each corner
		  //glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs );
		  //glEnableVertexAttribArray(attribute_uv);
		  
		  /*
		  glGenBuffers(1, &vertexbuffer);
		  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		  glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(float), &data[0], GL_STATIC_DRAW);  // 3 korifes * sizeof(float) data.size()*sizeof(float)
		  glEnableClientState(GL_VERTEX_ARRAY);
		 // glVertexAttribPointer( attribute_pos, 3,  GL_FLOAT, GL_FALSE,  0, (void*)0 );
		  glVertexPointer (3, GL_FLOAT, 0, 0);
		  
		  //glVertexAttribPointer( attribute_uv, 2,  GL_FLOAT, GL_FALSE,  5*sizeof(float), (void*)(3*sizeof(float)) );  // (void*)(3*sizeof(float) because uv start aftre 3 floats from the beggining
		  
		  //glEnableVertexAttribArray(attribute_pos);
		  //glEnableVertexAttribArray(attribute_uv);
		  

		 GLuint elementbuffer;
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLubyte), &indices[0] , GL_STATIC_DRAW);
		  //mb.get_mesh(m);
		  //glDrawArrays(GL_QUADS, 0, data.size()); //GL_TRIANGLE_FAN
		  */
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		  //glDrawElements(GL_TRIANGLES, indices.size(), GL_INT, (void*)0);
		  //glDrawElements(GL_QUADS, indices.size(),  GL_UNSIGNED_BYTE, &indices);
		  //m.render();
		  //glDrawArrays(GL_QUADS, 0, data.size()); //GL_TRIANGLE_FAN
		  //glEnableClientState(GL_VERTEX_ARRAY);
		 // glVertexPointer(3, GL_FLOAT, 0, 0); 
		//glDrawElements(GL_QUADS, indices.size(),  GL_UNSIGNED_BYTE, (void*)0);
	  
		 // box_mesh->render();
		void reset () {
			
			modelToWorld.loadIdentity();
			modelToWorldMatrices.reset();
			mesh_.reset();

			//index_number = 0;
		}
	};

  class Catmull_Rom_Spline {
		dynarray<float> points;
		dynarray<vec3> control_points;
		dynarray<vec3> start_end_points;

    HandleMesh *mesh;

		float length, posZ;
		int knots;
    int num;
    float noiseX, noiseY, noiseZ, angle;
    string type;
    mat4t modelToWorld;

    // produce one point in a spline
    void formula (float p, const vec3 &p0, const vec3 &p1, const vec3 &p2, const vec3 &p3) {
			float t = p;
			float u3 = t * t * t; 
			float u2 = t * t;

			vec3 point;
			// polynomials
			float b0 = 0.5f*(-u3 + 2*u2 - t);
			float b1 = 0.5f*(3*u3 - 5*u2 + 2);
			float b2 = 0.5f*(-3*u3 + 4*u2 + t);
			float b3 = 0.5f*(u3 - u2);
			// desired point
			point.x() = b0*p0.x() + b1*p1.x() + b2*p2.x() + b3*p3.x();
			point.y() = b0*p0.y() + b1*p1.y() + b2*p2.y() + b3*p3.y();
			point.z() = b0*p0.z() + b1*p1.z() + b2*p2.z() + b3*p3.z();

			points.push_back(point.x());
			points.push_back(point.y());
			points.push_back(point.z());

      //printf("%f %f %f\n", point.x(), point.y(), point.z() );

		}

    void calculatePoints (int num, dynarray<vec3> &vectors) {
			float t = 0.0f;
      vec3 center;
			for ( int i = 0; i <= num; i ++) {
				formula(t, vectors[0], vectors[1], vectors[2], vectors[3]);
				t += 1.0f/num;
			}
      //getCenter( center );
      //calculateRenderedTriangles(center);
		}

    void calculateRenderedTriangles ( vec3 &center, float distX, float distY ) {
      vec2 uv;
      mat4t rotate;
      rotate.loadIdentity();
      rotate = modelToWorld;
      printf("%d\n", points.size());
      for ( unsigned int j = 0; j < num; j++ ) { // create as many as the input says      
        rotate.rotateZ(angle);
        for ( unsigned int i = 0; i < points.size()/3-1; i++ ) {  // I am not sure if the condition is correct!!!!!!!!!!!!!!!
          mesh->pushVertex( center*rotate );
          //mesh->pushVertex( vec3( center.x(), center.y(), -3.0f ) );
          mesh->pushIndex();
          uv = findUVCoordinates( center, distX, distY);
          mesh->pushUV( uv.x(), uv.y() );

          // push 1st
          mesh->pushVertex( vec3( points[3*i], points[3*i+1], points[3*i+2]  ) *rotate );
          mesh->pushIndex();
          uv = findUVCoordinates( vec3( points[3*i], points[3*i+1], points[3*i+2]  ), distX, distY);
          mesh->pushUV( uv.x(), uv.y() );

          // push 2st
          mesh->pushVertex( vec3( points[3*i+3], points[3*i+4], points[3*i+5]  ) *rotate );
          mesh->pushIndex();

          uv = findUVCoordinates( vec3( points[3*i+3], points[3*i+4], points[3*i+5] ), distX, distY);
          mesh->pushUV( uv.x(), uv.y() );
        }
        //rotate.rotateZ(angle);
      }
     // reset();
    }

    void createObjects () {
      dynarray<vec3> four_points;
      vec3 center;
      float distX = 0.0f,distY = 0.0f;
			// for every 2 points in the array
      // this will cause trouble with more than 2 splines!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // because I create the points for 2 splines and then calculate center and triangles
      // if I put 4 splines it will calculate center and triangels for all of them so I need an extra for loop
			for (unsigned int i = 0; i < control_points.size()/2; i++ ) {
        four_points.push_back(control_points[i*2]); // first control point
			  four_points.push_back(start_end_points[i*2]); // start point
			  four_points.push_back(start_end_points[i*2+1]); // end point 
			  four_points.push_back(control_points[i*2+1]); // second control point

			  calculatePoints(knots, four_points); // integer is the number of points that will be generated
        
        four_points.reset();
			}
      getCenter( center, distX, distY );
      calculateRenderedTriangles(center, distX, distY);
      reset();
		}

    void getCenter( vec3 &center, float &distX, float &distY) {
      float _minX = 0.0f, _maxX = 0.0f;
      float _minY = 0.0f, _maxY = 0.0f;
      float _minZ = 0.0f, _maxZ = 0.0f;
      for ( unsigned int i = 0; i < points.size(); i++ ) {
        if ( i%3 == 0 && points[i] < _minX )  // if it is x value
          _minX = points[i];
        else if ( i%3 == 0 && points[i] > _maxX )
          _maxX = points[i];
        if (i%3 == 1 && points[i] < _minY ) // if it is y value
          _minY = points[i];
        else if ( i%3 == 1 && points[i] > _maxY )
          _maxY = points[i];   
        if (i%3 == 2 && points[i] < _minZ ) // if it is y value
          _minZ = points[i];
        else if ( i%3 == 2 && points[i] > _maxZ )
          _maxZ = points[i];  
      }
      //printf ("Center: %f %f\n", (_maxX-fabs(_minX))/2.0f, (_maxY-fabs(_minY))/2.0f);
      distX = abs(_maxX)-_minX;
      distY = abs(_maxY)-_minY;
      center[0]=(_maxX-abs(_minX))/2.0f;
      center[1]=(_maxY-abs(_minY))/2.0f;
      center[2]=(_maxZ-abs(_minZ))/2.0f;
    }

    vec2 findUVCoordinates ( const vec3 &v_, float distX, float distY) {
      float n = 1.0f/4.0f;
		  // if I divide each time x value with the distance then I have the correct coordinate on [0,1]
      if ( type == "leaf")
		    return vec2( n + n*2.0f*abs(v_.x() + distX/2.0f)/(distX*2.0f), abs(v_.y())/distY );
        //vec2( n + n*2.0f*abs(v_.x())/distX, abs(v_.y())/distY );
      else 
        return vec2( 2*n + n*2.0f*(abs(v_.x()+distX/2.0f)/(distX*2.0f)), abs(v_.y())/distY );
	  }

    void createPoints () {   
      start_end_points.push_back(vec3(0.0f, 0.0f, 0.0f));
      start_end_points.push_back(vec3(0.0f, length, posZ));

      start_end_points.push_back(vec3(0.0f, 0.0f, 0.0f));
      start_end_points.push_back(vec3(0.0f, length, posZ));
	  }

    void constructControlPoints() {
      mat4t rotate;
      rotate.loadIdentity();
			for (unsigned int i = 0; i < start_end_points.size()/4; i++ ) { // i < start_end_points.size()/2 !!!!!!!!!!!!!!!!
        control_points.push_back(vec3( -start_end_points[i*4+1].y() * noiseX, start_end_points[i*4].x(), 0.0f ));
				control_points.push_back(vec3( -start_end_points[i*4+1].y() * noiseX, start_end_points[i*4+1].y() * noiseY, noiseZ ));
        control_points.push_back(vec3( start_end_points[i*4+1].y() * noiseX, start_end_points[i*4+1].x(), 0.0f ));
				control_points.push_back(vec3( start_end_points[i*4+1].y() * noiseX, start_end_points[i*4+1].y() * noiseY, noiseZ ));
			}
		}

    void reset () {
		  points.reset();
      control_points.reset();
      start_end_points.reset();
      type = "";
		}

	public:

		Catmull_Rom_Spline() {}

    void init ( HandleMesh &mesh_ ) {
      mesh = &mesh_;     
      //set ( l, k, num_, type_, angle_, noiseX_, noiseY_, noiseZ_ );
    }

    void set ( float l, int k, float posZ_, int num_, string type_, float angle_, mat4t &modelToWorld_, float noiseX_= 0.0f, float noiseY_= 0.0f, float noiseZ_ = 0.0f ) {
      num = num_;
      length = l;
      posZ = posZ_;
			knots = k;
      type = type_;
      angle = angle_;
      noiseX = noiseX_;
      noiseY = noiseY_;
      noiseZ = noiseZ_;
      points.reset();
      modelToWorld = modelToWorld_;
      createPoints (); // create start and end points
			constructControlPoints(); // create control points		
			createObjects(); 
    }
  
		void moveSpline1 () {
			reset();
			for (unsigned int i = 0; i < control_points.size()/2; i++ ) {
				// move control points depending on quadrant that the end point belongs
				if ( start_end_points[i*2+1].x() * start_end_points[i*2+1].y() < 0 ) { // if it belongs to 2nd or 4th quadrant
					control_points[i*2].x() += 1.0f;
					control_points[i*2+1].x() -= 1.0f;
					control_points[i*2].y() += 1.0f;
					control_points[i*2+1].y() -= 1.0f;
					control_points[i*2].z() += 1.0f;   
					control_points[i*2+1].z() -= 1.0f;
				}
				else {
					control_points[i*2].x() += 1.0f;
					control_points[i*2+1].x() -= 1.0f;
					control_points[i*2].y() -= 1.0f;
					control_points[i*2+1].y() += 1.0f;
					control_points[i*2].z() -= 1.0f;
					control_points[i*2+1].z() += 1.0f;
				}
			}
			createObjects ();
		}

		void moveSpline2 () {
			reset();
			for (unsigned int i = 0; i < control_points.size()/2; i++ ) {
				// move control points depending on quadrant that the end point belongs
				if ( start_end_points[i*2+1].x() * start_end_points[i*2+1].y() < 0 ) { // if it belongs to 2nd or 4th quadrant
					control_points[i*2].x() -= 1.0f;
					control_points[i*2+1].x() += 1.0f;
					control_points[i*2].y() -= 1.0f;
					control_points[i*2+1].y() += 1.0f;
					control_points[i*2].z() -= 1.0f;   // sth has to be done here!!!
					control_points[i*2+1].z() += 1.0f;
				}
				else {
					control_points[i*2].x() -= 1.0f;
					control_points[i*2].y() += 1.0f;
					control_points[i*2+1].x() += 1.0f;
					control_points[i*2+1].y() -= 1.0f;

					control_points[i*2].z() += 1.0f;   // sth has to be done here!!!
					control_points[i*2+1].z() -= 1.0f;
				}
			}
			createObjects ();
		}

		void render ( color_shader &shader, mat4t &cameraToWorld ) {
       mat4t modelToWorld;
       modelToWorld.loadIdentity();
       mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
		   shader.render(modelToProjection, vec4(1.0f, 0.0f, 0.0f, 1.0f));
			 glPointSize(5.0f);
			 glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)&points );
		   glEnableVertexAttribArray(attribute_pos);
    
			 glDrawArrays(GL_LINE_STRIP, 0, points.size()/2);
			 glDrawArrays(GL_POINTS, 0, points.size());
       //glDrawArrays(GL_TRIANGLES, 0, rendered_points.size() );
       //glDrawArrays(GL_POINTS, 0, rendered_points.size() );

     // _buffer.draw();
		}
	};
  
  class MeshEngine{
	  L_Systems file;
	  HandleMesh *mesh;

	  // this array consists the number of trees that should be generated of each species of tree
	  dynarray<int> numberOfSpecies;
	  dynarray<string> trees; // type of trees
	  string finalAxiom;
	  time_t timer;
	  enum Type { BUSH, LEAFTREE, BRANCHTREE, A } treeType;

    Catmull_Rom_Spline flower;


  public:
	  MeshEngine () { }

	  void init ( dynarray<int> &num, dynarray<string> &types, HandleMesh &mesh_ ) {
		  mesh = &mesh_;
		  //mesh.init();
		  numberOfSpecies = num;
		  trees = types;
		  srand(time(NULL));

      flower.init( mesh_ );  // petal:4, 2   leaf:2, 1.5
      //flower.set( 5, 5, 3, "rose", 120.0f, mat, 4.0f, 2.0f, -10.0f );
      //flower.set( 10, 5, 1, "leaf", 45.0f, mat, 2.0f, 1.5f, 10.0f );
		 
		  createObjects();		  
	  }

	  void readFile() {
		  file.reset();
		  if (treeType == BUSH)
			  file.openFiles(app_utils::get_path("assets/source1.txt")); 
		  else if (treeType == BRANCHTREE)
			  file.openFiles(app_utils::get_path("assets/source2.txt")); 
		  else if (treeType == LEAFTREE)
			  file.openFiles(app_utils::get_path("assets/source3.txt")); 
		  else if (treeType == A)
			  file.openFiles(app_utils::get_path("assets/source4.txt")); 
	  }

	  void translateTrees ( int i ) {
		  if ( trees[i] == "mountain")
			  treeType = BUSH;
		  else if ( trees[i] == "dessert")
			  treeType = BRANCHTREE;
		  else if ( trees[i] == "river")
			  treeType = LEAFTREE;
		  else if ( trees[i] == "coast")
			  treeType = A;
	  }

	  void createObjects () {
		  translateTrees(0);
		  readFile();
		  createSequence ();
		  for( unsigned int j = 0; j < numberOfSpecies.size(); j++) { // for every input in numberOfSpecies array which actuall says how many differenst species of trees we have
			  for ( unsigned int i = 0; i < numberOfSpecies[j]; i++){ // number of trees of that species
				  mesh->loadNewObject(file.getPosition(0) + i*20.0f, 0.0f, file.getPosition(0) + i*20.0f, 0.5f, 3.5f );
				  manageRendering(finalAxiom);

          if ( file.isStohastic() ) {
					  readFile();
					  createSequence ();
				  }
			  }
			  if ( j < numberOfSpecies.size() - 1 ) {
				  translateTrees(j+1);
				  readFile();
				  createSequence ();
			  }
		  }
		  //mesh.meshReady();
	  }

	  void manageRendering ( string axiom ) {
			for ( int i = 0; i < axiom.size(); i++){
				if ( file.existInVariables(axiom[i]) ) {  
					if ( axiom[i] == 'F' ) {
						mesh->puchCube();
						mesh->translate( 0.0f, mesh->getHeight(), 0.0f );					
					}
					// it will cause problem the leaf height because it is not the same as branch height and here
					// we translate brunch height
					else if (axiom[i] == 'X' ) {
            flower.set( static_cast<float>(rand()%2+1), 10, static_cast<float>(rand()%1+1), 3, "rose", 120.0f, mesh->getModelToWorldMatrix(), static_cast<float>(rand()%4+1), static_cast<float>(rand()%2+1), 0.0f );  // noise: 4, 2, 0
            flower.set( static_cast<float>(rand()%3+1), 10, static_cast<float>(-rand()%1+1), 1, "leaf", 45.0f, mesh->getModelToWorldMatrix(), static_cast<float>(rand()%2+1), static_cast<float>(rand()%1+1), 0.0f );  // noise: 2, 1.5, 0
						//mesh->pushLeafs();
						//mesh.translate( 0.0f, mesh.getHeight() / 2.0f, 0.0f );
					}	
          else if (axiom[i] == 'P' ) {
            /*
            flower.set( 3.0f, 10, -0.5f, 1, "leaf", 0.0f, mesh->getModelToWorldMatrix(), 2.0f, 3.5f,  0.0f );
            flower.set( 3.0f, 10, 0.5f, 1, "leaf", 40.0f, mesh->getModelToWorldMatrix(), 2.0f, 3.5f,  0.0f );
            flower.set( 3.0f, 10, 0.5f, 1, "leaf", -40.0f, mesh->getModelToWorldMatrix(), 2.0f, 3.5f,  0.0f );
            */
            
            flower.set( 3, 10, 0.5f, 1, "leaf", 0.0f, mesh->getModelToWorldMatrix(), 2.0f, 5.0f,  -1.0f );
            //flower.set(  2, 10, 0.5f, 1, "leaf", 50.0f, mesh->getModelToWorldMatrix(), 1.0f, 2.0f,  .0f );
            //flower.set(  2, 10, 0.5f, 1, "leaf", -50.0f, mesh->getModelToWorldMatrix(), 1.0f, 2.0f,  .0f );
            /*
            flower.set(  1, 10, 0.5f, 1, "leaf", 100.0f, mesh->getModelToWorldMatrix(), 1.0f, 2.0f,  .0f );
            flower.set(  1, 10, 0.5f, 1, "leaf", -100.0f, mesh->getModelToWorldMatrix(), 1.0f, 2.0f,  .0f );
            */
          }
          else if ( axiom[i] == 'T' ) {
            flower.set( 2.0f, 10, 0.0f, 3, "rose", 120.0f, mesh->getModelToWorldMatrix(), 4.0f, 2.0f, .0f );
            flower.set( 1.0f, 10, 1.0f, 1, "rose", 45.0f, mesh->getModelToWorldMatrix(), 4.0f, 2.0f, -1.0f );
            flower.set( 1.0f, 10, 1.0f, 1, "rose", -45.0f, mesh->getModelToWorldMatrix(), 4.0f, 2.0f, -1.0f );
            flower.set( 1.0f, 10, 1.0f, 1, "rose", 180.0f, mesh->getModelToWorldMatrix(), 4.0f, 2.0f, -1.0f );
            //flower.set(  4, 10, 0.0f, 1, "leaf", 45.0f, mesh->getModelToWorldMatrix(), 2.0f, 1.5f,  1.0f );
          }
          else if ( axiom[i] == 'L' ) {
            flower.set( 2.0f, 10, 3.0f, 4, "rose", 90.0f, mesh->getModelToWorldMatrix(), 2.0f, 2.0f, 1.0f );
            
          }
				}
				else if ( axiom[i] == '+' ) {
					mesh->rotateZ( file.getAngle());
				}
				else if ( axiom[i] == '-' ) {
					mesh->rotateZ( -file.getAngle());
				}
				else if ( axiom[i] == '[' ) {
					mesh->pushModelMatrix();
				}
				else if ( axiom[i] == ']' ) {
					mesh->getModelToWorldMatrix() = mesh->getLastModelToWorldmatrix();
					mesh->popModelMatrix();
				}
				else if ( axiom[i] == '^' ) {
					mesh->rotateY( -file.getAngle() );
				}
				else if ( axiom[i] == '&' ) {
					mesh->rotateY( file.getAngle() );
				}
				else if ( axiom[i] == '<' ) {
					mesh->rotateX( file.getAngle() );
				}
				else if ( axiom[i] == '>' ) {
					mesh->rotateX( -file.getAngle() );
				}
				else if ( axiom[i] == '|' ) {
					mesh->rotateX( 180.0f );
				}
			}
	}

	  string chooseRule( int i, char c ) {
		  int temp = rand()%100;
		  if ( !file.isStohastic() ) // if not stochastic
			  return file.getRules(i);
		  else {                      // if it is stochastic
			  return file.selectRule(c, temp); // select rule depending on propabilities
		  }
		  
	  }

	  // replace its variable with the right rule
	  string ManageSequenceRules ( char temp ) {
		string s;
		s = &temp;
		for ( int i = 0; i < file.sizeOfVariables(); i++) {
			
			if ( temp == file.getVariables(i)) //'F'
				return chooseRule( i, temp );//file.getRules(i);// edw parapioisi
		}
	    if (temp == '\n')
			return "";
		else		
			return s;		
	}

	  // create the sequence that will be rendered
	  void createSequence () {
		string axiom = file.getAxiom();
		string seq;
		for ( int i=0; i<file.getIterations(); i++) {
			for ( unsigned int j=0; j<strlen(axiom); j++) {
				seq += ManageSequenceRules(axiom[j]); //tree.getRules();
			}
			axiom = seq;
			seq = "";
		}
		if (file.getIterations() <= 0)
			axiom = "";
		//createObjects(axiom);
		//manageRendering ( axiom );
		finalAxiom = axiom;
		printf("Final axiom is %s\n", finalAxiom);
	}

	  void render ( texture_shader &shader, mat4t &cameraToWorld) {
		  mesh->render(shader, cameraToWorld);
	  }

  };

  class Terrain {
	  const enum { SIZE = 20, DISTANCE = 10 };
	  unsigned int map[SIZE][SIZE];
	  HandleMesh *_mesh;
	  dynarray<dynarray<vec3> > vertices;
	  
  public:
	  Terrain() {  }

	  void init( HandleMesh &mesh ) {
		  setValuesInMap ();
		  //_mesh.init();
		  _mesh = &mesh;
		  createVertices ();

	  }
	  
	  // set random values to the height map
	  void setValuesInMap () {
		  
		  for ( int i = 0; i < (sizeof(map)/sizeof(int))/(sizeof(map[0])/sizeof(int)); i++ ) {  // or just SIZE
			  for ( int j = 0; j < sizeof(map[0])/sizeof(int); j++ ) {   // or just SIZE
				  map[i][j] = rand() % 25;   // 255
			  }
		  }
	  }

	  // for SIZE*SIZE array we create (SIZE*SIZE) number of vertices
	  void createVertices () {
		  float n =  1.0f/4.0f; // 4 is the number of photos in the .gif

		  vertices.reset();
		  // vertices are stored like this:
		  // first row all vertices with z = 0, second row all vertises with z = DISTANCE...
		  for ( int j = 0; j < sizeof(map[0])/sizeof(int); j++ ) {	
			  vertices.push_back(dynarray<vec3>());
			  for ( int i = 0; i < (sizeof(map)/sizeof(int))/(sizeof(map[0])/sizeof(int)); i++ ) {
				  vec3 v = vec3((float)DISTANCE*i, (float)map[i][j], (float)DISTANCE*j);
				  vertices[j].push_back(vec3((float)DISTANCE*i, (float)map[i][j], (float)DISTANCE*j));
			  }
		  }
		  
		  vec2 tempuv;
		  // these should be returned from the function polygonDistance() each time for each polygon
		  float distX = static_cast<float>(SIZE*DISTANCE-DISTANCE); 
		  float distY = static_cast<float>(SIZE*DISTANCE-DISTANCE);
		  // push each vertices that form a square like that:
		  // [0][0], [0][1], [1][0], [1][1] so take 2 vertices of the first row and 2 of the second and form a square
		  for ( unsigned int i = 0; i < vertices.size()-1; i++ ) {
			  int temp = 2;  // defines which image will be textured
			  for ( unsigned int j = 0; j < vertices[0].size()-1; j++ ) {
				  _mesh->pushVertex( vertices[i][j] );
				  tempuv = findUVCoordinates( vertices[i][j], distX, distY );
				  //_mesh->pushUV( temp * n, 0.0f );
				  _mesh->pushUV( tempuv.x()*n + temp*n, tempuv.y() );
				  //_mesh->pushUV( tempuv.x(), tempuv.y() );

				  _mesh->pushVertex( vertices[i+1][j] );
				  tempuv = findUVCoordinates( vertices[i+1][j], distX, distY );
				  //_mesh->pushUV( temp * n + n, 1.0f );
				  _mesh->pushUV( tempuv.x()*n + temp*n, tempuv.y() );
				  //_mesh->pushUV( tempuv.x(), tempuv.y() );

				  _mesh->pushVertex( vertices[i][j+1] );
				  tempuv = findUVCoordinates( vertices[i][j+1], distX, distY );
				  //_mesh->pushUV( temp * n + n, 0.0f );
				  _mesh->pushUV( tempuv.x()*n + temp*n, tempuv.y() );
				  //_mesh->pushUV( tempuv.x(), tempuv.y() );
			  

				  _mesh->pushVertex( vertices[i+1][j+1] );
				  tempuv = findUVCoordinates( vertices[i+1][j+1], distX, distY );
				 // _mesh->pushUV( temp * n, 1.0f );
				  _mesh->pushUV( tempuv.x()*n + temp*n, tempuv.y() );
				  //_mesh->pushUV( tempuv.x(), tempuv.y() );

				  // push the indices to form a square that consists of 2 triangles
			      _mesh->pushIndexForQuad();
			  }		  
		  }
		  // mesh is ready enable the attributes and set the buffers
		  //_mesh.meshReady();
	  }

	  // return the bounding box that a polygon is in
	  vec2 polygonDistance ( dynarray<vec3> &v ) {
		  float minX, minZ, maxX, maxZ;
		  minX = minZ = maxX = maxZ = 0.0f;
		  for ( unsigned int i = 0; i < v.size(); i++ ) {
			  if ( v[i].x() < minX )
				  minX = v[i].x();
			  else if ( v[i].x() > maxX )
				  maxX = v[i].x();
			  if ( v[i].z() < minZ )
				  minZ = v[i].z();
			  else if ( v[i].z() > maxZ )
				  maxZ = v[i].z();
		  }
		  return vec2( abs(maxX-minX), abs(maxZ - minZ) );
	  }

	  vec2 findUVCoordinates ( const vec3 &v_, float distX, float distY) {
		  //float u = 1.0f/(SIZE*DISTANCE-DISTANCE); // distance on x axis
		  //float v = 1.0f/(SIZE*DISTANCE-DISTANCE); // distance on y axis
		  // if I divide each time x value with the distance then I have the correct coordinate on [0,1]
		  return vec2( v_.x()/distX, v_.z()/distY );
		  //return vec2( v_.x()*u, v_.z()*v );
	  }

	  void render ( color_shader &shader, mat4t &cameraToWorld) {
		  _mesh->render(shader, cameraToWorld);
	  }

	  void render ( texture_shader &shader, mat4t &cameraToWorld) {
		  _mesh->render(shader, cameraToWorld);
	  }
  };

 
  class Lsystems_app : public octet::app {
	camera cameraToWorld;
	MeshEngine tree;	
	HandleMesh mesh_;
	dynarray<int> treenums;
	dynarray<string> landtypes;
	Terrain _terrain;
	color_shader color_shader_;
	texture_shader texture_shader_;
  Catmull_Rom_Spline cut_splines;

  public:

    Lsystems_app(int argc, char **argv) : app(argc, argv) {
    }

    void app_init() {
     // dynarray<vec3> points;
      //points.push_back(vec3(0.0f, 0.0f, 0.0f));
     // points.push_back(vec3(0.0f, 0.0f, 0.0f));
		  color_shader_.init();
		  texture_shader_.init();
		  cameraToWorld.translateUp(10.0f);

      mat4t mat;
      mat.loadIdentity();
      //mat.rotateY(90.0f);

		  setValues(); // set number of trees for each species
		  //tree.init(treenums, landtypes, mesh_);
		  //_terrain.init( mesh_ );
      cut_splines.init( mesh_ );  // petal:4, 2   leaf:2, 1.5
      cut_splines.set( 5, 10, 0.0f, 3, "rose", 120.0f, mat, 4.0f, 2.0f, .0f );
      //cut_splines.set( 4, 10, 1.5f, 1, "rose", 45.0f, mat, 4.0f, 2.0f, -1.0f );
      //cut_splines.set( 4, 10, 1.5f, 1, "rose", -45.0f, mat, 4.0f, 2.0f, -1.0f );
      //cut_splines.set( 4, 10, 1.5f, 1, "rose", 180.0f, mat, 4.0f, 2.0f, -1.0f );
      //cut_splines.set(  6, 10, 0.0f, 1, "leaf", 45.0f, mat, 2.0f, 1.5f,  .0f );
		  mesh_.meshReady(); // this SHOULD be calded after mesh is ready with all vertices in it
    }	

	// how many trees of each file
	void setValues () {
		//treenums.push_back(1);
		//landtypes.push_back("mountain");
		treenums.push_back(3);
		landtypes.push_back("dessert");
	  //treenums.push_back(3);
		//landtypes.push_back("river");
		//treenums.push_back(3);
		//landtypes.push_back("coast");

	}
	
	void handleKeyStrokes () {
		
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
	}

    void draw_world(int x, int y, int w, int h) {
    
      glViewport(x, y, w, h);
	    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	  //_terrain.render(texture_shader_, cameraToWorld.getCamera() );
	  //mesh_.render( texture_shader_, cameraToWorld.getCamera() );
	  //tree.render(texture_shader_, cameraToWorld.getCamera());
	  
      //splines.render( color_shader_, cameraToWorld.getCamera() );
      //mesh_.render( color_shader_, cameraToWorld.getCamera() );
      mesh_.render( texture_shader_, cameraToWorld.getCamera() );

	  handleKeyStrokes ();	  
	  //get_mouse_pos(cameraToWorld.getMouseX(), cameraToWorld.getMouseY());
	  //cameraToWorld.detectMouse();
    }
  };
}

