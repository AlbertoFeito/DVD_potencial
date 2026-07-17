#ifndef ZPDCAAA_H
#define ZPDCAAA_H

#include <QWidget>
#include<QStatusBar>
#include <QProcess>
#include <QDir>

#include "hilo.h"
#include "zdcaaentrada.h"
#include "perfil.h"
#include "angulocierre.h"
#include "dvdpselaltvuelo.h"

namespace Ui {
class ZPDCAAA;
}

class ZPDCAAA : public QWidget
{
    Q_OBJECT

public:
    explicit ZPDCAAA(QWidget *parent = 0);
    ~ZPDCAAA();

    QStatusBar * barra;

    int gradlong{};
    int gradlat{};
    int minlong{};
    int minlat{};
    int seglong{};
    int seglat{};
    float Alrampa{};
    QString cohete;
    // double CoefK;
    QString file;
    QDir DirIni;
    int medio{};
    int arma{};
    int grupo{};
    void seleccion(bool select);
    bool selecciona{};
    int velocidad{};
    int tiempo{};
public slots:
    void completo(bool terminado, int ba);

    void distaCerro();
    void perCerro();
    void anguloCerro();
    void cancelarEntrada();
signals:
    void termino();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pB_EntrarDatos_clicked();

    void on_pB_Perfil_clicked();

    void on_pB_AnguloCierre_clicked();

    void on_pB_ZPDCAA_clicked();

    void on_pB_Cerrar_clicked();    

private:
    Ui::ZPDCAAA *ui;

    hilo *hilo1;
    Perfil perfil;
    angulocierre angcierre;

    dvdpSelAltVuelo selAltVuelo;
};

#endif // ZPDCAAA_H
