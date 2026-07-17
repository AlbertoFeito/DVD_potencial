#include "dlgsectorbiestatico.h"
#include "ui_dlgsectorbiestatico.h"

DlgSectorBiestatico::DlgSectorBiestatico(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSectorBiestatico)
{
    ui->setupUi(this);
    angulo = ui->sB_Angulo->text ().toInt ();
    SectorBiestatico = "";
    if(medio == 46)
    {
        ui->sB_Distancia->setEnabled(true);
        distanciaObj = ui->sB_Distancia->value();
    }
}

DlgSectorBiestatico::~DlgSectorBiestatico()
{
    delete ui;
}
double DlgSectorBiestatico::AZimut( double LGO, double LGD, double LTD, double LTO)//triangulo biestatico
{
    double AZ = 0;
    double Lx = 0.0;
    double Ly = 0.0;

    Lx = (LGO-LGD)*111.42325*cos((LTO+LTD)/2*RADIAN);
    Ly = (LTD-LTO)*(110.26477+((LTO+LTD)/2)*0.0186644);

    double cita = 0.0;
    cita = (180.0/M_PI)*(atan2(Ly,Lx));
    if((Ly>0.0F)&(Lx>0.0F))   //primer cuadrante
        AZ=90-cita;
    else if((Ly>0.0F)&(Lx<0.0F))   //segungo cuadrante
        AZ=450-cita;//270
    else if((Ly<0.0F)&(Lx<0.0F))   //tercer cuadrante
        AZ=90-cita;//180
    else    //cuarto cuadrante
    {
        AZ=90-cita;  //90
    }
    if(AZ == -90)
    {
        AZ = 270;
    }
    return AZ;
}
void DlgSectorBiestatico::Sector(int ang)
{
    //  double azimut = AZimut (QUtiles::GmsToGrados(lonPosTX),QUtiles::GmsToGrados(lonPosRX),QUtiles::GmsToGrados(latPosTX),QUtiles::GmsToGrados(latPosRX));

    angDer = 0;
    angIzq = 0;
    angDer2 = 0;
    angIzq2 = 0;
    if(medio == 45)
    {
        //      angulo2 = AZimut (lonRX,xVTx.at (ang)*-1,yVTx.at (ang),latRX);
        angDer = ang + 15;
        angIzq = ang - 15;

        //      angDer2 = angulo2 + 15;
        //      angIzq2 = angulo2 - 15;
    }
    else if(medio == 46)
    {
        angulo2 = AZimut (lonTX*-1,xVRx.at (ang)*-1,yVRx.at (ang),latTX);

        distanciaObj = ui->sB_Distancia->value()*1000;

        geocoordinateRx.setLatitude(latRX);
        geocoordinateRx.setLongitude(lonRX);


        geocoordinateTx.setLatitude(latTX);
        geocoordinateTx.setLongitude(lonTX);

        geocoordinateObj = geocoordinateRx.atDistanceAndAzimuth(distanciaObj,ang);
        AnguloPrueba = geocoordinateTx.azimuthTo(geocoordinateObj);
        qDebug()<<"AnguloPrueba"<<AnguloPrueba;
        AnguloPrueba = AZimut(lonTX*-1,geocoordinateObj.longitude()*-1,geocoordinateObj.latitude(),latTX);
qDebug()<<"AnguloPrueba"<<AnguloPrueba;
qDebug()<<"distanciaObj"<<distanciaObj;
        angDer = ang + 2;
        angIzq = ang - 2;

        angDer2 = angulo2 + 2;
        angIzq2 = angulo2 - 2;
        angDerObj = AnguloPrueba + 2;
        angIzqObj = AnguloPrueba - 2;
    }
    else
    {
        angulo2 = AZimut (lonTX,xVRx.at (ang)*-1,yVRx.at (ang),latTX);

        angDer = ang + 45;
        angIzq = ang - 45;

        angDer2 = angulo2 + 45;
        angIzq2 = angulo2 - 45;
    }
    if(angDer < 0)
        angDer = 360 + angDer;

    if(angIzq < 0)
        angIzq = 360 + angIzq;

    if(angDer >= 360)
        angDer =  angDer - 360 ;

    if(angIzq >= 360)
        angIzq = angIzq - 360 ;

    if(angDer2 < 0)
        angDer2 = 360 + angDer2;

    if(angIzq2 < 0)
        angIzq2 = 360 + angIzq2;

    if(angDer2 >= 360)
        angDer2 =  angDer2 - 360 ;

    if(angIzq2 >= 360)
        angIzq2 = angIzq2 - 360 ;

    if(angDerObj < 0)
        angDerObj = 360 + angDerObj;

    if(angIzqObj < 0)
        angIzqObj = 360 + angIzqObj;

    if(angDerObj >= 360)
        angDerObj =  angDerObj - 360 ;

    if(angIzqObj >= 360)
        angIzqObj = angIzqObj - 360 ;

    sector = "Izquierda: " +QString::number (fmod(angIzq,360))+"; Centro: " +QString::number (ang)+"; Derecha: "+QString::number (fmod(angDer,360));
    sector2 = "Izquierda: " +QString::number (fmod(angIzq2,360))+"; Centro: " +QString::number (angulo2)+"; Derecha: "+QString::number (fmod(angDer2,360));
}

void DlgSectorBiestatico::on_pB_Aceptar_clicked()
{

    if(SectorBiestatico != "")
        accept ();
    else
        reject ();
}

void DlgSectorBiestatico::on_pB_Calcular_clicked()
{
    ui->lE_Sector->clear ();
    Sector (angulo);
    ui->lE_Sector->setText (sector);

    SectorBiestatico = sector;
}

void DlgSectorBiestatico::on_sB_Angulo_valueChanged(int arg1)
{
    angulo = arg1;
}
