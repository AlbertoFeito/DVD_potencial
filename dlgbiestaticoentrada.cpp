#include "dlgbiestaticoentrada.h"
#include "ui_dlgbiestaticoentrada.h"

DlgBiestaticoEntrada::DlgBiestaticoEntrada(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DlgBiestaticoEntrada)
{
  ui->setupUi(this);
  setWindowFlags (Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

  ext = ".m00";

  alvue = QString(ui->cB_AltVuelo->currentText ()).remove (" m").toInt ();
  setWindowTitle ("Ventana de entrada de datos de los Sistemas Biestáticos");
}

DlgBiestaticoEntrada::~DlgBiestaticoEntrada()
{
  delete ui;
}

void DlgBiestaticoEntrada::guardaConfiguracion()
{
  QSettings setting("trabaltura.ini", QSettings::IniFormat);
  setting.beginGroup("ZVD_Biestatico");
  setting.setValue("GlonTX" ,GlonTX);
  setting.setValue("MlonTX" ,MlonTX);
  setting.setValue("SlonTX" ,SlonTX);
  setting.setValue("GlatTX" ,GlatTX);
  setting.setValue("MlatTX" ,MlatTX);
  setting.setValue("SlatTX" ,SlatTX);

  setting.setValue("GlonRX" ,GlonRX);
  setting.setValue("MlonRX" ,MlonRX);
  setting.setValue("SlonRX" ,SlonRX);
  setting.setValue("GlatRX" ,GlatRX);
  setting.setValue("MlatRX" ,MlatRX);
  setting.setValue("SlatRX" ,SlatRX);

  setting.setValue("AntenaTX" ,AntenaTX);
  setting.setValue("AntenaRX" ,AntenaRX);

  setting.setValue("medio",medio);
  setting.endGroup();

  accept();
}

void DlgBiestaticoEntrada::leeConfiguracion()
{
  QSettings setting("trabaltura.ini", QSettings::IniFormat);
  setting.beginGroup("ZVD_Biestatico");

  GlonTX = (setting.value("GlonTX",82).toInt ());
  MlonTX = (setting.value("MlonTX", 0).toInt ());
  SlonTX = (setting.value("SlonTX", 0).toInt ());
  GlatTX = (setting.value("GlatTX",23).toInt ());
  MlatTX = (setting.value("MlatTX", 0).toInt ());
  SlatTX = (setting.value("SlatTX", 0).toInt ());
  AntenaTX = (setting.value ("AntenaTX",2).toInt ());

  GlonRX = (setting.value("GlonRX",82).toInt ());
  MlonRX = (setting.value("MlonRX", 0).toInt ());
  SlonRX = (setting.value("SlonRX", 0).toInt ());
  GlatRX = (setting.value("GlatRX",23).toInt ());
  MlatRX = (setting.value("MlatRX", 0).toInt ());
  SlatRX = (setting.value("SlatRX", 0).toInt ());
  AntenaRX = (setting.value ("AntenaRX",2).toInt ());

  setting.endGroup();

  ui->sB_GLongTX->setValue(GlonTX);
  ui->sB_MLongTX->setValue(MlonTX);
  ui->sB_SLongTX->setValue(SlonTX);
  ui->sB_GLatTX->setValue (GlatTX);
  ui->sB_MLatTX->setValue (MlatTX);
  ui->sB_SLatTX->setValue (SlatTX);

  ui->sB_GLongRX->setValue (GlonRX);
  ui->sB_MLongRX->setValue (MlonRX);
  ui->sB_SLongRX->setValue (SlonRX);
  ui->sB_GLatRX->setValue  (GlatRX);
  ui->sB_MLatRX->setValue  (MlatRX);
  ui->sB_SLatRX->setValue  (SlatRX);

  ui->cB_AntenaRX->setCurrentText (QString::number (AntenaRX));
  ui->cB_AntenaTX->setCurrentText (QString::number (AntenaTX));

  connect(ui->sB_GLatTX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_MLatTX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_SLatTX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_GLongTX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_MLongTX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_SLongTX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->cB_AntenaTX,SIGNAL(currentTextChanged(QString)),this,SLOT(editTX(QString)));

  connect(ui->sB_GLatRX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_MLatRX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_SLatRX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_GLongRX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_MLongRX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->sB_SLongRX,SIGNAL(valueChanged(QString)),this,SLOT(editTX(QString)));
  connect(ui->cB_AntenaRX,SIGNAL(currentTextChanged(QString)),this,SLOT(editTX(QString)));
  FicherosNombres (false);
}

void DlgBiestaticoEntrada::FicherosNombres(bool cambio)
{
  if(cambio)
    {
      GlatTX = ui->sB_GLatTX->value ();
      MlatTX = ui->sB_MLatTX->value ();
      SlatTX = ui->sB_SLatTX->value ();
      GlonTX = ui->sB_GLongTX->value();
      MlonTX = ui->sB_MLongTX->value();
      SlonTX = ui->sB_SLongTX->value();
      AntenaTX = ui->cB_AntenaTX->currentText ().toInt ();

      GlatRX = ui->sB_GLatRX->value ();
      MlatRX = ui->sB_MLatRX->value ();
      SlatRX = ui->sB_SLatRX->value ();
      GlonRX = ui->sB_GLongRX->value ();
      MlonRX = ui->sB_MLongRX->value ();
      SlonRX = ui->sB_SLongRX->value ();
      AntenaRX = ui->cB_AntenaRX->currentText ().toInt ();
    }
  fileTx = QString("ZVD_"+QString::number (medio)+"_%1%2%3_%4%5%6_%7Tx"+ext)//confecciono el nombre del fichero
      .arg(GlatTX, 2, 10, QLatin1Char('0'))
      .arg(MlatTX, 2, 10, QLatin1Char('0'))
      .arg(SlatTX, 2, 10, QLatin1Char('0'))
      .arg(GlonTX, 2, 10, QLatin1Char('0'))
      .arg(MlonTX, 2, 10, QLatin1Char('0'))
      .arg(SlonTX, 2, 10, QLatin1Char('0'))
      .arg(AntenaTX, 2, 10, QLatin1Char('0'));

  ui->lE_FileTX->setText (fileTx);

  fileRx = QString("ZVD_"+QString::number (medio)+"_%1%2%3_%4%5%6_%7Rx"+ext)//confecciono el nombre del fichero
      .arg(GlatRX, 2, 10, QLatin1Char('0'))
      .arg(MlatRX, 2, 10, QLatin1Char('0'))
      .arg(SlatRX, 2, 10, QLatin1Char('0'))
      .arg(GlonRX, 2, 10, QLatin1Char('0'))
      .arg(MlonRX, 2, 10, QLatin1Char('0'))
      .arg(SlonRX, 2, 10, QLatin1Char('0'))
      .arg(AntenaRX, 2, 10, QLatin1Char('0'));

  ui->lE_FileRX->setText (fileRx);
}

void DlgBiestaticoEntrada::editTX(const QString& a)
{
  Q_UNUSED (a);

  FicherosNombres(true);
}

void DlgBiestaticoEntrada::on_pB_Aceptar_clicked()
{
  guardaConfiguracion ();

}

void DlgBiestaticoEntrada::on_pB_Cancelar_clicked()
{
  reject ();
}

void DlgBiestaticoEntrada::on_cB_AltVuelo_currentIndexChanged(int index)
{
  alvue = QString(ui->cB_AltVuelo->currentText ()).remove (" m").toInt ();
  qDebug()<<alvue;
  switch (index) {
    case 0: ext = ".m00"; break;//0

    case 1: ext = ".m25"; break;//25

    case 2: ext = ".m50"; break;//50

    case 3: ext = ".001"; break;//100

    case 4: ext = ".002"; break;//200

    case 5: ext = ".003"; break;//300

    case 6: ext = ".004"; break;//400

    case 7: ext = ".005"; break;//500

    case 8: ext = ".010"; break;//1000

    case 9: ext = ".020"; break;//2000

    case 10: ext = ".040"; break;//4000

    case 11: ext = ".060" ;break;//6000

    case 12: ext = ".100"; break;//10000

    case 13: ext = ".170"; break;//17000

    case 14: ext = ".300"; break;//30000

    default:
      break;
    }

  FicherosNombres(true);
}
