#include "zpdcaaa.h"
#include "ui_zpdcaaa.h"

#include <QDebug>
#include <QMessageBox>
ZPDCAAA::ZPDCAAA(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ZPDCAAA)
{
    ui->setupUi(this);
 setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);


    hilo1 = new hilo();

    barra = new QStatusBar(this);
    barra->showMessage("Entre los datos del la posición");
    ui->layautbarra->addWidget(barra);
    ui->pBar_ZPDCAA->setVisible(false);

    ui->pB_EntrarDatos->setEnabled(true);
    ui->pB_Perfil->setEnabled(false);
    ui->pB_AnguloCierre->setEnabled(false);
    ui->pB_ZPDCAA->setEnabled(false);

    connect(&hilo1->calculos,SIGNAL(fin(bool,int)),this,SLOT(completo(bool,int)));

//    connect(&hilo1->calculos,SIGNAL(fichNom(QString)),&selAltVuelo,SLOT(recibirFile(QString)));

//    connect(&hilo1->calculos,SIGNAL(fichNomDistaAz(QString)),&selAltVuelo,SLOT(recibirFile2(QString)));

    connect(&perfil,SIGNAL(cerro()),this,SLOT(perCerro()));

    connect(&angcierre,SIGNAL(cerro()),this,SLOT(anguloCerro()));

    connect(&selAltVuelo,SIGNAL(cerro()),this,SLOT(distaCerro()));

    connect(this,SIGNAL(termino()),this,SLOT(cancelarEntrada()));

//    velocidad = 300;
//    tiempo = 25;
}

ZPDCAAA::~ZPDCAAA()
{
    delete hilo1;  
    delete ui;
}
void ZPDCAAA::closeEvent(QCloseEvent *event)
{
    Q_UNUSED (event);
    if (hilo1->isRunning())
        hilo1->calculos.final = true;
    hilo1->quit();
    hilo1->wait();
    selAltVuelo.parar();
    barra->showMessage("Borrando datos incompletos");
    selAltVuelo.close();
    perfil.close();
    angcierre.close();
    emit termino();
}
void ZPDCAAA::seleccion(bool select)
{
  selecciona = select;
    if(select)
    {
        barra->showMessage("Z.P.D.C.C.A.A.");
        ui->pB_ZPDCAA->setText("Z.P.D.C.C.A.A.");
    }
    else
    {
        barra->showMessage("Z.V.D.C.C.A.A.");
        ui->pB_ZPDCAA->setText("Z.V.D.C.C.A.A.");
    }
}
void ZPDCAAA::perCerro()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_Perfil->setEnabled(true);
}

void ZPDCAAA::anguloCerro()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_AnguloCierre->setEnabled(true);
}
void ZPDCAAA::distaCerro()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_ZPDCAA->setEnabled(true);
}

void ZPDCAAA::on_pB_EntrarDatos_clicked()
{
    if (hilo1->isRunning())
        hilo1->calculos.final = true;
    hilo1->quit();
    hilo1->wait();
    selAltVuelo.parar();
    barra->showMessage("Borrando datos incompletos");
    selAltVuelo.close();
    perfil.close();
    angcierre.close();
    ZDCAAEntrada entradaZDCAA/*(this)*/;
    entradaZDCAA.grupo = grupo;
    entradaZDCAA.grupos(grupo);
    if(entradaZDCAA.exec()==1)
    {
        gradlong=entradaZDCAA.gradlong;
        gradlat=entradaZDCAA.gradlat;
        minlong=entradaZDCAA.minlong;
        minlat=entradaZDCAA.minlat;
        seglong=entradaZDCAA.seglong;
        seglat=entradaZDCAA.seglat;
        Alrampa=entradaZDCAA.Alrampa;
        file=entradaZDCAA.file;
        cohete=entradaZDCAA.cohete;        
        hilo1->cohete = cohete;
        hilo1->medio = entradaZDCAA.medio;
        medio = entradaZDCAA.medio;

//        velocidad = entradaZDCAA.velocidad;
//        tiempo = entradaZDCAA.tiempo;

        hilo1->arma = 3;

        hilo1->recibirDatos(0,gradlong,minlong,seglong,gradlat,minlat,seglat,Alrampa,grupo);
        hilo1->start();
    }
    else
    {
        barra->showMessage("Entre los datos del la posición.");
        ui->pB_EntrarDatos->setEnabled(true);
        ui->pB_Perfil->setEnabled(false);
        ui->pB_AnguloCierre->setEnabled(false);
        ui->pB_ZPDCAA->setEnabled(false);
    }
}

