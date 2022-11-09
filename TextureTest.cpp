// TextureTest.cpp
// -- test of texture stuff
// cs200 10/20
//
// usage:
//   TextureTest [<file>]
// where:
//   <file> -- (optional) name of BMP file (uncompressed 24 bit color)
// output:
//   'TextureTest.bmp'

#include <iostream>
#include <fstream>
#include <cstring>
#include "Texture.h"
#include "Affine.h"
using namespace std;



int main(int argc, char *argv[]) {
  if (argc != 1 && argc != 2)
    return 0;

  cs200::Bitmap *texture_in = nullptr;
  if (argc == 2) {
    try {
      texture_in = new cs200::Bitmap(argv[1]);
    }
    catch (...) {
      cout << "failed to load '" << argv[1] << "'" << endl;
      cout << "using default texture" << endl;
    }
  }
  if (texture_in == nullptr)
    texture_in = new cs200::Bitmap(128,128);

  cs200::Bitmap texture_out(500,500);
  float aspect = float(texture_in->width())/float(texture_in->height());
  glm::mat4 out_to_texture = cs200::translate(cs200::vector(-0.5f,-0.5f))
                             * cs200::scale(5.0f,5.0f*aspect)
                             * cs200::rotate(-20.0f)
                             * cs200::bitmapToTextureTransform(texture_out);
  for (int j=0; j < texture_out.height(); ++j) {
    for (int i=0; i < texture_out.width(); ++i) {
      glm::vec4 uv = out_to_texture * cs200::point(i,j);
      glm::vec3 color = cs200::getColor(*texture_in,uv[0],uv[1]);
      int index = texture_out.offset(i,j);
      for (int k=0; k < 3; ++k)
        texture_out.data()[index+k] = color[k];
    }
  }

  fstream out("TextureTest.bmp",ios_base::out|ios_base::binary);
  char header[54];
  memset(header,0,54);
  strncpy(header,"BM",2);
  unsigned size = texture_out.height() * texture_out.stride();
  *reinterpret_cast<unsigned*>(header+2) = 54u + size;
  *reinterpret_cast<unsigned*>(header+10) = 54;
  *reinterpret_cast<unsigned*>(header+14) = 40;
  *reinterpret_cast<int*>(header+18) = texture_out.width();
  *reinterpret_cast<int*>(header+22) = texture_out.height();
  *reinterpret_cast<unsigned short*>(header+26) = 1;
  *reinterpret_cast<unsigned short*>(header+28) = 24;
  *reinterpret_cast<unsigned*>(header+34) = size;
  out.write(header,54);
  out.write(reinterpret_cast<char*>(texture_out.data()),size);

  delete texture_in;
  return 0;
}

