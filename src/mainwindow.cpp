#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include "iostream"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    // Initialize QWidgetListItem
    connect(ui->mygl, SIGNAL(sig_sendMeshData(Mesh*)),
            this, SLOT(slot_receiveMeshData(Mesh*)));

    // Gives information about the state of the selected item
    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl, SLOT(slot_setSelectedItem(QListWidgetItem*)));
    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl, SLOT(slot_setSelectedItem(QListWidgetItem*)));
    connect(ui->halfEdgesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl, SLOT(slot_setSelectedItem(QListWidgetItem*)));

    // Initialize Spin Boxes for vertex positions
    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_vertX(double)));
    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_vertY(double)));
    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_vertZ(double)));

    // Initialize Spin boxes for color
    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_colorRed(double)));
    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_colorGreen(double)));
    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_colorBlue(double)));

    // Enable Topology Editing Functions
    connect(ui->splitEdgeButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_splitHalfEdge()));
    connect(ui->triangulateFaceButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_triangulateFace()));

    // Enable Catmull-Clark Subdivision
    connect(ui->subDivideButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_subDivideMesh()));

    // Enable Extrusion
    connect(ui->extrudeButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_extrudeFace()));

    // Enable OBJ Import
    connect(ui->importOBJButton, SIGNAL(clicked(bool)), ui->mygl, SLOT(slot_importOBJ()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}


void MainWindow::slot_receiveMeshData(Mesh* m) {
    for (const uPtr<Face> &f : m->face) {
        ui->facesListWidget->addItem(f.get());
    }
    for(const uPtr<HalfEdge> &he : m->halfEdge) {
        ui->halfEdgesListWidget->addItem(he.get());
    }
    for(const uPtr<Vertex> &v : m->vertex) {
        ui->vertsListWidget->addItem(v.get());
    }
}
