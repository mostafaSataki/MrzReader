#include "Mrz_GUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Mrz_GUI w;
    w.show();
    return a.exec();
}
