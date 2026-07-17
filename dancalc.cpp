
#include "defcomunes.h"
#include <cmath>
//#include "danview.h"
#include "dancalc.h"


/*extern*/ double Esq_a,Esq_b,Esq_c,Esq_d;	//mio pendiente son externas	//esquinas del mapa
/*extern*/ int max_x,max_y;                 //valor máximo de la pantalla en pixel


DanCalc::DanCalc(QObject *parent) :
    QObject(parent)
{
    fAnchoLat[0]=110.4607462;fAnchoLat[1]=110.4607462;fAnchoLat[2]=110.4794106;
    fAnchoLat[3]=110.4980750;fAnchoLat[4]=110.5167394;fAnchoLat[5]=110.5354038;
    fAnchoLat[6]=110.5540682;fAnchoLat[7]=110.5727326;fAnchoLat[8]=110.5913970;
    fAnchoLat[9]=110.6100614;fAnchoLat[10]=110.6287258;fAnchoLat[11]=110.6473902;
    fAnchoLat[12]=110.6660546;fAnchoLat[13]=110.6847190;fAnchoLat[14]=110.7033834;
    fAnchoLat[15]=110.7220478;fAnchoLat[16]=110.7407122;fAnchoLat[17]=110.7593766;
    fAnchoLat[18]=110.7780410;fAnchoLat[19]=110.7967054;fAnchoLat[20]=110.8153698;
    fAnchoLat[21]=110.8340342;fAnchoLat[22]=110.8526986;fAnchoLat[23]=110.8713630;
    fAnchoLat[24]=110.8900274;fAnchoLat[25]=110.9086918;



    fSumaLat[0]=0.0;fSumaLat[1]=110.4607462;fSumaLat[2]=220.9401568;
    fSumaLat[3]=331.4382318;fSumaLat[4]=441.9549712;fSumaLat[5]=552.4903750;
    fSumaLat[6]=663.0444432;fSumaLat[7]=773.6171758;fSumaLat[8]=884.2085728;
    fSumaLat[9]=994.8186342;fSumaLat[10]=1105.4473600;fSumaLat[11]=1216.0947502;
    fSumaLat[12]=1326.7608048;fSumaLat[13]=1437.4455238;fSumaLat[14]=1548.1489072;
    fSumaLat[15]=1658.870955;fSumaLat[16]=1769.6116672;fSumaLat[17]=1880.3710438;
    fSumaLat[18]=1991.1490848;fSumaLat[19]=2101.9457902;fSumaLat[20]=2212.76116;
    fSumaLat[21]=2323.5951942;fSumaLat[22]=2434.4478928;fSumaLat[23]=2545.3192558;
    fSumaLat[24]=2656.2092832;fSumaLat[25]=2767.117975;



    fPmapaY[0]=187.0;fPmapaY[1]=191.9736422;fPmapaY[2]=196.9503161;
    fPmapaY[3]=201.9330607;fPmapaY[4]=206.9249299;fPmapaY[5]=211.929;
    fPmapaY[6]=216.8866813;fPmapaY[7]=221.8473845;fPmapaY[8]=226.8141389;
    fPmapaY[9]=231.7899886;fPmapaY[10]=236.778;fPmapaY[11]=242.0044243;
    fPmapaY[12]=247.2340344;fPmapaY[13]=252.4700237;fPmapaY[14]=257.7156014;
    fPmapaY[15]=262.974;fPmapaY[16]=268.4093137F;fPmapaY[17]=273.8479405;
    fPmapaY[18]=279.2932015;fPmapaY[19]=284.7484341F;fPmapaY[20]=290.217;
    fPmapaY[21]=296.1624667;fPmapaY[22]=302.1115574F;fPmapaY[23]=308.0679050;
    fPmapaY[24]=314.0351601;fPmapaY[25]=320.017;

    fDmapaY[0]=0.0;fDmapaY[1]=4.9736422;fDmapaY[2]=4.9766739;fDmapaY[3]=4.9827446;fDmapaY[4]=4.9918692;
    fDmapaY[5]=5.0040701;fDmapaY[6]=4.9576813;fDmapaY[7]=4.9607032;fDmapaY[8]=4.9667544;fDmapaY[9]=4.9758497;
    fDmapaY[10]=4.9880114;fDmapaY[11]=5.2264243;fDmapaY[12]=5.2296101;fDmapaY[13]=5.2359893;fDmapaY[14]=5.2455777;
    fDmapaY[15]=5.2583986;fDmapaY[16]=5.4353127;fDmapaY[17]=5.4386268;fDmapaY[18]=5.4452610;fDmapaY[19]=5.4552326;
    fDmapaY[20]=5.4685659;fDmapaY[21]=5.9454667;fDmapaY[22]=5.9490907;fDmapaY[23]=5.9563476;fDmapaY[24]=5.9672551;
    fDmapaY[25]=5.9818399;

    fAnchoLong[0]=109.7304805;fAnchoLong[1]=109.3760911;fAnchoLong[2]=108.9883847;fAnchoLong[3]=108.5674793;
    fAnchoLong[4]=108.1135033;fAnchoLong[5]=107.6265948;fAnchoLong[6]=107.1069023;fAnchoLong[7]=106.5545839;
    fAnchoLong[8]=105.969808;fAnchoLong[9]=105.3527526;fAnchoLong[10]=104.7036058;fAnchoLong[11]=104.0225653;
    fAnchoLong[12]=103.3098384;fAnchoLong[13]=102.5656424;fAnchoLong[14]=101.7902039;fAnchoLong[15]=100.9837591;
    fAnchoLong[16]=100.1465537;fAnchoLong[17]=99.2788427;fAnchoLong[18]=98.3808904;fAnchoLong[19]=97.4529703;
    fAnchoLong[20]=96.4953651;fAnchoLong[21]=95.5083664;fAnchoLong[22]=94.4922750;fAnchoLong[23]=93.4474004;
    fAnchoLong[24]=92.3740607;fAnchoLong[25]=91.272583;

    fDifLong[0]=0.0;fDifLong[1]=0.3543894;fDifLong[2]=0.3877064;fDifLong[3]=0.4209054F;fDifLong[4]=0.4539760;
    fDifLong[5]=0.4869085;fDifLong[6]=0.5196925;fDifLong[7]=0.5523184;fDifLong[8]=0.5847759F;fDifLong[9]=0.6170554;
    fDifLong[10]=0.6491468;fDifLong[11]=0.6810405;fDifLong[12]=0.7127269;fDifLong[13]=0.7441960F;fDifLong[14]=0.7754385;
    fDifLong[15]=0.8064448;fDifLong[16]=0.8372054;fDifLong[17]=0.8677110;fDifLong[18]=0.8979523F;fDifLong[19]=0.9279201;
    fDifLong[20]=0.9576052;fDifLong[21]=0.9869987;fDifLong[22]=1.0160914;fDifLong[23]=1.0448746F;fDifLong[24]=1.0733397;
    fDifLong[25]=1.1014777;
}

