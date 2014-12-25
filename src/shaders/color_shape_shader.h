////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Solid color shader
//
// shader which renders with a solid color

namespace octet {
  class color_shape_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;

    // index for flat shader emissive color
    GLuint emissive_colorIndex_;
	GLuint boxpositionIndex_;

  public:
    void init() {
      // this is the vertex shader. main() will be called for every vertex
      // of every triangle. The output is gl_Position which is used to generate
      // 2D triangles in the rasterizer.
      const char vertex_shader[] = SHADER_STR(
        attribute vec4 pos;
	    varying vec4 pos_;
        uniform mat4 modelToProjection;
        void main() { 
			gl_Position = modelToProjection * pos;
			pos_ = pos;
		}
      );

      // this is the fragment shader. It is called once for every pixel
      // in the rasterized triangles.
      const char fragment_shader[] = SHADER_STR(
			uniform vec4 emissive_color;
			uniform vec4 boxposition;
			varying vec4 pos_;
			void main() {
				//gl_FragColor = vec4( 1.0, cos( gl_FragCoord.y ), sin(  gl_FragCoord.x )  , 1.0 );//emissive_color;
				//if ( gl_FragCoord.x/496.0f > 0.5)
				//if ( abs(pos_.x - boxposition.x) < 0.62f )
				if ( pos_.y > 0.5f*sin(pos_.x*3.14f*2.0f)  )//wave	
					gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
				
				else
					gl_FragColor = emissive_color;	
			}
      );
    
      // compile and link the shaders
      shader::init(vertex_shader, fragment_shader);

      // set up handles to access the uniforms.
      modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
      emissive_colorIndex_ = glGetUniformLocation(program(), "emissive_color");
    }

    // start drawing with this shader
    void render(const mat4t &modelToProjection, const vec4 &emissive_color, const vec4 &boxposition) {
      // start using the program
      shader::render();

      // set the uniforms.
      glUniform4fv(emissive_colorIndex_, 1, emissive_color.get());
	  glUniform4fv(boxpositionIndex_, 1, boxposition.get());
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());

      // now we are ready to define the attributes and draw the triangles.
    }
  };
}