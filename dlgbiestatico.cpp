#include "dlgbiestatico.h"
#include "ui_dlgbiestatico.h"

DlgBiestatico::DlgBiestatico(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DlgBiestatico)
{
  ui->setupUi(this);
  setWindowFlags (Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

  numVentana = 0;
}

DlgBiestatico::~DlgBiestatico()
{
  delete ui;
}
void DlgBiestatico::on_pB_Transmisor_clicked()
{
  tipo = "Tx";

  qDebug()<<"Button Transmisor";
  auto *VentanaInicialBiestatico = new QVentanaInicialERLyPOV();
  lisVentaInicialBiesta.append (VentanaInicialBiestatico);

  VentanaInicialBiestatico->numWindow = numVentana;
  VentanaInicialBiestatico->setObjectName (QString::number (numVentana));
  numVentana++;
  connect (VentanaInicialBiestatico,SIGNAL(biestatico(int)),this, SLOT(limpiarlista2(int)));
//  connect (this,SIGNAL(cierra(int)),VentanaInicialBiestatico, SLOT(biestaticocerro(int)));


  VentanaInicialBiestatico->grupo = 1;
  VentanaInicialBiestatico->medio = medio;

  VentanaInicialBiestatico->arma = 4;

  if(VentanaInicialBiestatico->medio == 45 )
    {
      VentanaInicialBiestatico->hilo1->calculos.tipoBiesta = tipo;
      VentanaInicialBiestatico->seleccionArma(true);
      VentanaInicialBiestatico->setWindowTitle("Ventana inicial de trabajo Bi-18");
    }
  else if(VentanaInicialBiestatico->medio == 46)
    {
      VentanaInicialBiestatico->hilo1->calculos.tipoBiesta = tipo;
      VentanaInicialBiestatico->setWindowTitle("Ventana inicial de trabajo Bi-P");
      VentanaInicialBiestatico->seleccionArma(false);
    }
  else
    {
      VentanaInicialBiestatico->hilo1->calculos.tipoBiesta = tipo;
      VentanaInicialBiestatico->setWindowTitle("Ventana inicial de trabajo Bi-M");
      VentanaInicialBiestatico->seleccionArma(false);
    }

  VentanaInicialBiestatico->show();
}

void DlgBiestatico::on_pB_Receptor_clicked()
{
  tipo = "Rx";

  qDebug()<<"Button Receptor";

  auto *VentanaInicialBiestatico = new QVentanaInicialERLyPOV();
  lisVentaInicialBiesta.append (VentanaInicialBiestatico);

  VentanaInicialBiestatico->numWindow = numVentana;
  VentanaInicialBiestatico->setObjectName (QString::number (numVentana));
  numVentana++;
  connect (VentanaInicialBiestatico,SIGNAL(biestatico(int)),this, SLOT(limpiarlista2(int)));
  connect (this,SIGNAL(cierra(int)),VentanaInicialBiestatico, SLOT(biestaticocerro(int)));

  VentanaInicialBiestatico->grupo = 1;
  VentanaInicialBiestatico->medio = medio;

  VentanaInicialBiestatico->arma = 4;

  if(VentanaInicialBiestatico->medio == 45 )
    {
      VentanaInicialBiestatico->hilo1->calculos.tipoBiesta = tipo;
      VentanaInicialBiestatico->seleccionArma(true);
      VentanaInicialBiestatico->setWindowTitle("Ventana inicial de trabajo Bi-18");
    }
  else if(VentanaInicialBiestatico->medio == 46)
    {
      VentanaInicialBiestatico->hilo1->calculos.tipoBiesta = tipo;
      VentanaInicialBiestatico->setWindowTitle("Ventana inicial de trabajo Bi-P");
      VentanaInicialBiestatico->seleccionArma(false);
    }
  else
    {
      VentanaInicialBiestatico->hilo1->calculos.tipoBiesta = tipo;
      VentanaInicialBiestatico->setWindowTitle("Ventana inicial de trabajo Bi-M");
      VentanaInicialBiestatico->seleccionArma(false);
    }

  VentanaInicialBiestatico->show();
}

void DlgBiestatico::on_pB_Cancelar_clicked()
{
  close ();
}

void DlgBiestatico::limpiarlista2(int num)
{
//  qDebug()<<"limpialista2"<<num<< lisVentaInicialBiesta.at (num)->isVisible ();
  if(lisVentaInicialBiesta.at (num)->isVisible ())
    lisVentaInicialBiesta.at (num)->close ();
}
void DlgBiestatico::on_pB_Biestatico_clicked()
{
  //  DlgBiestaticoEntrada DlgBiestEntrada;

  //  DlgBiestEntrada.medio = medio;
  //  DlgBiestEntrada.leeConfiguracion();

  //  if(DlgBiestEntrada.exec ()== QDialog::Accepted)
  //    {
  //      DistBiesta = new CDistanciaBiestatico();
  //      lisDistBiesta.append (DistBiesta);

  //      DistBiesta->fileRX = DlgBiestEntrada.fileRx;
  //      DistBiesta->fileTX = DlgBiestEntrada.fileTx;
  //      DistBiesta->medio = medio;
  //      DistBiesta->alvue = DlgBiestEntrada.alvue;
  //      if(medio == 45)
  //        {
  //          DistBiesta->setWindowTitle ("Sistema Biestático de la ERL P-18");
  //          DistBiesta->aux = "P-18";
  //        }
  //      else if(medio == 46)
  //        {
  //          DistBiesta->setWindowTitle ("Sistema Biestático del CCAA Pechora");
  //          DistBiesta->aux = "Pechora";
  //        }
  //      else
  //        {
  //          DistBiesta->setWindowTitle ("Sistema Biestático de la MGR");
  //          DistBiesta->aux = "MGR";
  //        }

  //      DistBiesta->pintar();
  //      if(DistBiesta->noFile)
  //        DistBiesta->show ();

  //    }
  DistBiesta = new CDVDBiestaticos(medio);
//  DistBiesta->medio = medio;
  if(medio == 45)
    {
      DistBiesta->setWindowTitle ("Sistema Biestático de la ERL P-18");
      DistBiesta->aux = "P-18";
    }
  else if(medio == 46)
    {
      DistBiesta->setWindowTitle ("Sistema Biestático del CCAA Pechora");
      DistBiesta->aux = "Pechora";
    }
  else
    {
      DistBiesta->setWindowTitle ("Sistema Biestático de la MGR");
      DistBiesta->aux = "MGR";
    }
  DistBiesta->leerZonas ();
  DistBiesta->showMaximized ();
}

void DlgBiestatico::closeEvent(QCloseEvent *event)
{
  Q_UNUSED (event);

  for (int i = 0; i < lisVentaInicialBiesta.length (); ++i)
    {
      emit cierra (i);
    }

  for (auto i : lisDistBiesta)
    {
      i->close ();
    }
}
