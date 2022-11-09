// TextureDemo.cpp
// -- texture map demo program
// cs200 10/20
//
// From Visual Studio command prompt:
//   cl /EHsc TextureDemo.cpp Affine.lib Texture.cpp opengl32.lib
//      glew32.lib SLD2.lib SLD2main.lib /link /subsystem:console
// From Linux command line:
//   g++ TextureDemo.cpp Affine.cpp Texture.cpp -lGL -lGLEW -lSDL2
//
// usage:
//   TextureDemo [<bitmap>]
// where:
//   <bitmap> -- (optional) name of BMP file

#include <iostream>
#include <algorithm>
#include <vector>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Affine.h"
#include "SquareMesh.h"
#include "Camera.h"
using namespace std;


const glm::vec4 O = cs200::point(0,0),
                EX = cs200::vector(1,0),
                EY = cs200::vector(0,1);


/////////////////////////////////////////////////////////////////
class Client {
  public:
    Client(const char *fname=nullptr);
    ~Client(void);
    void draw(double dt);
    void keypress(SDL_Keycode kc);
    void resize(int W, int H);
  private:
    GLuint program,
           texture_buffer,
           vertex_buffer,
           face_buffer,
           vao;
    GLint utransform_matrix,
          utexture_matrix;
    int mode;
    glm::mat4 view_matrix,
              model_matrix[5],
              texture_matrix;
};


Client::Client(const char *fname) {
  GLint value;

  // compile/link shader program
  const char *fragment_shader_text =
    "#version 130\n\
     uniform sampler2D usamp;\
     in vec2 vtexcoord;\
     out vec4 frag_color;\
     void main(void) {\
       frag_color = texture(usamp,vtexcoord);\
     }";
  GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fshader,1,&fragment_shader_text,0);
  glCompileShader(fshader);
  glGetShaderiv(fshader,GL_COMPILE_STATUS,&value);
  if (!value) {
    char buffer[1024];
    glGetShaderInfoLog(fshader,1024,0,buffer);
    cerr << buffer << endl;
  }

  const char *vertex_shader_text =
    "#version 130\n\
     in vec4 position;\
     uniform mat4 transform_matrix;\
     uniform mat4 texture_matrix;\
     out vec2 vtexcoord;\
     void main() {\
       gl_Position = transform_matrix * position;\
       vtexcoord = (texture_matrix * position).xy;\
     }";
  GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vshader,1,&vertex_shader_text,0);
  glCompileShader(vshader);
  glGetShaderiv(vshader,GL_COMPILE_STATUS,&value);
  if (!value) {
    char buffer[1024];
    glGetShaderInfoLog(vshader,1024,0,buffer);
    cerr << buffer << endl;
  }

  program = glCreateProgram();
  glAttachShader(program,fshader);
  glAttachShader(program,vshader);
  glLinkProgram(program);
  glGetProgramiv(program,GL_LINK_STATUS,&value);
  if (!value) {
    cerr << "shader program failed to link" << endl;
    char buffer[1024];
    glGetProgramInfoLog(program,1024,0,buffer);
    cerr << buffer << endl;
  }
  glDeleteShader(fshader);
  glDeleteShader(vshader);

  // get texture
  cs200::Bitmap *texture = nullptr;
  if (fname != nullptr) {
    try {
      texture = new cs200::Bitmap(fname);
      cs200::reverseRGB(*texture);
    }
    catch (exception &e) {
      cout << "failed to open '" << fname << "'" << endl;
    }
  }
  if (texture == nullptr)
    texture = new cs200::Bitmap(300,300);

  glGenTextures(1,&texture_buffer);

  // GPU buffer for texture
  glBindTexture(GL_TEXTURE_2D,texture_buffer);
  glPixelStorei(GL_UNPACK_ALIGNMENT,4);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture->width(),texture->height(),
               0,GL_RGB,GL_UNSIGNED_BYTE,texture->data());
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

  // GPU buffers for mesh
  cs200::SquareMesh square;
  glGenBuffers(1,&vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glm::vec4)*square.vertexCount(),
               square.vertexArray(),GL_STATIC_DRAW);

  glGenBuffers(1,&face_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,face_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(cs200::Mesh::Face)*square.faceCount(),
               square.faceArray(),GL_STATIC_DRAW);

  // VAO
  GLint aposition = glGetAttribLocation(program,"position");
  glGenVertexArrays(1,&vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
  glVertexAttribPointer(aposition,4,GL_FLOAT,false,0,0);
  glEnableVertexAttribArray(aposition);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,face_buffer);
  glBindVertexArray(0);

  // shader uniform locations
  utransform_matrix = glGetUniformLocation(program,"transform_matrix");
  utexture_matrix = glGetUniformLocation(program,"texture_matrix");

  mode = 0;
  resize(600,600);  // force intialization of view_matrix
  float aspect = float(texture->width())/float(texture->height());
  model_matrix[0] = cs200::scale(1.5f,1.5f/aspect);
  model_matrix[1] = cs200::translate( 2.0f*EX + 2.0f*EY) * model_matrix[0];
  model_matrix[2] = cs200::translate(-2.0f*EX + 2.0f*EY) * model_matrix[0];
  model_matrix[3] = cs200::translate(-2.0f*EX - 2.0f*EY) * model_matrix[0];
  model_matrix[4] = cs200::translate( 2.0f*EX - 2.0f*EY) * model_matrix[0];
  texture_matrix = cs200::scale(0.5f,0.5f)
                   * cs200::translate(EX+EY);
 
  delete texture;
}


