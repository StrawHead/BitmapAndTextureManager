// SquareMesh.h
// -- standard square mesh
// cs200 1/19

#ifndef CS200_SQUAREMESH_H
#define CS200_SQUAREMESH_H

#include "Affine.h"
#include "Mesh.h"


namespace cs200 {


  class SquareMesh : public Mesh {
    public:
      int vertexCount(void) const;
      const glm::vec4* vertexArray(void) const;
      glm::vec4 dimensions(void) const;
      glm::vec4 center(void) const;
      int faceCount(void) const;
      const Face* faceArray(void) const;
      int edgeCount(void) const;
      const Edge* edgeArray(void) const;
    private:
      static const glm::vec4 vertices[4];
      static const Face faces[2];
      static const Edge edges[4];
  };


}

inline
int cs200::SquareMesh::vertexCount(void) const {
  return 4;
}


inline
const glm::vec4* cs200::SquareMesh::vertexArray(void) const {
  return vertices;
}


inline
glm::vec4 cs200::SquareMesh::dimensions(void) const {
  return cs200::vector(2,2);
}


inline
glm::vec4 cs200::SquareMesh::center(void) const {
  return cs200::point(0,0);
}


inline
int cs200::SquareMesh::faceCount(void) const {
  return 2;
}


inline
const cs200::Mesh::Face* cs200::SquareMesh::faceArray(void) const {
  return faces;
}


inline
int cs200::SquareMesh::edgeCount(void) const {
  return 4;
}


inline
const cs200::Mesh::Edge* cs200::SquareMesh::edgeArray(void) const {
  return edges;
}


#endif

