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
  class star_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;

    // index for flat shader emissive color
    GLuint emissive_colorIndex_;

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
			varying vec4 pos_;
			void main() {
				// dot product of the vertex with vector(1.0,0.0,0.0)
				float temp = (pos_.x )/sqrt(pos_.x*pos_.x + pos_.y*pos_.y);
				float temp2 = acos(temp)*180/3.14; // in degreees
				if ( floor(mod( temp2, 20.0f)) == 0.0f || floor(mod( temp2, 20.0f)) == 2.0f || floor(mod( temp2, 20.0f)) == 3.0f)			
					gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
				
				else
					gl_FragColor = emissive_color;
			
			}
			//}//emissive_color;
      );
    
      // compile and link the shaders
      shader::init(vertex_shader, fragment_shader);

      // set up handles to access the uniforms.
      modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
      emissive_colorIndex_ = glGetUniformLocation(program(), "emissive_color");
    }

    // start drawing with this shader
    void render(const mat4t &modelToProjection, const vec4 &emissive_color) {
      // start using the program
      shader::render();

      // set the uniforms.
      glUniform4fv(emissive_colorIndex_, 1, emissive_color.get());
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());

      // now we are ready to define the attributes and draw the triangles.
    }
  };
}