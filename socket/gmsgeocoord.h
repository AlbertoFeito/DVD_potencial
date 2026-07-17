#ifndef GMSGEOCOORD_H
#define GMSGEOCOORD_H

#include <QFile>


struct GMSG
{
    short G;
    uchar M;
    uchar S;
    GMSG()
    {
        G = 0; M = 0; S = 0;
    }
    GMSG(short g, uchar m, uchar s)
    {
        G = g; M = m; S = s;
    }
    bool operator ==(GMSG gms)
    {
        return (gms.G == G && gms.M == M && gms.S == S);
    }
    bool operator !=(GMSG gms)
    {
        return (gms.G != G || gms.M != M || gms.S != S);
    }
};



static class gmsGeoCoord
{
public:
    gmsGeoCoord(void);
    ~gmsGeoCoord(void);

    static double GmsToGrados(GMSG gms);
    static GMSG GradosToGms(double Valor);
//    static short determinarAltura(GMS latPos, GMS lonPos);

}gmsCoord;

#endif // GMSGEOCOORD_H
