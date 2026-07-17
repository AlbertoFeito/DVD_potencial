#include "qeditenteros.h"

#include <QKeyEvent>

QEditEnteros::QEditEnteros(QWidget *parent):QLineEdit(parent)
{

}

QEditEnteros::~QEditEnteros()
= default;

void QEditEnteros::keyPressEvent(QKeyEvent * event)
{
    QChar c(event->key());
    if(c.isNumber() || event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete ||
            event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
        QLineEdit::keyPressEvent(event);
}
