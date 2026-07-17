#include "cpuntoapunto.h"
#include "ui_cpuntoapunto.h"
#include "QMessageBox"
#include <cmath>
#include <QSettings>
#include "dancalc.h"


CPuntoAPunto::CPuntoAPunto(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPuntoAPunto)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    representar = new ComonDirecta();
    ui->groupBox->setVisible(true);
    ui->groupBox_3->setVisible(false);

    ui->lineEdit->setAlignment(Qt::AlignCenter);
    ui->lineEdit_2->setAlignment(Qt::AlignCenter);
    ui->lineEdit_3->setAlignment(Qt::AlignCenter);
    ui->lineEdit_4->setAlignment(Qt::AlignCenter);
    ui->lineEdit_5->setAlignment(Qt::AlignCenter);
    ui->lineEdit_6->setAlignment(Qt::AlignCenter);
    ui->lineEdit_7->setAlignment(Qt::AlignCenter);
    ui->lineEdit_8->setAlignment(Qt::AlignCenter);
    ui->lineEdit_9->setAlignment(Qt::AlignCenter);
    ui->lineEdit_10->setAlignment(Qt::AlignCenter);
    ui->lineEdit_11->setAlignment(Qt::AlignCenter);
    ui->lineEdit_12->setAlignment(Qt::AlignCenter);
    ui->lineEdit_13->setAlignment(Qt::AlignCenter);
    ui->lineEdit_14->setAlignment(Qt::AlignCenter);
    ui->lineEdit_15->setAlignment(Qt::AlignCenter);
    ui->lineEdit_16->setAlignment(Qt::AlignCenter);
    ui->lineEdit_17->setAlignment(Qt::AlignCenter);


    QSettings setting("trabaltura.ini", QSettings::IniFormat);
    setting.beginGroup("DatosComones");
    ui->lineEdit->setText(QString ("%1").arg(setting.value("gradlong",82).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_2->setText(QString ("%1").arg(setting.value("minlong",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_3->setText(QString ("%1").arg(setting.value("seglong",0).toFloat()));
    ui->lineEdit_4->setText(QString ("%1").arg(setting.value("gradlat",23).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_5->setText(QString ("%1").arg(setting.value("minlat",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_6->setText(QString ("%1").arg(setting.value("seglat",0).toFloat()));
    ui->lineEdit_7->setText(QString ("%1").arg(setting.value("antena",10).toFloat()));

    ui->lineEdit_12->setText(QString ("%1").arg(setting.value("gradlong2",82).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_14->setText(QString ("%1").arg(setting.value("minlong2",10).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_17->setText(QString ("%1").arg(setting.value("seglong2",0).toFloat()));
    ui->lineEdit_13->setText(QString ("%1").arg(setting.value("gradlat2",23).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_15->setText(QString ("%1").arg(setting.value("minlat2",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_16->setText(QString ("%1").arg(setting.value("seglat2",0).toFloat()));
    ui->lineEdit_11->setText(QString ("%1").arg(setting.value("antena2",10).toFloat()));

    ui->lineEdit_8->setText(QString ("%1").arg(setting.value("direccion",0).toInt(), 3, 10, QLatin1Char('0')));
    ui->lineEdit_9->setText(QString ("%1").arg(setting.value("antena2",10).toFloat()));
    ui->lineEdit_10->setText(QString ("%1").arg(setting.value("distancia",90).toInt()));

    setting.endGroup();
    fichero = "";
}

CPuntoAPunto::~CPuntoAPunto()
{
    representar->close();
    delete representar;
    delete ui;
}

void CPuntoAPunto::on_pushButton_clicked()
{

    distterreno.clear();

    int gradlong= 0;
    int gradlat = 0;
    int minlong = 0;
    int minlat = 0;
    double seglong = 0.0;
    double seglat = 0.0;
    double AlAntena1 = 0.0;

    int gradlong2= 0;
    int gradlat2 = 0;
    int minlong2 = 0;
    int minlat2 = 0;
    double seglong2 = 0.0;
    double seglat2 = 0.0;
    double AlAntena2 = 0.0;

    int direccion = 0;
    double disAntenas= 0;

    double dist = 0;

    AlAntena1 = ui->lineEdit_7->text().toDouble();//altura de la antena

    gradlong = ui->lineEdit->text().toInt();
    minlong = ui->lineEdit_2->text().toInt();
    seglong = ui->lineEdit_3->text().toDouble();

    gradlat = ui->lineEdit_4->text().toInt();
    minlat = ui->lineEdit_5->text().toInt();
    seglat = ui->lineEdit_6->text().toDouble();

    gradlong2 = ui->lineEdit_12->text().toInt();
    minlong2 = ui->lineEdit_14->text().toInt();
    seglong2 = ui->lineEdit_17->text().toDouble();

    gradlat2 = ui->lineEdit_13->text().toInt();
    minlat2 = ui->lineEdit_15->text().toInt();
    seglat2 = ui->lineEdit_16->text().toDouble();

    if ((gradlong < 60)||(gradlong > 100))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los grados de longitud deben estar entre 60 y 100.");
        return;
    }
    if ((minlong < 0)||(minlong > 59))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los minutos deben estar entre 0 y 59.");
        return;
    }
    if ((seglong < 0)||(seglong > 60))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los segundos deben estar entre 0 y 60.");
        return;
    }
    if ((gradlat < 10)||(gradlat > 35))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los grados de latitud deben estar entre 10 y 35.");
        return;
    }
    if ((minlat < 0)||(minlat > 59))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los minutos deben estar entre 0 y 59.");
        return;
    }
    if ((seglat < 0)||(seglat > 60))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los segundos deben estar entre 0 y 60.");
        return;
    }
    if ((gradlong2 < 60)||(gradlong2 > 100))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los grados de longitud deben estar entre 60 y 100.");
        return;
    }
    if ((minlong2 < 0)||(minlong2 > 59))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los minutos deben estar entre 0 y 59.");
        return;
    }
    if ((seglong2 < 0)||(seglong2 > 60))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los segundos deben estar entre 0 y 60.");
        return;
    }
    if ((gradlat2 < 10)||(gradlat2 > 35))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los grados de latitud deben estar entre 10 y 35.");
        return;
    }
    if ((minlat2 < 0)||(minlat2 > 59))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los minutos deben estar entre 0 y 59.");
        return;
    }
    if ((seglat2 < 0)||(seglat2 > 60))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","Los segundos deben estar entre 0 y 60.");
        return;
    }

    //calculos
    latPos.G = gradlat;
    latPos.M = minlat;
    latPos.S = seglat;
    lonPos.G = gradlong;
    lonPos.M = minlong;
    lonPos.S = seglong;

    latPos2.G = gradlat2;
    latPos2.M = minlat2;
    latPos2.S = seglat2;
    lonPos2.G = gradlong2;
    lonPos2.M = minlong2;
    lonPos2.S = seglong2;

    GeoPos.setLatitude (QUtiles::GmsToGrados(latPos));
    GeoPos.setLongitude (QUtiles::GmsToGrados(lonPos)*-1);
    GeoPos2.setLatitude (QUtiles::GmsToGrados(latPos2));
    GeoPos2.setLongitude (QUtiles::GmsToGrados(lonPos2)*-1);

    if(ui->radioButton_2->isChecked())
    {
        direccion =ui->lineEdit_8->text().toInt();//perfil a ver
        AlAntena2 = ui->lineEdit_9->text().toDouble();//altura de antena 2
        disAntenas=ui->lineEdit_10->text().toDouble()/*/1000*/;//distancia entre antenas.
    }
    else
    {
        AlAntena2 = ui->lineEdit_11->text().toDouble();
        //determinar azimut y distancia
//        disAntenas = DeterDis(QUtiles::GmsToGrados(lonPos),QUtiles::GmsToGrados(lonPos2),QUtiles::GmsToGrados(latPos),QUtiles::GmsToGrados(latPos2));
//        direccion = AZimut(QUtiles::GmsToGrados(lonPos),QUtiles::GmsToGrados(lonPos2),QUtiles::GmsToGrados(latPos),QUtiles::GmsToGrados(latPos2));

        disAntenas = distanciaEntrePos();
        direccion = (int)CalculaAzimuthTo ();

    }
    if((direccion < 0) || (direccion > 359))
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","El Azimut debe estar entre 0 y 359.");
        return;
    }
    if(disAntenas < 90)
    {
        QMessageBox::information(this,"Mensaje de la Aplicación.","La distancia mínima debe ser 90m.");
        return;
    }

    short datoAux = 0;

    //para guardar la posicion de la antena
    //datoterrenoMiPos.determinarAltura (&danca,QUtiles::GmsToGrados(latPos),QUtiles::GmsToGrados(lonPos),0,0,0,&datoAux);//determinar la posicion de la antena
    datoterrenoMiPos.determinarAlturaFeito (GeoPos,0,0,&datoAux);
    distterreno.append(datoterrenoMiPos.altura);
    CDatoTerreno *dt;//para guardar los datos del terreno

    const int length1 = qRound(disAntenas / 90.0);
    puntos = new PUNTO*[360];
    for (int a = 0; a < 360; a++)
    {
        puntos[a] = new PUNTO[length1*2];
        for (int d = 0; d < length1*2; d++)
        {
            QUtiles::PolarToReal(a, 0.09 * (d + 1), PM_0_90, &puntos[a][d].X, &puntos[a][d].Y);
        }
    }
    int i = 0;
    while (i < length1*2)//ciclo de distancia
    {
        dist = 90.0 * (i + 1);
        dt = new CDatoTerreno();
        dt->determinarAlturaFeito (GeoPos,direccion,dist,&datoAux);
//        dt->determinarAltura(&danca,QUtiles::GmsToGrados(latPos),QUtiles::GmsToGrados(lonPos),puntos[direccion][i].X,puntos[direccion][i].Y,dist, &datoAux);//determino la altura del terreno
        distterreno.append(dt->altura);
        i++;
    }

    representar->azimut = direccion;
    representar->Antena1 = AlAntena1;
    representar->Antena2 = AlAntena2;
    representar->distancia = disAntenas;
    representar->datosterreno.clear();
    representar->datosterreno = distterreno;


    for (int a = 0; a < 360; a++)
    {
        delete[] puntos[a];
    }
    delete[] puntos;

    fichero = "PaP_"+QString("%1%2%3_%4%5%6_%7_%8_%9")
            .arg(gradlat, 2, 10, QLatin1Char('0'))
            .arg(minlat, 2, 10, QLatin1Char('0'))
            .arg(seglat, 2, 'g',-1, QLatin1Char('0'))
            .arg(gradlong, 2, 10, QLatin1Char('0'))
            .arg(minlong, 2, 10, QLatin1Char('0'))
            .arg(seglong, 2,'g',-1, QLatin1Char('0'))
            .arg(AlAntena1, 2, 'g',-1, QLatin1Char('0'))
            .arg(direccion, 2, 'g',-1, QLatin1Char('0'))
            .arg(disAntenas, 2, 'g',-1, QLatin1Char('0'));

    this->close();
    representar->fichero = fichero;
    representar->pintarComones();
    representar->showMaximized();
}

void CPuntoAPunto::on_pushButton_2_clicked()
{
    representar->close();

    this->close();
}

void CPuntoAPunto::on_radioButton_toggled(bool checked)
{
    if(!checked)
    {
        ui->groupBox->setVisible(true);
        ui->groupBox_3->setVisible(false);
    }
    else
    {
        ui->groupBox->setVisible(false);
        ui->groupBox_3->setVisible(true);
    }
}

void CPuntoAPunto::closeEvent(QCloseEvent *event)
{
    Q_UNUSED (event);
    representar->close();
}
double CPuntoAPunto::DeterDis( double LGO, double LGD, double LTD, double LTO)//PuntoPunto
{
    double Lx;
    double Ly;
    double D;

    double dLatMedia=(LTO+LTD)/2.0;
    double dKfc=111.42325*cos(dLatMedia*RADIAN);
    if(LGO>LGD)
        Lx=(double)((LGO-LGD)*dKfc);
    else
        Lx=(double)((LGD-LGO)*dKfc);

    if(LTO>LTD)
        Ly=(double)((LTO-LTD)*(110.26477+(dLatMedia*0.0186644)));
    else
        Ly=(double)((LTD-LTO)*(110.26477+(dLatMedia*0.0186644)));
    D = danca.Distancia(Lx,Ly)*1000;//lo llevo a metros
    return D;
}

qreal CPuntoAPunto::distanciaEntrePos()
{
  return GeoPos.distanceTo (GeoPos2);
}
qreal CPuntoAPunto::CalculaAzimuthTo()
{    
    return  GeoPos.azimuthTo (GeoPos2);
}
double CPuntoAPunto::AZimut( double LGO, double LGD, double LTD, double LTO)//PuntoPunto
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
