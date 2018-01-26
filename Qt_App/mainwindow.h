#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QImageReader>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QResizeEvent>
#include <QRect>
#include <QRgb>
#include <QDebug>
#include <QPainter>
#include <QSlider>
#include <QGridLayout>
#include <QVector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QLabel *my_label = new QLabel(this);
    QLabel *my_3D_label = new QLabel();
    QLabel *cv_label = new QLabel();
    QImage *my_img = new QImage();
    QImage *left = new QImage();
    QImage *right = new QImage();
    MainWindow(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *my_event);
    void imSideBySide(QImage first_image, cv::Mat inMat);
    ~MainWindow();

public slots:
    void my_quit();
    void openFile();
    void separate();
    void paint3d(int);
    void launch3d(bool);
    void myOpenCVMat();
    void my_blur();
    void my_sobel();
    void my_canny();
};

#endif // MAINWINDOW_H
