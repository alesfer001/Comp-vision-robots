#include "mainwindow.h"


                    /* FENETRE PRINCIPALE */
/* ---------------------------------------------------------- */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    this->resize(1024, 576);

    //initialisation
    createLabels();
    createActions();
    createMenus();
    createButtons();
    createSliders();


}


MainWindow::~MainWindow()
{
}




                /* FONCTIONS D'INITIALISATION */
/* ---------------------------------------------------------- */


void MainWindow::createLabels() //cree les labels de la fenetre
{
    mainLabel = new QLabel(this);
    RGBLabel = new QLabel(this);
    //imageLabel->move(0, 25);
    //problème : fenêtre pas resize -> 25 pixels du bas de l'image coupés
    cvLabel = new QLabel(this);
    //cvLabel->move(0, mainLabel->height());
    cvLabel->move(mainLabel->width(), 0);
    mainImage = new QImage();
}

void MainWindow::createActions() //cree les actions de la fenetre
{
    aboutAct = new QAction(tr("&A propos..."), this);
    QAction::connect(aboutAct, SIGNAL(triggered()), this, SLOT(displayAbout()));

    openFileAct = new QAction(tr("&Ouvrir"), this);
    openFileAct->setShortcuts(QKeySequence::Open);
    openFileAct->setStatusTip(tr("Ouvrir une image"));
    QAction::connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));    

    exitAct = new QAction(tr("&Quitter"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    QAction::connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    blurAct = new QAction(tr("&Basique"), this);
    QAction::connect(blurAct, SIGNAL(triggered()), this, SLOT(displayBlur()));

    sobelAct = new QAction(tr("&Sobel"), this);
    QAction::connect(sobelAct, SIGNAL(triggered()), this, SLOT(displaySobel()));

    cannyAct = new QAction(tr("&Canny"), this);
    QAction::connect(cannyAct, SIGNAL(triggered()), this, SLOT(displayCanny()));

    disparityAct = new QAction(tr("&Disparity map"), this);
    QAction::connect(disparityAct, SIGNAL(triggered()), this, SLOT(displayDisparityMap()));
}

void MainWindow::createMenus() //cree les menus de la fenetre
{
    fileMenu = menuBar()->addMenu(tr("&Fichier"));
    fileMenu->addAction(openFileAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    toolMenu = menuBar()->addMenu(tr("&Outils"));
    toolMenu->addAction(blurAct);
    toolMenu->addAction(sobelAct);
    toolMenu->addAction(cannyAct);
    toolMenu->addAction(disparityAct);

    aboutMenu = menuBar()->addMenu(tr("&Aide"));
    aboutMenu->addAction(aboutAct);
}

void MainWindow::createButtons() //cree les boutons de la fenetre
{
    rgbImageButton = new QPushButton(tr("&DispRGB"), this);
    rgbImageButton->move(0, mainLabel->height());
    rgbImageButton->hide();
    QPushButton::connect(rgbImageButton, SIGNAL(clicked()), this, SLOT(displayRGBImage()));
}

void MainWindow::createSliders()
{
    rgbSlider = new QSlider(Qt::Horizontal, this);
    rgbSlider->hide();

    cannySlider = new QSlider(Qt::Horizontal, this);
    cannySlider->hide();
}





                          /* SLOTS */
/* --------------------------------------------------------- */


void MainWindow::openFile() //ouvre une image depuis l'explorateur de fichiers
{
    QString fileName = QFileDialog::getOpenFileName(mainLabel, tr("Ouvrir image"), "/tmp", tr("Images (*.png *.jpg *.bmp *.GIF)"));
    if (fileName != NULL){
        QImageReader *fileReader = new QImageReader(fileName);
        *mainImage = fileReader->read();
        QPixmap* filePixmap = new QPixmap(QPixmap::fromImage(*MainWindow::mainImage));
        scalePixmap(filePixmap, mainLabel);
        hideLabels();
        hideSliders();
        mainLabel->show();
        rgbImageButton->show();
    }
}

void MainWindow::displayAbout() //affiche le a propos
{
    QMessageBox::information(this, "A propos", "Interface graphique pour le projet Vision 3D.");
}

void MainWindow::displayRGBImage(){ //affiche une image 3d rgb
    if(mainImage == NULL){
        QMessageBox::critical(this, "Attention !", "Veuillez d'abord ouvrir une image.");
    }
    else{
        //QImage* images[2]; //tableau de QImage* qui contiendra la partie gauche et la partie droite de l'image stereo
        spliceImage(rgbImages);
        if(rgbImages == NULL) return;
        QPixmap* filePixmap = new QPixmap(QPixmap::fromImage((*rgbImage(rgbImages, 0))));
        scalePixmap(filePixmap, RGBLabel);
        updateRGBSlider();
        rgbImageButton->hide();
        hideLabels();
        RGBLabel->show();
    }
}

void MainWindow::updateRGBImage(int offset) //met à jour l'image en prenant en compte l'offset du slider
{
    if(rgbImages == NULL) return;
    QPixmap* filePixmap = new QPixmap(QPixmap::fromImage((*rgbImage(rgbImages, offset))));
    scalePixmap(filePixmap, RGBLabel);
}


                /* FONCTIONS AUXILIAIRES IMAGE*/
/* ---------------------------------------------------------- */


void MainWindow::scalePixmap(QPixmap* filePixmap, QLabel* mainLabel) //remet la fenetre a l'echelle de l'image
{
    if(filePixmap != NULL){
        filePixmap->scaled(filePixmap->size(), Qt::KeepAspectRatio);
        this->resize(filePixmap->size());
        mainLabel->setFixedSize(filePixmap->size());
        mainLabel->setPixmap(*filePixmap);
        mainLabel->setScaledContents(true);
    }
}

void MainWindow::spliceImage(QImage* images[2]) //coupe une image (stereo) en deux parties gauche et droite et modifie le tableau de QImage* fourni en argument pour qu'il contienne les deux parties
{   if (mainImage != NULL){
        QImage* image = mainImage;
        bool odd = false;
        int middle = (image->width())/2;
        if (((image->width())%2)==1) odd = true; //si l'image est de longueur impaire
        QImage* imageG = new QImage(middle, image->height(), image->format()); //image gauche
        QImage* imageD = new QImage(middle, image->height(), image->format()); //image droite
        for (int i = 0; i < image->height(); i++){
            for (int j = 0; j < image->width(); j++){
                if (j < middle){
                    imageG->setPixel(j, i, image->pixel(j, i)); //on copie la partie gauche de l'image dans la nouvelle QImage* pixel par pixel
                }
                else{
                    if (odd == false) imageD->setPixel(j - middle, i, image->pixel(j, i)); //on copie la partie droite de l'image dans la nouvelle QImage* pixel par pixel
                    else{
                        if ((j+1)==(image->width())) break; //eviter out of range
                        else imageD->setPixel(j - middle, i, image->pixel(j+1, i)); //si la longueur est impaire, on ignore la colonne au milieu de l'image
                    }
                }
            }
        }
        images[0] = imageG;
        images[1] = imageD;
    }
}

QImage* MainWindow::rgbImage(QImage* images[2], int offset) //renvoie une image dont la couleur de chaque pixel a pour composante rouge celle du pixel correspondant du 1er element du tableau
{                                               //en argument, et pour composantes vertes et bleues celles du pixel correspondant du 2e element du tableau en argument (gauche et droite)
    int absoffset = offset;
    if (offset < 0) absoffset = absoffset * (-1);
    if((images[0]!=NULL)&&(images[1] != NULL)){
        QImage* image = new QImage(images[0]->width() + absoffset, images[0]->height(), images[0]->format()); //meme format que l'image de gauche, on admet que les 2 images du tableau en entree sont de meme taille
        QColor* color = new QColor(0, 0, 0);
        for (int i = 0; i < image->height(); i++){
            for(int j = 0; j < image->width(); j++){
                if (offset >= 0){
                    if (((j - offset) >= 0) && ((j + offset) < image->width())){
                        color->setRgb(qRed(images[0]->pixel(j, i)), qGreen(images[1]->pixel(j - offset, i)), qBlue(images[1]->pixel(j - offset, i)));
                    }
                    if ((j - offset) < 0){
                        color->setRgb(qRed(images[0]->pixel(j, i)), 0, 0);
                    }
                    if ((j + offset) >= image->width()){
                        color->setRgb(0, qGreen(images[1]->pixel(j - offset, i)), qBlue(images[1]->pixel(j - offset, i)));
                    }
                    image->setPixel(j, i, color->rgb());
                }
                else{
                    if (((j + offset) >= 0) && ((j - offset) < image->width())){
                        color->setRgb(qRed(images[0]->pixel(j + offset, i)), qGreen(images[1]->pixel(j, i)), qBlue(images[1]->pixel(j, i)));
                    }
                    if ((j + offset) < 0){
                        color->setRgb(0, qGreen(images[1]->pixel(j, i)), qBlue(images[1]->pixel(j, i)));
                    }
                    if ((j - offset) >= image->width()){
                        color->setRgb(qRed(images[0]->pixel(j + offset, i)), 0, 0);
                    }
                    image->setPixel(j, i, color->rgb());
                }
            }
        }
        return image;
    }
    return NULL;
}


void MainWindow::updateRGBSlider() //cree le slider des images 3D
{
    hideSliders();
    rgbSlider->setSliderPosition(0);
    rgbSlider->setRange(((RGBLabel->width())) * (-1), (RGBLabel->width()));
    rgbSlider->move(0, 25);
    rgbSlider->show();
    QSlider::connect(rgbSlider, SIGNAL(valueChanged(int)), this, SLOT(updateRGBImage(int)));
}

void MainWindow::updateCannySlider()
{
    hideSliders();
    cannySlider->setSliderPosition(50);
    cannySlider->setRange(0, 200);
    cannySlider->move(0, 25);
    cannySlider->show();
    QSlider::connect(cannySlider, SIGNAL(valueChanged(int)), this, SLOT(updateCanny(int)));
}

void MainWindow::hideLabels(){
    RGBLabel->hide();
    mainLabel->hide();
    cvLabel->hide();
}

void MainWindow::hideSliders(){
    rgbSlider->hide();
    cannySlider->hide();
}




                         /* OPENCV */
/* ---------------------------------------------------------- */


//je me suis servi d'un exemple sur Internet pour la fonction suivante car je ne suis pas sûr du format utilisé par les images importées, apparemment
//la méthode QPixmap::toImage() renvoie le plus souvent des images au format ARGB32_Premultiplied ou RGB32
cv::Mat MainWindow::QImageToMat(QImage* image){

    switch(image->format())
    {
        //8-bit, alpha
        case QImage::Format_ARGB32_Premultiplied:
        {
            cv::Mat mat(image->height(), image->width(), CV_8UC4, image->bits(), image->bytesPerLine());
            return mat.clone();
        }

        //8-bit, pas d'alpha
        case QImage::Format_RGB32:
        {
            QImage swapped = image->rgbSwapped();
            swapped = swapped.convertToFormat(QImage::Format_RGB888);
            cv::Mat mat(swapped.height(), swapped.width(), CV_8UC3, swapped.bits(), swapped.bytesPerLine());
            return mat.clone();
        }

        default:
        {
            QMessageBox::critical(this, "Attention !", "Format de QImage non supporté");
            break;
        }
    }

    return cv::Mat();
}

QImage MainWindow::MatToQImage(const cv::Mat mat)
{

    switch(mat.type())
    {
        //8-bit, alpha
        case CV_8UC4:
        {
            QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32_Premultiplied);
            return image;
        }

        //8-bit, pas d'alpha
        case CV_8UC3:
        {
            QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            image = image.rgbSwapped();
            image.convertToFormat(QImage::Format_RGB32);
            return image;
        }

        default:
        {
            QMessageBox::critical(this, "Attention !", "Format de cv::Mat non supporté");
            break;
        }
    }

    return QImage();
}


void MainWindow::displayBlur()
{
    if(mainLabel->pixmap() != NULL){
        hideLabels();
        hideSliders();

        cv::Mat mat = QImageToMat(mainImage);
        //cv::imshow("Image OpenCV", mat);
        //cv::waitKey();
        //cv::destroyWindow("Image OpenCV");
        cv::blur(mat, mat, cv::Size(3, 3), cv::Point(-1, -1));
        //cv::imshow("Image OpenCV floutée", mat);
        //cv::waitKey();
        //cv::destroyWindow("Image OpenCV floutée");
        QPixmap* filePixmap = new QPixmap(QPixmap::fromImage(MatToQImage(mat)));
        scalePixmap(filePixmap, cvLabel);
        /*
        this->resize(mainLabel->width(), mainLabel->height()+cvLabel->height());
        cvLabel->move(0, mainLabel->height());
        */
        this->resize(mainLabel->width()+cvLabel->width(), mainLabel->height());
        cvLabel->move(mainLabel->width(), 0);
        rgbImageButton->hide();
        mainLabel->show();
        cvLabel->show();
    }
}

void MainWindow::displaySobel()
{
    if(mainLabel->pixmap() != NULL){
        hideLabels();
        hideSliders();

        cv::Mat mat = QImageToMat(mainImage);
        cv::GaussianBlur(mat, mat, cv::Size(3,3), 0, 0);
        cv::cvtColor(mat, mat, CV_BGR2GRAY);
        cv::Mat grad_x, grad_y;
        cv::Mat abs_grad_x, abs_grad_y;
        cv::Sobel(mat, grad_x, -1, 1, 0);
        cv::convertScaleAbs(grad_x, abs_grad_x);
        cv::Sobel(mat, grad_y, -1, 0, 1);
        cv::convertScaleAbs(grad_y, abs_grad_y);
        cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, mat);
        cv::cvtColor(mat, mat, CV_GRAY2BGR);

        QPixmap* filePixmap = new QPixmap(QPixmap::fromImage(MatToQImage(mat)));
        scalePixmap(filePixmap, cvLabel);
        this->resize(mainLabel->width()+cvLabel->width(), mainLabel->height());
        cvLabel->move(mainLabel->width(), 0);
        rgbImageButton->hide();

        mainLabel->show();
        cvLabel->show();
    }
}

