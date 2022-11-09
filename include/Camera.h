// Camera.h
// cs200 1/19

#ifndef CS200_CAMERA_H
#define CS200_CAMERA_H

#include "Affine.h"


namespace cs200 {


  glm::mat4 affineInverse(const glm::mat4 &A);


  class Camera {
    public:
      Camera(void);
      Camera(const glm::vec4 &C, const glm::vec4 &v, float W, float H);
      glm::vec4 center(void) const             { return center_point; }
      glm::vec4 right(void) const              { return right_vector; }
      glm::vec4 up(void) const                 { return up_vector; }
      float width(void) const                  { return rect_width; }
      float height(void) const                 { return rect_height; }
      Camera& moveRight(float x);
      Camera& moveUp(float y);
      Camera& rotate(float t);
      Camera& zoom(float f);
    private:
      glm::vec4 center_point;
      glm::vec4 right_vector, up_vector;
      float rect_width, rect_height;
  };


  glm::mat4 cameraToWorld(const Camera& cam);
  glm::mat4 worldToCamera(const Camera& cam);
  glm::mat4 cameraToNDC(const Camera& cam);
  glm::mat4 NDCToCamera(const Camera& cam);


}


#endif