DanCalc::~DanCalc()
= default;

//---Función que convierte la coordenada X en Kilometros en coordenadas de Mapa en el plano X---//
double DanCalc::Convierte_x_CoordMapa_x(double Xoc,double dKoc)
{
        return(139.0+(Xoc/dKoc)*4.924);
}
//----Función que convierte la coordenada Y en Kilometros en coordenadas de Mapa en el plano Y----//
double DanCalc::Convierte_y_CoordMapa_y(double Y)
{

        int i=(int)(Y/110.4607462);
        return((((Y-fSumaLat[i])/fAnchoLat[i+1]))*fDmapaY[i+1]+fPmapaY[i]);
}
//----Función que convierte la Longitud en coordenadas de Mapa en el plano X----//
double DanCalc::Convierte_Long_CoordMapa_x(double fLong)
{
        return(139.0F+4.924F*(100.0F-fLong));
}
//----Función que convierte la Latitud en coordenadas de Mapa en el plano Y----//
double DanCalc::Convierte_Lat_CoordMapa_y(double fLat)
{
        int i;
        int nLatBase[]={10,15,20,25,30,35};
        float fPtoBase[]={187.0F,211.929F,236.778F,262.974F,290.217F};
        float fValorBase[]={284.9397738F,284.0253696F,299.4216501F,311.3889148F,340.6155586F};

        for(i=0;i<6;i++)
        {
                if(fLat<nLatBase[i])
                        break;
        }
        i--;
        return(fPtoBase[i]+fValorBase[i]*(float)tan(RADIAN*(fLat-nLatBase[i])));

}

