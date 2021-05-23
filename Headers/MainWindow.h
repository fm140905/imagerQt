#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "qrootcanvas.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TText.h"

#include "worker.h"

#define SOURCE_X 3.6
#define SOURCE_Y -10.0
#define SOURCE_Z 0.0

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual void changeEvent(QEvent *e);
    ~MainWindow();
    // reconstruct images
    void run();
    void closeEvent (QCloseEvent *event);
    bool aborted=false;
public slots:

private:
    Ui::MainWindow *ui;
    Setup* config;
    SPSC* coneQueue;
    TH2D* hist;
    void createHist();
    ULong64_t counts=0;
    TText* countsText;
    void createCountsLabel();
    // gridlines
    std::vector<TGraph*> latitudes;
    std::vector<TGraph*> longitudes;
    void aitoff2xy(const double& l, const double& b, double &Al, double &Ab);
    void createGridlines();
    void redraw();
//    void createSourceMarker();
    // worker thread responsible for image reconstruction
    Worker* workerThread;
    bool threadExecutionFinished=false;
    bool finished=false;
    bool stop=true;

//    bool closed=false;
//    void customClose();
    // update image
    void updateImage(std::vector<Cone>::const_iterator first,
                     std::vector<Cone>::const_iterator last,
                     const bool& normalized=true);

protected:

private slots:
    void handleOpen();
    void handleSave();
    void handleSaveAs();
    void handleClose();

    void handleStart();
    void handleStop();
    void handleClear();

    void handleAbout();

    void notifyThreadFinished();
signals:
    void threadStopped();
};

#endif // MAINWINDOW_H
