#pragma once
#include "glm/glm.hpp"
#include "halfedge.h"
#include "QListWidget"



class HalfEdge;

static int id_Face;

class Face : public QListWidgetItem
{
public:
    int id;
    HalfEdge *halfEdge;
    glm::vec3 color;

    QString name;

    Face();
};

