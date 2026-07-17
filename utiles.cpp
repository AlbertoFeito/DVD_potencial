#include "utiles.h"


uint QUtiles::HexToDec(QString Hex)
{
    return Hex.toUInt(0, 16);
}
void QUtiles::GrayToBin12(ushort *pnGray)
{
    BITFIELD16 *pGray = (BITFIELD16 *)pnGray;
    /*pGray->Bit15 = 0;
    pGray->Bit14 = 0;
    pGray->Bit13 = 0;
    pGray->Bit12 = 0;*/
    pGray->Bit10 ^= pGray->Bit11;
    pGray->Bit9 ^= pGray->Bit10;
    pGray->Bit8 ^= pGray->Bit9;
    pGray->Bit7 ^= pGray->Bit8;
    pGray->Bit6 ^= pGray->Bit7;
    pGray->Bit5 ^= pGray->Bit6;
    pGray->Bit4 ^= pGray->Bit5;
    pGray->Bit3 ^= pGray->Bit4;
    pGray->Bit2 ^= pGray->Bit3;
    pGray->Bit1 ^= pGray->Bit2;
    pGray->Bit0 ^= pGray->Bit1;
}
double QUtiles::GmsToGrados(GMS gms)
{
    return ((double)gms.G + (double)gms.M / 60.0 + (double)gms.S / 3600.0);
}
GMS QUtiles::GradosToGms(double Valor)
{
    GMS gms;
    gms.G = (short)Valor;
    gms.M = (uchar)fmod(Valor * 60, 60);
    gms.S = (uchar)fmod(Valor * 3600, 60);
    return gms;
}
double QUtiles::getDistancia2D(double x1, double y1, double x2, double y2)
{
    return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
}
double QUtiles::getDistancia3D(double x1, double y1, double z1, double x2, double y2, double z2)
{
    return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2)));
}
bool QUtiles::EnSector(double Ang, double SectorInicio, double SectorFin)//para saber si un angulo esta dentro de un sector
{
    if((Ang >= 0 && Ang < 360) && (SectorInicio >= 0 && SectorInicio < 360) && (SectorFin >= 0 && SectorFin < 360))
    {
        bool inside;
        if(SectorInicio == SectorFin)
            inside = (Ang == SectorInicio);
        else if(SectorInicio < SectorFin)
            inside = (Ang >= SectorInicio && Ang <= SectorFin);
        else
            inside = (Ang >= SectorInicio || Ang <= SectorFin);
        return inside;
    }
    return false;
}
double QUtiles::GetAnguloMedio(double SectorInicio, double SectorFin)
{
    if((SectorInicio >= 0 && SectorInicio < 360) && (SectorFin >= 0 && SectorFin < 360))
    {
        double medio;
        if(SectorInicio == SectorFin)
            medio = SectorInicio;
        else if(SectorInicio < SectorFin)
            medio = (SectorInicio + SectorFin) / 2.0;
        else
            medio = fmod((SectorInicio + (360.0 + SectorFin)) / 2.0, 360.0);
        return medio;
    }
    return -1;
}
bool QUtiles::IsPolarModeClockWiseOnRepre(PolarMode pm)//
{
    bool bClockWise;
    switch(pm)
    {
    case PM_0_90:
    case PM_180_270:
    case PM_90_180:
    case PM_270_0:
    {
        bClockWise = false;
        break;
    }
    default:
    {
        bClockWise = true;
        break;
    }
    }
    return bClockWise;
}
bool QUtiles::IsEqualMem(void *Mem1, void *Mem2, uint nSizeInBytes)
{
    for(uint i = 0; i < nSizeInBytes; i++)
    {
        if(((uchar*)Mem1)[i] != ((uchar*)Mem2)[i])
            return false;
    }
    return true;
}


