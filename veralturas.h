#ifndef VERALTURAS_H
#define VERALTURAS_H

#include <QWidget>
#include <QFile>
#include<QTextStream>
#include <QDir>
#include <QDebug>
#include "qcustomplot.h"
#include "customplotf.h"
#include "defcomunes.h"
#include "dancalc.h"

#pragma pack(push, 1)
struct DVDP
{
    QList<double> ldvd[3];
};


#pragma pack(pop)
namespace Ui {
  class VerAlturas;
}

class VerAlturas : public QWidget
{
  Q_OBJECT

public:
  explicit VerAlturas(QWidget *parent = 0);
  ~VerAlturas();
  QCPTextElement* plottitle;
  QCPTextElement* plottitle2;
  int cont = 0;

  DVDP StrucdisVDirecta;
  QList<DVDP> LdisVDirecta;
  QList<double> disVDirecta[360];
  QList<double> disVDirecta2[360];
  QList<QList<double>> disVDirectaAll;
  bool leer(const QString& extencion);
  bool leerProbabilidad(const QString& extencion);
  QString fichero;
  QString file;
  void initGraph();
  void pintaElipses(QCustomPlot *customplot, int coord, bool text, bool align);
  void pintaLineaAzimut(QCustomPlot *customPlot);
  void pintarDistancias(const QPen& pen, int alt, int z);
  void pintarTodasProbabilidades(const QPen& pen, int p, const QString& prob, bool zona);
  QString cual;
  void salvarGrafica(const QString& altura);
  int probabilidad;
  double zonaMuerta(const QString& strFile);
  QList<QCPItemEllipse *> listaZM;

  void pintarZonaMuerta(double dedZone, double latitud, double longitud);
  int alvuelo(const QString& strFile);
  int Av{};
  QList<int> LisAv;
  int medio{};
  int grupo{};
  int velocidad{};
  int velocidadCohete{};
  int tiempo{};
  int parametro{};
  bool probOno{};
  void pintarZonaMuerta(double zonamuerta);
  void llenartabla(bool llenar,int medios);

  void configInterface();
public slots:
  void graphClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
  void cerrartablaP(int number);
  void salvaTablaP(bool b);
  void legendItemClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event);
private slots:
  void on_pB_ZoomMas_clicked();

  void on_pB_ZoomMenos_clicked();

  void on_pB_SalvarGrafica_clicked();

  void on_checkBox_toggled(bool checked);

  void on_chB_Tabla_toggled(bool checked);


  void on_rB_DVDP_toggled(bool checked);

  void on_rB_DZL_toggled(bool checked);

  void on_rB_ZDR_toggled(bool checked);

private:
  Ui::VerAlturas *ui;
QPushButton *salvarTabla;
  QDialog *dialogTabla;
  QHeaderView* headerView;
  QTableWidget * tabla;
  QTableWidgetItem*tablaitem{};
};

#endif // VERALTURAS_H