//-----Función que convierte los paralelos en coordenadas de Pantalla-----------//

void DanCalc::Convierte_Paral_Pantalla(qint16 iGrado, qint16 iMin, qint16 iSeg,float * pY1)
{
        float fL; float fH;
        float GradoY[]={187.0F,191.976F,196.950F,201.933F,206.9249F,211.732F,216.8867F,
                221.8474F,226.8142F,231.79F,236.778F,242.0044F,247.234F,252.47F,257.7156F,
                262.974F,268.4093F,273.8479F,279.2932F,284.7484F,290.217F,296.1625F,
                302.1116F,308.0679F,314.0352F,320.017F};
        float SegY[]={.0013816F,.0013824F,.0013841F,.0013866F,.00139F,.0013771F,
                          .001378F,.0013797F,.0013822F,.0013856F,.0014518F,.0014527F,
                          .0014544F,.0014571F,.0014607F,.0015098F,.0015107F,.0015126F,
                          .0015153F,.0015191F,.0016515F,.0016525F,.0016545F,.0016576F,
                          .0016616F};

        fL=iMin*60.0F+iSeg;
        fH=fL*SegY[iGrado-10];
        *(pY1)=fH+GradoY[iGrado-10];

}

//-----Función que convierte los meridianos en coordenadas de Pantalla----//
void DanCalc::Convierte_Merid_Pantalla(qint16 iGrado, qint16 iMin, qint16 iSeg,float * pX1)
{
        float fK; float fL; float fH;

        fK=(100.0F-iGrado)*3600.0F;
        fL=fK-(iMin*60+iSeg);
        fH=fL*0.0013678F;
        *(pX1)=fH+139.0F;
}

//-----Función que convierte los pixel en coordenadas de mapa en el plano X-------//

double DanCalc::Convierte_pixel_CoordMapa_x(int x)
{
        double g; double Px;
                Px=(double)(Esq_c-Esq_a)/max_x;
        g=(double)x*Px+Esq_a;
        return(g);
}

//-----Función que convierte los pixel en coordenadas de mapa en el plano Y--------//
double DanCalc::Convierte_pixel_CoordMapa_y(int y)
{
        double P;
                P=(double)Esq_d-((Esq_d-Esq_b)/max_y)*y;
        return(P);
}
//------Función que cálcula la latitud a partir de la coordenada Y----------//
double DanCalc::Calcula_Lat(double Y)
{
                int i=(int)(Y/110.4607462);
        return(LATCENTRAL+i+((Y-fSumaLat[i])/fAnchoLat[i+1]));
}

//------Función que cálcula el coeficiente K para la conversión de coordenadas--------//

double DanCalc::Coeficiente_K(double LatMedia)
{
        int i=(int)(LatMedia-LATCENTRAL);

        return(fAnchoLong[i]-fDifLong[i+1]*(LatMedia-i-LATCENTRAL));
}

