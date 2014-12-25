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
  class colorful_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;

    // index for flat shader emissive color
    GLuint emissive_colorIndex_;
	GLuint divideIndex_;
	GLuint shapeIndex_;

  public:
    void init() {
      // this is the vertex shader. main() will be called for every vertex
      // of every triangle. The output is gl_Position which is used to generate
      // 2D triangles in the rasterizer.
      const char vertex_shader[] = SHADER_STR(
        attribute vec4 pos;
        uniform mat4 modelToProjection;
		    varying vec4 _pos;
		    attribute vec2 uv;
		    varying vec2 uv_;
            void main() { 
			    gl_Position = modelToProjection * pos; 
			    _pos = gl_Position;
			    uv_ = uv;
		    }
      );

      // this is the fragment shader. It is called once for every pixel
      // in the rasterized triangles.
      const char fragment_shader[] = SHADER_STR(
        uniform vec4 emissive_color;
	      uniform float divide;
		    uniform float shape;
	      varying vec4 _pos;
		    varying vec2 uv_;
            void main() {
			    // leafs
			    if ( shape == 1.0f )
				    //gl_FragColor = mix ( vec4( cos(_pos.x), cos(_pos.y), 0.1f, 1.0f ), vec4(uv_, 0.0, 1.0), 0.5f);
					    gl_FragColor = mix ( vec4( cos(_pos.x)+0.2f, cos(_pos.y)+0.3f, 0.1f, 1.0f ), vec4(uv_, 0.0, 1.0), 0.5f);
			    else
			    // lines
				    gl_FragColor = vec4( sin(gl_FragCoord.x*divide), sin(gl_FragCoord.y*divide) , 0.1f, 1.0f);
		    }
      );
    
      // compile and link the shaders
      shader::init(vertex_shader, fragment_shader);

      // set up handles to access the uniforms.
      modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
      emissive_colorIndex_ = glGetUniformLocation(program(), "emissive_color");
	  divideIndex_ = glGetUniformLocation(program(), "divide");
	  shapeIndex_ = glGetUniformLocation(program(), "shape");
    }

    // start drawing with this shader
    void render(const mat4t &modelToProjection, const vec4 &emissive_color, float divide, float shape) {
      // start using the program
      shader::render();

      // set the uniforms.
      glUniform4fv(emissive_colorIndex_, 1, emissive_color.get());
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
	  glUniform1f(divideIndex_, divide);
	  glUniform1f(shapeIndex_, shape);

      // now we are ready to define the attributes and draw the triangles.
    }
  };
}
