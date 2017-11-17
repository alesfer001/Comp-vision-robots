#include "mainwindow.h"

void paint3d(int offset, QImage r_component, QImage gb_component);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QAction *quit_action = new QAction("&Quit", this);
    QAction *open_action = new QAction("&Open", this);
    QAction *sep_action = new QAction("&Separate", this);
    QAction *new3d_action = new QAction("&3D Image", this);
    QAction *openCVMat = new QAction("openCV Image", this);

    this->resize(1024, 576);

    QMenu *file = new QMenu("My Menu");
    file = menuBar()->addMenu("&File");
    file->addAction(open_action);
    file->addAction(sep_action);
    file->addAction(new3d_action);
    file->addAction(quit_action);

    QMenu *openCV = new QMenu("OpenCV");
    openCV = menuBar()->addMenu("&OpenCV");
    openCV->addAction(openCVMat);

    connect(quit_action, SIGNAL(triggered(bool)),
            this, SLOT(my_quit()));
    connect(open_action, SIGNAL(triggered(bool)),
            this, SLOT(openFile()));
    connect(sep_action, SIGNAL(triggered(bool)),
            this, SLOT(separate()));
    connect(new3d_action, SIGNAL(triggered(bool)),
            this, SLOT(launch3d(bool)));
    connect(openCVMat, SIGNAL(triggered(bool)),
            this, SLOT(myOpenCVMat()));
}

MainWindow::~MainWindow()
{
}

// Slot to quit the app
void MainWindow::my_quit(){
    QMessageBox *mybox = new QMessageBox();
    mybox->setText("App exited succesfully!");
    mybox->exec();
    QApplication::quit();
}

/* Slot to open image and show it in the window's qlabel
 * To change default directory change the first line
 */
void MainWindow::openFile(){
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO" /*"/home/alesfer/Desktop/Univ/Comp-vision-robots/Stereo_Images/STEREO"*/, tr("Images (*.png *.jpg *.gif)"));

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

// Method resizeEvent redefined to scale label containing the image to the new size of the window
void MainWindow::resizeEvent(QResizeEvent *my_event){
    MainWindow::my_label->resize(my_event->size());
    MainWindow::my_label->setScaledContents(1);
}

/* Slot to crop the stereo images into different files
 * Saves the two images created to be later used to make a red cyan 3d image
 * To change the default save directories change the last two lines of the function
 */
void MainWindow::separate(){
    QRect my_rect_1(0, 0, 512, 576);
    QRect my_rect_2(512, 0, 512, 576);
    QPixmap cropped_1 = MainWindow::my_label->pixmap()->copy(my_rect_1);
    QPixmap cropped_2 = MainWindow::my_label->pixmap()->copy(my_rect_2);

    QImage first_image = cropped_1.toImage();
    QImage second_image = cropped_2.toImage();

    first_image.save("/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO/Cropped_1.jpg"/*"/home/alesfer/Desktop/Univ/Comp-vision-robots/Stereo_Images/STEREO/Cropped_1.jpg"*/);
    second_image.save("/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO/Cropped_2.jpg"/*"/home/alesfer/Desktop/Univ/Comp-vision-robots/Stereo_Images/STEREO/Cropped_2.jpg"*/);
}

/* Function uses the two cropped images saved previously by MainWindow::separate() to extract red,green,blue components.
 * Returns QVector containing two QImages, the first is the red component, the second is the green blue component.
 */
QVector<QImage> new3d(){
    QImageReader *my_img_reader = new QImageReader("/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO/Cropped_1.jpg"/*"/home/alesfer/Desktop/Univ/Comp-vision-robots/Stereo_Images/STEREO/Cropped_1.jpg"*/);
    QImage first_image = my_img_reader->read();
    my_img_reader = new QImageReader("/net/cremi/alesfer/TD/Comp-vision-robots/Stereo_Images/STEREO/Cropped_2.jpg"/*"/home/alesfer/Desktop/Univ/Comp-vision-robots/Stereo_Images/STEREO/Cropped_2.jpg"*/);
    QImage second_image = my_img_reader->read();

    QImage r_component = QPixmap(512, 576).toImage();
    QImage gb_component = QPixmap(512, 576).toImage();

    QRgb my_rgb;
    int my_red;
    int my_green;
    int my_blue;

    for(int i=0; i<512; i++){
        for(int j=0; j<576; j++){
            my_rgb = first_image.pixel(i, j);
            my_red = qRed(my_rgb);
            my_rgb = second_image.pixel(i, j);
            my_green = qGreen(my_rgb);
            my_blue = qBlue(my_rgb);
            QColor c(my_red, 0, 0);
            r_component.setPixel(i, j, c.rgba());
            QColor d(0, my_green, my_blue);
            gb_component.setPixel(i, j, d.rgba());
        }
    }
    QVector<QImage> tab;
    tab.push_back(r_component);
    tab.push_back(gb_component);

    return tab;
}

/* Slot to paint the green blue component over the red component and show it in a new QLabel
   with a slider widget to change the horizontal position of the two images
 * int offset is the offset of the cyan component
 */
void MainWindow::paint3d(int offset){

    QVector<QImage> tab = new3d();
    QImage r_component = tab[0];
    QImage gb_component = tab[1];

    QPainter my_painter(&r_component);
    my_painter.setOpacity(0.5);
    my_painter.drawImage(offset, 0, gb_component);

    MainWindow::my_3D_label->resize(512, 576);
    MainWindow::my_3D_label->setPixmap(QPixmap::fromImage(r_component));

    QGridLayout *layout = new QGridLayout(MainWindow::my_3D_label);
    QSlider *my_slider = new QSlider(Qt::Horizontal, MainWindow::my_3D_label);
    QWidget *empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    layout->addWidget(empty, 0, 0);
    layout->addWidget(my_slider, 1, 0);
    MainWindow::my_3D_label->show();

    connect(my_slider, SIGNAL(valueChanged(int)), this, SLOT(paint3d(int)));
}

// Slot to paint the initial red cyan image with an offset set to 0
void MainWindow::launch3d(bool b){
    MainWindow::paint3d(0);
}

void MainWindow::myOpenCVMat(){
    QImage my_image = MainWindow::my_label->pixmap()->toImage();
    cv::Mat tmp(my_image.height(), my_image.width(), CV_8UC3,(uchar*)my_image.bits(),my_image.bytesPerLine());
    cv::imshow("Display", tmp);
    cv::waitKey(5000);
}
