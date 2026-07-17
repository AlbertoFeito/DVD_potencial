#ifndef QUTILES_H
#define QUTILES_H

#pragma once

#include <QtGlobal>
#include <QColor>
#include <QtMath>
#include <QSettings>
#include "defcomunes.h"

#define PI			3.1415926535897932384626433832795
//#define RADIAN		0.017453292519943295769236907684886
//#define GRADOS		57.295779513082320876798154814105
#define RAD90		1.5707963267948966192313216916398
#define RAD270		4.7123889803846898576939650749193
#define RAD360		6.283185307179586476925286766559


#ifndef BZERO
    #define BZERO(buf)	memset(&buf, 0, sizeof(buf))
#endif


#pragma pack(push, 1)

enum PolarMode
{
    PM_0_90 = 0,
    PM_0_270 = 1,
    PM_180_90 = 2,
    PM_180_270 = 3,
    PM_90_0 = 4,
    PM_90_180 = 5,
    PM_270_0 = 6,
    PM_270_180 = 7
};

struct BITFIELD16
{
    uchar Bit0 : 1;
    uchar Bit1 : 1;
    uchar Bit2 : 1;
    uchar Bit3 : 1;
    uchar Bit4 : 1;
    uchar Bit5 : 1;
    uchar Bit6 : 1;
    uchar Bit7 : 1;
    uchar Bit8 : 1;
    uchar Bit9 : 1;
    uchar Bit10 : 1;
    uchar Bit11 : 1;
    uchar Bit12 : 1;
    uchar Bit13 : 1;
    uchar Bit14 : 1;
    uchar Bit15 : 1;
};

struct GMS
{
    short G;
    uchar M;
    uchar S;
    GMS()
    {
        G = 0; M = 0; S = 0;
    }
    GMS(short g, uchar m, uchar s)
    {
        G = g; M = m; S = s;
    }
    bool operator ==(GMS gms)
    {
        return (gms.G == G && gms.M == M && gms.S == S);
    }
    bool operator !=(GMS gms)
    {
        return (gms.G != G || gms.M != M || gms.S != S);
    }
};
struct PUNTO
{
    double X;
    double Y;
};
struct CENTROS
{
  double Lat;
  double Lon;
  QString name;
};
typedef QList<CENTROS> LCentros;
#pragma pack(pop)

class QUtiles
{
public:

    // Funciones útiles
    static uint HexToDec(QString Hex);
    static void GrayToBin12(ushort *pnGray);
    static double GmsToGrados(GMS gms);
    static GMS GradosToGms(double Valor);
    static double getDistancia2D(double x1, double y1, double x2, double y2);
    static double getDistancia3D(double x1, double y1, double z1, double x2, double y2, double z2);
    static bool EnSector(double Ang, double SectorInicio, double SectorFin);
    static double GetAnguloMedio(double SectorInicio, double SectorFin);
    static bool IsPolarModeClockWiseOnRepre(PolarMode pm);
    static bool IsEqualMem(void* Mem1, void* Mem2, uint nSizeInBytes);

    // Funciones para escritura de valores en un fichero de configuración
    static void WriteConfigString(QString strFileName, QString strSection, QString strEntry, QString strValue);
    static void WriteConfigInt(QString strFileName, QString strSection, QString strEntry, int nValue);
    static void WriteConfigUInt(QString strFileName, QString strSection, QString strEntry, uint nValue);
    static void WriteConfigHex(QString strFileName, QString strSection, QString strEntry, uint nValue);
    static void WriteConfigDouble(QString strFileName, QString strSection, QString strEntry, double dValue);
    static void WriteConfigBool(QString strFileName, QString strSection, QString strEntry, bool bValue);
    static void WriteConfigRGB(QString strFileName, QString strSection, QString strEntry, QRgb rgbValue);
    static void WriteConfigGMS(QString strFileName, QString strSection, QString strEntry, double dValue);
    static void WriteConfigGMS(QString strFileName, QString strSection, QString strEntry, GMS gmsValue);
    static void WriteConfigData(QString strFileName, QString strSection, QString strEntry, void* pData, uint nDataSize);

