#include "ccalculos.h"

#include <QFile>
#include <QDir>
#include <ctime>
#include <QDebug>
#include "dancalc.h"

const int length1p = qRound(320000.0 / 90.0);//distancia 320km para el calculo del perfil del terreno
const int length2p = qRound(190000.0 / 90.0);//distancia 190km para el calculo de los angulos de cierre

CCalculos::CCalculos(QObject *parent) : QObject(parent)
{
    angulositio = 0;
    puntos = new PUNTO*[360];
    for (int a = 0; a < 360; a++)
    {
        puntos[a] = new PUNTO[length1p];
        for (int d = 0; d < length1p; d++)
        {
            QUtiles::PolarToReal(a, 0.09 * (d + 1), PM_0_90, &puntos[a][d].X, &puntos[a][d].Y);
        }
    }
    final = false;

    tipoBiesta = "NULL";

    dismax = 0;
}

CCalculos::~CCalculos()
{
    for (int a = 0; a < 360; a++)
    {
        delete[] puntos[a];
    }
    delete[] puntos;
}

void CCalculos::calcularDatos()
{
    distanciaGrupo(grupo,medio);
    // dismax = determDistanCiclo (grupo,arma,medio);
    short datoAux = 0;
    DanCalc danca;
    //para guardar la posicion de la antena
    datoterrenoMiPos.determinarAltura(&danca,QUtiles::GmsToGrados(latPos),QUtiles::GmsToGrados(lonPos),0,0,0,&datoAux);//determinar la posicion de la antena

    if(!leer())
    {
        double listAC[length2];//para almacenar todos los angulos de cierre cada 90m hasta 150km
        int i = 0;
        int i_ang_c = 0;
        int i_p_max = 0;
        double dist;
        double last_ac_max = INT_MIN;//

        CDatoTerreno *dt;//para guardar los datos del terreno
        CDatoAnguloCierre *Ac;//para guardar los angulosde cierre
        for (int a = 0; a < 360; a++)//ciclo de los azimut
        {
            if(final)
            {
                emit fin(final,0);
                return;
            }
            i = 0;
            i_ang_c = 0;
            i_p_max = 0;
            last_ac_max = INT_MIN;
            DatosAzimut[a].clearlistaDTerreno();//libero la memoria de la lista
            DatosAzimut[a].clearlistaDAcierre();//libero la memoria de la lista

            Ac = new CDatoAnguloCierre();
            Ac->angulo = -3440 * (AlAnt/1.5);
            Ac->distancia = 1.5;
            Ac->tangente = Ac->angulo / 3440;
            Ac->altura = datoterrenoMiPos.altura;
            DatosAzimut[a].datosAngulosCierre.append(Ac);
            //              last_ac_max = listAC[dismax/90];
            while (i < length1)//ciclo de distancia
            {
                dist = 90.0 * (i + 1);
                dt = new CDatoTerreno();
                dt->determinarAltura(&danca,QUtiles::GmsToGrados(latPos),QUtiles::GmsToGrados(lonPos),puntos[a][i].X,puntos[a][i].Y,dist, &datoAux);//determino la altura del terreno
                DatosAzimut[a].datosterreno.append(dt);//agrego a la lista de datos del terreno

                if(i_ang_c < length2)
                {
                    //                  if(i == 133)
                    //                    qDebug()<<dt->altura<<datoterrenoMiPos.altura<<AlAnt<<dist<<"  ----  "<<3440.0 * (dt->altura - datoterrenoMiPos.altura - AlAnt) / dist;
                    listAC[i_ang_c] = 3440.0 * (dt->altura - datoterrenoMiPos.altura - AlAnt) / dist;//calculo los angulos de cierre
                    i_ang_c++;
                }

                if (i_ang_c > 1 && i_p_max < length2)//para determinar los picos maximos de angulo de cierre
                {
                    if (i_p_max > 0 && i_p_max < (length2 - 1))
                    {
                        if (listAC[i_p_max] > listAC[i_p_max - 1] && listAC[i_p_max] > listAC[i_p_max + 1] && listAC[i_p_max] > last_ac_max)
                        {
                            Ac = new CDatoAnguloCierre();
                            Ac->angulo = listAC[i_p_max];//para almacenar los angulos de cierre mayores(picos maximos)
                            Ac->distancia = 90.0 * (i_p_max + 1);//para almacenar la distancia a la que se encuentran los picos maximos
                            Ac->tangente = Ac->angulo / 3440;//para calcular las tangentes de cada pico maximo
                            Ac->altura = DatosAzimut[a].datosterreno[i_p_max]->altura;
                            DatosAzimut[a].datosAngulosCierre.append(Ac);//para almacenar los datos de la lista de angulo de cierre
                            last_ac_max = listAC[i_p_max];
                        }
                    }
                    else if(i_p_max == 0) // Chequeando primer angulo de cierre
                    {
                        if (listAC[i_p_max] > listAC[i_p_max + 1] && listAC[i_p_max] > last_ac_max)
                        {
                            Ac = new CDatoAnguloCierre();
                            Ac->angulo = listAC[i_p_max];
                            Ac->distancia = 90.0 * (i_p_max + 1);
                            Ac->tangente = Ac->angulo / 3440;
                            Ac->altura = DatosAzimut[a].datosterreno[i_p_max]->altura;
                            DatosAzimut[a].datosAngulosCierre.append(Ac);
                            last_ac_max = listAC[i_p_max];
                        }
                    }
                    else // Chequeando último angulo de cierre
                    {
                        if (listAC[i_p_max] > listAC[i_p_max - 1] && listAC[i_p_max] > last_ac_max)
                        {
                            Ac = new CDatoAnguloCierre();
                            Ac->angulo = listAC[i_p_max];
                            Ac->distancia = 90.0 * (i_p_max + 1);
                            Ac->tangente = Ac->angulo / 3440;
                            Ac->altura = DatosAzimut[a].datosterreno[i_p_max]->altura;
                            DatosAzimut[a].datosAngulosCierre.append(Ac);
                            last_ac_max = listAC[i_p_max];
                        }
                    }
                    i_p_max++;
                }
                i++;
            }
            if(DatosAzimut[a].datosAngulosCierre.length ()<1)
            {
                //              Ac = new CDatoAnguloCierre();
                //              Ac->angulo = -3440 * (AlAnt/1.5);
                //              Ac->distancia = 1.5;
                //              Ac->tangente = Ac->angulo / 3440;
                //              Ac->altura = datoterrenoMiPos.altura;
                //              DatosAzimut[a].datosAngulosCierre.append(Ac);
                ////              last_ac_max = listAC[dismax/90];
            }
            emit fin(false,a);
        }
        guardar();//para guardar en el fichero todos los datos obtenido
    }
    emit fin(true,0);
}

