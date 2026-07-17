#ifndef CAMPOS_H
#define CAMPOS_H

#include <QWidget>
#include <QDebug>
#include "utiles.h"
#include "dancalc.h"
#include "qcustomplot.h"
#include "qfindfiles.h"
#include "customplotf.h"
#include "dlgsectorbiestatico.h"
struct SectorInfo {
    QString nombreCurva;           // Nombre de la curva asociada
//    QVector<QCPAbstractPlottable*> sectores;  // Plottables del sector
    QList<QCPCurve *> sectores;
    int a1;                         // Ángulo inicial
    int a2;                         // Ángulo final
};
namespace Ui {
  class Campos;
}

class Campos : public QWidget
{
  Q_OBJECT

public:
  explicit Campos(QWidget *parent = 0);
  ~Campos();
  PUNTO** puntos;//arreglo bidimencional
  QVector<double>xV,yV,XSector,YSector,xVTx,yVTx,xVRx,yVRx,xZM1,yZM1,xZM2,yZM2,xV1,yV1,xV2,yV2,xVSectorP,yVSectorP;

  GMS latPos, lonPos;
  double lat{};
  double lon{};
  int medio{},grupo{};
  int angulositio{};
  int Vuelo{};
  int probabilidad;
  QString prob;
  bool probabilidadAux;
  GMS latPosBlanco, lonPosBlanco;
  int cont;
  QList<QCPItemEllipse *> listaElipse;
  QList<QCPItemEllipse *> listaZM;
  QList<QCPItemText *> listaText;
  QList<QCPItemLine *> listaLine;
  QList<QCPItemLine *> sectorLine;
  QList<QCPCurve *> sectorCurva;
    QList<double> disVDirecta[360];
  CENTROS Centros;
  LCentros ListCentros;

  QPen pen1,pen2;
  QString file;
  QStringList todo;
  QStringList z;
  void pintar();
  void leerZonas();
  bool leer(const QString &fichero);

  QCPTextElement* plottitle;
  QCPTextElement* plottitle2;

  void pintarZonaMuerta(/*int m,int Av,*/double dedZone,double latitud, double longitud);
  void pintarZonaMuertaMuisHaz ();
  void borrarSeleccion(QCPAbstractPlottable *plottable);
  void borrarTodo();
  int alvuelo(const QString &strFile);
  double zonaMuerta(const QString &strFile);
  int Av{};
  bool selected{};
int vel{};
  QList<double> ZonaMuertaMuis[360];
  QList<double> ZonaMuertaMuis2[360];
  QList<double> zonaMuertaMuisInternas[360];
  QList<double> zonaMuertaMuisInternas2[360];

  int velocidad(const QString &strFile);
public slots:
  void graphClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
  void itemClick(QCPAbstractItem *item,QMouseEvent *event);
  void widgetClik();
  void deterProbabilidad(const QString& strFile);

private slots:
  void on_pB_Pintar_clicked();

  void on_rB_ZVD_toggled(bool checked);

  void on_rB_ZPD_toggled(bool checked);

  void on_rB_ZPF_toggled(bool checked);

  void on_rB_Todos_toggled(bool checked);

  void on_pB_Borrar_clicked();

  void on_pB_Centro_clicked();

  void on_pB_Salvar_clicked();

  void on_pB_BorrarSelecction_clicked();

  void on_pB_Sector_clicked();

  void on_pB_ZoomMas_clicked();

  void on_pB_ZoomMen_clicked();

  void on_cB_Probabilidad_currentIndexChanged(int index);

  void on_cB_AlturaObjetivo_currentTextChanged(const QString &arg1);

  void on_cb_Velocidad_currentTextChanged(const QString &arg1);

  void on_pB_SectorPrincipal_clicked();

private:
  Ui::Campos *ui;
  DanCalc dancal;
  QFindFiles qfindfiles;

  QString curvaSeleccionadaActual;  // Nombre de la curva actualmente seleccionada
  // Añade esta lista para almacenar los sectores
   QVector<SectorInfo> sectoresPorCurva;

   // Declaración de funciones auxiliares
   int encontrarSectorPorCurva(const QString& nombreCurva);
   void eliminarSectorDeCurva(const QString& nombreCurva);
};

#endif // CAMPOS_H
