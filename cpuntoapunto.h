#ifndef CPUNTOAPUNTO_H
#define CPUNTOAPUNTO_H

#include <QWidget>
#include <QDebug>

#include "comondirecta.h"
#include "utiles.h"
#include "cdatoterreno.h"
#include <QGeoCoordinate>
//struct PUNTOS
//{
//    double X;
//    double Y;
//};
namespace Ui {
class CPuntoAPunto;
}

class CPuntoAPunto : public QWidget
{
    Q_OBJECT

public:
    explicit CPuntoAPunto(QWidget *parent = 0);
    ~CPuntoAPunto();
 QGeoCoordinate GeoPos,GeoPos2;
    GMS latPos, lonPos, latPos2, lonPos2;
    CDatoTerreno datoterrenoMiPos;
    PUNTO** puntos{};//arreglo bidimencional
    QList<double> distterreno;

    DanCalc danca;

    double DeterDis(double LGO, double LGD, double LTD, double LTO);
    double AZimut(double LGO, double LGD, double LTD, double LTO);
    QString fichero;
    qreal CalculaAzimuthTo();
    qreal distanciaEntrePos();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_radioButton_toggled(bool checked);
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::CPuntoAPunto *ui;

    ComonDirecta * representar;
};

#endif // CPUNTOAPUNTO_H
