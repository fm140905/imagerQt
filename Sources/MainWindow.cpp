#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "TMath.h"
#include "TString.h"
//#include <iostream>
//#include "TList.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TAxis.h"

#include "qrootcanvas.h"
#include <QVBoxLayout>
#include <TSystem.h>
#include <QTimer>
#include <QDebug>
#include <QWindow>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::handleOpen);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::handleClose);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::handleSave);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::handleSaveAs);
    connect(ui->actionScreenshot, &QAction::triggered, this, &MainWindow::handleScreenshot);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::handleAbout);

    config = new Setup(Vector3D(SOURCE_X, SOURCE_Y, SOURCE_Z));
    image = new RecoImage(config);
    createGridlines();
    createCountsLabel();

    // update image on worker thread
    workerThread = new Worker(this, config, image);
    connect(workerThread, &Worker::finished, workerThread, &QObject::deleteLater);
    connect(workerThread, &Worker::finished, this, &MainWindow::notifyThreadFinished);
    connect(ui->actionStart, &QAction::triggered, workerThread, &Worker::handleStart);
    connect(ui->actionStop, &QAction::triggered, workerThread, &Worker::handleStop);
    connect(ui->actionClear, &QAction::triggered, workerThread, &Worker::handleClear);
    connect(this, &MainWindow::threadStopped, workerThread, &Worker::stopExecution);
    workerThread->start();

    // update every 0.1 second
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::redraw));
    timer->start(100);

    setWindowTitle(tr("Back projection"));
    ui->canvas->Canvas()->Modified();
    ui->canvas->Canvas()->Update();
    ui->statusBar->showMessage("Ready.");
}

void MainWindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::WindowStateChange){
        QWindowStateChangeEvent *event = static_cast<QWindowStateChangeEvent *>(e);
        if((event->oldState() & Qt::WindowMaximized)||
                (event->oldState() & Qt::WindowMinimized)||
                (event->oldState() & Qt::WindowNoState &&
                 this->windowState() == Qt::WindowMaximized)){
            if(ui->canvas->Canvas()){
                ui->canvas->Canvas()->Resize();
                ui->canvas->Canvas()->Update();
            }
        }
    }
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Close",
                                                               tr("Are you sure you want to quit?\n"),
                                                               QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        if (workerThread) {
//            qDebug() << "Exiting thread..";
            // send stop signal to worker thread
            aborted=true;
            emit threadStopped();
            gSystem->Sleep(500);
        }
        event->accept();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    if (image)
        delete image;
    if (config)
        delete config;
    for (int i = 0; i < longitudes.size(); ++i) {
       if(longitudes[i]) {
           delete longitudes[i];
       }
    }
    for (int i = 0; i < latitudes.size(); ++i) {
       if (latitudes[i]) {
           delete latitudes[i];
       }
    }
}


void MainWindow::createGridlines()
{
    const int Nl = 19; // Number of drawn latitudes
    const int NL = 19; // Number of drawn longitudes
    const int M = 30;
    float conv=M_PI/180;
    float la, lo, x, y, z;
    for (int j=0;j<Nl;++j) {
        // latitudes[j]=new TGraph();
        latitudes.push_back(new TGraph());
        la = -90+180/(Nl-1)*j;
        for (int i=0;i<M+1;++i) {
            lo = -180+360/M*i;
            z  = sqrt(1+cos(la*conv)*cos(lo*conv/2));
            x  = 180*cos(la*conv)*sin(lo*conv/2)/z;
            y  = 90*sin(la*conv)/z;
            latitudes[j]->SetPoint(i,x,y);
        }
    }

    for (int j=0;j<NL;++j) {
        // longitudes[j]=new TGraph();
        longitudes.push_back(new TGraph());
        lo = -180+360/(NL-1)*j;
        for (int i=0;i<M+1;++i) {
            la = -90+180/M*i;
            z  = sqrt(1+cos(la*conv)*cos(lo*conv/2));
            x  = 180*cos(la*conv)*sin(lo*conv/2)/z;
            y  = 90*sin(la*conv)/z;
            longitudes[j]->SetPoint(i,x,y);
        }
    }
    // Draw the grid
    TPad *pad2 = new TPad("pad2","",0,0,1,1);
    pad2->SetFillStyle(4000);
    pad2->SetFillColor(0);
    pad2->SetBorderSize(0);
    pad2->SetFrameBorderMode(0);
    pad2->SetFrameLineColor(0);
    pad2->SetFrameBorderMode(0);
    pad2->Draw();
    pad2->cd();
    Double_t ymin = -89.5;
    Double_t ymax = 89.5;
    Double_t dy = (ymax-ymin)/0.8; //10 per cent margins top and bottom
    Double_t xmin = -180;
    Double_t xmax = 180;
    Double_t dx = (xmax-xmin)/0.8; //10 per cent margins left and right

    pad2->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);

    for (int j=0;j<Nl;++j) latitudes[j]->Draw("l");
    for (int j=0;j<NL;++j) longitudes[j]->Draw("l");
}

