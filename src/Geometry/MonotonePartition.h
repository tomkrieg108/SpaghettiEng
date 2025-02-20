#pragma once
#include "Polygon.h"
#include "Line.h"

namespace Geom
{
  //Todo Trapezpoidal partition->Monotone->Triangles (Seidel algorithm)
  //https://stackoverflow.com/questions/5247994/simple-2d-polygon-triangulation

  enum class VertexCategory {
    Start, End, Regular, Split, Merge, Invalid
  };

  VertexCategory CategorizeVertex(DCEL::Vertex* vertex);

  //diagonals is an output arg - the list of diagonals to partition the poly into monotone pieces
  std::vector<DCEL::Polygon*> GetMonotonPolygons(DCEL::Polygon* poly, std::vector<LineSeg2D>& diagonals);
}