#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<statusType>("statusType");
    qRegisterMetaType<statusType>("statusType&");
    Widget w;
    w.show();
    return a.exec();
}