// Funciones para escritura de valores en un fichero de configuración
void QUtiles::WriteConfigString(QString strFileName, QString strSection, QString strEntry, QString strValue)
{
    QSettings s(strFileName, QSettings::IniFormat);
    s.setValue(strSection + "/" + strEntry, strValue);
}
void QUtiles::WriteConfigInt(QString strFileName, QString strSection, QString strEntry, int nValue)
{
    QSettings s(strFileName, QSettings::IniFormat);
    s.setValue(strSection + "/" + strEntry, nValue);
}
void QUtiles::WriteConfigUInt(QString strFileName, QString strSection, QString strEntry, uint nValue)
{
    QSettings s(strFileName, QSettings::IniFormat);
    s.setValue(strSection + "/" + strEntry, nValue);
}
void QUtiles::WriteConfigHex(QString strFileName, QString strSection, QString strEntry, uint nValue)
{
    WriteConfigString(strFileName, strSection, strEntry, QString::number(nValue, 16).toUpper());
}
void QUtiles::WriteConfigDouble(QString strFileName, QString strSection, QString strEntry, double dValue)
{
    QSettings s(strFileName, QSettings::IniFormat);
    s.setValue(strSection + "/" + strEntry, dValue);
}
void QUtiles::WriteConfigBool(QString strFileName, QString strSection, QString strEntry, bool bValue)
{
    QSettings s(strFileName, QSettings::IniFormat);
    s.setValue(strSection + "/" + strEntry, bValue);
}
void QUtiles::WriteConfigRGB(QString strFileName, QString strSection, QString strEntry, QRgb rgbValue)
{
    QString strValue = QString("rgb(%1,%2,%3)")
            .arg(qRed(rgbValue))
            .arg(qGreen(rgbValue))
            .arg(qBlue(rgbValue));
    WriteConfigString(strFileName, strSection, strEntry, strValue);
}
void QUtiles::WriteConfigGMS(QString strFileName, QString strSection, QString strEntry, GMS gmsValue)
{
    QString strValue = QString("gms(%1,%2,%3)")
            .arg(gmsValue.G)
            .arg(gmsValue.M)
            .arg(gmsValue.S);
    WriteConfigString(strFileName, strSection, strEntry, strValue);
}
void QUtiles::WriteConfigGMS(QString strFileName, QString strSection, QString strEntry, double dValue)
{
    QUtiles::WriteConfigGMS(strFileName, strSection, strEntry, QUtiles::GradosToGms(dValue));
}
void QUtiles::WriteConfigData(QString strFileName, QString strSection, QString strEntry, void* pData, uint nDataSize)
{
    QString strValue;
    uchar* pChar = (uchar*)pData;
    for(uint i = 0; i < nDataSize; i++)
    {
        strValue.append(QString("%1").arg((uint)pChar[i], 2, 16, QLatin1Char('0')));
    }
    WriteConfigString(strFileName, strSection, strEntry, strValue);
}


// Funciones para lectura de valores de un fichero de configuración
QString QUtiles::GetConfigString(QString strFileName, QString strSection, QString strEntry, QString strDefault, bool bUpdateFile)
{
    QSettings s(strFileName, QSettings::IniFormat);
    QString strValue = s.value(strSection + "/" + strEntry, strDefault).toString();
    if(bUpdateFile)
        WriteConfigString(strFileName, strSection, strEntry, strValue);
    return strValue;
}
int QUtiles::GetConfigInt(QString strFileName, QString strSection, QString strEntry, int nDefault, bool bUpdateFile)
{
    QSettings s(strFileName, QSettings::IniFormat);
    int nValue = s.value(strSection + "/" + strEntry, nDefault).toInt();
    if(bUpdateFile)
        WriteConfigInt(strFileName, strSection, strEntry, nValue);
    return nValue;
}
uint QUtiles::GetConfigUInt(QString strFileName, QString strSection, QString strEntry, uint nDefault, bool bUpdateFile)
{
    QSettings s(strFileName, QSettings::IniFormat);
    int nValue = s.value(strSection + "/" + strEntry, nDefault).toUInt();
    if(bUpdateFile)
        WriteConfigUInt(strFileName, strSection, strEntry, nValue);
    return nValue;
}
uint QUtiles::GetConfigHex(QString strFileName, QString strSection, QString strEntry, uint nDefault, bool bUpdateFile)
{
    uint nValue = GetConfigString(strFileName, strSection, strEntry, QString::number(nDefault, 16)).toUInt(0, 16);
    if(bUpdateFile)
        WriteConfigHex(strFileName, strSection, strEntry, nValue);
    return nValue;
}
double QUtiles::GetConfigDouble(QString strFileName, QString strSection, QString strEntry, double dDefault, bool bUpdateFile)
{
    QSettings s(strFileName, QSettings::IniFormat);
    double dValue = s.value(strSection + "/" + strEntry, dDefault).toDouble();
    if(bUpdateFile)
        WriteConfigDouble(strFileName, strSection, strEntry, dValue);
    return dValue;
}

