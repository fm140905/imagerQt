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
    void closeEvent (QCloseEvent *event);
public slots:

private:
    Ui::MainWindow *ui;
    Setup* config;
    RecoImage* image;
    TText* countsText;
    void createCountsLabel();
    // gridlines
    std::vector<TGraph*> latitudes;
    std::vector<TGraph*> longitudes;
    void aitoff2xy(const double& l, const double& b, double &Al, double &Ab);
    void createGridlines();
    void redraw();
    // worker thread responsible for image reconstruction
    Worker* workerThread;
    bool aborted=false;

    bool saveCanvas(const QString& fileName);
    QString curFile;

protected:

private slots:
    void handleOpen();
    bool handleSave();
    bool handleSaveAs();
    void handleClose();
    void handleScreenshot();
    void handleAbout();

    void notifyThreadFinished();
signals:
    void threadStopped();
};

#endif // MAINWINDOW_H