    // Funciones para lectura de valores de un fichero de configuración
    static QString GetConfigString(QString strFileName, QString strSection, QString strEntry, QString strDefault = "", bool bUpdateFile = true);
    static int GetConfigInt(QString strFileName, QString strSection, QString strEntry, int nDefault = 0, bool bUpdateFile = true);
    static uint GetConfigUInt(QString strFileName, QString strSection, QString strEntry, uint nDefault = 0, bool bUpdateFile = true);
    static uint GetConfigHex(QString strFileName, QString strSection, QString strEntry, uint nDefault = 0, bool bUpdateFile = true);
    static double GetConfigDouble(QString strFileName, QString strSection, QString strEntry, double dDefault = 0, bool bUpdateFile = true);
    static bool GetConfigBool(QString strFileName, QString strSection, QString strEntry, bool bDefault = true, bool bUpdateFile = true);
    static QRgb GetConfigRGB(QString strFileName, QString strSection, QString strEntry, QRgb rgbDefault = 0, bool bUpdateFile = true);
    static GMS GetConfigGMS(QString strFileName, QString strSection, QString strEntry, GMS Default = GMS(0,0,0), bool bUpdateFile = true);
    static double GetConfigGMS(QString strFileName, QString strSection, QString strEntry, double dDefault = 0, bool bUpdateFile = true);
    static void GetConfigData(QString strFileName, QString strSection, QString strEntry, void* pData, uint nDataSize, bool bUpdateFile = true);

    // Funciones "getAngulo"
    static double getAngulo_0_90(double Xreal, double Yreal);
    static double getAngulo_0_270(double Xreal, double Yreal);
    static double getAngulo_180_90(double Xreal, double Yreal);
    static double getAngulo_180_270(double Xreal, double Yreal);
    static double getAngulo_90_0(double Xreal, double Yreal);
    static double getAngulo_90_180(double Xreal, double Yreal);
    static double getAngulo_270_0(double Xreal, double Yreal);
    static double getAngulo_270_180(double Xreal, double Yreal);

    // Funciones para conversión de coordenadas (Pixel, Cartesianas, Polares)
    static void PixelToUnidad(double UnidadMax, int PixelMax, int Pixel, double* Unidad);
    static void UnidadToPixel(double UnidadMax, int PixelMax, double Unidad, int* Pixel);
    static void CalcularPixelCentro(double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int GraphicWidth, int GraphicHeight, int* XPixelCentro, int* YPixelCentro);
    static void PixelToReal(int Xpixel, int Ypixel, int XPixelCentro, int YPixelCentro, double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int Width, int Height, double* Xreal, double* Yreal);
    static void RealToPixel(double Xreal, double Yreal, int XPixelCentro, int YPixelCentro, double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int Width, int Height, int* Xpixel, int* Ypixel);
    static void RealToPolar(double Xreal, double Yreal, PolarMode pmPolarMode, double* Angulo, double* Distancia);
    static void PolarToReal(double Angulo, double Distancia, PolarMode pmPolarMode, double* Xreal, double* Yreal);
    static void PixelToPolar(int Xpixel, int Ypixel, int XPixelCentro, int YPixelCentro, double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int Width, int Height, PolarMode pmPolarMode, double* Angulo, double* Distancia);
    static void PolarToPixel(double Angulo, double Distancia, int XPixelCentro, int YPixelCentro, double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int Width, int Height, PolarMode pmPolarMode, int* Xpixel, int* Ypixel);

    // Funciones para manipulación de textos
    static QString CodificaString(QString s);
    static QString DecodificaString(QString s);
    static QByteArray CodificaString(QByteArray s);
    static QByteArray DecodificaString(QByteArray s);
    static int JoinedStringCount(QString strJoinedString);
    static QStringList SplitJoinedString(QString strJoinedString);
    static bool IsStringInJoinedString(QString strString, QString strJoinedString);
};

#endif // QUTILES_H
