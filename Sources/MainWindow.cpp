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

#include <QMessageBox>

#include "reconstruction.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::handleOpen);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::handleClose);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::handleSave);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::handleSaveAs);

    connect(ui->actionStart, &QAction::triggered, this, &MainWindow::handleStart);
    connect(ui->actionStop, &QAction::triggered, this, &MainWindow::handleStop);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::handleClear);

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::handleAbout);

    config = new Setup(Vector3D(SOURCE_X, SOURCE_Y, SOURCE_Z));
    coneQueue = new SPSC(config->capacity);

    workerThread = new Worker(this, config, coneQueue);
    connect(workerThread, &Worker::finished, workerThread, &QObject::deleteLater);
    connect(workerThread, &Worker::finished, this, &MainWindow::notifyThreadFinished);
    connect(this, &MainWindow::threadStopped, workerThread, &Worker::stopExecution);
    workerThread->start();

    createHist();
    createGridlines();
    createCountsLabel();
    ui->canvas->Canvas()->Modified();
    ui->canvas->Canvas()->Update();
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
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Close",
                                                                tr("Are you sure you want to quit?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        finished=true;
        if (workerThread) {
//            qDebug() << "Exiting thread..";
            aborted=true;
            // send stop signal to worker thread
            emit threadStopped();
        }
//        event->accept();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    if (coneQueue)
        delete coneQueue;
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

void MainWindow::run()
{
    std::vector<Cone> cones(config->chuckSize);
    bool emptyQueue = false;
    while (!finished)
    {
        if (!stop)
        {
            int i(0);
            while (i < config->chuckSize)
            {
                emptyQueue = !(coneQueue->wait_dequeue_timed(cones[i],
                                                           std::chrono::milliseconds(10)));
                if (emptyQueue) break;
                i++;
            }
            // update image
            updateImage(cones.cbegin(), cones.cbegin() + i, true);
            if (emptyQueue && threadExecutionFinished)
            {
                // stop updating after this final run
                finished = true;
//                qDebug() << "Processing finished.";
                QMessageBox messageBox;
                messageBox.setText("Image reconstruction finished.");
                messageBox.setWindowTitle("Finished");
                messageBox.exec();
                break;
            }
            // gSystem->Sleep(100);
        }
        QApplication::processEvents();
    }
}

void MainWindow::updateImage(std::vector<Cone>::const_iterator first,
                             std::vector<Cone>::const_iterator last,
                             const bool& normalized)
{
    if (first == last)
        return;
    // update image
    if (normalized)
        addConesNormalized(config, hist, counts, first, last);
    else
        addCones(config, hist, counts, first, last);
    // redraw
    redraw();
}

void MainWindow::createHist()
{
    hist = new TH2D("ROI", " ; Azimuth (degree); Elevation (degree)",
                    config->phiBins, -180, 180,
                    config->thetaBins, -90, 90);
    // init image
    for (int i = 0; i < config->phiBins; i++)
    {
        for (int j = 0; j < config->thetaBins; j++)
        {
            hist->SetBinContent(i+1, j+1, 0);
        }
    }
    gStyle->SetOptStat(0);
    hist->GetZaxis()->SetLabelSize(0.02);
    hist->Draw("z aitoff");

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
    std::string strtmp = "Total counts: " + std::to_string(counts);
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
    std::string strtmp = "Total counts: " + std::to_string(counts);
    countsText->SetText(0.7, 0.92, strtmp.c_str());
    ui->canvas->Canvas()->Modified();
    ui->canvas->Canvas()->Update();
}

void MainWindow::handleOpen()
{
    qDebug() << "Open file clicked";
}

void MainWindow::handleSave()
{
    qDebug() << "Save clicked";
}

void MainWindow::handleSaveAs()
{
    qDebug() << "Save as clicked";
}

void MainWindow::handleClose()
{
//    qDebug() << "Close clicked";
    this->close();
}

void MainWindow::handleStart()
{
    stop=false;
//    qDebug() << "Start clicked";
}

void MainWindow::handleStop()
{
    stop=true;
//    qDebug() << "Stop clicked";
}

void MainWindow::handleClear()
{
    counts = 0;
    hist->Reset();
    redraw();
//    qDebug() << "Clear clicked";
}

void MainWindow::handleAbout()
{
    QMessageBox messageBox;
    messageBox.setText("This application is created using Qt 5.13.2. Source code is available at Gitlab.");
    messageBox.setWindowTitle("About");
    messageBox.exec();
//    qDebug() << "About clicked";
}

void MainWindow::notifyThreadFinished()
{
    // pop up a message box indicating processing is finished.
    threadExecutionFinished = true;
}
