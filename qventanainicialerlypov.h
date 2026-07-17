#ifndef QVENTANAINICIALERLYPOV_H
#define QVENTANAINICIALERLYPOV_H

#include <QWidget>
#include <QStatusBar>
#include <QDir>
#include <QThread>
#include <QFile>

#include "dvdpentrada.h"
#include "ccalculos.h"
#include "dancalc.h"
#include "hilo.h"
#include "perfil.h"
#include "angulocierre.h"
#include "dvdpselaltvuelo.h"
#include "c_plazoletazc.h"
namespace Ui {
class QVentanaInicialERLyPOV;
}

class QVentanaInicialERLyPOV : public QWidget
{
    Q_OBJECT

public:
    explicit QVentanaInicialERLyPOV(QWidget *parent = 0);
    ~QVentanaInicialERLyPOV();

    QStatusBar * barra;
    hilo *hilo1;
    void verPuntoIni();
    int arma;
    int medio{};
    int grupo{};

int numWindow;

    int angulositio{};
    void seleccionArma(bool erl);
public slots:

    void completo(bool terminado, int ba);

    void cancelarEntrada();

    void distaCerro();

    void anguloCerro();

    void perCerro();

    void plazoletaCerrometri();

    void plazoletaCerroDeci();

//    void biestaticocerro(int num);

private slots:
    void on_pB_EntrarDatos_clicked();

    void on_pB_BandaMetrica_clicked();

    void on_pB_BandaDecimetrica_clicked();

    void on_pB_Perfil_clicked();

    void on_pB_AngulosCierre_clicked();

    void on_pB_DVDP_clicked();

    void on_pB_Cerrar_clicked();


signals:
    void termino();
    void biestatico(int numVentana);

protected:
     void closeEvent(QCloseEvent *event);

private:
    Ui::QVentanaInicialERLyPOV *ui;

    Perfil perfil;
    angulocierre angcierre;
    dvdpSelAltVuelo selAltVuelo;

    C_PlazoletaZC plazoletaZCMetrica;
    C_PlazoletaZC plazoletaZCDeci;

};

#endif // QVENTANAINICIALERLYPOV_H
