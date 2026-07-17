#ifndef CDVDBIESTATICOS_H
#define CDVDBIESTATICOS_H

#include <QWidget>
#include <QDebug>
#include "utiles.h"
#include "dancalc.h"
#include "qcustomplot.h"
#include "qfindfiles.h"
#include "dlgsectorbiestatico.h"
#include "customplotf.h"

namespace Ui {
  class CDVDBiestaticos;
}

class CDVDBiestaticos : public QWidget
{
  Q_OBJECT

public:
  explicit CDVDBiestaticos(int m,QWidget *parent = 0);
  ~CDVDBiestaticos();

  PUNTO** puntos;//arreglo bidimencional

  CENTROS Centros;
  LCentros ListCentros;

  QCPTextElement* plottitle;

  int medio;

  QVector<double> xV,yV,xVTx,yVTx,xVRx,yVRx,XSector,YSector,XSector2,YSector2,XSectorObj,YSectorObj;
  QString fileTX;
  QString fileRX;
  GMS latPos, lonPos;
  double lat{};
  double lon{};
  double latRX{};
  double lonRX{};
  double latTX{};
  double lonTX{};

  GMS latPosBlanco, lonPosBlanco;
  int cont{};
  QList<QCPItemEllipse *> listaElipse;
  QList<QCPItemEllipse *> listaZM;
  QList<QCPItemText *> listaText;
  QList<QCPItemLine *> listaLine;
  QList<QCPCurve *> sectorCurva;

  QString aux;
  QPen pen1,pen2;
  QStringList todoZonas;
  void leerZonas();
  bool leer(const QString& fichero);
  bool existe(const QString& zona);
  void initGraph();
  void pintar(QString file);
  void borrar();

  double DeterDis(double LGO, double LGD, double LTD, double LTO);
  double AZimut(double LGO, double LGD, double LTD, double LTO);

  int alvuelo(const QString& strFile);
private slots:
  void on_pB_Pintar_clicked();

  void on_pB_Borrar_clicked();

  void on_pB_Centro_clicked();

  void on_pB_Sector_clicked();

  void on_pB_Zoommas_clicked();

  void on_pB_Zoommenos_clicked();

  void on_pB_Salvar_clicked();

  void on_cB_Transmisor_currentIndexChanged(const QString &arg1);

  void on_cB_Receptores_currentIndexChanged(const QString &arg1);

  void on_chB_Leyenda_toggled(bool checked);

  void on_chB_VerSector_toggled(bool checked);



  void on_chB_VerObjetivo_toggled(bool checked);

private:
  Ui::CDVDBiestaticos *ui;

  DanCalc dancal;
  QFindFiles qfindfiles;
};

#endif // CDVDBIESTATICOS_H
