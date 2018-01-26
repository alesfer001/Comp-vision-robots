#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QImageReader>
#include <QPixmap>
#include <QResizeEvent>
#include <QImage>
#include <QColor>
#include <QSlider>

#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void openFile();
    void displayAbout();
    void displayRGBImage();
    void updateRGBImage(int offset);

    void displayBlur();
    void displaySobel();
    void displayCanny();
    void updateCanny(int lowTreshold);
    void displayDisparityMap();

private:

    //fonctions d'initialisation
    void createLabels();
    void createActions();
    void createMenus();
    void createButtons();
    void createSliders();


    //fonctions auxiliaires image
    QImage* rgbImages[2];
    void scalePixmap(QPixmap* filePixmap, QLabel* imageLabel);
    void spliceImage(QImage* image[2]);
    QImage* rgbImage(QImage* image[2], int offset);
    void updateRGBSlider();
    void hideLabels();
    void hideSliders();

    //opencv
    cv::Mat QImageToMat(QImage* image);
    QImage MatToQImage(const cv::Mat mat);
    void updateCannySlider();

    //images
    QImage* mainImage;

    //labels
    QLabel* mainLabel;
    QLabel* RGBLabel;
    QLabel* cvLabel;

    //menus
    QMenu* aboutMenu;
    QMenu* fileMenu;
    QMenu* toolMenu;

    //actions
    QAction* aboutAct;
    QAction* openFileAct;
    QAction* exitAct;
    QAction* blurAct;
    QAction* sobelAct;
    QAction* cannyAct;
    QAction* disparityAct;

    //buttons
    QPushButton* rgbImageButton;

    //sliders
    QSlider* rgbSlider;
    QSlider* cannySlider;

protected:

    //events
    void resizeEvent(QResizeEvent* event);
};

#endif // MAINWINDOW_H
