#include "qficheroini.h"

QFicheroINI::QFicheroINI()
= default;

void QFicheroINI::readInicio()
{
  QString sDirTrab = QDir::currentPath();
  sTrabIni=sDirTrab+"/trabaltura.ini";

  QSettings setting(sTrabIni, QSettings::IniFormat);

  QStringList groups = setting.childGroups ();
  if(!groups.contains ("ZVD"))
    {
      setting.beginGroup("ZVD");
      setting.setValue("gradlong",82);
      setting.setValue("minlong",0);
      setting.setValue("seglong",0);
      setting.setValue("gradlat",23);
      setting.setValue("minlat",0);
      setting.setValue("seglat",0);
      setting.setValue("antena",2);
      setting.setValue("camara",20);
      setting.setValue("cooperacion",0);
      setting.setValue("medio",0);
      setting.endGroup ();
    }
  if(!groups.contains ("ZPD"))
    {
      setting.beginGroup("ZPD");
      setting.setValue("gradlong",82);
      setting.setValue("minlong",0);
      setting.setValue("seglong",0);
      setting.setValue("gradlat",23);
      setting.setValue("minlat",0);
      setting.setValue("seglat",0);
      setting.setValue("antena",2);
      setting.setValue("cooperacion",0);
      setting.setValue("medio",31);
      setting.endGroup ();
    }
  if(!groups.contains ("ZPF"))
    {
      setting.beginGroup("ZPF");
      setting.setValue("gradlong",82);
      setting.setValue("minlong",0);
      setting.setValue("seglong",0);
      setting.setValue("gradlat",23);
      setting.setValue("minlat",0);
      setting.setValue("seglat",0);
      setting.setValue("antena",2);
      setting.setValue("cooperacion",0);
      setting.setValue("medio",21);
      setting.endGroup ();
    }
}

void QFicheroINI::readIni(const QString& group)
{
  QString sDirTrab = QDir::currentPath();
  sTrabIni=sDirTrab+"/trabaltura.ini";

  QSettings setting(sTrabIni, QSettings::IniFormat);
  setting.beginGroup(group);
  cooperacion=setting.value("cooperacion",0).toInt();
  if(cooperacion == 1)
    {
      gradlong=setting.value("gradlong", 82).toInt();
      minlong=setting.value("minlong",0).toInt();
      seglong=setting.value("seglong",0).toInt();
      gradlat=setting.value("gradlat",23).toInt();
      minlat=setting.value("minlat",0).toInt();
      seglat=setting.value("seglat",0).toInt();
      medio = setting.value("medio",0).toInt();
      AlAntena = setting.value("antena",4).toDouble ();
      camara = setting.value("camara",20).toInt();
      setting.endGroup();
      if ((gradlong<74)||(gradlong>84))
        {
          QMessageBox mens;
          mens.setText("Los grados de longitud deben estar entre 74 y 86.");
          mens.exec();
          exit(0);
        }
      if ((minlong<0)||(minlong>59))
        {
          QMessageBox mens;
          mens.setText("Los minutos deben estar entre 0 y 59.");
          mens.exec();
          exit(0);
        }
      if ((seglong<0)||(seglong>59))
        {
          QMessageBox mens;
          mens.setText("Los segundos deben estar entre 0 y 59.");
          mens.exec();
          exit(0);
        }
      if ((gradlat<19)||(gradlat>24))
        {
          QMessageBox mens;
          mens.setText("Los grados de latitud deben estar entre 19 y 24.");
          mens.exec();
          exit(0);
        }
      if ((minlat<0)||(minlat>59))
        {
          QMessageBox mens;
          mens.setText("Los minutos deben estar entre 0 y 59.");
          mens.exec();
          exit(0);
        }
      if ((seglat<0)||(seglat>59))
        {
          QMessageBox mens;
          mens.setText("Los segundos deben estar entre 0 y 59.");
          mens.exec();
          exit(0);
        }
    }
  else
    setting.endGroup();
}
double QFicheroINI::DatDelMedio(int m)
{
  double alMedio = 0.0;
  switch (m) {
    case (0):
      alMedio = 2;
      arma = 0;
      break;
    case (11):
      alMedio = 6;
      arma = 1;
      break;
    case (12):
      alMedio = 6;
      arma = 1;
      break;
    case (13):
      alMedio = 6;
      arma = 1;
      break;
    case (21):
      alMedio = 1.5;
      arma = 2;
      break;
    case (22):
      alMedio = 1.5;
      arma = 2;
      break;
    case (23):
      alMedio = 2;
      arma = 2;
      break;
    case (24):
      alMedio = 2;
      arma = 2;
      break;
    case (25):
      alMedio = 2;
      arma = 2;
      break;
    case (26):
      alMedio = 3.7;
      arma = 2;
      break;
    case (27):
      alMedio = 3.7;
      arma = 2;
      break;
    case (31):
      alMedio = 2;
      arma = 3;
      break;
    case (32):
      alMedio = 1.6;
      arma = 3;
      break;
    case (33):
      alMedio = 1.6;
      arma = 3;
      break;
    case (34):
      alMedio = 1.6;
      arma = 3;
      break;
    case (35):
      alMedio = 2;
      arma = 3;
      break;
    case (36):
      alMedio = 5;
      arma = 3;
      break;
    case (37):
      alMedio = 6;
      arma = 3;
      break;
    case (38):
      alMedio = 4;
      arma = 3;
      break;
    case (39):
      alMedio = 6;
      arma = 3;
      break;
    case (40):
      alMedio = 6;
      arma = 4;
      break;
    case (41):
      alMedio = 4;
      arma = 4;
      break;
    case (42):
      alMedio = 3.7;
      arma = 4;
      break;
    case (43):
      alMedio = 3.7;
      arma = 4;
      break;
    case (44):
      alMedio =2.5;
      arma = 4;
      break;
    case (48):
      alMedio = 4;
      arma = 4;
      break;
    case (49):
      alMedio = 6;
      arma = 4;
    case (50):
      alMedio = 6;
      arma = 4;
      break;
    }
  AlAntena = alMedio;
  return alMedio;
}
void QFicheroINI::Salir(const QString& group)
{
  QSettings setting(sTrabIni, QSettings::IniFormat);
  setting.beginGroup(group);
  setting.setValue("gradlong",gradlong);
  setting.setValue("minlong",minlong);
  setting.setValue("seglong",seglong);
  setting.setValue("gradlat",gradlat);
  setting.setValue("minlat",minlat);
  setting.setValue("seglat",seglat);
  setting.setValue("antena",AlAntena);
  setting.setValue("camara",camara);
  setting.setValue("medio",medio);
  setting.setValue("cooperacion",0);
  setting.endGroup();
}
void QFicheroINI::readZPD()
{

}

void QFicheroINI::readZPF()
{

}
