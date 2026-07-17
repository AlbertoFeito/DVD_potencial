#ifndef DLGSELECPO_H
#define DLGSELECPO_H

#include <QDialog>

namespace Ui {
class dlgSelecPO;
}

class dlgSelecPO : public QDialog
{
    Q_OBJECT

public:
    explicit dlgSelecPO(QWidget *parent = 0);
    ~dlgSelecPO();
    bool seleccion;
private slots:

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();
private:
    Ui::dlgSelecPO *ui;
};

#endif // DLGSELECPO_H
