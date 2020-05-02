#include "vertex.h"

Vertex::Vertex()
    : QListWidgetItem(), id(id_Vertex++), position(glm::vec3()),
      halfEdge(nullptr),
      name("v")
{
    name.append(QString::number(id));
    this->setText(name);
}
