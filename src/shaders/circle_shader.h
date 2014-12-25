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
  class cirlce_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;

    // index for flat shader emissive color
    GLuint emissive_colorIndex_;
	GLuint boxpositionIndex_;
	GLuint dimensionsIndex_;

  public:
    void init() {
      // this is the vertex shader. main() will be called for every vertex
      // of every triangle. The output is gl_Position which is used to generate
      // 2D triangles in the rasterizer.
      const char vertex_shader[] = SHADER_STR(
        attribute vec4 pos;
	    varying vec4 pos_;
        uniform mat4 modelToProjection;
		//in vec2 texCoord;
		//out vec2 texCoordV;
		varying vec2 uv_;
		attribute vec2 uv;
        void main() { 
			//gl_Position = modelToProjection * pos;
			gl_Position = modelToProjection * pos;
			pos_ = pos;
			uv_ = uv;
			//texCoordV = texCoord;
		}
      );

      // this is the fragment shader. It is called once for every pixel
      // in the rasterized triangles.
      const char fragment_shader[] = SHADER_STR(
			uniform vec4 emissive_color;
			uniform vec4 boxposition;
			uniform float dimensions; //  halfwidth
			varying vec4 pos_;
			varying vec2 uv_;
			//in vec2 texCoordV;
			void main() {
				// temp stores the distance of the fragment from the center of the box
				float temp = sqrt((pos_.x - boxposition.x)*(pos_.x - boxposition.x) + (pos_.y - boxposition.y)*(pos_.y - boxposition.y));
				// mix the uv parametres with a vector that each parameter of the color is depending on the distance
				// so as a viewing result we have a kind of circle
				gl_FragColor = mix( vec4(1.0*temp, 1.0*temp, 1.0*temp, 1.0), vec4(uv_, 0.0, 1.0), 0.5); // interpolates 2 colors!!!				
				//	gl_FragColor = vec4(0.0, sin(gl_FragCoord.x), 0.0, 1.0);  //stripes			
			}
      );
    
      // compile and link the shaders
      shader::init(vertex_shader, fragment_shader);

      // set up handles to access the uniforms.
      modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
      emissive_colorIndex_ = glGetUniformLocation(program(), "emissive_color");
	  //boxpositionIndex_ = glGetUniformLocation(program(), "boxposition");
	  dimensionsIndex_ = glGetUniformLocation(program(), "dimensions");
    }

    // start drawing with this shader
    void render(const mat4t &modelToProjection, const vec4 &emissive_color, const vec4 &boxposition, float dimensions) {
      // start using the program
      shader::render();

      // set the uniforms.
      glUniform4fv(emissive_colorIndex_, 1, emissive_color.get());
	  glUniform4fv(boxpositionIndex_, 1, boxposition.get());
	  glUniform1f(dimensionsIndex_, dimensions); // here I have to use the right function for float otherwise 0 is what I take
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());

      // now we are ready to define the attributes and draw the triangles.
    }
  };
}