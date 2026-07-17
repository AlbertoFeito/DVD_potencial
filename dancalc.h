#ifndef DANCALC_H
#define DANCALC_H

#include <QObject>
//#include "../General/fuente.h"

class DanCalc : public QObject
{
    Q_OBJECT
public:
    DanCalc(QObject *parent = 0);
    ~DanCalc();


public:

    double fAnchoLat[26]{};
    double fSumaLat[26]{};
    double fPmapaY[26]{};
    double fDmapaY[26]{};
    double fAnchoLong[26]{};
    double fDifLong[26]{};
    // Operations
public:
    double Convierte_x_CoordMapa_x(double,double);
    double Convierte_y_CoordMapa_y(double);
    double Convierte_pixel_CoordMapa_x(int);
    double Convierte_pixel_CoordMapa_y(int);
    void Convierte_Paral_Pantalla(qint16 , qint16 , qint16 ,float * );
    //void Convierte_Paral_Pantalla(WORD , float ,float * );
    void Convierte_Merid_Pantalla(qint16 , qint16 , qint16 ,float * );
    //void Convierte_Merid_Pantalla(WORD , float ,float * );
    double Convierte_Long_CoordMapa_x(double );
    double Convierte_Lat_CoordMapa_y(double );
    double Convierte_CoordMapaX_a_Geog(double);
    double Convierte_CoordMapaY_a_Geog(double);
    double Convierte_CoordXGeog_a_Km(double,double,double,double);
    double Convierte_CoordYGeog_a_Km(double,double);
    //double Convierte_KmY_a_GeogLat(double,double);
    //double Convierte_KmX_a_GeogLong(double,double,double);
    double Distancia(double,double);
    double Calcula_Lat(double);
    double Coeficiente_K(double);


    void RealToPixel(double Xreal, double Yreal, int* Xpixel, int* Ypixel,double yFtekm, double LatFte, double LongFte);
    void PolarToReal(double Angulo, double Distancia,quint8 ,double* Xreal, double* Yreal);

    double Latitud_2(double dy, double Latitud_1);
    double Longitud_2(double dx, double Longitud_1, double Latitud_2);
    double Abcisa_Obj(double Latitud_1, double Longitud_2);
    double Ordenada_Obj(double Latitud_2);
    void Traslacion_Del_Radar_Al_OrigenComun(double dx, double dy, double *p_xoc, double *p_yoc,double dLat,double dLong);
    //void Conviertea_HmMapa(Fuente *Fte,double dx, double dy,double *sX_HmOrigen, double *sY_HmOrigen);
    int CalculaXPantalla(double , double );
    int CalculaYPantalla(double );

    int Aprox(double n);


signals:

public slots:

};

#endif // DANCALC_H
