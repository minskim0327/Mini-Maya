#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include "mesh.h"
#include "QListWidget"
#include "vertexdisplay.h"
#include "facedisplay.h"
#include "halfedgedisplay.h"
#include "iostream"
#include <iterator>
#include <map>

class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)

    Mesh m_mesh;
    VertexDisplay m_vertexDisplay;
    FaceDisplay m_faceDisplay;
    HalfEdgeDisplay m_halfEdgeDisplay;


    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    bool isHighLight;
    Camera m_glCamera;

    QListWidgetItem* mp_selectedItem;
    Vertex *mp_vertex;
    Face *mp_face;
    HalfEdge *mp_halfEdge;


    glm::vec3 spinBoxColor;

public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void computeCentroid(std::map<Face*, Vertex*> *, std::vector<uPtr<Vertex>>*);
    void computeMidpoints(std::map<Face*, Vertex*> *,
                          std::map<HalfEdge*, Vertex*> *,
                          std::vector<HalfEdge*> *,
                          std::vector<uPtr<Vertex>> *);
    void smoothOriginal(std::map<Face*, Vertex*> *, std::map<HalfEdge*, Vertex*> *);
    void splitHalfEdge(HalfEdge *, Vertex*, std::vector<uPtr<HalfEdge>> *);

    void initFullEdge(std::vector<HalfEdge*> *);

    void createTempHalfEdge(Face*, std::vector<HalfEdge*> *);
    void mapPrevOfNewHalfEdge(std::map<HalfEdge*, HalfEdge*> *, std::vector<HalfEdge*> *);
    void extrudeSingleEdge(HalfEdge*,
                           std::vector<HalfEdge*>*, std::vector<HalfEdge*>*,
                           Vertex*, Vertex*);
protected:
    void keyPressEvent(QKeyEvent *e);

public slots:
    void slot_vertX(double x);
    void slot_vertY(double y);
    void slot_vertZ(double z);
    void slot_colorRed(double x);
    void slot_colorGreen(double y);
    void slot_colorBlue(double z);
    void slot_setSelectedItem(QListWidgetItem *i);
    void slot_splitHalfEdge();
    void slot_triangulateFace();
    void slot_subDivideMesh();
    void slot_extrudeFace();
    void slot_importOBJ();

signals:
    void sig_sendMeshData(Mesh*);
};


#endif // MYGL_H
