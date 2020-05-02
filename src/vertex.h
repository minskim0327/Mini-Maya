#pragma once
#include "glm/glm.hpp"
#include "halfedge.h"
#include "QListWidget"


class HalfEdge;

static int id_Vertex;
class Vertex : public QListWidgetItem
{
public:
    int id;
    glm::vec3 position;
    HalfEdge *halfEdge;
    QString name;
    Vertex();
};

