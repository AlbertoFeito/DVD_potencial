#include "hilografplazoleta.h"
#include <QDebug>
HiloGrafPlazoleta::HiloGrafPlazoleta(QObject *parent):
  QThread(parent)
{
  AltPos = 0;
  posicion = 0;
  anguloINCidencia = 0;
}

HiloGrafPlazoleta::~HiloGrafPlazoleta()
= default;
//intercepto entre el perfil y los puntos de la horizontal
QList<double> HiloGrafPlazoleta::InterceptoDisCono()
{

  QList<double> listdistancias;
  double m = 0;
  bool encuentra = false;
  QList<double> perfil;
  double dist = 0;
  double DistanciaSelected = 0;
  double DistanciaAux = 0;
  double mcu = 0;
  double mre = 0;
  double ycu = 0;
  double yre = 0;

  AltPos = calculos->datoterrenoMiPos.altura;
  posicion = AltPos+calculos->AlAnt;

  QList<CDatoTerreno*> *datosterreno;
  datosterreno= &calculos->DatosAzimut[azimut].datosterreno;

  perfil.append(AltPos);

  for (int i=0; i < /*Disvuelo/90*/30060/90; ++i)
    {
      perfil.append(datosterreno->at(i)->altura);
    }
  for (int a = 0; a < distanciasPlazoleta.length(); ++a)//a va de 0 a 4
    {
      m = (AltPos - posicion)/(distanciasPlazoleta.at(a));
      encuentra = false;
      for (int i = 1; i < perfil.length(); ++i)
        {
          DistanciaSelected = m * i*90 + posicion;
          DistanciaAux = m * (i-1)*90 + posicion;
          if(DistanciaSelected - perfil.at(i) < 0)
            {
              mcu=(perfil.at(i) - perfil.at(i-1))/90;
              mre=(DistanciaSelected-DistanciaAux)/90;

              for(double x=0;x<=90;x+=0.0001)
                {
                  ycu=mcu*x+perfil.at(i-1);
                  yre=mre*x+DistanciaAux;
                  if (yre<=ycu)
                    {
                      dist=(i-1)*90+(x);
                      listdistancias.append(dist);
                      encuentra = true;
                      if(a == 2)
                        {
                          anguloINCidencia = anguloincidencia(mcu,mre);
                          angulosincidencia[azimut].append(anguloINCidencia);
                          angulosincidencias.append(anguloINCidencia);
                          //                    qDebug()<<anguloINCidencia;
                        }
                      break;
                    }
                }
            }
          if(encuentra)
            break;
        }
    }
  if(listdistancias.length() >=3)
    {
      if(listdistancias.at(2)>=distanciasPlazoleta.at(1) && listdistancias.at(2)<=1.3* distanciasPlazoleta.at(4))
        {
          extensionPlazoleta[azimut].append(QString::number(listdistancias.at(2))+",Corresponde");
          extension.append(listdistancias.at(2));
        }

      if(listdistancias.at(2) < distanciasPlazoleta.at(1))
        {
          extensionPlazoleta[azimut].append(QString::number(listdistancias.at(2))+",No corresponde\n(Insuficiente)");
          extension.append(listdistancias.at(2));
        }

      if(listdistancias.at(2) >1.3* distanciasPlazoleta.at(4))
        {
          extensionPlazoleta[azimut].append(QString::number(listdistancias.at(2))+",No corresponde\n(En exceso)");
          extension.append(listdistancias.at(2));
        }
    }
  else
    {
      extensionPlazoleta[azimut].append("--,No corresponde\n(En exceso)");
      extension.append(0.0);
    }
  return listdistancias;
  //intercepto entre el perfil y los puntos de la horizontal
}
////intercepto entre angulos de cierre y perfil***************************//
QList<double> HiloGrafPlazoleta::InterceptoAnguloyPerfil()
{
  QList<double> interceptos;
  QList<CDatoTerreno*> *datosterreno;

  ////intercepto entre angulos de cierre y perfil***************************//
  datosterreno= &calculos->DatosAzimut[azimut].datosterreno;
  //    datosAngulosCierre = &calculos->DatosAzimut[azimut].datosAngulosCierre;
  QList<double> datosCurva;
  datosCurva.append(AltPos);
  QList<double> datosRecta;//lista de intercepto entre angulos de cierre y perfil
  //    datosRecta.append(posicion);
  int ac = 0;
  int length = calculos->DatosAzimut[azimut].datosAngulosCierre.length();//tamano de la lista de angulos de cierre(picos maximos)
  double dlimt = calculos->DatosAzimut[azimut].datosAngulosCierre.at(length>1?1:0)->distancia;//distancia de los picos maximos
  double recta=0;//intercepto entre angulos de cierre y perfil

  for (int i=0; i < Disvuelo/90; ++i)
    {
      //funcion que define la curva del relieve del terreno
      if(i <=Disvuelo/90-1)
        datosCurva.append(datosterreno->at(i)->altura);
      ///****************//
      if(ac < length)
        {
          if(i*90 > dlimt)
            {
              if(ac < length - 1)
                {
                  ac++;
                  if(ac < length - 1)
                    dlimt = calculos->DatosAzimut[azimut].datosAngulosCierre.at(ac+1)->distancia;
                }
            }
          if(i*90>0)
            recta = calculos->DatosAzimut[azimut].datosAngulosCierre.at(ac)->tangente * i*90 +posicion;
          else
            recta = AltPos;
        }
      datosRecta.append(recta);
    }
  //    datosCurva.removeLast();
  //qDebug()<<azimut<<datosCurva;
  double auxre1 = 0;
  double auxcu1 = 0;
  double mcu = 0;
  double mre = 0;
  double ycu = 0;
  double yre = 0;
  char Signo2 = '+';
  bool tan = true;
  double dist = 0;
  double re = 0;
  double cu = 0;
  //intercepto entre el perfil y los angulos de cierre

  for (auto i : calculos->DatosAzimut[azimut].datosAngulosCierre)
    {//para agragar los angulos de cierre
      interceptos.append(i->distancia);
    }
  for (int j = 1; j < datosRecta.length(); j++)//busco intercepto
    {
      if(interceptos.contains((j-1)*90))
        Signo2 = '+';
      tan = true;
      re=datosRecta.at(j);
      cu=datosCurva.at(j);
      double mod=(re-cu);
      if (mod>0)
        {
          if (Signo2=='-')
            {
              auxre1=datosRecta[j-1];
              auxcu1=datosCurva.at(j-1);

              mre=(re-auxre1)/90;
              mcu=(cu-auxcu1)/90;
              for(double x=0;x<=90;x+=0.0001)
                {
                  ycu=mcu*x+auxcu1;
                  yre=mre*x+auxre1;
                  if (ycu<=yre)
                    {
                      double dist=(j-1)*90+(x);
                      if(!interceptos.contains(dist))
                        {
                          interceptos.append(dist);
                          break;
                        }
                      
                        tan = false;
                    }
                }
              if(tan)
                Signo2='+';
            }
        }
      if (mod<0)
        {
          if (Signo2=='+')
            {
              auxre1=datosRecta[j-1];
              auxcu1=datosCurva.at(j-1);

              mre=(re-auxre1)/90;
              mcu=(cu-auxcu1)/90;
              for(double x=0;x<=90;x+=0.0001)
                {
                  ycu=mcu*x+auxcu1;
                  yre=mre*x+auxre1;
                  if (yre<=ycu)
                    {
                      dist=(j-1)*90+(x);
                      if(!interceptos.contains(dist))
                        {
                          interceptos.append(dist);
                          break;
                        }
                      
                        tan = false;
                    }
                }
              if(tan)
                Signo2='-';
            }
        }
    }
  qSort(interceptos.begin(),interceptos.end());//comparar y ordenar
  //    bool elimine;
  for(int i = 0; i < interceptos.length(); i++)
    {
      if(interceptos.at(i) > 30060 /*listdistplazoleta[azimut].last()*/)
        {
          interceptos.removeAt(i);
          //            elimine = true;
          i--;
        }
    }
  return interceptos;
  //intercepto entre el perfil y los angulos de cierre fin
}
//distancias según tabla
QList<double> HiloGrafPlazoleta::distanciasPZ(double antena)
{
  QList<double> distanciasPz;
  //llenar lista
  if(banda == 0)
    {
      switch (int(antena))
        {
        case 2:
          distanciasPz.append(1.493);
          distanciasPz.append(12);
          distanciasPz.append(25.6);
          distanciasPz.append(37.5);
          distanciasPz.append(49.7);
          DHword = 1.5;
          break;
        case 4:
          distanciasPz.append(5.973);
          distanciasPz.append(47);
          distanciasPz.append(102.4);
          distanciasPz.append(150);
          distanciasPz.append(198.8);
          DHword = 3.0;
          break;
        case 6:
          distanciasPz.append(13.44);
          distanciasPz.append(115);
          distanciasPz.append(230.4);
          distanciasPz.append(335);
          distanciasPz.append(447.3);
          DHword = 4.5;
          break;
        case 8:
          distanciasPz.append(23.893);
          distanciasPz.append(221);
          distanciasPz.append(409.6);
          distanciasPz.append(560);
          distanciasPz.append(795.3);
          DHword = 6.0;
          break;
        case 10:
          distanciasPz.append(37.333);
          distanciasPz.append(302);
          distanciasPz.append(640);
          distanciasPz.append(940);
          distanciasPz.append(1242.6);
          DHword = 7.5;
          break;
        case 16:
          distanciasPz.append(95.573);
          distanciasPz.append(840);
          distanciasPz.append(1638.4);
          distanciasPz.append(2409.91);
          distanciasPz.append(3181.2);
          DHword = 12.0;
          break;
        case 20:
          distanciasPz.append(149.333);
          distanciasPz.append(1360);
          distanciasPz.append(2560);
          distanciasPz.append(3700);
          distanciasPz.append(4970.6);
          DHword = 15.0;
          break;
        case 25:
          distanciasPz.append(233.4);
          distanciasPz.append(2116.7);
          distanciasPz.append(4000);
          distanciasPz.append(5883.3);
          distanciasPz.append(7766.6);
          DHword = 18.75;
          break;
        default:
          break;
        }
    }
  else
    {
      switch (int(antena))
        {
        case 2:
          distanciasPz.append(8.026);
          distanciasPz.append(65);
          distanciasPz.append(137.6);
          distanciasPz.append(202);
          distanciasPz.append(267.1);

          DHword = 1.5;
          break;
        case 4:
          distanciasPz.append(32.106);
          distanciasPz.append(260);
          distanciasPz.append(550.4);
          distanciasPz.append(775);
          distanciasPz.append(1068.6);

          DHword = 3.0;
          break;
        case 6:
          distanciasPz.append(72.24);
          distanciasPz.append(620);
          distanciasPz.append(1238.4);
          distanciasPz.append(1740);
          distanciasPz.append(2404.5);

          DHword = 4.5;
          break;
        case 8:
          distanciasPz.append(128.426);
          distanciasPz.append(1165);
          distanciasPz.append(2201.6);
          distanciasPz.append(3216);
          distanciasPz.append(4274.7);

          DHword = 6.0;
          break;
        case 10:
          distanciasPz.append(200.666);
          distanciasPz.append(1300);
          distanciasPz.append(3440);
          distanciasPz.append(5000);
          distanciasPz.append(6679.3);

          DHword = 7.5;
          break;
        case 16:
          distanciasPz.append(513.706);
          distanciasPz.append(2763);
          distanciasPz.append(8806.4);
          distanciasPz.append(12400);
          distanciasPz.append(17099);

          DHword = 12.0;
          break;
        case 20:
          distanciasPz.append(802.666);
          distanciasPz.append(6400);
          distanciasPz.append(13760);
          distanciasPz.append(19700);
          distanciasPz.append(26717);

          DHword = 15.0;
          break;
        case 25:
          distanciasPz.append(1254.3);
          distanciasPz.append(11378.3);
          distanciasPz.append(21502.3);
          distanciasPz.append(31626.3);
          distanciasPz.append(41750.3);

          DHword = 18.75;
          break;
        default:
          break;
        }
    }
  return distanciasPz ;
}

