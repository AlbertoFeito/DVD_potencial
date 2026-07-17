#ifndef ZPFAAA_H
#define ZPFAAA_H

#include <QWidget>
#include <QDir>
#include <QProcess>
#include<QStatusBar>

#include "zpfaaaentrada.h"
#include "hilo.h"
#include "perfil.h"
#include "angulocierre.h"
#include "dvdpselaltvuelo.h"
namespace Ui {
class ZPFAAA;
}

class ZPFAAA : public QWidget
{
    Q_OBJECT

public:
    explicit ZPFAAA(QWidget *parent = 0);
    ~ZPFAAA();

    QStatusBar * barra;

    int gradlong{};
    int gradlat{};
    int minlong{};
    int minlat{};
    int seglong{};
    int seglat{};
    float AlCannon{};
    QString file;
    float calibre{};
    QDir DirIni;
    int arma{};
    int medio{};
    int grupo{};
    void seleccion(bool select);
    bool selecion{};
public slots:
    void completo(bool terminado, int ba);
    void distaCerro();
    void perCerro();
    void anguloCerro();
    void cancelarEntrada();
signals:
    void termino();
private slots:
    void on_pB_EntrarDatos_clicked();

    void on_pB_Perfil_clicked();

    void on_pB_AnguloCierre_clicked();

    void on_pB_ZPFAAA_clicked();

    void on_pB_Cerrar_clicked();

protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::ZPFAAA *ui;

    hilo *hilo1;
    Perfil perfil;
    angulocierre angcierre;
    dvdpSelAltVuelo selAltVuelo;

};
#endif // ZPFAAA_H
