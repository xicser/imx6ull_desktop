#include "qindicatormainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIndicatorMainWindow w;
    w.show();

    return a.exec();
}
