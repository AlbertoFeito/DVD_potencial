#ifndef C_PLAZOLETAZC_H
#define C_PLAZOLETAZC_H

#include <QWidget>
#include "ccalculos.h"
#include "c_perfizc.h"
#include "c_grafplazoleta.h"
#include "hilografplazoleta.h"
namespace Ui {
class C_PlazoletaZC;
}

class C_PlazoletaZC : public QWidget
{
    Q_OBJECT

public:
    explicit C_PlazoletaZC(QWidget *parent = 0);
    ~C_PlazoletaZC();

    CCalculos *calculos{};
    QString fichero;

    int banda{};//metrica = 0, decimetrica = 1

    int azimut;
    double Disvuelo{};
signals:
    void cerro();
public slots:
    void progreso(int c, const QString& mensaje);

    void findhilo(bool final);

    void findhiloPerfil(bool final);
private slots:
    void on_pB_Plazoleta_clicked();

    void on_pB_PerfilZC_clicked();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::C_PlazoletaZC *ui;
    C_PerfiZC *perfilZC;
    QList<c_GrafPlazoleta*> lisCustomPlot;
    c_GrafPlazoleta *grafplazoleta;
    HiloGrafPlazoleta hilografplazoleta;
    HiloGrafPlazoleta hiloperfil;
};

#endif // C_PLAZOLETAZC_H
