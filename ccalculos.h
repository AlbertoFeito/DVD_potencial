#ifndef CCALCULOS_H
#define CCALCULOS_H

#include <QObject>
#include "cdatoazimut.h"
#include "utiles.h"
//#include "distanthread.h"
//#include "cdistanciabiestatico.h"

//struct PUNTO
//{
//    double X;
//    double Y;
//};

class CCalculos : public QObject
{
    Q_OBJECT
public:
    explicit CCalculos(QObject *parent = 0);
    ~CCalculos();


    GMS latPos, lonPos;
    double AlAnt{};
    PUNTO** puntos;//arreglo bidimencional
    CDatoAzimut DatosAzimut[360];
    CDatoTerreno datoterrenoMiPos;

    bool cambios{};
    QString fichero;
    QString label;

    void calcularDatos();
    bool guardar();
    bool leer();

    void nombres(QString *, QString *);

int dismax;
    bool final;
    int medio{};
    int arma{};
    int camara{};
    float calibre{};
    QString cohete;
    int grupo{};
    void distanciaGrupo(int g, int m);//distancia segun el grupo para los calculos

    int length1{};
    int length2{};
    void nombres();
    int angulositio;

    QString tipoBiesta;
    int determDistanCiclo(int g, int a, int m);
signals:
    void fin(bool termino,int bar);
    //    void cooperacion(int avuelo, CCalculos *calculos,QString nombre);
public slots:
private:
//    CDistanciaBiestatico DisBies;

};

#endif // CCALCULOS_H
