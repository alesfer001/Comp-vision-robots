#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QAction *quit_action = new QAction("&Quit", this);
    QAction *open_action = new QAction("&Open", this);

    QMenu *file = new QMenu("My Menu");
    file = menuBar()->addMenu("&File");
    file->addAction(open_action);
    file->addAction(quit_action);

    connect(quit_action, SIGNAL(triggered(bool)),
            this, SLOT(my_quit()));
    connect(open_action, SIGNAL(triggered(bool)),
            this, SLOT(openFile()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::my_quit(){
    QMessageBox *mybox = new QMessageBox();
    mybox->setText("App exited succesfully!");
    mybox->exec();
    QApplication::quit();
}

void MainWindow::openFile(){
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/alesfer", tr("Images (*.png *.jpg *.gif)"));

    QMessageBox *mybox = new QMessageBox();

    QImageReader *my_img_reader = new QImageReader(file_name);
    QImage my_img = my_img_reader->read();

    QPixmap my_pixmap;
    my_pixmap = QPixmap::fromImage(my_img);
    my_pixmap = my_pixmap.scaled(1024,576);


    MainWindow::my_label.setPixmap(my_pixmap);
    MainWindow::my_label.adjustSize();
    MainWindow::my_label.setFixedSize(1024,576);

    if(file_name != ""){
        MainWindow::my_label.show();
    }
    else{
        mybox->setText("You have canceled!");
        mybox->exec();
    }
}
