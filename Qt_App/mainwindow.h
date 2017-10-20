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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QLabel *my_label = new QLabel(this);
    MainWindow(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *my_event);
    ~MainWindow();

public slots:
    void my_quit();
    void openFile();
    void separate();
    void new3d();
};

#endif // MAINWINDOW_H
