#ifndef C_PERFIZC_H
#define C_PERFIZC_H

#include <QWidget>
#include "qcustomplot.h"
#include "ccalculos.h"
#include "customplotf.h"
namespace Ui {
class C_PerfiZC;
}

class C_PerfiZC : public QWidget
{
    Q_OBJECT

public:
    explicit C_PerfiZC(QWidget *parent = 0);
    ~C_PerfiZC();
    CCalculos *calculos{};
    int aazimut{};
    QString fichero;
    double x{};
    double y{};
    double posicion{};
    double AltPos{};
//    QCPPlotTitle * plottitle;
    QCPTextElement* plottitle;
    QCPTextElement *plottitle2;
    void initGraph();
    void pintar();

    int banda{};//metrica = 0, decimetrica = 1

    QCPItemTracer *pTracerListaDisPZ{};
    QList<QCPItemTracer *> listpTracerListaDisPZ;

    QList<double> distanciasPlazoleta;
    QList<double> listdist;

    double DistanciaSelected;

    QCPItemTracer *phaseTracer{};
    QList<QCPItemTracer *> listphaseTracer;

    QCPItemTracer *pTracerInterceptos{};
    QList<QCPItemTracer *> listpTracerInterceptos;

    QCPItemText *textgrado{};
    QList<QCPItemText*> Listtextgrado;
    double Disvuelo;

    double anguloINCidencia{};

    void filePrintPdf();
    void pintartexto(const QString& t, double x, double y, Qt::Alignment alignment);
public slots:
    void seleccionDis(bool cambio);

    void seleccionAC(bool cambio);

    void selectionChanged();
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);
    void itemClicked(QCPAbstractItem *item, QMouseEvent *event);
    void legendItemClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event);
    void widgetClik();
private slots:
    void mouseWheel(QWheelEvent * event);

    void mouseMove(QMouseEvent * event);

    void on_checkBox_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

    void on_pB_Zoommas_clicked();

    void on_pB_Zoommen_clicked();

    void on_pushButton_4_clicked();

    void on_cheB_leyenda_toggled(bool checked);

    void on_checkBox_3_toggled(bool checked);

private:
    Ui::C_PerfiZC *ui;
};

#endif // C_PERFIZC_H
