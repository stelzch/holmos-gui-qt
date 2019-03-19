#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("FRSEM");
    a.setOrganizationDomain("freiburg-seminar.de");
    a.setApplicationName("HolMOS GUI");

    QLocale defaultLocale;
    QTranslator german;
    if(defaultLocale.language() == QLocale::German) {
        qDebug() << "Loading german translations...";
        german.load(":/translations/langs/german.qm");

        qDebug() << "Installing translator...";
        a.installTranslator(&german);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
