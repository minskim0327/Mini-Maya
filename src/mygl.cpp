#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QFileDialog>

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this),
      m_progLambert(this), m_progFlat(this), m_mesh(this),
      m_vertexDisplay(this), m_faceDisplay(this), m_halfEdgeDisplay(this),
      isHighLight(false),
      m_glCamera(),
      mp_selectedItem(nullptr), mp_vertex(nullptr),mp_face(nullptr), mp_halfEdge(nullptr),
      spinBoxColor(glm::vec3())
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
//    m_geomSquare.create();
    m_mesh.buildCube();
    m_mesh.create();
    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

//    emit sig_sendMeshData(m_mesh.createMeshData());
    emit sig_sendMeshData(&m_mesh);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setCamPos(glm::vec3(m_glCamera.eye));
    m_progFlat.setModelMatrix(glm::mat4(1.f));


    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
//    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
//    //Send the geometry's transformation matrix to the shader
//    m_progLambert.setModelMatrix(model);
//    //Draw the example sphere using our lambert shader
//    m_progLambert.draw(m_geomSquare);
//    //Now do the same to render the cylinder
//    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
//    model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
//    m_progLambert.setModelMatrix(model);
//    m_progLambert.draw(m_geomSquare);

    glm::mat4 model = glm::mat4();
    m_progLambert.setModelMatrix(model);
    m_progLambert.draw(m_mesh);

    if (isHighLight) {
        glDisable(GL_DEPTH_TEST);
        m_progFlat.setModelMatrix(model);

        Vertex* vertex = dynamic_cast<Vertex*>(mp_selectedItem);
        Face* face = dynamic_cast<Face*>(mp_selectedItem);
        HalfEdge* halfEdge = dynamic_cast<HalfEdge*>(mp_selectedItem);

        if (vertex) {
            m_vertexDisplay.create();
            m_progFlat.draw(m_vertexDisplay);
        } else if (face) {
            m_faceDisplay.create();
            m_progFlat.draw(m_faceDisplay);
        } else if (halfEdge) {
            m_halfEdgeDisplay.create();
            m_progFlat.draw(m_halfEdgeDisplay);
        }
        glEnable(GL_DEPTH_TEST);
    }
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    } else if (e->key() == Qt::Key_N) {
        HalfEdge* halfEdge = dynamic_cast<HalfEdge*>(mp_selectedItem);
        if (halfEdge != nullptr) {
            slot_setSelectedItem(halfEdge->next);
        }
    } else if (e->key() == Qt::Key_M) {
        HalfEdge* halfEdge = dynamic_cast<HalfEdge*>(mp_selectedItem);
        if (halfEdge != nullptr) {
            slot_setSelectedItem(halfEdge->sym);
        }
    } else if (e->key() == Qt::Key_F) {
        HalfEdge* halfEdge = dynamic_cast<HalfEdge*>(mp_selectedItem);
        if (halfEdge != nullptr) {
            slot_setSelectedItem(halfEdge->face);
        }
    } else if (e->key() == Qt::Key_V) {
        HalfEdge* halfEdge = dynamic_cast<HalfEdge*>(mp_selectedItem);
        if (halfEdge != nullptr) {
            slot_setSelectedItem(halfEdge->vertex);
        }
    } else if ((e->modifiers() & Qt::ShiftModifier) && e->key() == Qt::Key_H) {
        Face* face = dynamic_cast<Face*>(mp_selectedItem);
        if (face != nullptr) {
            slot_setSelectedItem(face->halfEdge);
        }
    } else if (e->key() == Qt::Key_H) {
        Vertex* vertex = dynamic_cast<Vertex*>(mp_selectedItem);
        if (vertex != nullptr) {
            slot_setSelectedItem(vertex->halfEdge);
        }
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::slot_vertX(double x) {
    if (mp_selectedItem) {
        mp_vertex = dynamic_cast<Vertex*>(mp_selectedItem);
    } else {
        return;
    }

    if (mp_vertex) {
        mp_vertex->position[0] = x;
        m_mesh.destroy();
        m_mesh.create();
        update();
    } else {
        return;
    }
}
void MyGL::slot_vertY(double y) {
    if (mp_selectedItem) {
        mp_vertex = dynamic_cast<Vertex*>(mp_selectedItem);
    } else {
        return;
    }

    if (mp_vertex) {
        mp_vertex->position[1] = y;
        m_mesh.destroy();
        m_mesh.create();
        update();
    } else {
        return;
    }
}
void MyGL::slot_vertZ(double z) {
    if (mp_selectedItem) {
        mp_vertex = dynamic_cast<Vertex*>(mp_selectedItem);
    } else {
        return;
    }

    if (mp_vertex) {
        mp_vertex->position[2] = z;
        m_mesh.destroy();
        m_mesh.create();
        update();
    } else {
        return;
    }
}

void MyGL::slot_colorRed(double red) {
    if (mp_selectedItem) {
        mp_face = dynamic_cast<Face*>(mp_selectedItem);
    } else {
        return;
    }

    if (mp_face) {
        spinBoxColor[0] = red;
        mp_face->color = spinBoxColor;

        m_mesh.destroy();
        m_mesh.create();
        update();
    } else {
        return;
    }
}

void MyGL::slot_colorGreen(double green) {
    if (mp_selectedItem) {
        mp_face = dynamic_cast<Face*>(mp_selectedItem);
    } else {
        return;
    }

    if (mp_face) {
        spinBoxColor[1] = green;
        mp_face->color = spinBoxColor;

        m_mesh.destroy();
        m_mesh.create();
        update();
    } else {
        return;
    }
}

void MyGL::slot_colorBlue(double blue) {
    if (mp_selectedItem) {
        mp_face = dynamic_cast<Face*>(mp_selectedItem);
    } else {
        return;
    }

    if (mp_face) {
        spinBoxColor[2] = blue;
        mp_face->color = spinBoxColor;

        m_mesh.destroy();
        m_mesh.create();
        update();
    } else {
        return;
    }
}

void MyGL::slot_setSelectedItem(QListWidgetItem *i) {
    if (i == nullptr) {
        isHighLight = false;
        return;
    }
    mp_selectedItem = i;
    m_mesh.isInitialize = false;
    Vertex* vertex = dynamic_cast<Vertex*>(i);
    Face* face = dynamic_cast<Face*>(i);
    HalfEdge* halfEdge = dynamic_cast<HalfEdge*>(i);

    m_vertexDisplay.destroy();
    m_faceDisplay.destroy();
    m_halfEdgeDisplay.destroy();

    if (vertex) {
        m_vertexDisplay.updateVertex(vertex);
    } else if (face) {
        m_faceDisplay.updateFace(face);
    } else if (halfEdge) {
        m_halfEdgeDisplay.updateHalfEdge(halfEdge);
    }

    isHighLight = true;
    update();
}

void MyGL::slot_splitHalfEdge() {
    if (mp_selectedItem == nullptr) {
        return;
    }
    HalfEdge* he1 = dynamic_cast<HalfEdge*>(mp_selectedItem);
    if (he1 == nullptr) {
        return;
    }
    /// Create a new vertex (position should be mean btw the vertices in half edge)
    // Create a vertex
    // Get two vertices corresponding to half-edge
    HalfEdge* priorHalfEdge = he1;
    HalfEdge* he2 = he1->sym;
    while(priorHalfEdge->next != he1) {
        priorHalfEdge = priorHalfEdge->next;
    }
    Vertex* v1 = he1->vertex;
    Vertex* v2 = priorHalfEdge->vertex;
    uPtr<Vertex> v3 = mkU<Vertex>();
    v3->position = (v1->position + v2->position) / 2.0f;

    // Create two new half-edges (also set corresponding vertex and face)
    uPtr<HalfEdge> he1B = mkU<HalfEdge>();
    uPtr<HalfEdge> he2B = mkU<HalfEdge>();

    he1B->vertex = v1;
    he2B->vertex = v2;

    he1B->face = he1->face;
    he2B->face = he2->face;

    // Adjust the sym, next, and vert of original edges and new edges
    he1B->next = he1->next;
    he2B->next = he2->next;

    he1->next = he1B.get();
    he2->next = he2B.get();

    he1->vertex = v3.get();
    he2->vertex = v3.get();

    he1->sym = he2B.get();
    he2B->sym = he1;

    he2->sym = he1B.get();
    he1B->sym = he2;

    // Push these new vertex and half-edges to the mesh data sturcture
    m_mesh.vertex.push_back(std::move(v3));
    m_mesh.halfEdge.push_back(std::move(he1B));
    m_mesh.halfEdge.push_back(std::move(he2B));

    // Pass these data into glsl shader
    m_mesh.destroy();
    m_mesh.create();
    update();
    emit sig_sendMeshData(&m_mesh);
}

void MyGL::slot_triangulateFace() {
    if (mp_selectedItem == nullptr) {
        return;
    }
    Face* face_1 = dynamic_cast<Face*>(mp_selectedItem);
    if (face_1 == nullptr) {
        return;
    }

    HalfEdge* start = face_1->halfEdge;
    HalfEdge* curr = start;
    int numSides = 0;
    do {
        curr = curr->next;
        numSides++;
    } while (curr != start);

    for (int i = 0; i < numSides - 3; i++) {
        // Create two new half-edges HE_A and HE_B
        uPtr<HalfEdge> he_A = mkU<HalfEdge>();
        uPtr<HalfEdge> he_B = mkU<HalfEdge>();
        HalfEdge* he_0 = face_1->halfEdge;

        he_A->vertex = he_0->vertex;
        he_B->vertex = he_0->next->next->vertex;

        he_A->sym = he_B.get();
        he_B->sym = he_A.get();

        // Create a second face
        uPtr<Face> face_2 = mkU<Face>();

        he_A->face = face_2.get();
        he_0->face = face_2.get();
        he_0->next->face = face_2.get();

        he_B->face = face_1;

        face_2->halfEdge = he_A.get();
        face_2->color = face_1->color;

        // Fix up the next pointers for our half-edges
        he_B->next = he_0->next->next->next;
        he_0->next->next->next = he_A.get();
        he_A->next = he_0->next;
        he_0->next = he_B.get();

        // Push these new vertex and half-edges to the mesh data sturcture
        m_mesh.face.push_back(std::move(face_2));
        m_mesh.halfEdge.push_back(std::move(he_A));
        m_mesh.halfEdge.push_back(std::move(he_B));
    }


    // Pass these data into glsl shader
    m_mesh.destroy();
    m_mesh.create();
    update();
    emit sig_sendMeshData(&m_mesh);
}

void MyGL::slot_subDivideMesh()
{
    std::vector<uPtr<Vertex>> v_new;
    std::vector<uPtr<HalfEdge>> he_new;
    std::vector<uPtr<Face>> f_new;
    /// For Each Mesh, compute its centroid
    // split half-edges
    std::vector<HalfEdge*> fullEdge;

    // initialize half-edges to be split
    initFullEdge(&fullEdge);

    std::map<Face*, Vertex*> map_centroids;
    computeCentroid(&map_centroids, &v_new);

    /// Compute the smootehed midpoint of each edge in the mesh
    std::map<HalfEdge*, Vertex*> map_midPoints;
    computeMidpoints(&map_centroids, &map_midPoints, &fullEdge, &v_new);


    /// Smooth the original vertices
    smoothOriginal(&map_centroids, &map_midPoints);

    /// For each original face, split that into N quadrangle faces
    for (HalfEdge *he: fullEdge) {
        splitHalfEdge(he, map_midPoints.find(he)->second, &he_new);
    }


    for (uPtr<Face> &f : m_mesh.face) {
        // number of faces to create
        // store temporary next edges
        std::vector<HalfEdge*> temp_new_halfEdge;
        createTempHalfEdge(f.get(), &temp_new_halfEdge);


        std::map<HalfEdge*, HalfEdge*> map_prev;
        mapPrevOfNewHalfEdge(&map_prev, &temp_new_halfEdge);

        std::vector<uPtr<HalfEdge>> ins;
        std::vector<uPtr<HalfEdge>> outs;
        std::vector<uPtr<Face>> faces;
        int numNewFace = 0;
        // create half-edges
        for (HalfEdge *he : temp_new_halfEdge) {

            uPtr<HalfEdge> in = mkU<HalfEdge>();
            uPtr<HalfEdge> out = mkU<HalfEdge>();

            in->setVertex(map_centroids.find(he->face)->second);
            out->setVertex(map_prev.find(he)->second->vertex);
            he->next->next = in.get();
            in->next = out.get();
            out->next = he;

            ins.push_back(std::move(in));
            outs.push_back(std::move(out));
        }
        numNewFace = temp_new_halfEdge.size() - 1;

        // assign sym attributes to new edges
        for (unsigned int i = 0; i < ins.size() - 1; i++) {
            ins.at(i)->sym = outs.at(i + 1).get();
            outs.at(i + 1)->sym = ins.at(i).get();
        }
        ins.at(ins.size() - 1)->sym = outs.at(0).get();
        outs.at(0)->sym = ins.at(ins.size() - 1).get();

        // create faces and assign faces to them
        for (int i = 0; i < numNewFace; i++) {
            uPtr<Face> face = mkU<Face>();
            temp_new_halfEdge.at(i)->setFace(face.get());
            temp_new_halfEdge.at(i)->next->setFace(face.get());
            temp_new_halfEdge.at(i)->next->next->setFace(face.get());
            temp_new_halfEdge.at(i)->next->next->next->setFace(face.get());
            faces.push_back(std::move(face));
        }
        temp_new_halfEdge.at(numNewFace)->setFace(f.get());
        temp_new_halfEdge.at(numNewFace)->next->setFace(f.get());
        temp_new_halfEdge.at(numNewFace)->next->next->setFace(f.get());
        temp_new_halfEdge.at(numNewFace)->next->next->next->setFace(f.get());

        // Push these new vertex and half-edges to the mesh data sturcture
        for (uPtr<HalfEdge> &he : ins) {
            he_new.push_back(std::move(he));
        }
        for (uPtr<HalfEdge> &he : outs) {
            he_new.push_back(std::move(he));
        }
        for (uPtr<Face> &face : faces) {
            f_new.push_back(std::move(face));
        }
    }

    for (uPtr<Vertex> &v : v_new) {
        m_mesh.vertex.push_back(std::move(v));
    }
    for (uPtr<HalfEdge> &he : he_new) {
        m_mesh.halfEdge.push_back(std::move(he));
    }
    for (uPtr<Face> &face : f_new) {
        m_mesh.face.push_back(std::move(face));
    }

    // Pass these data into glsl shader
    m_mesh.destroy();
    m_mesh.create();
    update();
    emit sig_sendMeshData(&m_mesh);
}

void MyGL::computeCentroid(std::map<Face*, Vertex*> *map_centroids,
                           std::vector<uPtr<Vertex>> *v_new) {
    for (uPtr<Face> &f : m_mesh.face) {
        int numSides = 0;
        glm::vec3 centroid_pos = glm::vec3();

        HalfEdge *start = f->halfEdge;
        HalfEdge *curr = start;

        uPtr<Vertex> centroid = mkU<Vertex>();
        //Vertex centroid = Vertex();
        do {
            numSides++;
            centroid_pos += curr->vertex->position;
            curr = curr->next;
        } while(curr != start);

        centroid_pos /= numSides;

        centroid->position = centroid_pos;

        map_centroids->insert(std::make_pair(f.get(), centroid.get()));
        v_new->push_back(std::move(centroid));
    }
}

void MyGL::computeMidpoints(std::map<Face*, Vertex*> *map_centroids,
                            std::map<HalfEdge*, Vertex*> *map_midPoints,
                            std::vector<HalfEdge*> *fullEdge,
                            std::vector<uPtr<Vertex>> *v_new) {


    for (HalfEdge* he : *fullEdge) {
        // find the previous half edge
        HalfEdge *prev = he;
        while (prev->next != he) {
            prev = prev->next;
        }
        uPtr<Vertex> midPoint = mkU<Vertex>();
        if (he->sym == nullptr) {
            midPoint->position = (prev->vertex->position +
                                  he->vertex->position +
                                  map_centroids->find(he->face)->second->position) / 3.0f;
        } else {
            midPoint->position = (prev->vertex->position +
                                  he->vertex->position +
                                  map_centroids->find(he->face)->second->position +
                                  map_centroids->find(he->sym->face)->second->position) / 4.0f;
        }
        map_midPoints->insert(std::make_pair(he, midPoint.get()));
        v_new->push_back(std::move(midPoint));
    }
}

void MyGL::smoothOriginal(std::map<Face*, Vertex*> *map_centroids,
                          std::map<HalfEdge*, Vertex*> *map_midPoints) {
    for (uPtr<Vertex> &v : m_mesh.vertex) {
        HalfEdge *start = v->halfEdge;
        HalfEdge *curr = start;

        glm::vec3 sum_mid = glm::vec3();
        glm::vec3 sum_cent = glm::vec3();

        float num_mid = 0;
        do {
            if (map_midPoints->find(curr) != map_midPoints->end()) {
                sum_mid += map_midPoints->find(curr)->second->position;
            } else {
                sum_mid += map_midPoints->find(curr->sym)->second->position;
            }

            sum_cent += map_centroids->find(curr->face)->second->position;
            num_mid++;
            curr = curr->next->sym;
        } while (start != curr);

        v->position = (1.0f * (num_mid - 2.0f)) / num_mid * v->position +
                      sum_mid / (1.0f * glm::pow(num_mid, 2)) +
                      sum_cent / (1.0f * glm::pow(num_mid, 2));
    }
}


void MyGL::initFullEdge(std::vector<HalfEdge*> *fullEdge) {
    // initialize half-edges to be split
    for (const uPtr<HalfEdge> &he: m_mesh.halfEdge) {
        if (std::find(fullEdge->begin(), fullEdge->end(), he.get()->sym) == fullEdge->end()) {
            fullEdge->push_back(he.get());
        }
    }
}

void MyGL::createTempHalfEdge(Face* f, std::vector<HalfEdge*> *temp){
    HalfEdge *start = f->halfEdge;
    HalfEdge *curr = start;

    do {
        temp->push_back(curr);
        curr = curr->next->next;
    } while (curr != start);
}

void MyGL::mapPrevOfNewHalfEdge(std::map<HalfEdge*, HalfEdge*> *map_prev, std::vector<HalfEdge*> *new_halfEdge){
    int numFaces = new_halfEdge->size();
    HalfEdge *oldStart = new_halfEdge->at(numFaces - 1)->next;

    for (HalfEdge *he: *new_halfEdge) {
        map_prev->insert(std::make_pair(he, oldStart));
        oldStart = he->next;
    }
}
void MyGL::splitHalfEdge(HalfEdge * he1, Vertex* midPoint,
                         std::vector<uPtr<HalfEdge>>* he_new) {
    if (he1 == nullptr) {
        return;
    }
    /// Create a new vertex (position should be mean btw the vertices in half edge)
    // Create a vertex
    // Get two vertices corresponding to half-edge
    HalfEdge* priorHalfEdge = he1;
    HalfEdge* he2 = he1->sym;
    while(priorHalfEdge->next != he1) {
        priorHalfEdge = priorHalfEdge->next;
    }
    Vertex* v1 = he1->vertex;
    Vertex* v2 = priorHalfEdge->vertex;


    // Create two new half-edges (also set corresponding vertex and face)
    uPtr<HalfEdge> he1B = mkU<HalfEdge>();
    uPtr<HalfEdge> he2B = mkU<HalfEdge>();

    he1B->setVertex(v1);
    he2B->setVertex(v2);

    he1B->setFace(he1->face);
    he2B->setFace(he2->face);

    // Adjust the sym, next, and vert of original edges and new edges
    he1B->next = he1->next;
    he2B->next = he2->next;

    he1->next = he1B.get();
    he2->next = he2B.get();

    he1->setVertex(midPoint);
    he2->setVertex(midPoint);

    he1->sym = he2B.get();
    he2B->sym = he1;

    he2->sym = he1B.get();
    he1B->sym = he2;

    // Push these new vertex and half-edges to the mesh data sturcture
    he_new->push_back(std::move(he1B));
    he_new->push_back(std::move(he2B));
}


void MyGL::slot_extrudeFace() {
    if (mp_selectedItem == nullptr) {
        return;
    }
    Face* face = dynamic_cast<Face*>(mp_selectedItem);

    HalfEdge* start = face->halfEdge;
    HalfEdge* curr = start;

    glm::vec3 displacement = 0.3f * glm::normalize(
                glm::cross(
                    start->next->vertex->position -
                    start->vertex->position,
                    start->next->next->vertex->position -
                    start->next->vertex->position));
    std::vector<uPtr<Vertex>> new_verts;
    std::vector<HalfEdge*> ins;
    std::vector<HalfEdge*> outs;

    int count = 0;
    // initialize vertices to be newly created by extruding a face
    do {
        uPtr<Vertex> v = mkU<Vertex>();
        new_verts.push_back(std::move(v));
        curr = curr->next;
        count++;
    } while (curr != start);
    int i = 0;
    do {
        if (i == 0) {
            extrudeSingleEdge(curr,
                              &ins, &outs,
                              new_verts.at(0).get(), new_verts.at(new_verts.size() - 1).get());
        } else {
            extrudeSingleEdge(curr,
                              &ins, &outs,
                              new_verts.at(i).get(), new_verts.at(i - 1).get());
        }
        i++;
        curr = curr->next;
    } while(curr != start);

    // Set sym pointers
    for (unsigned int i = 1; i < ins.size() - 1; i++) {
        ins.at(i)->sym = outs.at(i + 1);
        outs.at(i + 1)->sym = ins.at(i);
    }
    ins.at(ins.size() - 1)->sym = outs.at(0);
    outs.at(0) = ins.at(ins.size() - 1)->sym;

    // Displace the vertices and then put them into mesh
    for (unsigned int i = 0; i < new_verts.size(); i++) {
        if (i == 0 || i == new_verts.size() - 1) {
            new_verts.at(i)->position += 1.5f * displacement;
        } else {
            new_verts.at(i)->position += displacement;
        }
        m_mesh.vertex.push_back(std::move(new_verts.at(i)));
    }

    m_mesh.destroy();
    m_mesh.create();
    update();
    emit sig_sendMeshData(&m_mesh);
}

void MyGL::extrudeSingleEdge(HalfEdge *start,
                             std::vector<HalfEdge*>* ins,
                             std::vector<HalfEdge*>* outs,
                             Vertex* v3, Vertex*v4) {
    HalfEdge* he1 = start;
    HalfEdge* he2 = he1->sym;

    // Create two new vertices, v3, v4

    Vertex* v2 = he2->vertex;
    HalfEdge* curr = he2;
    while (curr->next != he2) {
        curr = curr->next;
    }
    Vertex* v1 = curr->vertex;

    v3->position = v1->position;
    v4->position = v2->position;

    // Adjust he1 so that it points to v3 and its prev points to v4
    HalfEdge* he1_prev = he1;
    while(he1_prev->next != he1) {
        he1_prev = he1_prev->next;
    }


    he1->setVertex(v3);
    he1_prev->setVertex(v4);

    // Create two new half edges he1b and he2b
    uPtr<HalfEdge> he1b = mkU<HalfEdge>();
    uPtr<HalfEdge> he2b = mkU<HalfEdge>();

    he1->sym = he1b.get();
    he2->sym = he2b.get();
    he1b->sym = he1;
    he2b->sym = he2;
    he1b->setVertex(v4);
    he2b->setVertex(v1);

    // Create a new face F and another two half edges he3 and he4
    uPtr<Face> F = mkU<Face>();
    uPtr<HalfEdge> he3 = mkU<HalfEdge>();
    uPtr<HalfEdge> he4 = mkU<HalfEdge>();

    he1b->setFace(F.get());
    he2b->setFace(F.get());
    he3->setFace(F.get());
    he4->setFace(F.get());

    he3->setVertex(v3);
    he4->setVertex(v2);

    he1b->next = he4.get();
    he4->next = he2b.get();
    he2b->next = he3.get();
    he3->next = he1b.get();

    ins->push_back(he3.get());
    outs->push_back(he4.get());

    // set face color
    F->color = 0.8f * he2->face->color;

    // push back to the mesh
    m_mesh.halfEdge.push_back(std::move(he1b));
    m_mesh.halfEdge.push_back(std::move(he2b));
    m_mesh.halfEdge.push_back(std::move(he3));
    m_mesh.halfEdge.push_back(std::move(he4));
    m_mesh.face.push_back(std::move(F));
}

void MyGL::slot_importOBJ() {


    // READ FILE
    QString fileName = QFileDialog::getOpenFileName(0,
                                                    tr("Open File"),
                                                    QDir::currentPath().append(QString("../../..")),
                                                    QString("*.obj"));


    // FOR storing indices
    std::vector<glm::vec3> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec3> vn;

    // FOR SENDING VBOS
    std::vector<glm::vec4> pos;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec4> nor;
    std::vector<GLuint> indices;


    // FOR FINDING SYM EDGES
    std::map<std::pair<GLuint, GLuint>, HalfEdge*> map_sym;
    QFile file(fileName);


    if (file.exists()) {
        std::cout << "file read" << std::endl;
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            std::cout << "file read" << std::endl;
            // CLEAR VBO DATA
            m_mesh.face.clear();
            m_mesh.vertex.clear();
            m_mesh.halfEdge.clear();

            while (!file.atEnd()) {
                QString line = file.readLine().trimmed();
                QStringList lineParts = line.split(QRegularExpression("\\s+"));
                if (lineParts.count() > 0) {
                    // if it's a comment
                    if (lineParts.at(0).compare("#", Qt::CaseInsensitive) == 0) {
                       continue;
                    }
                    // if it's a vertex position (v)
                    else if (lineParts.at(0).compare("v", Qt::CaseInsensitive) == 0) {
                        glm::vec3 position = glm::vec3(lineParts.at(1).toFloat(),
                                                       lineParts.at(2).toFloat(),
                                                       lineParts.at(3).toFloat());
                        m_mesh.vertex.push_back(mkU<Vertex>());
                        m_mesh.vertex.at(m_mesh.vertex.size() - 1)->position = position;
                    }
                    else if (lineParts.at(0).compare("vn", Qt::CaseInsensitive) == 0) {
                        vn.push_back(glm::vec3(lineParts.at(1).toFloat(),
                                               lineParts.at(2).toFloat(),
                                               lineParts.at(3).toFloat()));
                    }
                    else if (lineParts.at(0).compare("vt", Qt::CaseInsensitive) == 0) {
                        vt.push_back(glm::vec2(lineParts.at(1).toFloat(),
                                               lineParts.at(2).toFloat()));
                    }
                    else if (lineParts.at(0).compare("f", Qt::CaseInsensitive) == 0) {
                        float r = (float) rand() / RAND_MAX;
                        float g = (float) rand() / RAND_MAX;
                        float b = (float) rand() / RAND_MAX;
                        // Create a face
                        uPtr<Face> face = mkU<Face>();
                        face->color = glm::vec3(r, g, b);



                        // Init Half Edge (without sym, next)
                        for (int i = 1; i < lineParts.length(); i++) {
                            QStringList data = lineParts.at(i).split("/");

                            int vertPos = data.at(0).toInt() - 1;

                            uPtr<HalfEdge> halfEdge = mkU<HalfEdge>();
                            halfEdge->setFace(face.get());
                            halfEdge->setVertex(m_mesh.vertex.at(vertPos).get());

                            m_mesh.halfEdge.push_back(std::move(halfEdge));
                        }

                        // set next
                        for (int i = 1; i < lineParts.length() - 1; i++) {
                            int j = lineParts.length() - i;
                            m_mesh.halfEdge.at(m_mesh.halfEdge.size() - j)->next =
                                    m_mesh.halfEdge.at(m_mesh.halfEdge.size() - j + 1).get();
                        }
                        m_mesh.halfEdge.at(m_mesh.halfEdge.size() - 1)->next =
                                m_mesh.halfEdge.at(m_mesh.halfEdge.size() - lineParts.length() + 1).get();


                        for (int i = 1; i < lineParts.length(); i++) {

                            int j = lineParts.length() - i;
                            HalfEdge* halfEdge = m_mesh.halfEdge.at(m_mesh.halfEdge.size() - j).get();

                            QStringList currData, prevData;
                            if (i == 1) {
                                currData = lineParts.at(1).split("/");
                                prevData = lineParts.at(lineParts.size() - 1).split("/");
                            } else {
                                currData = lineParts.at(i).split("/");
                                prevData = lineParts.at(i - 1).split("/");
                            }

                            std::pair indice = std::make_pair(currData.at(0).toInt(),
                                                              prevData.at(0).toInt());
                            std::pair symIndice = std::make_pair(indice.second, indice.first);

                            if (map_sym.find(symIndice) != map_sym.end()) {
                                halfEdge->sym = map_sym.find(symIndice)->second;
                                map_sym.find(symIndice)->second->sym = halfEdge;
                            } else {
                                map_sym.insert(std::make_pair(indice, halfEdge));
                            }
                        }
                        m_mesh.face.push_back(std::move(face));
                    }
                }
            }
            file.close();

            m_mesh.destroy();
            m_mesh.create();
            update();
            emit sig_sendMeshData(&m_mesh);
        }
    }

}