bool CCalculos::guardar()
{
    QString auxFichero;
    QString auxLabel;
    nombres(&auxFichero,&auxLabel);

    qDebug()<<"guardar"<<"auxLabel"<<auxLabel<<"auxFichero"<<auxFichero<<"medio"<<medio;
//    int aux;
//    if (medio == 21 || medio == 22 || medio == 26 || medio == 27 || medio == 32 || medio == 33 || medio == 34 || medio == 42 || medio == 43)
//        aux = 2;
//    else
//        aux = AlAnt;

    label = QString(auxLabel+"_%1%2%3_%4%5%6_%7")//confecciono el nombre del fichero
            .arg(latPos.G, 2, 10, QLatin1Char('0'))
            .arg(latPos.M, 2, 10, QLatin1Char('0'))
            .arg(latPos.S, 2, 10, QLatin1Char('0'))
            .arg(lonPos.G, 2, 10, QLatin1Char('0'))
            .arg(lonPos.M, 2, 10, QLatin1Char('0'))
            .arg(lonPos.S, 2, 10, QLatin1Char('0'))
            .arg(AlAnt, 2, 'g',-1, QLatin1Char('0'));

    fichero = QString(auxFichero+"_%1%2%3_%4%5%6_%7")//confecciono el nombre del fichero
            .arg(latPos.G, 2, 10, QLatin1Char('0'))
            .arg(latPos.M, 2, 10, QLatin1Char('0'))
            .arg(latPos.S, 2, 10, QLatin1Char('0'))
            .arg(lonPos.G, 2, 10, QLatin1Char('0'))
            .arg(lonPos.M, 2, 10, QLatin1Char('0'))
            .arg(lonPos.S, 2, 10, QLatin1Char('0'))
            .arg(/*aux*/AlAnt, 2, 'g',-1, QLatin1Char('0'));
    fichero.replace ('.',',');
    if(tipoBiesta != "NULL")
    {
        fichero = fichero + tipoBiesta;
        label = label + tipoBiesta;
    }
    QString filename = QString(QDir::currentPath() + "/Temporal/ZVD_%1%2%3_%4%5%6_%7_dta")//confecciono el nombre del fichero
            .arg(latPos.G, 2, 10, QLatin1Char('0'))
            .arg(latPos.M, 2, 10, QLatin1Char('0'))
            .arg(latPos.S, 2, 10, QLatin1Char('0'))
            .arg(lonPos.G, 2, 10, QLatin1Char('0'))
            .arg(lonPos.M, 2, 10, QLatin1Char('0'))
            .arg(lonPos.S, 2, 10, QLatin1Char('0'))
            .arg(AlAnt, 2, 'g',-1, QLatin1Char('0'));

    QFile f(filename);
    QFile f2(filename+".csv");
    if(f.open(QFile::WriteOnly))
    {
        f2.open (QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&f2);
        qint32 length = 360;
        f.write((char*)&length,sizeof (length));
        for (int i = 0; i < length; i++)//para ir guardando azimut por azimut todos los datos
        {
            out << i << "; ";
            DatosAzimut[i].guardar(&f);
            for (int j = 0;j<27;j++) {
              out << DatosAzimut[i].datosterreno.at (j)->altura <<"; ";
            }
            out << "\n";
        }
        f.close();
        f2.flush();
        f2.close();
        return true;
    }

    return false;
}

