#ifndef C_GRAFPLAZOLETA_H
#define C_GRAFPLAZOLETA_H

#include <QWidget>
#include <QDebug>
#include <QDialog>

#include "ccalculos.h"
#include "qcustomplot.h"
#include "dancalc.h"
#include "customplotf.h"

namespace Ui {
class c_GrafPlazoleta;
}

class c_GrafPlazoleta : public QWidget
{
    Q_OBJECT

public:
    explicit c_GrafPlazoleta(QWidget *parent = 0);
    ~c_GrafPlazoleta();
//    CCalculos *calculos;
    int banda{};//metrica = 0, decimetrica = 1
    QString fichero;
//    QCPPlotTitle * plottitle;
    QCPTextElement* plottitle;
    double antena{};
    void initGraph();
    void pintar();
    QList<double> distanciasPlazoleta;//lista de distancia segun tabla
    QList<double> listdistplazoleta[360];//lista de los interceptos entre el perfil y los puntos de la horizontal en los 360 grados
    QList<double> InterceptoAnguloyPerfil[360];
    QList<double> sumDisInternas;
    double ContSumDisInterna{};
    void pintarDistancias();
    QList<double> anguloincidencia[360];
    QList<double> anguloincidencias;

    QStringList  extensionPlazoleta[360];

    QStringList deformaciones[360];

    QCPItemTracer *pTracerCalifGeneral{};
    QList<QCPItemTracer *> listpTracerCalifGeneral;

    double Disvuelo{};

    void filePrintPdf(bool cual);
    void llenartabla(bool llenar);
    void pintarCalifGeneral();
    QStringList califGeneral;


    void pintaElipses(QCustomPlot *customplot, int coord, bool text, const QString &t, bool align);
    void pintaLinea(QCustomPlot *customPlot);
public slots:
    void mouseMove(QMouseEvent *event);
    void cerrartabla(int number);
    void salvarTablas();

private slots:
    void mouseWheel(QWheelEvent *event);
    void on_pB_ZoomMas_clicked();

    void on_pB_ZoomMen_clicked();

    void on_pB_SalvarGrafica_clicked();

    void on_checkBox_toggled(bool checked);
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::c_GrafPlazoleta *ui;

    DanCalc *danca;
    QTableWidget * tabla;
    QTableWidgetItem*tablaitem{};
    QDialog *dialogTabla;
    QPushButton *salvarTabla;
};

#endif // C_GRAFPLAZOLETA_H
