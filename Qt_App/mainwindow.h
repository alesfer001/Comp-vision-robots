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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QLabel my_label;
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void my_quit();
    void openFile();
};

#endif // MAINWINDOW_H