bool QUtiles::GetConfigBool(QString strFileName, QString strSection, QString strEntry, bool bDefault, bool bUpdateFile)
{
    QSettings s(strFileName, QSettings::IniFormat);
    bool bValue = s.value(strSection + "/" + strEntry, bDefault).toBool();
    if(bUpdateFile)
        WriteConfigBool(strFileName, strSection, strEntry, bValue);
    return bValue;
}
QRgb QUtiles::GetConfigRGB(QString strFileName, QString strSection, QString strEntry, QRgb rgbDefault, bool bUpdateFile)
{
    QString strDefault = QString("rgb(%1,%2,%3)")
            .arg(qRed(rgbDefault))
            .arg(qGreen(rgbDefault))
            .arg(qBlue(rgbDefault));
    QString strValue = GetConfigString(strFileName, strSection, strEntry, strDefault, false);
    QStringList list = strValue.remove(QRegExp("[rgb()]")).split(',', QString::SkipEmptyParts);
    QRgb rgbValue = qRgb(list[0].toInt(), list[1].toInt(), list[2].toInt());
    if(bUpdateFile)
        WriteConfigRGB(strFileName, strSection, strEntry, rgbValue);
    return rgbValue;
}
GMS QUtiles::GetConfigGMS(QString strFileName, QString strSection, QString strEntry, GMS gmsDefault, bool bUpdateFile)
{
    QString strDefault = QString("gms(%1,%2,%3)")
            .arg(gmsDefault.G)
            .arg(gmsDefault.M)
            .arg(gmsDefault.S);
    QString strValue = GetConfigString(strFileName, strSection, strEntry, strDefault, false);
    QStringList list = strValue.remove(QRegExp("[gms()]")).split(',', QString::SkipEmptyParts);
    GMS gmsValue = GMS(list[0].toInt(), list[1].toInt(), list[2].toInt());
    if(bUpdateFile)
        WriteConfigGMS(strFileName, strSection, strEntry, gmsValue);
    return gmsValue;
}
double QUtiles::GetConfigGMS(QString strFileName, QString strSection, QString strEntry, double dDefault, bool bUpdateFile)
{
    return GmsToGrados(GetConfigGMS(strFileName, strSection, strEntry, GradosToGms(dDefault), bUpdateFile));
}
void QUtiles::GetConfigData(QString strFileName, QString strSection, QString strEntry, void* pData, uint nDataSize, bool bUpdateFile)
{
    QString strDefault;
    uchar* pChar = (uchar*)pData;
    for(uint i = 0; i < nDataSize; i++)
    {
        strDefault.append(QString("%1").arg((uint)pChar[i], 2, 16, QLatin1Char('0')));
    }
    QString strValue = GetConfigString(strFileName, strSection, strEntry, strDefault, false);
    for(uint i = 0, j = 0; i < nDataSize; i++, j += 2)
    {
        pChar[i] = strValue.mid(j, 2).toUInt(0, 16);
    }
    if(bUpdateFile)
        WriteConfigData(strFileName, strSection, strEntry, pData, nDataSize);
}