//-----Función que convierte la coordenada de mapa en el plano X en coordenada Geográfica (Longitud)---//
double DanCalc::Convierte_CoordMapaX_a_Geog(double X)
{
        return(double(100.0-(X-139.0)/4.924));
}
//-----Función que convierte la coordenada de mapa en el plano Y en coordenada Geográfica (Latitud)------//
double DanCalc::Convierte_CoordMapaY_a_Geog(double Y)
{
        int Latitud[8]={5,10,15,20,25,30,35,40};
        float PuntoMapa[8]={162.15F,187.0F,211.929F,236.778F,262.974F,290.217F,320.017F,326.0F};
        double ValorBase[7]={284.0367997236,284.9397738555,284.0253696713,299.4216501231,311.3889148841,340.6155586223,342.330};
        //Comparar a Y con los puntos de mapa del arreglo y coger el menor
        int i;
        for(i=0;i<6;i++)
        {
                if(Y<PuntoMapa[i])
                {
                        i--;
                        break;
                }
        }
        return(double(GRADOS *atan((Y-PuntoMapa[i])/ValorBase[i])+Latitud[i]));//57.29577951
}

//-----Función que convierte la coordenada Geográfica en Kilometro en el plano X-----//
double DanCalc::Convierte_CoordXGeog_a_Km(double X1Geog,double X2Geog,double Y1Geog, double Y2Geog)
{
        return((double)(X1Geog-X2Geog)*111.42325 * cos(((Y1Geog+Y2Geog)/2)*RADIAN));
}
//-----Función que convierte la coordenada Geográfica en Kilometro en el plano Y-----//
double DanCalc::Convierte_CoordYGeog_a_Km(double Y1Geog,double Y2Geog)
{
        return((double)((Y2Geog-Y1Geog)*110.26477 + (((Y1Geog+Y2Geog)/2)*0.0186644)));  //060407
}

//-----Función que cálcula la distancia entre dos puntos en Kilometros------//
double DanCalc::Distancia(double X,double Y)
{
        return((double)(sqrt(pow(X,2)+pow(Y,2))));
}

//double DanCalc::Convierte_KmY_a_GeogLat(double Y,double LatCentral)
//{
//	double Temp=3039.5788758+(1.0290129*LatCentral)+(0.0000871*pow(LatCentral,2))+(0.0093322*Y));
//	return(((-55.132385+sqrt(Temp))/0.0093322));
//	return 0;
//}


//------------------------------------------------------------------------------------------------//
void DanCalc::PolarToReal(double Angulo, double Distancia,quint8 pmPolarMode  ,double* Xreal, double* Yreal)
{
        double aciRadianes = Angulo * RADIAN;
        switch (pmPolarMode)
        {
        case (0):
            *Xreal = Distancia * sin(aciRadianes);
            *Yreal = Distancia * cos(aciRadianes);
            break;
        case (1):
            *Xreal = -Distancia * sin(aciRadianes);
            *Yreal = Distancia * cos(aciRadianes);
            break;
        case (2):
            *Xreal = Distancia * sin(aciRadianes);
            *Yreal = -Distancia * cos(aciRadianes);
            break;
        case (3):
            *Xreal = -Distancia * sin(aciRadianes);
            *Yreal = -Distancia * cos(aciRadianes);
            break;
        case (4):
            *Xreal = Distancia * cos(aciRadianes);
            *Yreal = Distancia * sin(aciRadianes);
            break;
        case (5):
            *Xreal = -Distancia * cos(aciRadianes);
            *Yreal = Distancia * sin(aciRadianes);
            break;
        case (6):
            *Xreal = Distancia * cos(aciRadianes);
            *Yreal = -Distancia * sin(aciRadianes);
            break;
        case (7):
            *Xreal = -Distancia * cos(aciRadianes);
            *Yreal = -Distancia * sin(aciRadianes);
            break;
        }
}

