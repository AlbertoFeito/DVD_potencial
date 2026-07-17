#include "zpfaaa.h"
#include "ui_zpfaaa.h"

#include <QDebug>
ZPFAAA::ZPFAAA(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ZPFAAA)
{
    ui->setupUi(this);
 setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);


    hilo1 = new hilo();

    barra = new QStatusBar(this);
    barra->showMessage("Entre los datos del la posición");
    ui->layautbarra->addWidget(barra);
    ui->pBar_ZPFAAA->setVisible(false);

//    connect(&hilo1->calcIni,SIGNAL(fin(bool,int)),this,SLOT(completo(bool,int)));
 connect(&hilo1->calculos,SIGNAL(fin(bool,int)),this,SLOT(completo(bool,int)));
//    connect(&hilo1->calcIni,SIGNAL(fichNom(QString)),&selAltVuelo,SLOT(recibirFile(QString)));

//    connect(&hilo1->calcIni,SIGNAL(fichNomDistaAz(QString)),&selAltVuelo,SLOT(recibirFile2(QString)));


    connect(&perfil,SIGNAL(cerro()),this,SLOT(perCerro()));

    connect(&angcierre,SIGNAL(cerro()),this,SLOT(anguloCerro()));

    connect(&selAltVuelo,SIGNAL(cerro()),this,SLOT(distaCerro()));

    connect(this,SIGNAL(termino()),this,SLOT(cancelarEntrada()));
}

ZPFAAA::~ZPFAAA()
{

    delete hilo1;

    delete ui;
}

void ZPFAAA::seleccion(bool select)
{
  selecion = select;
    if(select)
    {
        barra->showMessage("Z.P.F.A.A.A.");
        ui->pB_ZPFAAA->setText("Z.P.F.A.A.A.");
    }
    else
    {
        barra->showMessage("Z.V.D.A.A.A.");
        ui->pB_ZPFAAA->setText("Z.V.D.A.A.A.");
    }
}

void ZPFAAA::closeEvent(QCloseEvent *event)
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

void ZPFAAA::perCerro()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_Perfil->setEnabled(true);
}

void ZPFAAA::anguloCerro()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_AnguloCierre->setEnabled(true);
}

void ZPFAAA::distaCerro()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_ZPFAAA->setEnabled(true);
}
void ZPFAAA::completo(bool terminado, int ba)
{
    ui->pBar_ZPFAAA->setMaximum(359);
    ui->pBar_ZPFAAA->setValue(ba);
    ui->pBar_ZPFAAA->setVisible(true);
    ui->pB_EntrarDatos->setEnabled(false);

    if (terminado)
    {
        this->barra->showMessage("Ahora puede trabajar");
        ui->pB_EntrarDatos->setEnabled(true);
        ui->pB_Perfil->setEnabled(true);
        ui->pB_AnguloCierre->setEnabled(true);
        ui->pB_ZPFAAA->setEnabled(true);
        ui->pBar_ZPFAAA->setVisible(false);
    }
    else
    {
        this->barra->showMessage("Espere un momento por favor. Calculando datos iniciales.");
        ui->pB_Perfil->setEnabled(false);
        ui->pB_AnguloCierre->setEnabled(false);
        ui->pB_ZPFAAA->setEnabled(false);
    }
}

