// SquareMesh.cpp
// -- non-inlines of SquareMesh class
// cs200 1/19

#include "SquareMesh.h"


const glm::vec4 cs200::SquareMesh::vertices[4]
  = { cs200::point(-1,-1),  // 0
      cs200::point(1,-1),   // 1
      cs200::point(1,1),    // 2
      cs200::point(-1,1)    // 3
    };


const cs200::Mesh::Face cs200::SquareMesh::faces[2]
  = { Face(0,1,2),  Face(0,2,3) };


const cs200::Mesh::Edge cs200::SquareMesh::edges[4]
  = { Edge(0,1),  Edge(1,2),  Edge(2,3),  Edge(3,0) };

