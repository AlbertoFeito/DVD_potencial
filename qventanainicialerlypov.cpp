#include "qventanainicialerlypov.h"
#include "ui_qventanainicialerlypov.h"
#include <QMessageBox>
#include <QDebug>
QVentanaInicialERLyPOV::QVentanaInicialERLyPOV(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QVentanaInicialERLyPOV)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    hilo1=new hilo();

    //    fin = false;
    numWindow = 0;
    barra = new QStatusBar(this);
    barra->showMessage("Entre los datos del la posición.");
    ui->horizontalLayout->addWidget(barra);
    ui->pBar_Progreso->setVisible(false);

    arma = 0;
    //    camara = 0;

    connect(&hilo1->calculos,SIGNAL(fin(bool,int)),this,SLOT(completo(bool,int)));

    connect(&perfil,SIGNAL(cerro()),this,SLOT(perCerro()));

    connect(&angcierre,SIGNAL(cerro()),this,SLOT(anguloCerro()));

    connect(&selAltVuelo,SIGNAL(cerro()),this,SLOT(distaCerro()));

    connect(&plazoletaZCDeci,SIGNAL(cerro()),this,SLOT(plazoletaCerroDeci()));

    connect(&plazoletaZCMetrica,SIGNAL(cerro()),this,SLOT(plazoletaCerrometri()));

    connect(this,SIGNAL(termino()),this,SLOT(cancelarEntrada()));

}

QVentanaInicialERLyPOV::~QVentanaInicialERLyPOV()
{
    //    delete barra;
    //    delete hilo1;
    delete ui;
}

void QVentanaInicialERLyPOV::completo(bool terminado, int ba)
{
    ui->pBar_Progreso->setMaximum(359);
    ui->pBar_Progreso->setValue(ba);
    ui->pBar_Progreso->setVisible(true);
    ui->pB_EntrarDatos->setEnabled(false);

    if (terminado)
    {
        barra->showMessage("Ahora puede trabajar");
        ui->pB_EntrarDatos->setEnabled(true);
        ui->pB_Perfil->setEnabled(true);
        ui->pB_AngulosCierre->setEnabled(true);
        ui->pB_DVDP->setEnabled(true);
        if(arma == 0 || arma == 1 || medio == 41 || medio == 49)
        {
            ui->pB_BandaDecimetrica->setEnabled(false);
            ui->pB_BandaMetrica->setEnabled(false);
        }
        else
        {
            ui->pB_BandaDecimetrica->setEnabled(true);
            ui->pB_BandaMetrica->setEnabled(true);
        }
        ui->pBar_Progreso->setVisible(false);

    }
    else
    {
        barra->showMessage("Espere un momento por favor. Calculando datos iniciales.");
        ui->pB_Perfil->setEnabled(false);
        ui->pB_AngulosCierre->setEnabled(false);
        ui->pB_DVDP->setEnabled(false);
        ui->pB_BandaDecimetrica->setEnabled(false);
        ui->pB_BandaMetrica->setEnabled(false);
    }
}

void QVentanaInicialERLyPOV::cancelarEntrada()
{
    barra->showMessage("Entre los datos del la posición.");
    ui->pB_EntrarDatos->setEnabled(true);
    ui->pB_Perfil->setEnabled(false);
    ui->pB_AngulosCierre->setEnabled(false);
    ui->pB_DVDP->setEnabled(false);
    ui->pB_BandaDecimetrica->setEnabled(false);
    ui->pB_BandaMetrica->setEnabled(false);
}

void QVentanaInicialERLyPOV::distaCerro()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_DVDP->setEnabled(true);
}

void QVentanaInicialERLyPOV::anguloCerro()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_AngulosCierre->setEnabled(true);
}

void QVentanaInicialERLyPOV::perCerro()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_Perfil->setEnabled(true);
}

void QVentanaInicialERLyPOV::plazoletaCerrometri()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_BandaMetrica->setEnabled(true);
}

void QVentanaInicialERLyPOV::plazoletaCerroDeci()
{
    barra->showMessage("Seleccione una acción para trabajar");
    ui->pB_BandaDecimetrica->setEnabled(true);
}

//void QVentanaInicialERLyPOV::biestaticocerro(int num)
//{
//  emit biestatico (numWindow);
//}

