#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vertex.h>
#include <face.h>
#include <halfedge.h>
#include <mesh.h>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

public slots:
//    void slot_receiveItem(std::vector<QListWidgetItem*>);
    void slot_receiveMeshData(Mesh*);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