// Funciones "getAngulo"
double QUtiles::getAngulo_0_90(double Xreal, double Yreal)
{
    double M = fmod(atan2(Xreal, Yreal) + RAD360, RAD360);
    if(M == 0)
    {
        if(Xreal == 0)
            M = Yreal >= 0 ? 0 : PI;
        else
            M = Xreal > 0 ? RAD90 : RAD270;
    }
    return M * GRADOS;
}
double QUtiles::getAngulo_0_270(double Xreal, double Yreal)
{
    double M = fmod(atan2(-Xreal, Yreal) + RAD360, RAD360);
    if(M == 0)
    {
        if(Xreal == 0)
            M = Yreal >= 0 ? 0 : PI;
        else
            M = Xreal > 0 ? RAD270 : RAD90;
    }
    return M * GRADOS;
}
double QUtiles::getAngulo_180_90(double Xreal, double Yreal)
{
    double M = fmod(atan2(Xreal, -Yreal) + RAD360, RAD360);
    if(M == 0)
    {
        if(Xreal == 0)
            M = Yreal >= 0 ? PI : 0;
        else
            M = Xreal > 0 ? RAD90 : RAD270;
    }
    return M * GRADOS;
}
double QUtiles::getAngulo_180_270(double Xreal, double Yreal)
{
    double M = fmod(atan2(-Xreal, -Yreal) + RAD360, RAD360);
    if(M == 0)
    {
        if(Xreal == 0)
            M = Yreal >= 0 ? PI : 0;
        else
            M = Xreal > 0 ? RAD270 : RAD90;
    }
    return M * GRADOS;
}
double QUtiles::getAngulo_90_0(double Xreal, double Yreal)
{
    double M = fmod(atan2(Yreal, Xreal) + RAD360, RAD360);
    if(M == 0)
    {
        if(Xreal == 0)
            M = Yreal >= 0 ? RAD90 : RAD270;
        else
            M = Xreal > 0 ? 0 : PI;
    }
    return M * GRADOS;
}
double QUtiles::getAngulo_90_180(double Xreal, double Yreal)
{
    double M = fmod(atan2(Yreal, -Xreal) + RAD360, RAD360);
    if(M == 0)
    {
        if(Xreal == 0)
            M = Yreal >= 0 ? RAD90 : RAD270;
        else
            M = Xreal > 0 ? PI : 0;
    }
    return M * GRADOS;
}
double QUtiles::getAngulo_270_0(double Xreal, double Yreal)
{
    double M = fmod(atan2(-Yreal, Xreal) + RAD360, RAD360);
    if(M == 0)
    {
        if(Xreal == 0)
            M = Yreal >= 0 ? RAD270 : RAD90;
        else
            M = Xreal > 0 ?  0: PI;
    }
    return M * GRADOS;
}
double QUtiles::getAngulo_270_180(double Xreal, double Yreal)
{
    double M = fmod(atan2(-Yreal, -Xreal) + RAD360, RAD360);
    if(M == 0)
    {
        if(Xreal == 0)
            M = Yreal >= 0 ? RAD270 : RAD90;
        else
            M = Xreal > 0 ? PI : 0;
    }
    return M * GRADOS;
}


// Funciones para conversión de coordenadas (Pixel, Cartesianas, Polares)
void QUtiles::PixelToUnidad(double UnidadMax, int PixelMax, int Pixel, double* Unidad)
{
    *Unidad = Pixel * (UnidadMax / PixelMax);
}
void QUtiles::UnidadToPixel(double UnidadMax, int PixelMax, double Unidad, int* Pixel)
{
    *Pixel = (int)(Unidad / (UnidadMax / PixelMax));
}
void QUtiles::CalcularPixelCentro(double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int GraphicWidth, int GraphicHeight, int* XPixelCentro, int* YPixelCentro)
{
    *XPixelCentro = qRound(XMinReal / ((XMinReal - XMaxReal) / GraphicWidth));
    *YPixelCentro = qRound(YMaxReal / ((YMaxReal - YMinReal) / GraphicHeight));
}
void QUtiles::PixelToReal(int Xpixel, int Ypixel, int XPixelCentro, int YPixelCentro, double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int Width, int Height, double* Xreal, double* Yreal)
{
    *Xreal = (XPixelCentro - Xpixel) * ((XMinReal - XMaxReal) / Width);
    *Yreal = (YPixelCentro - Ypixel) * ((YMaxReal - YMinReal) / Height);
}
void QUtiles::RealToPixel(double Xreal, double Yreal, int XPixelCentro, int YPixelCentro, double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int Width, int Height, int* Xpixel, int* Ypixel)
{
    *Xpixel = qRound(XPixelCentro - Xreal / ((XMinReal - XMaxReal) / Width));
    *Ypixel = qRound(YPixelCentro - Yreal / ((YMaxReal - YMinReal) / Height));
}
void QUtiles::RealToPolar(double Xreal, double Yreal, PolarMode pmPolarMode, double* Angulo, double* Distancia)
{
    *Distancia = sqrt(Xreal * Xreal + Yreal * Yreal);
    switch (pmPolarMode)
    {
    case (PM_0_90):
        *Angulo = QUtiles::getAngulo_0_90(Xreal, Yreal);
        break;
    case (PM_0_270):
        *Angulo = QUtiles::getAngulo_0_270(Xreal, Yreal);
        break;
    case (PM_180_90):
        *Angulo = QUtiles::getAngulo_180_90(Xreal, Yreal);
        break;
    case (PM_180_270):
        *Angulo = QUtiles::getAngulo_180_270(Xreal, Yreal);
        break;
    case (PM_90_0):
        *Angulo = QUtiles::getAngulo_90_0(Xreal, Yreal);
        break;
    case (PM_90_180):
        *Angulo = QUtiles::getAngulo_90_180(Xreal, Yreal);
        break;
    case (PM_270_0):
        *Angulo = QUtiles::getAngulo_270_0(Xreal, Yreal);
        break;
    case (PM_270_180):
        *Angulo = QUtiles::getAngulo_270_180(Xreal, Yreal);
        break;
    }
}
void QUtiles::PolarToReal(double Angulo, double Distancia, PolarMode pmPolarMode, double* Xreal, double* Yreal)
{
    double aciRadianes = Angulo * RADIAN;
    switch (pmPolarMode)
    {
    case (PM_0_90):
        *Xreal = Distancia * sin(aciRadianes);
        *Yreal = Distancia * cos(aciRadianes);
        break;
    case (PM_0_270):
        *Xreal = -Distancia * sin(aciRadianes);
        *Yreal = Distancia * cos(aciRadianes);
        break;
    case (PM_180_90):
        *Xreal = Distancia * sin(aciRadianes);
        *Yreal = -Distancia * cos(aciRadianes);
        break;
    case (PM_180_270):
        *Xreal = -Distancia * sin(aciRadianes);
        *Yreal = -Distancia * cos(aciRadianes);
        break;
    case (PM_90_0):
        *Xreal = Distancia * cos(aciRadianes);
        *Yreal = Distancia * sin(aciRadianes);
        break;
    case (PM_90_180):
        *Xreal = -Distancia * cos(aciRadianes);
        *Yreal = Distancia * sin(aciRadianes);
        break;
    case (PM_270_0):
        *Xreal = Distancia * cos(aciRadianes);
        *Yreal = -Distancia * sin(aciRadianes);
        break;
    case (PM_270_180):
        *Xreal = -Distancia * cos(aciRadianes);
        *Yreal = -Distancia * sin(aciRadianes);
        break;
    }
}
void QUtiles::PixelToPolar(int Xpixel, int Ypixel, int XPixelCentro, int YPixelCentro, double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int Width, int Height, PolarMode pmPolarMode, double* Angulo, double* Distancia)
{
    PixelToReal(Xpixel, Ypixel, XPixelCentro, YPixelCentro, XMinReal, XMaxReal, YMinReal, YMaxReal, Width, Height, Angulo, Distancia);
    RealToPolar(*Angulo, *Distancia, pmPolarMode, Angulo, Distancia);
}
void QUtiles::PolarToPixel(double Angulo, double Distancia, int XPixelCentro, int YPixelCentro, double XMinReal, double XMaxReal, double YMinReal, double YMaxReal, int Width, int Height, PolarMode pmPolarMode, int* Xpixel, int* Ypixel)
{
    double Xreal, Yreal;
    PolarToReal(Angulo, Distancia, pmPolarMode, &Xreal, &Yreal);
    RealToPixel(Xreal, Yreal, XPixelCentro, YPixelCentro, XMinReal, XMaxReal, YMinReal, YMaxReal, Width, Height, Xpixel, Ypixel);
}


