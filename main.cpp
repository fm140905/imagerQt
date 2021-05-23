#include "MainWindow.h"
#include <QApplication>
#include <TApplication.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TPluginManager.h>
#include <TVirtualGL.h>
#include <TVirtualX.h>
#include <TStyle.h>

int main(int argc, char *argv[])
{
    TApplication *rootapp=new TApplication("Simple Qt ROOT Application", &argc, argv);
    rootapp->SetReturnFromRun(true);

    QApplication a(argc, argv);
    MainWindow w;
    w.resize(w.sizeHint());
    w.show();
    w.resize(700,700);
    w.run();

    if (!w.aborted) {
        return a.exec();
    }
    else {
        gSystem->Sleep(500);
        return 0;
    }
}