bool CCalculos::leer()
{
    bool bOk = false;

    QString filename = QString(QDir::currentPath() + "/Temporal/ZVD_%1%2%3_%4%5%6_%7_dta")//confecciono el nombre del fichero
            .arg(latPos.G, 2, 10, QLatin1Char('0'))
            .arg(latPos.M, 2, 10, QLatin1Char('0'))
            .arg(latPos.S, 2, 10, QLatin1Char('0'))
            .arg(lonPos.G, 2, 10, QLatin1Char('0'))
            .arg(lonPos.M, 2, 10, QLatin1Char('0'))
            .arg(lonPos.S, 2, 10, QLatin1Char('0'))
            .arg(AlAnt, 2, 'g',-1, QLatin1Char('0'));

    QFile f(filename);
    if(f.open(QFile::ReadOnly))
    {
        if(f.size() >= 4)//para comprobar si está vacio
        {
            int cant = 0;
            f.read((char*)&cant, sizeof(int));
            for(int i = 0; i < cant; i++)
            {
                DatosAzimut[i].leer(&f);
                emit fin(false,i);
            }
            bOk = DatosAzimut[0].datosterreno.length() >= length1;
        }
        f.close();
    }
    if(bOk)
    {

        QString auxFichero;
        QString auxLabel;
        nombres(&auxFichero,&auxLabel);
//        int aux;
//        if (medio == 21 || medio == 22 || medio == 26 || medio == 27 || medio == 32 || medio == 33 || medio == 34 || medio == 42 || medio == 43)
//            aux = 2;
//        else
//            aux = AlAnt;

        label = QString(auxLabel+"_%1%2%3_%4%5%6_%7")//confecciono el nombre del fichero
                .arg(latPos.G, 2, 10, QLatin1Char('0'))
                .arg(latPos.M, 2, 10, QLatin1Char('0'))
                .arg(latPos.S, 2, 10, QLatin1Char('0'))
                .arg(lonPos.G, 2, 10, QLatin1Char('0'))
                .arg(lonPos.M, 2, 10, QLatin1Char('0'))
                .arg(lonPos.S, 2, 10, QLatin1Char('0'))
                .arg(AlAnt, 2, 'g',-1, QLatin1Char('0'));

        fichero = QString(auxFichero+"_%1%2%3_%4%5%6_%7")//confecciono el nombre del fichero
                .arg(latPos.G, 2, 10, QLatin1Char('0'))
                .arg(latPos.M, 2, 10, QLatin1Char('0'))
                .arg(latPos.S, 2, 10, QLatin1Char('0'))
                .arg(lonPos.G, 2, 10, QLatin1Char('0'))
                .arg(lonPos.M, 2, 10, QLatin1Char('0'))
                .arg(lonPos.S, 2, 10, QLatin1Char('0'))
                .arg(/*aux*/AlAnt, 2, 'g',-1, QLatin1Char('0'));
fichero.replace ('.',',');
        if(tipoBiesta != "NULL")
        {
            fichero = fichero + tipoBiesta;
            label = label + tipoBiesta;
        }
    }
    return bOk;
}