void MainWindow::createCountsLabel()
{
    std::string strtmp = "Total counts: " + std::to_string(image->counts);
    countsText = new TText(0.7, 0.92, strtmp.c_str());
    countsText->SetTextSizePixels(5);
    countsText->SetNDC(kTRUE);
    countsText->Draw("SAME");
}

void MainWindow::aitoff2xy(const double& l, const double& b, double &Al, double &Ab)
{
    Double_t x, y;
    Double_t alpha2 = (l/2)*TMath::DegToRad();
    Double_t delta  = b*TMath::DegToRad();
    Double_t r2     = TMath::Sqrt(2.);
    Double_t f      = 2*r2/TMath::Pi();
    Double_t cdec   = TMath::Cos(delta);
    Double_t denom  = TMath::Sqrt(1. + cdec*TMath::Cos(alpha2));
    x      = cdec*TMath::Sin(alpha2)*2.*r2/denom;
    y      = TMath::Sin(delta)*r2/denom;
    x     *= TMath::RadToDeg()/f;
    y     *= TMath::RadToDeg()/f;
    //  x *= -1.; // for a skymap swap left<->right
    Al = x;
    Ab = y;
}

void MainWindow::redraw()
{
    image->mMutex.lock();
    std::string strtmp = "Total counts: " + std::to_string(image->counts);
    countsText->SetText(0.7, 0.92, strtmp.c_str());
    ui->canvas->Canvas()->Modified();
    ui->canvas->Canvas()->Update();
    image->mMutex.unlock();
}

void MainWindow::handleOpen()
{
    qDebug() << "Open file clicked";
}

bool MainWindow::handleSave()
{
//    qDebug() << "Save clicked";
    if (curFile.isEmpty()) {
        return handleSaveAs();
    } else {
        return saveCanvas(curFile);
    }
}

bool MainWindow::handleSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Image"), "",
            tr("PNG file (*.png);;ROOT file (*.root);;C file (*.C);;text file (*.txt)"));
    if (fileName.isEmpty())
        return false;
    else {
        if (saveCanvas(fileName))
            curFile = fileName;
    }
}

bool MainWindow::saveCanvas(const QString& fileName)
{
    QFileInfo fileInfo(fileName);
    QString ext=fileInfo.completeSuffix();
    bool saved=false;
    if (ext == "root" || ext == "png" || ext == "C") {
//        qDebug() << "Save as " << ext << " file.";
        image->mMutex.lock();
        ui->canvas->Canvas()->SaveAs(fileName.toLocal8Bit().constData());
        image->mMutex.unlock();
        saved = true;
    }
    else if (ext == "txt") {
        saved = image->saveImage(fileName.toStdString());
    }
    if (saved)
    {
        ui->statusBar->showMessage(QString("Image saved to: %1").arg(fileName));
    }
    return saved;
}
void MainWindow::handleClose()
{
//    qDebug() << "Close clicked";
    this->close();
}

void MainWindow::handleScreenshot()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString currentDateTime = dateTime.toString("yyyy-MM-dd-HH-mm-ss");
    QString fileName = QString("Screenshot-%1.png").arg(currentDateTime);
    saveCanvas(fileName);
}
void MainWindow::handleAbout()
{
    QMessageBox::about(this, tr("About Application"),
             tr("This application is created using Qt 5.13.2. Source code is available at Gitlab."));
//    qDebug() << "About clicked";
}

void MainWindow::notifyThreadFinished()
{
    // pop up a message box indicating processing is finished.
    if (!aborted)
    {
        QMessageBox messageBox;
        messageBox.setText("Image reconstruction finished.");
        messageBox.setWindowTitle("Finished");
        messageBox.exec();
    }
}
