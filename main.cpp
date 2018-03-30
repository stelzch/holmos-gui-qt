#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("FRSEM");
    a.setOrganizationDomain("freiburg-seminar.de");
    a.setApplicationName("HolMOS GUI");
    MainWindow w;
    w.show();

    return a.exec();
}
