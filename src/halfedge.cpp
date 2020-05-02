#include "halfedge.h"

HalfEdge::HalfEdge()
    : QListWidgetItem(), id(id_halfEdge++),
      next(nullptr), sym(nullptr), face(nullptr), vertex(nullptr),
      name("h")
{
    name.append(QString::number(id));
    this->setText(name);
}

void HalfEdge::setFace(Face *f) {
    this->face = f;
    f->halfEdge = this;
}

void HalfEdge::setVertex(Vertex *v) {
    this->vertex = v;
    v->halfEdge = this;
}
