#include "face.h"

Face::Face()
    : QListWidgetItem(), id(id_Face++), halfEdge(nullptr),
      color(glm::vec3(0, 0, 0)),
      name("f")
{
    name.append(QString::number(id));
    this->setText(name);
}
