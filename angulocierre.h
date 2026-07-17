#ifndef ANGULOCIERRE_H
#define ANGULOCIERRE_H

#include <QWidget>
#include "angcierre1.h"
#include "angucierre2.h"
#include "ccalculos.h"
#include <QStatusBar>
namespace Ui {
class angulocierre;
}

class angulocierre : public QWidget
{
    Q_OBJECT

public:
    explicit angulocierre(QWidget *parent = 0);
    ~angulocierre();

    CCalculos *calculos{};
    QStatusBar * barra;
    QString fichero;
    int arma{},medio{},grupo{};
    int camara{};
    int calibre{};
    QString cohete;
    int angulositio{};
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

signals:
    void cerro();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::angulocierre *ui;
    angcierre1 *forma1{};
    AnguCierre2 * forma2{};
    QList<AnguCierre2*> lisAnguCierre2;
    QList<angcierre1*> lisangcierre1;
};

#endif // ANGULOCIERRE_H
