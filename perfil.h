#ifndef PERFIL_H
#define PERFIL_H

#include <QDialog>
#include "graficaperfil.h"
#include <QStatusBar>
#include "ccalculos.h"

namespace Ui {
class Perfil;
}

class Perfil : public QWidget
{
    Q_OBJECT

public:
    explicit Perfil(QWidget *parent = 0);
    ~Perfil();

    CCalculos *calculos{};
    QStatusBar * barra;

    QString fichero;
    int arma;
    int grupo{}, medio{};
    int camara;
    int calibre{};
    QString cohete;
    int angulositio{};
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

signals:
    //void azimut(int a,QStringList datosaltura1, QStringList tanAlfa1, float antena,QString fichero,double cfk,double RDistancia, QStringList disangcierre);
  void cerro();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::Perfil *ui;
    Graficaperfil *grafperfil;
};

#endif // PERFIL_H
