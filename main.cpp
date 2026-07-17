#include "qinicial.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QInicial w;
    w.show();

    return QApplication::exec();
}
