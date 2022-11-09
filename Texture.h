// Texture.h
// -- bitmaps and texture coordinates
// cs200 10/20

#ifndef CS200_TEXTURE_H
#define CS200_TEXTURE_H

#include <vector>
#include <glm/glm.hpp>


namespace cs200 {

  class Bitmap {
    public:
      explicit Bitmap(unsigned W=64, unsigned H=64);
      Bitmap(const char *bmp_file);
      unsigned char* data(void)             { return &bmp_data[0]; }
      const unsigned char* data(void) const { return &bmp_data[0]; }
      unsigned width(void) const            { return bmp_width; }
      unsigned height(void) const           { return bmp_height; }
      unsigned stride(void) const           { return bmp_stride; }
      unsigned offset(int i, int j) const;
    private:
      std::vector<unsigned char> bmp_data;
      unsigned bmp_width, bmp_height, bmp_stride;
  };

  unsigned computeStride(unsigned W);
  void reverseRGB(Bitmap &b);
  glm::mat4 bitmapToTextureTransform(const Bitmap &b);
  glm::mat4 textureToBitmapTransform(const Bitmap &b);
  glm::vec3 getColor(const Bitmap &b, float u, float v);

}


#endif

