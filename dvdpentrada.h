#ifndef DVDPENTRADA_H
#define DVDPENTRADA_H

#include <QDialog>

#include <QSettings>

namespace Ui {
class DVDPEntrada;
}

class DVDPEntrada : public QDialog
{
    Q_OBJECT

public:
    explicit DVDPEntrada(QWidget *parent = 0);
    ~DVDPEntrada();

    int gradlong{};
    int gradlat{};
    int minlong{};
    int minlat{};
    int seglong{};
    int seglat{};
    double AlAntena{};
    // double CoefK;
    QString file;
    int arma{};
    int medio{};
    int camara{};
    int angulositio;
public slots:
    void selecionArma();
private slots:
    void on_pB_Aceptar_clicked();

    void on_pB_Cancelar_clicked();

private:
    Ui::DVDPEntrada *ui;
};

#endif // DVDPENTRADA_H