void QVentanaInicialERLyPOV::seleccionArma(bool erl)
{
    ui->gB_Plazoleta->setVisible(erl);
    if(medio == 45 || medio == 50)
    {
        ui->pB_BandaDecimetrica->setVisible(false);
    }
}
void QVentanaInicialERLyPOV::on_pB_EntrarDatos_clicked()//ver principal de visibilidadTRT
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
    plazoletaZCDeci.close();
    plazoletaZCMetrica.close();

    DVDPEntrada entradaDvdp;
    entradaDvdp.arma = arma;
    entradaDvdp.medio = medio;
    entradaDvdp.selecionArma();
    if(entradaDvdp.exec()== QDialog::Accepted)
    {
        angulositio = entradaDvdp.angulositio;

        hilo1->arma = arma;
        if(arma == 1)
            medio = entradaDvdp.medio;
        hilo1->medio = medio;

        hilo1->camara = entradaDvdp.camara;
        hilo1->angulositio = entradaDvdp.angulositio;
        hilo1->recibirDatos(0,entradaDvdp.gradlong,entradaDvdp.minlong,entradaDvdp.seglong,entradaDvdp.gradlat,entradaDvdp.minlat,entradaDvdp.seglat,entradaDvdp.AlAntena,grupo);
        hilo1->start();
    }
    else
    {
        barra->showMessage("Entre los datos del la posición.");
        ui->pB_EntrarDatos->setEnabled(true);
        ui->pB_Perfil->setEnabled(false);
        ui->pB_AngulosCierre->setEnabled(false);
        ui->pB_DVDP->setEnabled(false);

        ui->pB_BandaDecimetrica->setEnabled(false);
        ui->pB_BandaMetrica->setEnabled(false);
    }
}

void QVentanaInicialERLyPOV::on_pB_BandaMetrica_clicked()
{
    barra->showMessage("Banda Métrica.");
    ui->pB_BandaMetrica->setEnabled(false);
    plazoletaZCMetrica.calculos = &hilo1->calculos;
    plazoletaZCMetrica.banda = 0;
    plazoletaZCMetrica.setWindowTitle("Banda Métrica");
    plazoletaZCMetrica.show();
}

void QVentanaInicialERLyPOV::on_pB_BandaDecimetrica_clicked()
{
    barra->showMessage("Banda Decimétrica.");
    ui->pB_BandaDecimetrica->setEnabled(false);
    plazoletaZCDeci.calculos = &hilo1->calculos;
    plazoletaZCDeci.banda = 1;
    plazoletaZCDeci.setWindowTitle("Banda Decimétrica");
    plazoletaZCDeci.show();
}

void QVentanaInicialERLyPOV::on_pB_Perfil_clicked()
{
    barra->showMessage("Perfil.");
    ui->pB_Perfil->setEnabled(false);

    perfil.angulositio = angulositio;

    perfil.calculos = &hilo1->calculos;

    perfil.show();
}

void QVentanaInicialERLyPOV::on_pB_AngulosCierre_clicked()
{
    barra->showMessage("Ángulos de Cierre.");
    ui->pB_AngulosCierre->setEnabled(false);

    angcierre.angulositio = angulositio;

    angcierre.calculos = &hilo1->calculos;

    angcierre.show();
}

void QVentanaInicialERLyPOV::on_pB_DVDP_clicked()
{
    barra->showMessage("D.V.D.P.");
    ui->pB_DVDP->setEnabled(false);

    selAltVuelo.calculos = &hilo1->calculos;

    selAltVuelo.dObjetLocale = true;
    selAltVuelo.dm00 = true;
    selAltVuelo.dm25 = true;
    selAltVuelo.dm50 = true;
    selAltVuelo.d100 = true;
    selAltVuelo.d200 = true;
    selAltVuelo.d300 = true;
    selAltVuelo.d400 = true;
    selAltVuelo.d500 = true;
    selAltVuelo.d1000 = true;
    selAltVuelo.d2000 = true;
    selAltVuelo.d4000 = true;
    selAltVuelo.d6000 = true;
    selAltVuelo.d10000 = true;
    selAltVuelo.d17000 = true;
    selAltVuelo.d30000 = true;

    selAltVuelo.radioButons(grupo,medio);
    selAltVuelo.angulositio = angulositio;
    selAltVuelo.configInterface();
    selAltVuelo.show();
}
void QVentanaInicialERLyPOV::closeEvent(QCloseEvent *event)
{
    Q_UNUSED (event);
    qDebug()<<"hilo"<<hilo1->isRunning();
    if (hilo1->isRunning())
        hilo1->calculos.final = true;
    hilo1->quit();
    hilo1->wait();
    selAltVuelo.parar();
    barra->showMessage("Borrando datos incompletos");
    selAltVuelo.close();
    perfil.close();
    angcierre.close();
    plazoletaZCDeci.close();
    plazoletaZCMetrica.close();

    emit termino();
    delete hilo1;
}

void QVentanaInicialERLyPOV::on_pB_Cerrar_clicked()
{
    this->close();
}

