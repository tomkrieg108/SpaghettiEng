add_library(${LIB_GEOM} STATIC 
  "./Geometry.cpp"
  "./Geometry.h"
  "./GeomBase.h"
  "./GeomUtils.h"
  "./GeomUtils.cpp"
  "./Line.h"
  "./Polygon.h"
  "./Polygon.cpp"
  "./MeshPrimitives2D.cpp"
  "./MeshPrimitives2D.h"
  "./ConvexHull.h"
  "./ConvexHull.cpp"
  "./Triangulate.cpp"
  "./Triangulate.h"
  "./MonotonePartition.cpp"
  "./MonotonePartition.h"
  "./BST.cpp"
  "./BST.h"
  "./KDTree.cpp"
  "./KDTree.h"
  "./RangeTree.cpp"
  "./RangeTree.h"
  "./BoundingVolume.h"
  "./QuadTree.cpp"
  "./QuadTree.h"
)

target_include_directories(${LIB_GEOM} PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${LIB_GEOM} PUBLIC ${LIB_COMMON} )
target_link_libraries(${LIB_GEOM} PUBLIC glm::glm-header-only) 