void MainWindow::displayCanny()
{
    if(mainLabel->pixmap() != NULL){
        hideLabels();
        hideSliders();

        cv::Mat mat = QImageToMat(mainImage);
        cv::blur(mat, mat, cv::Size(3, 3), cv::Point(-1, -1));
        cv::cvtColor(mat, mat, CV_BGR2GRAY);
        cv::Canny(mat, mat, 50, 3 * 50);
        cv::cvtColor(mat, mat, CV_GRAY2BGR);

        QPixmap* filePixmap = new QPixmap(QPixmap::fromImage(MatToQImage(mat)));
        scalePixmap(filePixmap, cvLabel);
        this->resize(mainLabel->width()+cvLabel->width(), mainLabel->height());
        cvLabel->move(mainLabel->width(), 0);
        updateCannySlider();
        rgbImageButton->hide();

        mainLabel->show();
        cvLabel->show();
    }
}

void MainWindow::updateCanny(int lowTreshold)
{
    if(mainLabel->pixmap() != NULL){
        hideLabels();

        cv::Mat mat = QImageToMat(mainImage);
        cv::blur(mat, mat, cv::Size(3, 3), cv::Point(-1, -1));
        cv::cvtColor(mat, mat, CV_BGR2GRAY);
        cv::Canny(mat, mat, lowTreshold, 3 * lowTreshold);
        cv::cvtColor(mat, mat, CV_GRAY2BGR);

        QPixmap* filePixmap = new QPixmap(QPixmap::fromImage(MatToQImage(mat)));
        scalePixmap(filePixmap, cvLabel);
        this->resize(mainLabel->width()+cvLabel->width(), mainLabel->height());
        cvLabel->move(mainLabel->width(), 0);
        rgbImageButton->hide();

        mainLabel->show();
        cvLabel->show();
    }
}

