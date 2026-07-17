#ifndef CUSTOMPLOTF_H
#define CUSTOMPLOTF_H

#include <QObject>
#include <QFile>
#include "qcustomplot.h"
#include "utiles.h"
#include "dancalc.h"
struct AZIDIS
{
  double X;
  double Y;
};
class CustomPlotF : public QCustomPlot
{
  Q_OBJECT
public:
  explicit CustomPlotF(QWidget *parent = 0);
  DanCalc dancal;
  AZIDIS** puntos;//arreglo bidimencional
  void initGraph(QCustomPlot *customPlot, double lowerY, double upperY, double lowerX, double upper, bool visibleX, bool visibleY);

  void pintaLinea(QCustomPlot *customPlot, const QString& layer, const QPen& pen, double xIni, double yIni, double xFin, double yFin);
  void pintaLineaAzimut(QCustomPlot *customPlot);
  void pintaLineaAzimut(QCustomPlot *customPlot, double lat, double lon, QList<QCPItemLine *> *listaLine, QList<QCPItemText *> *listaText);

  void pintaElipses(QCustomPlot *customplot, int coord, bool text, bool align);
  void pintaElipses(QCustomPlot *customplot, double coord, const QString& text, const QColor& textColor, bool align, const QPen& pen);
  void pintaElipses(QCustomPlot *customplot, double lat, double lon, QList<QCPItemEllipse *> *listaElipse, QList<QCPItemText *> *listaText);
  void pintaElipses(QCustomPlot *customplot, double coord, const QString& text, const QColor& textColor, bool align, const QPen& pen, QList<QCPItemEllipse *> *listaElipse);
  void pintaElipses(QCustomPlot *customplot, double coord, const QString& LayerName, const QString& text, const QColor& textColor, bool align, const QPen& pen);

  void pintaCurva(QCustomPlot *customplot, const QString& name, const QPen& pen, const QVector<double>& x1, const QVector<double>& y1);
  void pintaCurva(QCustomPlot *customplot, const QString& LayerName, const QString& name, const QPen& pen, const QBrush& brush, const QVector<double>& x1, const QVector<double>& y1);
  void pintaCurva(QCustomPlot *customplot, const QString& LayerName, const QString& name, const QPen& pen, const QBrush& brush, const QVector<double>& x1, const QVector<double>& y1,QList<QCPCurve *> *listaCurva);
  void pintaCurva(QCustomPlot *customplot, const QString& LayerName, const QString& name, const QPen& pen, double x, double y, const QCPScatterStyle& scStyle);

  void zoommas(QCustomPlot *customPlot);
  void zoommenos(QCustomPlot *customPlot);
  void filePrintPdf(QCustomPlot *customPlot, const QString& file);


  void pintaCurva(QCustomPlot *customplot, const QString& LayerName, const QString& name, const QPen& pen, double x, double y, const QCPScatterStyle& scStyle, QList<QCPCurve *> *listaCurva);
  void pintaSector(QCustomPlot *customPlot, int a1, int a2, bool dentro, QPen pen, int dismax);

  QVector<double> xV,yV;
  QCPCurve *curvaSector = nullptr;
  void pintaCurva(QCustomPlot *customplot, QString LayerName, QString name, QPen pen, QVector<double> x, QVector<double> y, QCPScatterStyle scStyle);
  QCPCurve *getCurvaSector() const;
  void setCurvaSector(QCPCurve *value);

signals:

public slots:
};

#endif // CUSTOMPLOTF_H
