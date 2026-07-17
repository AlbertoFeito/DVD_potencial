#ifndef QFICHEROINI_H
#define QFICHEROINI_H

#include <QString>
#include <QDir>
#include <QSettings>
#include <QMessageBox>
class QFicheroINI
{
public:
    QFicheroINI();

    int gradlong{};
    int gradlat{};
    int minlong{};
    int minlat{};
    int seglong{};
    int seglat{};
    double AlAntena{};
    double altcannon{};
    double Alrampa{};
    double calibre{};
    int cooperacion{};
    int arma{};
    int medio{};
    int camara{};

    QString sTrabIni;
    QDir DirIni;


    void readInicio();
    void readIni(const QString& group);
    void readZPD();
    void readZPF();
    double DatDelMedio(int m);
    void Salir(const QString& group);
};

#endif // QFICHEROINI_H
