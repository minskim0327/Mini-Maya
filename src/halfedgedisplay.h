#pragma once
#include "drawable.h"
#include "halfedge.h"


class HalfEdgeDisplay : public Drawable
{
protected:
    HalfEdge *representedHalfEdge;
public:
    // Creates VBO data to make a visual
    // representation of the currently selected Vertex
    void create() override;
    GLenum drawMode() override;

    // Change which Vertex representedVertex points to
    void updateHalfEdge(HalfEdge*);

    HalfEdgeDisplay(OpenGLContext* context);
    ~HalfEdgeDisplay();
};

