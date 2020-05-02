#include "mesh.h"
#include "iostream"


Mesh::Mesh(OpenGLContext* context)
    : Drawable(context), face{}, halfEdge{}, vertex{}, isInitialize(true)
{}

Mesh::~Mesh()
{}

void Mesh::create()
{
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> nor;
    std::vector<glm::vec4> color;
    std::vector<GLuint> indices;

    int i = 0;
    for (uPtr<Face> &f : face) {
        // Create 4 positions per face
        HalfEdge *startEdge = f->halfEdge;
        HalfEdge *currEdge = startEdge;
        int numSides = 0;
        do {
            // position
            pos.push_back(glm::vec4(currEdge->vertex->position, 1));

            // calculate normal
            HalfEdge* temp = currEdge;

            glm::vec3 v1 = temp->next->vertex->position - temp->vertex->position;
            glm::vec3 v2 = temp->next->next->vertex->position - temp->next->vertex->position;

            // in case of half edge
            while (glm::length(glm::cross(v1, v2)) < eps) {
                temp = temp->next;
                v1 = temp->next->vertex->position - temp->vertex->position;
                v2 = temp->next->next->vertex->position - temp->next->vertex->position;
            }
            glm::vec3 normal = glm::normalize(glm::cross(v1,v2));

            nor.push_back(glm::vec4(normal, 0));

            // colors
            if (isInitialize) {
                f->color = glm::normalize(
                                glm::abs(glm::vec3(normal.x,
                                                   normal.y,
                                                   normal.z)));
            }

            // colors
            color.push_back(glm::vec4(f->color, 1));

            currEdge = currEdge->next;
            numSides++;
        } while(currEdge != startEdge);


        // Create 3 * (numsides - 2) indices (numSides) per face
        for (int j = 0; j < numSides - 2; j++) {
            indices.push_back(i);
            indices.push_back(i + j + 1);
            indices.push_back(i + j + 2);
        }
        i += numSides;
    }

    count = indices.size();

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    bindNor();
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);
}

GLenum Mesh::drawMode()
{
    return GL_TRIANGLES;
}

void Mesh::buildCube()
{
    /// INITIALIZE
    initCube();
    /// SET DATA ACCORDINGLY
    setCubeData();
}

void Mesh::initCube()
{
    face.clear();
    vertex.clear();
    halfEdge.clear();

    // FACE
    for (int i = 0; i < 6; i++) {
        face.push_back(mkU<Face>());
    }

    // VERTEX
    for (int i = 0; i < 8; i++) {
        vertex.push_back(mkU<Vertex>());
    }

    // HALF-EDGE
    for (int i = 0; i < 24; i++) {
        halfEdge.push_back(mkU<HalfEdge>());
    }
}

void Mesh::setCubeData()
{
    /// FACE
    for (int i = 0; i < 6; i++) {
        face.at(i)->halfEdge = halfEdge.at(4 * i).get();
    }

    /// VERTEX
    // half-edge
    for (int i = 0; i < 8; i++) {
        vertex.at(i)->halfEdge = halfEdge.at(i).get();
    }
    // position
    vertex.at(0)->position = glm::vec3(-0.5f, 0.5f, 0.5f);
    vertex.at(1)->position = glm::vec3(-0.5f, -0.5f, 0.5f);
    vertex.at(2)->position = glm::vec3(0.5f, -0.5f, 0.5f);
    vertex.at(3)->position = glm::vec3(0.5f, 0.5f, 0.5f);
    vertex.at(4)->position = glm::vec3(0.5f, 0.5f, -0.5f);
    vertex.at(5)->position = glm::vec3(0.5f, -0.5f, -0.5f);
    vertex.at(6)->position = glm::vec3(-0.5f, -0.5f, -0.5f);
    vertex.at(7)->position = glm::vec3(-0.5f, 0.5f, -0.5f);

    /// HALF-EDGE
    // next
    for (int i = 0; i < 24 ; i++) {
        if (i % 4 == 3) {
            halfEdge.at(i)->next = halfEdge.at(i - 3).get();
        } else {
            halfEdge.at(i)->next = halfEdge.at(i + 1).get();
        }
    }
    // sym
    halfEdge.at(0)->sym = halfEdge.at(18).get();
    halfEdge.at(1)->sym = halfEdge.at(15).get();
    halfEdge.at(2)->sym = halfEdge.at(22).get();
    halfEdge.at(3)->sym = halfEdge.at(9).get();
    halfEdge.at(4)->sym = halfEdge.at(16).get();
    halfEdge.at(5)->sym = halfEdge.at(11).get();
    halfEdge.at(6)->sym = halfEdge.at(20).get();
    halfEdge.at(7)->sym = halfEdge.at(13).get();
    halfEdge.at(17)->sym = halfEdge.at(12).get();
    halfEdge.at(19)->sym = halfEdge.at(8).get();
    halfEdge.at(10)->sym = halfEdge.at(21).get();
    halfEdge.at(14)->sym = halfEdge.at(23).get();

    halfEdge.at(18)->sym = halfEdge.at(0).get();
    halfEdge.at(15)->sym = halfEdge.at(1).get();
    halfEdge.at(22)->sym = halfEdge.at(2).get();
    halfEdge.at(9)->sym = halfEdge.at(3).get();
    halfEdge.at(16)->sym = halfEdge.at(4).get();
    halfEdge.at(11)->sym = halfEdge.at(5).get();
    halfEdge.at(20)->sym = halfEdge.at(6).get();
    halfEdge.at(13)->sym = halfEdge.at(7).get();
    halfEdge.at(12)->sym = halfEdge.at(17).get();
    halfEdge.at(8)->sym = halfEdge.at(19).get();
    halfEdge.at(21)->sym = halfEdge.at(10).get();
    halfEdge.at(23)->sym = halfEdge.at(14).get();

    // face
    for (int i = 0; i < 24; i++) {
        halfEdge.at(i)->face = face.at(i / 4).get();
    }

    // vertex
    halfEdge.at(0)->vertex = vertex.at(0).get();
    halfEdge.at(15)->vertex = vertex.at(0).get();
    halfEdge.at(17)->vertex = vertex.at(0).get();

    halfEdge.at(1)->vertex = vertex.at(1).get();
    halfEdge.at(14)->vertex = vertex.at(1).get();
    halfEdge.at(22)->vertex = vertex.at(1).get();

    halfEdge.at(2)->vertex = vertex.at(2).get();
    halfEdge.at(9)->vertex = vertex.at(2).get();
    halfEdge.at(21)->vertex = vertex.at(2).get();

    halfEdge.at(3)->vertex = vertex.at(3).get();
    halfEdge.at(8)->vertex = vertex.at(3).get();
    halfEdge.at(18)->vertex = vertex.at(3).get();

    halfEdge.at(4)->vertex = vertex.at(4).get();
    halfEdge.at(11)->vertex = vertex.at(4).get();
    halfEdge.at(19)->vertex = vertex.at(4).get();

    halfEdge.at(5)->vertex = vertex.at(5).get();
    halfEdge.at(10)->vertex = vertex.at(5).get();
    halfEdge.at(20)->vertex = vertex.at(5).get();

    halfEdge.at(6)->vertex = vertex.at(6).get();
    halfEdge.at(13)->vertex = vertex.at(6).get();
    halfEdge.at(23)->vertex = vertex.at(6).get();

    halfEdge.at(7)->vertex = vertex.at(7).get();
    halfEdge.at(12)->vertex = vertex.at(7).get();
    halfEdge.at(16)->vertex = vertex.at(7).get();
}