void ZPFAAA::on_pB_EntrarDatos_clicked()
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
    ZPFAAAEntrada entradaZPFAAA/*(this)*/;
    entradaZPFAAA.grupo = grupo;
    entradaZPFAAA.grupos(grupo);
    if(entradaZPFAAA.exec()==1)
    {
        gradlong = entradaZPFAAA.gradlong;
        gradlat = entradaZPFAAA.gradlat;
        minlong = entradaZPFAAA.minlong;
        minlat = entradaZPFAAA.minlat;
        seglong = entradaZPFAAA.seglong;
        seglat = entradaZPFAAA.seglat;
        AlCannon = entradaZPFAAA.AlCannon;
        file = entradaZPFAAA.file;
        calibre = entradaZPFAAA.calibre;

        hilo1->calibre = calibre;
        hilo1->medio = entradaZPFAAA.medio;
        medio = entradaZPFAAA.medio;
        if(medio == 42 || medio == 43 || medio == 44)
            hilo1->arma = 4;
        else
            hilo1->arma = 2;
        arma = hilo1->arma;
        hilo1->recibirDatos(0,gradlong,minlong,seglong,gradlat,minlat,seglat,AlCannon,grupo);

        hilo1->start();
    }
    else
    {
        barra->showMessage("Entre los datos del la posición.");
        ui->pB_EntrarDatos->setEnabled(true);
        ui->pB_Perfil->setEnabled(false);
        ui->pB_AnguloCierre->setEnabled(false);
        ui->pB_ZPFAAA->setEnabled(false);
    }
}

void ZPFAAA::on_pB_Perfil_clicked()//perfil
{
    barra->showMessage("Perfil.");
    ui->pB_Perfil->setEnabled(false);

//    perfil.arma = arma;
//    perfil.grupo = grupo;
//    perfil.medio = medio;
//    perfil.calibre = int(calibre);
    perfil.angulositio = 0;
    perfil.calculos = &hilo1->calculos;
//    perfil.fichero = hilo1->calculos.label;
    perfil.show();
}

void ZPFAAA::on_pB_AnguloCierre_clicked()//Angulo cierre
{
    barra->showMessage("Ángulos de Cierre.");
    ui->pB_AnguloCierre->setEnabled(false);

//    angcierre.arma = arma;
//    angcierre.grupo = grupo;
//    angcierre.medio = medio;
//    angcierre.camara = 0;
//    angcierre.calibre = int(calibre);
    angcierre.calculos = &hilo1->calculos;
//    angcierre.fichero = hilo1->calculos.label;
    angcierre.show();
}

void ZPFAAA::on_pB_ZPFAAA_clicked()//Z.P.F.A.A.A
{

    ui->pB_ZPFAAA->setEnabled(false);

    selAltVuelo.calculos = &hilo1->calculos;
    if(!selecion)
      {        
        selAltVuelo.dm25 = true;        
      }
    selAltVuelo.dObjetLocale = true;
    selAltVuelo.dm00 = true;
    selAltVuelo.dm50 = true;
    selAltVuelo.d100 = true;
    selAltVuelo.d200 = true;
    selAltVuelo.d300 = true;
    selAltVuelo.d400 = true;
    selAltVuelo.d500 = true;
    selAltVuelo.d1000 = true;
    selAltVuelo.d2000 = true;
    selAltVuelo.d4000 = false;
    selAltVuelo.d6000 = true;
    selAltVuelo.d10000 = true;
    selAltVuelo.d17000 = false;
    selAltVuelo.d30000 = false;
//    selAltVuelo.nomb = hilo1->calculos.label;
//    selAltVuelo.fichero = hilo1->calculos.fichero;
//    selAltVuelo.arma = arma;
//    selAltVuelo.medio = medio;
//    selAltVuelo.calibre = int(calibre);
    selAltVuelo.radioButons(grupo,/*arma,*/medio);
//    selAltVuelo.grupo = grupo;
    selAltVuelo.configInterface();
    selAltVuelo.angulositio = 0;/////////////////////////////////////////
    selAltVuelo.show();
}

void ZPFAAA::on_pB_Cerrar_clicked()
{
    this->close();
    if(hilo1->isRunning())
        hilo1->quit();
}

void ZPFAAA::cancelarEntrada()
{
    ui->pB_EntrarDatos->setEnabled(true);
    ui->pB_Perfil->setEnabled(false);
    ui->pB_AnguloCierre->setEnabled(false);
    ui->pB_ZPFAAA->setEnabled(false);
}
