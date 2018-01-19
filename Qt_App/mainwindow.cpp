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
    QAction *blur = new QAction("Blur", this);
    QAction *sobel = new QAction("Sobel", this);
    QAction *canny = new QAction("Canny", this);

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
    openCV->addAction(blur);
    openCV->addAction(sobel);
    openCV->addAction(canny);

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
    connect(blur, SIGNAL(triggered(bool)),
            this, SLOT(my_blur()));
    connect(sobel, SIGNAL(triggered(bool)),
            this, SLOT(my_sobel()));
    connect(canny, SIGNAL(triggered(bool)),
            this, SLOT(my_canny()));
}

MainWindow::~MainWindow()
{
}

// Slot to quit the app
void MainWindow::my_quit(){
    QApplication::quit();
}

/* Slot to open image and show it in the window's qlabel
 * To change default directory change the first line
 */
void MainWindow::openFile(){
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "Stereo_Images/STEREO", tr("Images (*.png *.jpg *.gif)"));

    if(file_name != ""){
        QImageReader *my_img_reader = new QImageReader(file_name);
        *MainWindow::my_img = my_img_reader->read();
        int width = MainWindow::my_img->width();
        int height = MainWindow::my_img->height();

        QPixmap my_pixmap;
        my_pixmap = QPixmap::fromImage(*MainWindow::my_img);
        my_pixmap = my_pixmap.scaled(width,height);

        MainWindow::resize(width,height);
        MainWindow::my_label->resize(width,height);

        MainWindow::my_label->setPixmap(my_pixmap);
        MainWindow::my_label->show();
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
    int width = MainWindow::my_img->width();
    int height = MainWindow::my_img->height();

    QRect my_rect_1(0, 0, width/2, height);
    QRect my_rect_2(width/2, 0, width/2, height);
    QPixmap cropped_1 = MainWindow::my_label->pixmap()->copy(my_rect_1);
    QPixmap cropped_2 = MainWindow::my_label->pixmap()->copy(my_rect_2);

    cropped_1.toImage().save("Stereo_Images/STEREO/Cropped_1.jpg");
    cropped_2.toImage().save("Stereo_Images/STEREO/Cropped_2.jpg");
}

/* Function uses the two cropped images saved previously by MainWindow::separate() to extract red,green,blue components.
 * Returns QVector containing two QImages, the first is the red component, the second is the green blue component.
 */
QVector<QImage> new3d(){
    //MainWindow::separate();
    QImageReader *my_img_reader = new QImageReader("Stereo_Images/STEREO/Cropped_1.jpg");
    QImage first_image = my_img_reader->read();
    my_img_reader = new QImageReader("Stereo_Images/STEREO/Cropped_2.jpg");
    QImage second_image = my_img_reader->read();

    QImage r_component = QPixmap(first_image.width(), first_image.height()).toImage();
    QImage gb_component = QPixmap(second_image.width(), second_image.height()).toImage();

    QRgb my_rgb;
    int my_red;
    int my_green;
    int my_blue;

    for(int i=0; i<first_image.width(); i++){
        for(int j=0; j<first_image.height(); j++){
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

    MainWindow::my_3D_label->resize(r_component.width(), r_component.height());
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

// Slot to display cv::Mat created from QImage
void MainWindow::myOpenCVMat(){
    QImage my_image = *MainWindow::my_img;
    cv::Mat tmp(my_image.height(), my_image.width(), CV_8UC4,
                my_image.bits(),
                my_image.bytesPerLine());
    cv::namedWindow("Display", CV_WINDOW_NORMAL);
    cv::imshow("Display", tmp);
    cv::waitKey(0);
}

// Function to show first QImage and second QImage (created from cv::Mat) side by side
void MainWindow::imSideBySide(QImage first_image, cv::Mat inMat){
    QImage second_image(inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_ARGB32);

    QPixmap *pixmap = new QPixmap(first_image.width()+second_image.width(), first_image.height());
    QPainter *painter = new QPainter(pixmap);
    painter->drawPixmap(0, 0, first_image.width(), first_image.height(), QPixmap::fromImage(first_image));
    painter->drawPixmap(first_image.width(), 0, second_image.width(), second_image.height(), QPixmap::fromImage(second_image));
    painter->end();

    //MainWindow::my_label->repaint();
    MainWindow::cv_label->resize(QSize(first_image.width()+second_image.width(), first_image.height()));
    MainWindow::cv_label->setPixmap(*pixmap);
    //MainWindow::my_label->show();
    //MainWindow::my_label->update();
    //MainWindow::my_label->repaint();
    //qApp->processEvents();
    MainWindow::cv_label->show();
}

// Slot to apply blur effect to current image
void MainWindow::my_blur(){
    QImage my_image = *MainWindow::my_img;
    cv::Mat tmp(my_image.height(), my_image.width(), CV_8UC4,
                my_image.bits(),
                my_image.bytesPerLine());

    cv::Mat blurred_image;
    cv::blur(tmp, blurred_image, cv::Size(5, 5));

    MainWindow::imSideBySide(my_image, blurred_image);
}

// Slot to apply sobel effect to current image
void MainWindow::my_sobel(){
    QImage my_image = *MainWindow::my_img;
    cv::Mat tmp(my_image.height(), my_image.width(), CV_8UC4,
                my_image.bits(),
                my_image.bytesPerLine());

    cv::Mat sobel_image;
    cv::Sobel(tmp, sobel_image, CV_64F, 1, 0);

    MainWindow::imSideBySide(my_image, sobel_image);
}

// Slot to apply canny effect to current image
void MainWindow::my_canny(){
    QImage my_image = *MainWindow::my_img;
    cv::Mat tmp(my_image.height(), my_image.width(), CV_8UC4,
                my_image.bits(),
                my_image.bytesPerLine());

    cv::Mat canny_image, gray;
    cv::cvtColor(tmp, gray, CV_BGR2GRAY);
    cv::Canny(gray, canny_image, 10, 5000, 3);

    MainWindow::imSideBySide(my_image, canny_image);
}
