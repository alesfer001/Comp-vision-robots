#include "mainwindow.h"
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QAction *quit_action = new QAction("&Quit", this);
    QAction *buy = new QAction("&Buy Product", this);

    QMenu *file;
    file = menuBar()->addMenu("&File");
    file->addAction(quit_action);

    QMenu *apropos;
    apropos = menuBar()->addMenu("&A propos");
    apropos->addAction(buy);

    QMessageBox msgBox;
    msgBox.setText("Hello World!");
    msgBox.exec();

    //connect(quit_action, &QAction::triggered, this, QApplication::quit);
    connect(quit_action, SIGNAL(triggered()), qApp, SLOT(quit()));
}

MainWindow::~MainWindow()
{
}
