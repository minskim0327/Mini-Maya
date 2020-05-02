#include "vertexdisplay.h"

VertexDisplay::VertexDisplay(OpenGLContext* context)
    : Drawable(context), representedVertex(nullptr)
{}

VertexDisplay::~VertexDisplay()
{}

void VertexDisplay::create()
{
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;
    std::vector<GLuint> indices;

    pos.push_back(glm::vec4(representedVertex->position, 1));
    col.push_back(glm::vec4(1, 1, 1, 1));
    indices.push_back(0);


    count = indices.size();

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

GLenum VertexDisplay::drawMode()
{
    return GL_POINTS;
}

void VertexDisplay::updateVertex(Vertex * v) {
    representedVertex = v;
}
