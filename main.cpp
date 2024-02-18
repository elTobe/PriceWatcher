#include "preciosestantes.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PreciosEstantes w;
    w.show();
    return a.exec();
}
