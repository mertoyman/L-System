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
  class my_shader : public shader {
    // indices to use with glUniform*()

    // index for model space to projection space matrix
    GLuint modelToProjectionIndex_;

    // index for flat shader emissive color
    GLuint emissive_colorIndex_;
    GLuint colorfactor;
  public:
    void init() {
      // this is the vertex shader. main() will be called for every vertex
      // of every triangle. The output is gl_Position which is used to generate
      // 2D triangles in the rasterizer.
      const char vertex_shader[] = SHADER_STR(
        attribute vec4 pos;
        uniform mat4 modelToProjection;
        void main() { gl_Position = modelToProjection * pos; }
      );

      // this is the fragment shader. It is called once for every pixel
      // in the rasterized triangles.
      const char fragment_shader[] = SHADER_STR(
        uniform vec4 emissive_color;
        uniform float colorfactor;
        void main() {gl_FragColor = vec4(emissive_color.x*colorfactor, emissive_color.y*colorfactor, emissive_color.z*colorfactor, 1.0f);}
      );
    
      // compile and link the shaders
      shader::init(vertex_shader, fragment_shader);

      // set up handles to access the uniforms.
      modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
      emissive_colorIndex_ = glGetUniformLocation(program(), "emissive_color");
      colorfactor = glGetUniformLocation(program(), "colorfactor");
    }

    // start drawing with this shader
    void render(const mat4t &modelToProjection, const vec4 &emissive_color, float color) {
      // start using the program
      shader::render();

      // set the uniforms.
      glUniform4fv(emissive_colorIndex_, 1, emissive_color.get());
      glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
      glUniform1f(colorfactor, color);

      // now we are ready to define the attributes and draw the triangles.
    }
  };
}
