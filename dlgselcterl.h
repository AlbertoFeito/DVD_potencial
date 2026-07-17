#ifndef DLGSELCTERL_H
#define DLGSELCTERL_H


#include <QDialog>

namespace Ui {
class DlgSelctERL;
}

class DlgSelctERL : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSelctERL(QWidget *parent = 0);
    ~DlgSelctERL();

    int medio{};
private slots:
    void on_pB_Aceptar_clicked();

    void on_pB_Cancelar_clicked();

private:
    Ui::DlgSelctERL *ui;
};

#endif // DLGSELCTERL_H
