// Mesh.h
// -- 2D triangular mesh interface
// cs200 1/19

#ifndef CS200_MESH_H
#define CS200_MESH_H

#include <glm/glm.hpp>


namespace cs200 {


  struct Mesh {

    struct Face {
      unsigned index1, index2, index3;
      Face(int i=0, int j=0, int k=0)
        : index1(i), index2(j), index3(k) {}
    };

    struct Edge {
      unsigned index1, index2;
      Edge(int i=0, int j=0)
        : index1(i), index2(j) {}
    };


    virtual ~Mesh(void) {}
    virtual int vertexCount(void) const = 0;
    virtual const glm::vec4* vertexArray(void) const = 0;
    virtual glm::vec4 dimensions(void) const = 0;
    virtual glm::vec4 center(void) const = 0;
    virtual int faceCount(void) const = 0;
    virtual const Face* faceArray(void) const = 0;
    virtual int edgeCount(void) const = 0;
    virtual const Edge* edgeArray(void) const = 0;

  };


}


#endif