// Funciones para manipulación de textos
QString QUtiles::CodificaString(QString s)
{
    ushort Codigo = qrand() % 24 + 100;
    QString str(Codigo);
    if(s == "")
        return str + Codigo;
    for(int i = 0; i < s.length(); i++)
    {
        str.append(s[i].unicode() + Codigo);
    }
    return str;
}
QByteArray QUtiles::CodificaString(QByteArray s)
{
    char Codigo = qrand() % 24 + 100;
    QByteArray str(1, Codigo);
    if(s == "")
        return str + Codigo;
    for(int i = 0; i < s.length(); i++)
    {
        str.append(s[i] + Codigo);
    }
    return str;
}
QString QUtiles::DecodificaString(QString s)
{
    if(s.length() > 1)
    {
        QString str = "";
        ushort Codigo = s[0].unicode();
        for(int i = 1; i < s.length(); i++)
        {
            str.append(s[i].unicode() - Codigo);
        }
        return str;
    }
    return "";
}
QByteArray QUtiles::DecodificaString(QByteArray s)
{
    if(s.length() > 1)
    {
        QByteArray str = "";
        char Codigo = s[0];
        for(int i = 1; i < s.length(); i++)
        {
            str.append(s[i] - Codigo);
        }
        return str;
    }
    return "";
}
int QUtiles::JoinedStringCount(QString strJoinedString)
{
    return strJoinedString.split("\n").length();
}
QStringList QUtiles::SplitJoinedString(QString strJoinedString)
{
    return strJoinedString.split("\n");
}
bool QUtiles::IsStringInJoinedString(QString strString, QString strJoinedString)
{
    return strJoinedString.split("\n").contains(strString);
}
/**************************************************************/
