#ifndef DISTANCIAVD_H
#define DISTANCIAVD_H

#include <QWidget>
#include "qcustomplot.h"
#include "defcomunes.h"
#include "dancalc.h"
#include "customplotf.h"
#include <QVector>
#include <QDebug>
#include <QDialog>

namespace Ui {
class DistanciaVD;
}

class DistanciaVD : public QWidget
{
    Q_OBJECT

public:
    explicit DistanciaVD(int dismax, QWidget *parent = 0);
    ~DistanciaVD();

    bool alcance;
    void initGraph();

    QList<double> disVDirecta[360];
    QList<double> disInternas[360];
    QList<double> listAreas;

    QList<double> ZonaMuertaMuis[360];
    QList<double> ZonaMuertaMuis2[360];
    QList<double> ZonaMuertaCCAA[360];
    QList<double> zonaMuertaMuisInternas[360];
    QList<double> zonaMuertaMuisInternas2[360];

    int alvue;
    QString label;
    QString fichero;
    QString prob;
    int p{};
    bool pintarDistancias(int dis);
    bool pintarObjetoLocal(int dis);
    //    void filePrintPdf(bool cual);

    QCPTextElement* plottitle;

    void pintarZonaMuerta(double zonamuerta);
    void pintarZonaMuertaMuis();
    void pintarZonaMuertaCCAA();
//    void pintaLinea(QCustomPlot *customPlot, QString layer, QPen pen, double xIni,double yIni,double xFin,double yFin);

    int arma{};
    QString titulo;
    int angulositio{};
//    void pintaElipses(QCustomPlot *customplot, int coord, bool text, bool align);
//    void pintaLineaAzimut(QCustomPlot *customPlot);

    void llenartabla(bool llenar,int medios);
    void tablaProbabilidad(QString p,int medios);
QStringList datosProbabilidad;
    void calcularArea(int dis);
    double ListAreas[3][3]{};
    double dedZone;
    double Area;
    double areaCirculo = 0;
    double areaCuadrado = 0;
    double AreaZM;
    double AreaZM2;
    double D0 ,D1,r,r2,r3;
    double DLCmin{};
    double DRCmin{};
    double DECCM{}  ;
    int T{}, V{};
 int medio{};
 int grupo{};
 void configInterface(int g, int m, int a);
 CustomPlotF customplotf;
signals:
    void cerro(DistanciaVD*);

public slots:
    void mouseMove(QMouseEvent *event);
    void cerrartabla(int number);
    void filePrintPdf(bool cual);
    void cerrartablaP(int number);
    void salvaTablaP(bool b);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pB_SalvarGrafica_clicked();

//    void on_pB_SalvarTabla_clicked();

    void on_pB_ZoomMas_clicked();

    void on_pB_ZoomMen_clicked();


    void on_chB_ZMuerta1_toggled(bool checked);

    void on_chB_ZMuerta2_toggled(bool checked);

    void on_chB_Tabla_toggled(bool checked);

    void on_chB_Probabilidad_toggled(bool checked);

    void on_chB_DLanzamiento_toggled(bool checked);

    void on_chB_DZDRealizanle_toggled(bool checked);

    void on_chB_DECCMin_toggled(bool checked);

    void on_chB_DZDRealizanle_clicked();

    void on_chB_DECCMin_clicked();

    void on_chB_ZMuerta2_clicked();

    void on_chB_ZMuerta1_clicked();

    void on_chB_DLanzamiento_clicked();

    void on_chB_Tabla_clicked();

    void on_chB_DLanzMin_toggled(bool checked);


    void on_chB_DZDReaMin_toggled(bool checked);

    void on_pB_Sector_clicked();

private:
    Ui::DistanciaVD *ui;

    QHeaderView* headerView;
    QTableWidget * tabla;
    QTableWidgetItem*tablaitem{};
    QDialog *dialogTabla;
    QPushButton *salvarTabla;
    QPushButton *salvarTablaP;

    QHeaderView* headerViewP;
    QTableWidget * tablaP;
    QTableWidgetItem*tablaitemP{};
    QDialog *dialogTablaP;
int m_dismax;
};

#endif // DISTANCIAVD_H
