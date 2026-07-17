#ifndef ANGCIERRE1_H
#define ANGCIERRE1_H

#include <QWidget>
#include <QPoint>
#include <QRubberBand>

#include "ccalculos.h"
#include "qcustomplot.h"
#include "customplotf.h"

class QCustomPlotZoom : public QCustomPlot
{
    Q_OBJECT

public:
    QCustomPlotZoom(QWidget * parent = 0);
    virtual ~QCustomPlotZoom();

    void setZoomMode(bool mode);

protected:

private:
    bool mZoomMode;
    QRubberBand * mRubberBand;
    QPoint mOrigin;
};
namespace Ui {
class angcierre1;
}

class angcierre1 : public QWidget
{
    Q_OBJECT

public:
    explicit angcierre1(QWidget *parent = 0);
    ~angcierre1();

    CCalculos *calculos{};
    QString fichero;
    void initGraph();

    void pintar();

    QCPTextElement* plottitle;

    int arma{},medio{},grupo{};    
    int distanciaArma{};

    void determDistanCiclo(int g, int a, int m);
public slots:
    void mouseWheel(QWheelEvent * event);
    void mouseMove(QMouseEvent * event);
private slots:
    void on_pB_Zoommas_clicked();

    void on_pB_Zoommen_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::angcierre1 *ui;

    bool mZoomMode{};
    QRubberBand * mRubberBand{};
    QPoint mOrigin;
};

#endif // ANGCIERRE1_H
