#include "worker.h"
#include "TSystem.h"
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QDebug>

void Worker::run()
{
    // simulate some cones
    // or import cones from file
    QFile conefile(config->conefilePath.c_str());
    if (!conefile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw std::invalid_argument("Cannot find file: " + config->conefilePath);
    }

    QString line;
    QTextStream in(&conefile);
    // skip header (first line)
    in.readLineInto(&line);
    ulong counts(0);
    while(!stop) {
        //some work to get data (read board or file)
        int i(0);
        while (!stop && i < config->chuckSize && in.readLineInto(&line))
        {
            coneQueue->enqueue(Cone(line));
            i++;
        }
        counts+=i;
        if (stop || line.isNull() || counts >= config->maxN)
        {
            break;
        }

        while (!stop && coneQueue->size_approx() >= config->capacity)
        {
            gSystem->Sleep(50);
        }
    }
    conefile.close();
    qDebug() << "Worker thread exited.";
}

void Worker::stopExecution()
{
    stop = true;
}
