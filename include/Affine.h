// Affine.h
// -- 2D affine transformation package (uses core glm only)
// cs200 1/19

#ifndef CS200_AFFINE_H
#define CS200_AFFINE_H

#include <cmath>
#include <glm/glm.hpp>


namespace cs200 {

  inline bool near(float x, float y) { return std::abs(x-y)<1e-4f; }

  bool isPoint(const glm::vec4 &P);
  bool isVector(const glm::vec4 &v);
  bool isAffine(const glm::mat4 &A);

  glm::vec4 point(float x, float y);
  glm::vec4 vector(float x, float y);

  glm::mat4 affine(const glm::vec4 &u, const glm::vec4 &v, const glm::vec4 &C);
  glm::mat4 rotate(float t);
  glm::mat4 translate(const glm::vec4 &v);
  glm::mat4 scale(float r);
  glm::mat4 scale(float rx, float ry);
}


#endif

