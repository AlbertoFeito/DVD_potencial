#ifndef DLGSELECZPFOZVD_H
#define DLGSELECZPFOZVD_H

#include <QDialog>

namespace Ui {
class dlgSelecZPFoZVD;
}

class dlgSelecZPFoZVD : public QDialog
{
    Q_OBJECT

public:
    explicit dlgSelecZPFoZVD(QWidget *parent = 0);
    ~dlgSelecZPFoZVD();

    bool seleccion;
private slots:
    void on_pB_Aceptar_clicked();

    void on_pB_Cancelar_clicked();

private:
    Ui::dlgSelecZPFoZVD *ui;
};

#endif // DLGSELECZPFOZVD_H
