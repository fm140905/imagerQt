#include "qrootcanvas.h"
#include <QDebug>
#include "TVirtualX.h"
QRootCanvas::QRootCanvas(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen,false);
    setAttribute(Qt::WA_OpaquePaintEvent,true);
    setAttribute(Qt::WA_NativeWindow,true);
    setUpdatesEnabled(kFALSE);
    setMouseTracking(kTRUE);
    setMinimumSize(300,200);
    timer=new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(handleRootEvents()));
    timer->start(100);
    fCanvas=new TCanvas("canvas",width(),height(),gVirtualX->AddWindow(ULong_t(winId()),UInt_t(width()),UInt_t(height())));
    fCanvas->cd();


}

QRootCanvas::~QRootCanvas()
{
    delete fCanvas;
}

void QRootCanvas::handleRootEvents()
{
    gSystem->ProcessEvents();
}

void QRootCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if (fCanvas) {
       switch (e->button()) {
          case Qt::LeftButton :
             fCanvas->HandleInput(kButton1Motion, e->x(), e->y());
             break;
          case Qt::MidButton :
             fCanvas->HandleInput(kButton2Motion, e->x(), e->y());
             break;
          case Qt::RightButton :
             fCanvas->HandleInput(kButton3Motion, e->x(), e->y());
             break;
          default:
             break;
       }
    }
}

void QRootCanvas::mousePressEvent(QMouseEvent *e)
{
    if (fCanvas) {
       switch (e->button()) {
          case Qt::LeftButton :
             fCanvas->HandleInput(kButton1Down, e->x(), e->y());
             break;
          case Qt::MidButton :
             fCanvas->HandleInput(kButton2Down, e->x(), e->y());
             break;
          case Qt::RightButton :
             // does not work properly on Linux...
             // ...adding setAttribute(Qt::WA_PaintOnScreen, true)
             // seems to cure the problem
             fCanvas->HandleInput(kButton3Down, e->x(), e->y());
             break;
          default:
             break;
       }
    }
}

void QRootCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    if (fCanvas) {
       switch (e->button()) {
          case Qt::LeftButton :
             fCanvas->HandleInput(kButton1Up, e->x(), e->y());
             break;
          case Qt::MidButton :
             fCanvas->HandleInput(kButton2Up, e->x(), e->y());
             break;
          case Qt::RightButton :
             // does not work properly on Linux...
             // ...adding setAttribute(Qt::WA_PaintOnScreen, true)
             // seems to cure the problem
             fCanvas->HandleInput(kButton3Up, e->x(), e->y());
             break;
          default:
             break;
       }
    }
}

void QRootCanvas::paintEvent(QPaintEvent *)
{
    if(fCanvas){
        fCanvas->Resize();
        fCanvas->Update();
    }
}

void QRootCanvas::resizeEvent(QResizeEvent *e)
{
    if(fCanvas){
        fCanvas->SetCanvasSize(UInt_t(e->size().width()),UInt_t(e->size().height()));
        fCanvas->Resize();
        fCanvas->Update();
    }
}
