#ifndef ZDCAAENTRADA_H
#define ZDCAAENTRADA_H

#include <QDialog>

namespace Ui {
class ZDCAAEntrada;
}

class ZDCAAEntrada : public QDialog
{
    Q_OBJECT

public:
    explicit ZDCAAEntrada(QWidget *parent = 0);
    ~ZDCAAEntrada();

    int gradlong{};
    int gradlat{};
    int minlong{};
    int minlat{};
    int seglong{};
    int seglat{};
    double Alrampa{};
    QString cohete;
    // double CoefK;
    QString file;
    int medio{};
    int medios();
    int grupo{};
    void grupos(int g);
//    int velocidad;
//    int tiempo;
private slots:
    void on_comboBox_2_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

//    void on_cB_Velocidad_currentTextChanged(const QString &arg1);

//    void on_cB_Tiempo_currentTextChanged(const QString &arg1);

private:
    Ui::ZDCAAEntrada *ui;
};

#endif // ZDCAAENTRADA_H
