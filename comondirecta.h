#ifndef COMONDIRECTA_H
#define COMONDIRECTA_H

#include <QWidget>
#include "qcustomplot.h"
#include "customplotf.h"
namespace Ui {
class ComonDirecta;
}

class ComonDirecta : public QWidget
{
    Q_OBJECT

public:
    explicit ComonDirecta(QWidget *parent = 0);
    ~ComonDirecta();

    void initGraph();
    void pintarComones();
    QList<double> datosterreno;
    double distancia;
    double AlPos;
    double Antena1;
    double Antena2;
    double azimut;
    double pos1{};

    QString fichero;
    double RelAsp;

    void filePrintPdf();
//    QCPPlotTitle * plottitle;
    QCPTextElement* plottitle;

public slots:
    void mouseWheel();
    void mouseMove(QMouseEvent *event);
private slots:
    void on_pB_Zoommas_clicked();

    void on_pB_Zoommen_clicked();

    void on_pushButton_4_clicked();

    void selectionChanged();
private:
    Ui::ComonDirecta *ui;
};

#endif // COMONDIRECTA_H
