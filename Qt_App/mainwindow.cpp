#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QAction *quit_action = new QAction("&Quit", this);
    QAction *open_action = new QAction("&Open", this);
    QAction *sep_action = new QAction("&Separate", this);
    QAction *new3d_action = new QAction("&3D Image", this);

    this->resize(1024, 576);

    QMenu *file = new QMenu("My Menu");
    file = menuBar()->addMenu("&File");
    file->addAction(open_action);
    file->addAction(sep_action);
    file->addAction(new3d_action);
    file->addAction(quit_action);

    connect(quit_action, SIGNAL(triggered(bool)),
            this, SLOT(my_quit()));
    connect(open_action, SIGNAL(triggered(bool)),
            this, SLOT(openFile()));
    connect(sep_action, SIGNAL(triggered(bool)),
            this, SLOT(separate()));
    connect(new3d_action, SIGNAL(triggered(bool)),
            this, SLOT(new3d()));
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
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO", tr("Images (*.png *.jpg *.gif)"));

    if(file_name != ""){

        QImageReader *my_img_reader = new QImageReader(file_name);
        QImage my_img = my_img_reader->read();

        QPixmap my_pixmap;
        my_pixmap = QPixmap::fromImage(my_img);
        my_pixmap = my_pixmap.scaled(1024,576);

        MainWindow::resize(1024,576);
        MainWindow::my_label->resize(1024,576);

        MainWindow::my_label->setPixmap(my_pixmap);
        MainWindow::my_label->show();
    }
    else{

        QMessageBox *mybox = new QMessageBox();

        mybox->setText("You have canceled!");
        mybox->exec();
    }
}

void MainWindow::resizeEvent(QResizeEvent *my_event){
    MainWindow::my_label->resize(my_event->size());
    MainWindow::my_label->setScaledContents(1);
}

void MainWindow::separate(){
    QRect my_rect_1(0, 0, 512, 576);
    QRect my_rect_2(512, 0, 512, 576);
    QPixmap cropped_1 = MainWindow::my_label->pixmap()->copy(my_rect_1);
    QPixmap cropped_2 = MainWindow::my_label->pixmap()->copy(my_rect_2);

    QImage first_image = cropped_1.toImage();
    QImage second_image = cropped_2.toImage();

    first_image.save("/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO/Cropped_1.jpg");
    second_image.save("/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO/Cropped_2.jpg");
}

void MainWindow::new3d(){
    QImageReader *my_img_reader = new QImageReader("/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO/Cropped_1.jpg");
    QImage first_image = my_img_reader->read();
    my_img_reader = new QImageReader("/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO/Cropped_2.jpg");
    QImage second_image = my_img_reader->read();

    int red = qRed(first_image.color(0));
    int blue = qBlue(second_image.color(0));
    int green = qGreen(second_image.color(0));

    QImage my_3D;
}