void MainWindow::displayDisparityMap()
{
    if(mainLabel->pixmap() != NULL){
        hideSliders();
        spliceImage(rgbImages);
        if(rgbImages == NULL) return;

        cv::Mat imageG = QImageToMat(rgbImages[0]);
        cv::Mat imageD = QImageToMat(rgbImages[1]);

        cv::cvtColor(imageG, imageG, CV_BGR2GRAY);
        cv::cvtColor(imageD, imageD, CV_BGR2GRAY);

        cv::Mat disparity;
        disparity.create(imageG.size(), imageG.type());

        cv::StereoBM *sbm = cv::StereoBM::create(16, 15);

        //-- calculate the disparity image
        sbm->compute(imageG, imageD, disparity);


        cv::imshow("Image OpenCV", disparity);
        cv::waitKey();
        cv::destroyWindow("Image OpenCV");


        /*cv::cvtColor(disparity, disparity, CV_GRAY2BGR);


        QPixmap* filePixmap = new QPixmap(QPixmap::fromImage(MatToQImage((disparity))));
        scalePixmap(filePixmap, imageLabel);
        rgbImageButton->hide();*/
    }
}








                        /* EVENEMENTS */
/* ---------------------------------------------------------- */


void MainWindow::resizeEvent(QResizeEvent * event) //redefinition de l'event resize pour s'adapter automatiquement a la taille de l'image contenue dans le label
{
    QSize newSize = this->size();
    if(mainLabel->pixmap() != NULL && cvLabel->pixmap() == NULL){
        QPixmap *filePixmap = new QPixmap(*(mainLabel->pixmap()));
        filePixmap->scaled(newSize, Qt::KeepAspectRatio);
        mainLabel->setFixedSize(newSize);
        mainLabel->setPixmap(*filePixmap);
    }
    if(RGBLabel->pixmap() != NULL){
        QPixmap *filePixmap = new QPixmap(*(RGBLabel->pixmap()));
        filePixmap->scaled(newSize, Qt::KeepAspectRatio);
        RGBLabel->setFixedSize(newSize);
        RGBLabel->setPixmap(*filePixmap);
    }
    if(cvLabel->pixmap() != NULL){
        QSize size = newSize;
        QPixmap * filePixmap = new QPixmap(*(cvLabel->pixmap()));
        filePixmap->scaled(size.width(), size.height()/2, Qt::KeepAspectRatio);
        cvLabel->setFixedSize(size.width()/2, size.height());
        cvLabel->setPixmap(*filePixmap);
        if (mainLabel->pixmap() != NULL){
            filePixmap = new QPixmap(*(mainLabel->pixmap()));
            filePixmap->scaled(size.width()/2, size.height(), Qt::KeepAspectRatio);
            cvLabel->move(mainLabel->width(), 0);
        }
            mainLabel->setFixedSize(size.width()/2, size.height());
            mainLabel->setPixmap(*filePixmap);
    }

    mainLabel->update();
    RGBLabel->update();
    cvLabel->update();
}