void CCalculos::nombres(QString *auxFichero , QString *auxLabel)
{
    switch (grupo)
    {
    case 1:
    {
        if(medio == 48)
            *auxFichero = "ZVD_"+QString::number(medio)+ "_"+ QString::number(angulositio);
        else
            *auxFichero = "ZVD_"+QString::number(medio);
        switch (arma)
        {

        case 0: *auxLabel = "POV"; break;
        case 1: *auxLabel = "POP_C"+QString::number(camara); break;
        case 2: *auxLabel = "AAA_"+QString::number(calibre);break;
        case 3: *auxLabel = "CAA_"+cohete; break;
        case 4:
        {
            switch (medio)
            {
            case 40: *auxLabel = "ERL"; break;
            case 41: *auxLabel = "MUIS"; break;
            case 42: *auxLabel = "RPK1"; break;
            case 43: *auxLabel = "RPK2"; break;
            case 44: *auxLabel = "CHILKA"; break;
            case 45: *auxLabel = "BIESTATICO_18"; break;
            case 46: *auxLabel = "BIESTATICO_P"; break;
            case 47: *auxLabel = "BIESTATICO_M"; break;
            case 48: *auxLabel = "HAZ_ESTRECHO_" + QString::number(angulositio); break;
            case 49: *auxLabel = "KODEM"; break;
            case 50: *auxLabel = "P-14"; break;
            }
        }
            break;
        }
    }
        break;
    case 2:
    {
        *auxLabel = "AAA_"+QString::number(calibre);
        *auxFichero = "ZPF_"+QString::number(medio);
        switch (medio)
        {
        case 42: *auxLabel = "RPK1"; break;
        case 43: *auxLabel = "RPK2"; break;
        case 44: *auxLabel = "CHILKA"; break;
        }
    }
        break;
    case 3:
    {
        *auxLabel = "CAA_"+cohete;
        *auxFichero = "ZPD_"+QString::number(medio);
    }
        break;
    }
}
//int CCalculos::determDistanCiclo(int g, int a, int m)
//{
//  int distanciaAC=0;
//  switch (g)
//    {
//    case 1:
//      switch (a)
//        {
//        case 0: distanciaAC = 12060; break;
//        case 1:
//          switch (m)
//            {
//            case 11: distanciaAC = 20070; break;
//            case 12: distanciaAC = 30060; break;
//            case 13: distanciaAC = 40050; break;
//            }
//          break;
//        case 3:
//          switch (m)
//            {
//            case 36: distanciaAC = 45000; break;
//            case 37: distanciaAC = 65070; break;
//            case 38: distanciaAC = 80010; break;
//            case 39: distanciaAC = 150030; break;
//            }
//          break;
//        case 4:
//          switch (m)
//            {
//            case 40: distanciaAC = 190080; break;
//            case 41: distanciaAC = 185040; break;
//            case 42: distanciaAC = 60030; break;
//            case 43: distanciaAC = 18000; break;
//            case 44: distanciaAC = 22050; break;

