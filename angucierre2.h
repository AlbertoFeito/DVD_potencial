#ifndef ANGUCIERRE2_H
#define ANGUCIERRE2_H

#include <QWidget>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDebug>

#include "qcustomplot.h"
#include "defcomunes.h"
#include "ccalculos.h"
#include "customplotf.h"

namespace Ui {
class AnguCierre2;
}

class AnguCierre2 : public QWidget
{
    Q_OBJECT

public:
    explicit AnguCierre2(QWidget *parent = 0);
    ~AnguCierre2();

    CCalculos *calculos{};

    QString Fichero;
    QString titulo;
    double disan{};

    void pintar();

    void filePrintPdf(bool cual);

    QCPTextElement* plottitle;

    int arma{},medio{},grupo{};
    int camara{};
    int distanciaArma{};

    struct ANGCDIST
    {
        QString angulocierre;
        QString distanciaAngC;
    }angcdist;
    ANGCDIST listAngDist[360];

    void determDistanCiclo(int g, int a, int m);

    QVector<double> x1, y1;
    void determAC();
    void guardarAC();

public slots:
    void mouseMove(QMouseEvent *event);

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pB_Zoommas_clicked();

    void on_pB_Zoommen_clicked();


private:
    Ui::AnguCierre2 *ui;
};

#endif // ANGUCIERRE2_H
