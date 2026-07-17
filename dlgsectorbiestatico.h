#ifndef DLGSECTORBIESTATICO_H
#define DLGSECTORBIESTATICO_H

#include <QDialog>
#include <QDebug>
#include <QGeoCoordinate>
#include "utiles.h"

namespace Ui {
  class DlgSectorBiestatico;
}

class DlgSectorBiestatico : public QDialog
{
  Q_OBJECT

public:
  explicit DlgSectorBiestatico(QWidget *parent = 0);
  ~DlgSectorBiestatico();
 QGeoCoordinate geocoordinateTx,geocoordinateRx,geocoordinateObj;
 bool obj{};
  int medio{};
  int angulo;
  int angulo2{};
  int AnguloPrueba{};
  int angDer{};
  int angIzq{};
  int angDer2{};
  int angIzq2{};
  int angDerObj{};
  int angIzqObj{};
  QString SectorBiestatico;
  void Sector(int ang);
  QString sector;
  QString sector2;
  QVector<double> xV,yV,xVTx,yVTx,xVRx,yVRx;
  double latRX{};
  double lonRX{};
  double latTX{};
  double lonTX{};
int distanciaObj;
  double AZimut(double LGO, double LGD, double LTD, double LTO);
private slots:
  void on_pB_Aceptar_clicked();

  void on_pB_Calcular_clicked();

  void on_sB_Angulo_valueChanged(int arg1);

private:
  Ui::DlgSectorBiestatico *ui;

};

#endif // DLGSECTORBIESTATICO_H