//            case 45: distanciaAC = 120060; break;
//            case 46: distanciaAC = 30060; break;
//            case 47: distanciaAC = 60030; break;

//            case 48: distanciaAC = 185040; break;
//            case 49: distanciaAC = 185040; break;
//            case 50: distanciaAC = 700020; break;
//            }
//          break;
//        }
//      break;
//    case 2:
//      switch (m)
//        {
//        case 21: distanciaAC = 1530; break;
//        case 22: distanciaAC = 2520; break;
//        case 23: distanciaAC = 2520; break;
//        case 44: distanciaAC = 2520; break;
//        case 24: distanciaAC = 3060; break;
//        case 25: distanciaAC = 4050; break;
//        case 26: distanciaAC = 6030; break;
//        case 43: distanciaAC = 6030; break;
//        case 27: distanciaAC = 12060; break;
//        case 42: distanciaAC = 12060; break;
//        }
//      break;
//    case 3:
//      switch (m)
//        {
//        case 31: distanciaAC = 4050; break;
//        case 32: distanciaAC = 2070; break;
//        case 33: distanciaAC = 2520; break;
//        case 34: distanciaAC = 3060; break;
//        case 35: distanciaAC = 5040; break;
//        case 36: distanciaAC = 10080; break;
//        case 37: distanciaAC = 20070; break;
//        case 38: distanciaAC = 17010; break;
//        case 39: distanciaAC = 42840; break;
//        }
//      break;
//    }
//  return distanciaAC;
//}

void CCalculos::distanciaGrupo(int g, int m)
{
    double distancia;
    if(m == 0)
    {
        distancia = 20070.0;
        length2 = qRound(distancia / 90.0);

    }
    else if(m == 40 || medio == 50)//erl o p14
    {
        distancia = 320040.0;
        length2 = qRound(190080.0 / 90.0);
    }
    else if(m == 45)
    {
        if(tipoBiesta == "Tx")
        {
            distancia = 150030.0;
            length2 = qRound(distancia / 90.0);
        }
        else
        {
            distancia = 320040.0;
            length2 = qRound(190080.0 / 90.0);
        }
    }
    else if(m == 46)
    {
        distancia = 60030.0;
        length2 = qRound(distancia / 90.0);

    }
    else if(m == 47)
    {
        distancia = 65070.0;
        length2 = qRound(distancia / 90.0);
    }
    else if(m == 41 || m == 48 || m == 49)
    {
        distancia = 200070.0;
        length2 = qRound(180000 / 90.0);
    }
    else
    {
        if(g == 1 && (m == 42 || m == 43 || m == 44 || m == 36 || m==37 || m==38 || m==39))
        {
            distancia = 160020.0;
        }
        else if(g == 1 && (m == 31 || m == 32 || m == 33 || m == 34 || m==35 || m == 21 || m == 22 || m == 23 || m == 24 || m==25 || m==26 || m==27))
        {
            distancia = 12330.0;
        }
        else
        {
            distancia = 50040.0;
        }
        length2 = qRound(distancia / 90.0);
    }
    length1 = qRound(distancia / 90.0);
    dismax = distancia/1000;
}
