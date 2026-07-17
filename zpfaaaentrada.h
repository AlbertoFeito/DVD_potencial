#ifndef ZPFAAAENTRADA_H
#define ZPFAAAENTRADA_H

#include <QDialog>

namespace Ui {
class ZPFAAAEntrada;
}

class ZPFAAAEntrada : public QDialog
{
    Q_OBJECT

public:
    explicit ZPFAAAEntrada(QWidget *parent = 0);
    ~ZPFAAAEntrada();

    int gradlong{};
    int gradlat{};
    int minlong{};
    int minlat{};
    int seglong{};
    int seglat{};
    double AlCannon{};
    // double CoefK;
    QString file;
    float calibre{};
    int medio{};
    int medios();
    int cooperacion;
    int grupo{};
    int arma{};
    void grupos(int g);

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ZPFAAAEntrada *ui;
};

#endif // ZPFAAAENTRADA_H
