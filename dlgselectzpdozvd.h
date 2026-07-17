#ifndef DLGSELECTZPDOZVD_H
#define DLGSELECTZPDOZVD_H

#include <QDialog>

namespace Ui {
class dlgSelectZPDoZVD;
}

class dlgSelectZPDoZVD : public QDialog
{
    Q_OBJECT

public:
    explicit dlgSelectZPDoZVD(QWidget *parent = 0);
    ~dlgSelectZPDoZVD();

    bool seleccion{};
private slots:
    void on_pB_Aceptar_clicked();

    void on_pB_Cancelar_clicked();

private:
    Ui::dlgSelectZPDoZVD *ui;
};

#endif // DLGSELECTZPDOZVD_H