void DanCalc::RealToPixel(double Xreal, double Yreal, int* Xpixel, int* Ypixel,double yFtekm, double LatFte, double LongFte)
{
        double LatDato; double LatMedia; double Koc; double Kof;
        Xreal /= 1000;
        Yreal /= 1000;
        Yreal += yFtekm/*dCoorY_Km*/;
        LatDato = Calcula_Lat(Yreal);
        LatMedia = (LATCENTRAL + LatDato) / 2.0;
        Koc = 111.42325 * cos(LatMedia * RADIAN);
        LatMedia = (LatFte/*dLatitud*/ + LatDato) / 2.0;
        Kof = 111.42325 * cos(LatMedia * RADIAN);
        Xreal = Koc * (Xreal / Kof + (LONGCENTRAL - LongFte/*dLongitud*/));
        Xreal = Convierte_x_CoordMapa_x(Xreal, Koc);
        Yreal = Convierte_y_CoordMapa_y(Yreal);
        *Xpixel = Aprox((Xreal - Esq_a) / (Esq_c - Esq_a) * max_x) /*+ Xo*/;
        *Ypixel = Aprox((Esq_d - Yreal) / (Esq_d -Esq_b) * max_y)/* + Yo*/;

        //------------------------------- SIN GEO-REFERENCIA -----------------------------------
        //*Xpixel = Aprox(cpPixelCentro.x - Xreal / ((dXMinMts - dXMaxMts) / nRepreWidth));
        //*Ypixel = Aprox(cpPixelCentro.y - Yreal / ((dYMaxMts - dYMinMts) / nRepreHeight));
        //-------------------------------------------------------------------------------------
}


//COMENTARIO NUMERO 6
//Las cinco funciones que a continuación se desarrollan se utilizan para obtener
//las coordenadas cartesianas del objetivo, expresadas en Km, respecto al origen
//común, si se conocen las coordenadas geográficas del radar y las coordenadas
//cartesianas del objetivo, expresadas en Km, respecto al radar.

//Dados la ordenada dy del objetivo respecto al radar, expresada en Km,
//y la latitud Latitud_1 del radar, esta función obtiene, en coordenadas
//geográficas la latitud Latitud_2 del objetivo.
double DanCalc::Latitud_2(double dy, double Latitud_1)
{
        double a=110.26477; double b=0.0186644;
        double Lat2=(sqrt(pow(a+b*Latitud_1,2)+2.0F*b*dy)-a)/b;
        //Lat2 = (dy+Latitud_1*(a-b/2.0))/(a+b/2.0);
        return Lat2;
}

//Dados la abcisa dx del objetivo respecto al radar, expresada en Km,
//la longitud Longitud_1 del radar y calculada la latitud Latitud_2 del
//objetivo, esta función calcula la longitud Longitud_2 del objetivo.
double DanCalc::Longitud_2(double dx, double Longitud_1, double Latitud_2)
{
        double c=111.42325;
        double Long2=Longitud_1-dx/(c*cos((LATCENTRAL+Latitud_2)/2.0F*RADIAN));
        return Long2;
}
//Conociendo la latitud Latitud_1 del radar y la longitud, Longitud_2
//del objetivo, esta función calcula la abcisa del objetivo, expresada
//en Km, respecto al sistema de referencia del origen común.
double DanCalc::Abcisa_Obj(double Latitud_1, double Longitud_2)
{
        double c=111.42325;
        double Dx=c*(LONGCENTRAL-Longitud_2)*cos((LATCENTRAL+Latitud_1)/2.0F*RADIAN);
        return Dx;
}

//Conociendo la latitud Latitud_2 del objetivo, esta función calcula
//la ordenada del objetivo, expresada en Km, respecto al sistema de
//referencia del origen común.
double DanCalc::Ordenada_Obj(double Latitud_2)
{
        double a=110.26477; double b=0.0186644;
        double Dy=(Latitud_2-LATCENTRAL)*a+((LATCENTRAL+Latitud_2)/2.0F*b);
        return Dy;
}

