#ifndef DVDPSELALTVUELO_H
#define DVDPSELALTVUELO_H

#include <QWidget>
#include <QStatusBar>

#include "dancalc.h"
#include "distanthread.h"
#include "distanciavd.h"
#include "ccalculos.h"
#include "veralturas.h"
namespace Ui {
class dvdpSelAltVuelo;
}

class dvdpSelAltVuelo : public QWidget
{
    Q_OBJECT

public:
    explicit dvdpSelAltVuelo(QWidget *parent = 0);
    ~dvdpSelAltVuelo();

    QStatusBar * barra;

    CCalculos *calculos{};

    bool dm00;
    bool dm25;
    bool dm50;
    bool d100;
    bool d200;
    bool d300;
    bool d400;
    bool d500;
    bool d1000;
    bool d2000;
    bool d4000;
    bool d6000;
    bool d10000;
    bool d17000;
    bool d30000;
    bool dObjetLocale;
    bool todoCalculado;

    int alvue;
    int medio{};
    void calcular();
    void parar();

    void radioButons(int g, int m);
    int angulositio{};

    QPen EscalaColor[15];
    int confile;
    // QRadialGradient radialGrad(QPointF(310, 180), 200);
    void guardar();
    void configInterface();
    int probabilidad;
    QString extension(int h);

    int velocidad;
    int tiempo;
    int parametro;
signals:
    void cerro();

public slots:
    void progreso(int c, const QString& mensaje);

    void findhilo(bool final);

private slots:
    void on_pB_Aceptar_clicked();

    void on_pB_Cerrar_clicked();

    void on_pB_Cancelar_clicked();

    void on_pB_BAltura_clicked();

    void on_pB_MAltura_clicked();

    void on_pB_GAltura_clicked();

    void on_cB_Probabildad_currentIndexChanged(int index);

    void on_pB_TProbabilidades_clicked();

    void on_cB_Tiempo_currentTextChanged(const QString &arg1);

    void on_cB_Velocidad_currentTextChanged(const QString &arg1);

    void on_pB_Aceptar_toggled(bool checked);

    void on_pB_ObjetosLocales_clicked();

    void on_chB_Alcance_toggled(bool checked);

    void on_cB_Sigma_currentIndexChanged(int index);

    void on_cB_Banda_currentIndexChanged(int index);

protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::dvdpSelAltVuelo *ui;

    distanThread dista;

    QList<DistanciaVD*> lisCustomPlot;
    QList<VerAlturas*> lisverAlturas;
    DistanciaVD *customp{};

    bool alcance;

    void actualizarVisibilidadSigma();
    void actualizarVisibilidadBanda();
    void configurarCombosSigmaYBanda();
};

#endif // DVDPSELALTVUELO_H
