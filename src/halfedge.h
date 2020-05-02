#pragma once
#include "glm/glm.hpp"
#include "face.h"
#include "vertex.h"
#include "QListWidget"



class Face;
class Vertex;

static int id_halfEdge;

class HalfEdge : public QListWidgetItem
{
public:

    int id;
    HalfEdge *next;
    HalfEdge *sym;

    Face *face;
    Vertex *vertex;
    QString name;

    void setFace(Face *f);
    void setVertex(Vertex *v);
    HalfEdge();
};
