#include "networkmanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    networkmanager w;
    w.show();

    return a.exec();
}
