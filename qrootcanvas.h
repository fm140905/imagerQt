#ifndef QROOTCANVAS_H
#define QROOTCANVAS_H

#include <QWidget>
#include <TCanvas.h>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTimer>
#include <TSystem.h>
class QRootCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit QRootCanvas(QWidget *parent = nullptr);
    virtual ~QRootCanvas();
    TCanvas* Canvas(){return fCanvas;}


signals:

public slots:
    void handleRootEvents();
protected:
    TCanvas *fCanvas;
    QTimer *timer;

    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
};

#endif // QROOTCANVAS_H
