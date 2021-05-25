#ifndef WORKER_H
#define WORKER_H

#include <atomic>
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
    std::atomic<bool> stopped;
    std::atomic<bool> exitted;
public:
//    explicit Worker(QObject *parent = nullptr);
    Worker(QObject *parent, const Setup* config_, RecoImage* image_) :
        QThread(parent),
        config(config_),
        image(image_)
    {
        stopped=true;
        exitted=false;
    }

//signals:

public slots:
    void handleStart();
    void handleStop();
    void handleClear();
    void stopExecution();
};

#endif // WORKER_H
