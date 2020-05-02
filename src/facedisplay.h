#pragma once
#include "drawable.h"
#include "face.h"

class FaceDisplay : public Drawable
{
protected:
    Face *representedFace;
public:
    // Creates VBO data to make a visual
    // representation of the currently selected Vertex
    void create() override;
    GLenum drawMode() override;

    /// Change which Vertex representedVertex points to
    void updateFace(Face*);

    FaceDisplay(OpenGLContext* context);
    ~FaceDisplay();
};

