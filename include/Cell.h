#include "Entity.h"
#include "Vertex.h"
#include <vector>

struct Cell {
  std::vector<Entity *> entities;
  std::vector<Vertex *> vertices;
};
