#pragma once
#include "smartpointerhelp.h"
#include "face.h"
#include "vertex.h"
#include "halfedge.h"
#include "vector"
#include "drawable.h"

#define eps FLT_EPSILON

class Mesh : public Drawable
{
public:
    std::vector<uPtr<Face>> face;
    std::vector<uPtr<HalfEdge>> halfEdge;
    std::vector<uPtr<Vertex>> vertex;

    void create() override; // Populates the VBOs of the Drawable.
    GLenum drawMode() override;

    void buildCube();       // Builds a cube-shaped mesh using the half-edge structured
    void initCube();
    void setCubeData();

    bool isInitialize;

    Mesh(OpenGLContext* context);
    ~Mesh();
};

