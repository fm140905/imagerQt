#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include "setup.h"

class Worker : public QThread
{
    Q_OBJECT
    const Setup* config;
    SPSC* coneQueue;
    void run() override;
    bool stop=false;
public:
//    explicit Worker(QObject *parent = nullptr);
    Worker(QObject *parent, const Setup* config_, SPSC* coneQueue_) :
        QThread(parent),
        config(config_),
        coneQueue(coneQueue_)
    {}

//signals:

public slots:
    void stopExecution();
};

#endif // WORKER_H
