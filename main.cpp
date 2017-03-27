#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("qReplay");
    QCoreApplication::setOrganizationDomain("zolder.org");
    QCoreApplication::setApplicationName("Wargame Replay Browser");
    QCoreApplication::setApplicationVersion("0.9");

    const Config config(QCoreApplication::applicationDirPath(), "replay.json", "units.json");
    MainWindow w(config);
    w.show();

    return a.exec();
}
