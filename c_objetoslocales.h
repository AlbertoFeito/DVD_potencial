#ifndef C_OBJETOSLOCALES_H
#define C_OBJETOSLOCALES_H

#include <QObject>
#include <QThread>

#include "ccalculos.h"

class c_ObjetosLocales : public QThread
{
    Q_OBJECT
public:
    explicit c_ObjetosLocales(QObject *parent = nullptr);

    CCalculos *calculos{};
    void calcular();

signals:

protected:
    void run();
};

#endif // C_OBJETOSLOCALES_H
