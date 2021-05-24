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
    std::vector<Cone> cones(config->chuckSize, Cone());
    while(!exitted)
    {
        if (!stopped)
        {
            //some work to get data (read board or file)
            int i(0);
            while (!exitted && i < config->chuckSize && in.readLineInto(&line))
            {
                cones[i]=Cone(line);
                i++;
            }
            localCounts+=i;
            // update image
            image->updateImage(cones.cbegin(), cones.cbegin()+i, true);
            if (exitted || line.isNull() || localCounts >= config->maxN)
                break;
        }
    }
    conefile.close();
    qDebug() << "Worker thread exited.";
}

void Worker::handleStart()
{
    stopped=false;
}

void Worker::handleStop()
{
    stopped=true;
}

void Worker::stopExecution()
{
    exitted = true;
}

void Worker::handleClear()
{
    localCounts = 0;
    image->clear();
}
