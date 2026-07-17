#ifndef QINICIAL_H
#define QINICIAL_H

#include <QWidget>
#include <QStatusBar>
#include <QDir>
#include <QThread>
#include <QFile>
#include <QProcess>

#include "qventanainicialerlypov.h"
#include "ccalculos.h"
#include "cpuntoapunto.h"
#include "zpfaaa.h"
#include "zpdcaaa.h"
#include "dlgseleczpfozvd.h"
#include "dlgselectzpdozvd.h"
#include "dlgselcterl.h"
#include "qficheroini.h"

#include "dlgbiestatico.h"
#include "campos.h"
#include "trabsasa.h"

namespace Ui {
class QInicial;
}

class QInicial : public QWidget
{
    Q_OBJECT

public:
    explicit QInicial(QWidget *parent = 0);
    ~QInicial();


    QDir DirIni;

    void SASA(const QString& group);

    int alturaVuelo(int i,int g, int m);
    int medio{};
    QList<int> velocidad(int m);
public slots:
    void estadoSasa(bool estado);
    void trabIniciado();
    void slTimerSASA();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pB_PO_clicked();

    void on_pB_AAA_clicked();

    void on_pB_CCAA_clicked();

    void on_pB_ERL_clicked();

    void on_pB_PuntoAPunto_clicked();

    void on_pB_Ayuda_clicked();

    void on_pB_Cerrar_clicked();

    void on_pB_Campo_clicked();

private:
    Ui::QInicial *ui;

    ZPFAAA *zpfaaa{};
    ZPDCAAA *zpdcaaa{};
    QVentanaInicialERLyPOV *VentanaInicialERLyPOV{} ;
    QVentanaInicialERLyPOV *VentanaInicialBiestatico{} ;
    CPuntoAPunto comunicacion;
    Campos *campo{};


    TrabSasa trabsasa;
    bool estadoCalculo;
QTimer timerSASA;

};

#endif // QINICIAL_H
