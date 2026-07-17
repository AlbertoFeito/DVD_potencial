#include "c_plazoletazc.h"
#include "ui_c_plazoletazc.h"
#include <QDebug>
C_PlazoletaZC::C_PlazoletaZC(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::C_PlazoletaZC)
{
    ui->setupUi(this);
     setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    perfilZC = new C_PerfiZC();
    grafplazoleta = new c_GrafPlazoleta();
    azimut = 0;

    connect(&hilografplazoleta,SIGNAL(findehilo(bool)),this,SLOT(findhilo(bool)));

    connect(&hiloperfil,SIGNAL(findehiloPerfil(bool)),this,SLOT(findhiloPerfil(bool)));

    connect(&hilografplazoleta,SIGNAL(progreso(int,QString)),this,SLOT(progreso(int,QString)));
}

C_PlazoletaZC::~C_PlazoletaZC()
{
    delete perfilZC;
    delete grafplazoleta;
    delete ui;

}

void C_PlazoletaZC::progreso(int c, const QString& mensaje)
{Q_UNUSED(mensaje);
    ui->pBar_Progreso->setValue(c);
}

void C_PlazoletaZC::findhilo(bool final)
{Q_UNUSED(final);
    ui->pBar_Progreso->reset();
    ui->pBar_Progreso->setVisible(false);
    ui->pB_Plazoleta->setEnabled(true);
    grafplazoleta->Disvuelo = hilografplazoleta.Disvuelo;
    grafplazoleta->fichero = calculos->label;
    grafplazoleta->banda = banda;
    grafplazoleta->anguloincidencias.clear();
    grafplazoleta->anguloincidencias = hilografplazoleta.angulosincidencias;
    grafplazoleta->antena = calculos->AlAnt;
    for(int a = 0; a < 360; a++)
    {
        azimut = a;
        grafplazoleta->listdistplazoleta[a].clear();
        grafplazoleta->InterceptoAnguloyPerfil[a].clear();
        grafplazoleta->listdistplazoleta[a].append(hilografplazoleta.listdistplazoleta[a]);
        grafplazoleta->InterceptoAnguloyPerfil[a].append(hilografplazoleta.disLimites[a]);
        grafplazoleta->anguloincidencia[a].clear();
        grafplazoleta->anguloincidencia[a].append(hilografplazoleta.angulosincidencia[a]);
        grafplazoleta->extensionPlazoleta[a].clear();
        grafplazoleta->extensionPlazoleta[a].append(hilografplazoleta.extensionPlazoleta[a]);
        grafplazoleta->deformaciones[a].clear();
        grafplazoleta->deformaciones[a].append(hilografplazoleta.deformaciones[a]);
    }
    grafplazoleta->distanciasPlazoleta.clear();
    grafplazoleta->distanciasPlazoleta = hilografplazoleta.distanciasPlazoleta;


    grafplazoleta->pintar();
    grafplazoleta->pintarDistancias();
    grafplazoleta->llenartabla(true);
    grafplazoleta->pintarCalifGeneral();

    if(banda == 0)
    {
        grafplazoleta->setWindowTitle("Plazoleta Zona Cercana-Banda Métrica-Posición: "+fichero);
    }
    else
    {
        grafplazoleta->setWindowTitle("Plazoleta Zona Cercana-Banda Decimétrica-Posición: "+fichero);
    }

    grafplazoleta->showMaximized();
    lisCustomPlot.append(grafplazoleta);
}

void C_PlazoletaZC::findhiloPerfil(bool final)
{Q_UNUSED(final);
    ui->pB_PerfilZC->setEnabled(true);
    perfilZC->AltPos = hiloperfil.AltPos;
    perfilZC->posicion = hiloperfil.posicion;
    perfilZC->listdist = hiloperfil.listdist;
    perfilZC->Disvuelo = hiloperfil.Disvuelo;
    perfilZC->distanciasPlazoleta.clear();
    perfilZC->distanciasPlazoleta = hiloperfil.distanciasPlazoleta;
    perfilZC->anguloINCidencia = hiloperfil.anguloINCidencia;
    perfilZC->pintar();
    perfilZC->setWindowTitle("Perfil Zona cercana Az: "+QString::number(ui->spinBox->text().toInt())+"°");
    perfilZC->showMaximized();
}
void C_PlazoletaZC::closeEvent(QCloseEvent *event)
{
    Q_UNUSED (event);
    perfilZC->close();
    grafplazoleta->close();
    for (auto & var : lisCustomPlot)
    {
        var->close();
    }
    lisCustomPlot.clear();
hilografplazoleta.extension.clear();
    for(int a = 0; a < 360; a++)
    {
        for (int i = 0; i < hilografplazoleta.listdistplazoleta[a].length(); ++i)
        {
            hilografplazoleta.listdistplazoleta[a].clear();
            hilografplazoleta.disLimites[a].clear();
            hilografplazoleta.angulosincidencia[a].clear();
            hilografplazoleta.extensionPlazoleta[a].clear();
            hilografplazoleta.deformaciones[a].clear();

        }
    }
    emit  cerro();
}
void C_PlazoletaZC::on_pB_Plazoleta_clicked()
{
    hilografplazoleta.extension.clear();
    for(int a = 0; a < 360; a++)
    {
        for (int i = 0; i < hilografplazoleta.listdistplazoleta[a].length(); ++i)
        {
            hilografplazoleta.listdistplazoleta[a].clear();
            hilografplazoleta.disLimites[a].clear();
            hilografplazoleta.angulosincidencia[a].clear();
            hilografplazoleta.extensionPlazoleta[a].clear();

        }
    }
    ui->pBar_Progreso->setMaximum(360);
    ui->pBar_Progreso->setVisible(true);
    ui->pB_Plazoleta->setEnabled(false);
    grafplazoleta->close();
    hilografplazoleta.calculos = calculos;
    hilografplazoleta.boton = true;
    hilografplazoleta.banda = banda;
    if(hilografplazoleta.isRunning())
        hilografplazoleta.quit();
    hilografplazoleta.start();

}

void C_PlazoletaZC::on_pB_PerfilZC_clicked()
{
    for(int a = 0; a < 360; a++)
    {
        for (int i = 0; i < hiloperfil.listdistplazoleta[a].length(); ++i)
        {
            hiloperfil.listdistplazoleta[a].clear();
            hiloperfil.disLimites[a].clear();
        }
    }
    ui->pBar_Progreso->setMaximum(360);
    ui->pBar_Progreso->setVisible(true);
    ui->pB_PerfilZC->setEnabled(false);
    perfilZC->close();
    perfilZC->aazimut = ui->spinBox->text().toInt();
    perfilZC->calculos = calculos;
//    perfilZC->fichero = fichero;
    perfilZC->banda = banda;
    perfilZC->listdist.clear();

    hiloperfil.calculos = calculos;
    hiloperfil.boton = false;
    hiloperfil.azimut = ui->spinBox->text().toInt();
    hiloperfil.banda = banda;
    if(hiloperfil.isRunning())
        hiloperfil.quit();
    hiloperfil.start();
}
