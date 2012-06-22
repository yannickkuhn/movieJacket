#include <QtGui/QApplication>
#include "moviejacket.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MovieJacket w;
    w.show();
    return a.exec();
}
