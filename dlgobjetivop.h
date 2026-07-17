#ifndef DLGOBJETIVOP_H
#define DLGOBJETIVOP_H

#include <QDialog>

namespace Ui {
class dlgObjetivoP;
}

class dlgObjetivoP : public QDialog
{
    Q_OBJECT

public:
    explicit dlgObjetivoP(QWidget *parent = 0);
    ~dlgObjetivoP();

private slots:
    void on_pB_Aceptar_clicked();

private:
    Ui::dlgObjetivoP *ui;
};

#endif // DLGOBJETIVOP_H
