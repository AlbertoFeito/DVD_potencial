#ifndef QEDITENTEROS_H
#define QEDITENTEROS_H

#include <QLineEdit>

class QEditEnteros : public QLineEdit
{
public:
    QEditEnteros(QWidget *parent=0);
    ~QEditEnteros();


    void keyPressEvent(QKeyEvent *event);
};

#endif // QEDITENTEROS_H