Client::~Client(void) {
  glDeleteBuffers(1,&face_buffer);
  glDeleteBuffers(1,&vertex_buffer);
  glDeleteTextures(1,&texture_buffer);
  glDeleteVertexArrays(1,&vao);
  glUseProgram(0);
  glDeleteProgram(program);
}


void Client::draw(double dt) {
  // clear frame buffer
  glClearColor(0.5f,0.4f,0.5f,1);
  glClear(GL_COLOR_BUFFER_BIT);

  // update model & texture transforms
  const float MODEL_RATE = 360.0f/10,
              TEXTURE_RATE = 360.0f/7.0f;
  if (mode%2 == 1) {
    glm::mat4 U = cs200::rotate(MODEL_RATE*dt);
    for (int i=0; i < 5; ++i)
      model_matrix[i] = U * model_matrix[i];
  }
  if (mode > 1) {
    glm::mat4 U = cs200::translate(0.5f*EX+0.5f*EY)
                  * cs200::rotate(TEXTURE_RATE*dt)
                  * cs200::translate(-0.5f*EX-0.5f*EY);
    texture_matrix = U * texture_matrix;
  }

  // draw
  glUseProgram(program);
  glBindTexture(GL_TEXTURE_2D,texture_buffer);
  glUniformMatrix4fv(utexture_matrix,1,false,&texture_matrix[0][0]);
  glBindVertexArray(vao);
  for (int i=0; i < 5; ++i) {
    glm::mat4 M = view_matrix * model_matrix[i];
    glUniformMatrix4fv(utransform_matrix,1,false,&M[0][0]);
    glDrawElements(GL_TRIANGLES,2*3,GL_UNSIGNED_INT,0);
  }
  glBindVertexArray(0);
}


void Client::keypress(SDL_Keycode kc) {
  mode = (mode + 1) % 4;
}


void Client::resize(int W, int H) {
  glViewport(0,0,W,H);
  float aspect = float(W)/float(H);
  cs200::Camera cam(O,EY,10*aspect,10);
  view_matrix = cs200::cameraToNDC(cam)
                * cs200::worldToCamera(cam);
}


/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

  // SDL: initialize and create a window
  SDL_Init(SDL_INIT_VIDEO);
  const char *title = "CS 200: Texture Demo";
  int width = 600,
      height = 600;
  SDL_Window *window = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,width,height,
                                        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  // GLEW: get function bindings (if possible)
  glewInit();
  if (!GLEW_VERSION_2_0) {
    cout << "needs OpenGL version 3.0 or better" << endl;
    return -1;
  }

  // animation loop
  try {
    bool done = false;
    Client *client = (argc == 2) ? new Client(argv[1]) : new Client();
    Uint32 ticks_last = SDL_GetTicks();
    while (!done) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        switch (event.type) {
          case SDL_QUIT:
            done = true;
            break;
          case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
              done = true;
            else
              client->keypress(event.key.keysym.sym);
            break;
          case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED)
              client->resize(event.window.data1,event.window.data2);
            break;
        }
      }
      Uint32 ticks = SDL_GetTicks();
      double dt = 0.001*(ticks - ticks_last);
      ticks_last = ticks;
      client->draw(dt);
      SDL_GL_SwapWindow(window);
    }
    delete client;
  }

  catch (exception &e) {
    cout << e.what() << endl;
  }

  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}

