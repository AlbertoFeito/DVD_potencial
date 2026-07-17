#ifndef GRAFICAPERFIL_H
#define GRAFICAPERFIL_H

#include <QWidget>
#include "qcustomplot.h"
#include "customplotf.h"
#include "ccalculos.h"
namespace Ui {
class Graficaperfil;
}

class Graficaperfil : public QWidget
{
    Q_OBJECT

public:
    explicit Graficaperfil(QWidget *parent = 0);
    ~Graficaperfil();

    CCalculos *calculos{};
    int aazimut{};
    QString fichero;
    double k{};
    int angulositio;
    void initGraph();
    void pintar();

     QCPTextElement *plottitle;
     QCPTextElement *plottitle2;

     QCPItemText *textgrado{};
     QList<QCPItemText*> Listtextgrado;

    void pintartexto(const QString& t, double x, double y, Qt::Alignment alignment, const QColor& color);
    double x{};
    double y{};
    QCPItemTracer *phaseTracer{};
    QList<QCPItemTracer *> listphaseTracer;
    QList<QCPItemTracer *> listphaseTracerDis;

    double posicion{};

    void filePrintPdf();

    int distanciaCiclo{};
    int distanciaAC{};
    int determDistanCiclo(int g, int a, int m);

    bool leer(const QString& extencion, int curva);
    int cont;
    QList<double> disVDirecta[360];
    QList<double> disVDirecta2[15];

public slots:
    void seleccionAC(bool cambio);
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);
    void itemClicked(QCPAbstractItem *item,QMouseEvent* event);
    void legendItemClicked(QCPLegend *legend,QCPAbstractLegendItem *legendItem,QMouseEvent *event);
    void widgetClik();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void mouseWheel(QWheelEvent *event);
    void mouseMove(QMouseEvent * event);
    void selectionChanged();

    void on_cheB_AngulosDeSitio_toggled(bool checked);

    void on_cheB_leyenda_toggled(bool checked);

    void on_pB_Zoommas_clicked();

    void on_pB_Zoommen_clicked();

    void on_pushButton_4_clicked();

    void on_checkBox_toggled(bool checked);

private:
    Ui::Graficaperfil *ui;
};

#endif // GRAFICAPERFIL_H