void ZPDCAAA::completo(bool terminado, int ba)
{
    ui->pBar_ZPDCAA->setMaximum(360);
    ui->pBar_ZPDCAA->setValue(ba);
    ui->pBar_ZPDCAA->setVisible(true);
    ui->pB_EntrarDatos->setEnabled(false);

    if (terminado)
    {
        barra->showMessage("Ahora puede trabajar");
        ui->pB_EntrarDatos->setEnabled(true);
        ui->pB_Perfil->setEnabled(true);
        ui->pB_AnguloCierre->setEnabled(true);
        ui->pB_ZPDCAA->setEnabled(true);
        ui->pBar_ZPDCAA->setVisible(false);
    }
    else
    {
        barra->showMessage("Espere un momento por favor. Calculando datos iniciales.");
        ui->pB_Perfil->setEnabled(false);
        ui->pB_AnguloCierre->setEnabled(false);
        ui->pB_ZPDCAA->setEnabled(false);
    }
}

void ZPDCAAA::on_pB_Perfil_clicked()//Perfil
{
    barra->showMessage("Perfil.");
    ui->pB_Perfil->setEnabled(false);
    perfil.angulositio = 0;
    perfil.calculos = &hilo1->calculos;
    perfil.show();
}

void ZPDCAAA::on_pB_AnguloCierre_clicked()//Angulo cierre
{
    barra->showMessage("Ángulos de Cierre.");
    ui->pB_AnguloCierre->setEnabled(false);
    angcierre.calculos = &hilo1->calculos;
    angcierre.show();
}

void ZPDCAAA::on_pB_ZPDCAA_clicked()//Z.P.D.C.A.A
{
    ui->pB_ZPDCAA->setEnabled(false);

    selAltVuelo.calculos = &hilo1->calculos;
    if(!selecciona)
      {
        selAltVuelo.dm00 = true;
        selAltVuelo.dm25 = true;
      }
    selAltVuelo.dObjetLocale = true;
    selAltVuelo.dm50 = true;
    selAltVuelo.d100 = true;
    selAltVuelo.d200 = true;
    selAltVuelo.d300 = true;
    selAltVuelo.d400 = true;
    selAltVuelo.d500 = true;
    selAltVuelo.d1000 = true;
    selAltVuelo.d2000 = true;
    selAltVuelo.d4000 = true;
    selAltVuelo.d6000 = false;
    selAltVuelo.d10000 = true;
    selAltVuelo.d17000 = true;
    selAltVuelo.d30000 = true;
    selAltVuelo.radioButons(grupo,medio);
    selAltVuelo.configInterface();
    selAltVuelo.angulositio = 0;/////////////////////////////////////////
    selAltVuelo.show();
}

void ZPDCAAA::on_pB_Cerrar_clicked()
{
    this->close();
    if(hilo1->isRunning())
        hilo1->quit();
}


void ZPDCAAA::cancelarEntrada()
{
    barra->showMessage("Entre los datos del la posición.");
    ui->pB_EntrarDatos->setEnabled(true);
    ui->pB_Perfil->setEnabled(false);
    ui->pB_AnguloCierre->setEnabled(false);
    ui->pB_ZPDCAA->setEnabled(false);
}