//En esta función se traslada el sistema de referencia del radar al origen
//común de coordenadas. La entrada dx, dy son las coordenadas del punto
//expresadas en Km, respecto al radar. La salida p_xoc y p_yoc expresadas
//también en Km, son las coordenadas del punto en cuestión respecto al
//origen común.
void DanCalc::Traslacion_Del_Radar_Al_OrigenComun(double dx, double dy, double *p_xoc, double *p_yoc,double dLat,double dLong)
{
        double Lat2=Latitud_2(dy, dLat);
        double Lon2=Longitud_2(dx, dLong, Lat2);

        //if(Pert==INTRO_MANUAL)
        {
                //double latmedia = (Lat2+LATCENTRAL)/2.0;
                //double dKoc = 111.42325 * cos(latmedia*RADIAN);
                //latmedia = (Lat2 + pReceptor->dLatitud)/2.0;
                //double dKof = 111.42325 * cos(latmedia*RADIAN);

                //*p_xoc = dKoc*(dx/dKof+LONGCENTRAL-pReceptor->dLongitud);
                //*p_yoc = pReceptor->m_CoorY_Km+dy;


                double dLatMedia=(LATCENTRAL+Lat2)/2.0;
                double dKfc=111.42325*cos(dLatMedia*RADIAN);
                *p_xoc = (double)((LONGCENTRAL-Lon2)*dKfc);
                *p_yoc = (double)((Lat2-LATCENTRAL)*(110.26477+(dLatMedia*0.0186644)));
        }
        //else
        //{
        //	*p_xoc = pReceptor->m_CoorX_Km+dx;
        //	*p_yoc = pReceptor->m_CoorY_Km+dy;
        //}
        //*p_xoc = Abcisa_Obj(pReceptor->dLatitud, Lon2);
        //*p_yoc = Ordenada_Obj(Lat2);
}


//void DanCalc::Conviertea_HmMapa(Fuente *Fte,double dx, double dy,double *sX_HmOrigen, double *sY_HmOrigen)
//{
//    //esta funcion se cogio de la clase CMapa
//    // Valor  dX dY en km respecto a la posicion de la ERL
//#define a 110.26477
//#define b 0.0186644
//#define c 111.42325

//    double Lat2=(sqrt(pow(a+b*Fte->dLatitud,2)+2.0*b*dy)-a)/b;

//    //convierto de Km a Hm ... 1Hm=100m  ...1000m=10Hm=1Km
//    double dLatMedia=(LATCENTRAL+Lat2)/2.0;
//    double dKoc=c*cos(dLatMedia*RADIAN);
//    double Dy = (dy + Fte->Y_Fuente);
//    dLatMedia = (Fte->dLatitud + Lat2) / 2.0;
//    double dKof = c * cos(dLatMedia * RADIAN);
//    double Dx = dKoc * (dx / dKof + (LONGCENTRAL - Fte->dLongitud));

//    *sX_HmOrigen=Aprox(Dx);
//    *sY_HmOrigen=Aprox(Dy);
//}


int DanCalc::CalculaXPantalla(double x, double dK)
{
    double X=0.0;
    extern double Esq_a,Esq_c;
    
    int XX;
    double DK=dK;

    X=Convierte_x_CoordMapa_x(x,DK);
    XX=(int)((X-Esq_a)*(max_x/(Esq_c-Esq_a)));
    return (XX);
}

int DanCalc::CalculaYPantalla(double y)
{
    double Yt=0.0;
    extern double Esq_b,Esq_d;
    
    int YY;

    Yt=Convierte_y_CoordMapa_y(y);
    YY=(int)(max_y-((Yt-Esq_b)*max_y/(Esq_d-Esq_b)));
    return(YY);
}
//FIN DEL COMENTARIO NUMERO 6

int DanCalc::Aprox(double n)
{
        return (int)((n - (int)n) < 0.5 ? n : n+1);
}