double HiloGrafPlazoleta::anguloincidencia(double mcu,double mre)
{
  double anguloCurva =qAbs( qAtan(mcu)*GRADOS);
  double anguloRecta =qAbs( qAtan(mre)*GRADOS);
  double compleCurva = 90-anguloCurva;
  double compleRecta = 90-anguloRecta;

  return 180-(compleCurva+compleRecta);

}

void HiloGrafPlazoleta::run()
{
  //    extension.clear();
  distanciasPlazoleta.clear();
  angulosincidencias.clear();
  AltPos = calculos->datoterrenoMiPos.altura;
  posicion = AltPos+calculos->AlAnt;
  distanciasPlazoleta = distanciasPZ(calculos->AlAnt);
  Disvuelo=1000*(4.12*(sqrt(0)+sqrt(posicion)));
  //    qDebug()<<Disvuelo;

  if(boton == 1)   //plazoleta
    {
      double h1 = 0 ;
      double h2;
      double DH;
      int cont;
      double porcientoR;
      for(int a = 0; a < 360; a++)
        {
          QList<CDatoTerreno*> *datosterreno = &calculos->DatosAzimut[a].datosterreno;
          cont = 0;
          porcientoR = 0;
          azimut = a;
          listdistplazoleta[a].append(InterceptoDisCono());

          disLimites[a].append(InterceptoAnguloyPerfil());
          h1 = calculos->datoterrenoMiPos.altura;
          if (listdistplazoleta[a].length()>=3)
            {
              for (int i = 0; i < datosterreno->length()-1; ++i)
                {
                  if(i*90 <= listdistplazoleta[a].at(2))
                    {
                      h2 = datosterreno->at(i)->altura;
                      DH = qAbs(h1-h2);
                      if(DH <= DHword)
                        cont++;
                      h1 = h2;

                    }
                  else
                    {
                      porcientoR = double(cont)/(i) * 100;
                      break;
                    }
                }
            }
          else
            {
              for (int i = 0; i < datosterreno->length()-1; ++i)
                {
                  if(i*90 <= listdistplazoleta[a].last())
                    {
                      h2 = datosterreno->at(i)->altura;
                      DH = qAbs(h1-h2);
                      if(DH <= DHword)
                        cont++;
                      h1 = h2;
                    }
                  else
                    {
                      porcientoR = double(cont)/(i) * 100;
                      break;
                    }
                }
            }

          emit progreso(a,"Calculando datos");
          if(extension.at(a) > 180)
            {
              if(banda == 0  && calculos->AlAnt <=2)
                {
                  deformaciones[a].append("-----,-----");
                }
              else
                {
                  if(porcientoR >= 80.0)
                    {
                      deformaciones[a].append(QString::number(porcientoR,'g',4)+",Regular");
                    }
                  else
                    {
                      deformaciones[a].append(QString::number(porcientoR,'g',4)+",Irregular");
                    }
                }
            }
          else
            {
              deformaciones[a].append("-----,-----");
            }
        }
      emit findehilo(true);
      emit progreso(0,"Ahora puede trabajar");
    }
  else//perfil
    {
      listdist = InterceptoDisCono();
      emit findehiloPerfil(true);
    }
}
