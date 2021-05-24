#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include "reconstruction.h"

class Worker : public QThread
{
    Q_OBJECT
    const Setup* config;
    RecoImage* image;
    void run() override;
    ulong localCounts=0;
    bool stopped=true;
    bool exitted=false;
public:
//    explicit Worker(QObject *parent = nullptr);
    Worker(QObject *parent, const Setup* config_, RecoImage* image_) :
        QThread(parent),
        config(config_),
        image(image_)
    {}

//signals:

public slots:
    void handleStart();
    void handleStop();
    void handleClear();
    void stopExecution();
};

#endif // WORKER_H
