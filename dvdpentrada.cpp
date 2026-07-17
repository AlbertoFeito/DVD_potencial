#include "dvdpentrada.h"
#include "ui_dvdpentrada.h"


#include <QMessageBox>
#include <QIntValidator>

DVDPEntrada::DVDPEntrada(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DVDPEntrada)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    QSettings setting("trabaltura.ini", QSettings::IniFormat);
    setting.beginGroup("ZVD");
    ui->lE_GradLon->setText(QString ("%1").arg(setting.value("gradlong",82).toInt(), 2, 10, QLatin1Char('0')));
    ui->lE_MinLon->setText(QString ("%1").arg(setting.value("minlong",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lE_SegLon->setText(QString ("%1").arg(setting.value("seglong",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lE_GradLat->setText(QString ("%1").arg(setting.value("gradlat",23).toInt(), 2, 10, QLatin1Char('0')));
    ui->lE_MinLat->setText(QString ("%1").arg(setting.value("minlat",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lE_SegLat->setText(QString ("%1").arg(setting.value("seglat",0).toInt(), 2, 10, QLatin1Char('0')));
    int al;
    int ind;
    al=setting.value("antena",6).toInt();
    switch (al) {
    /*******************/
    case (0):
        ind=0;
        break;
    case (1):
        ind=1;
        break;
      /*********************/
    case (2):
        ind=2;
        break;
    case (4):
        ind=3;
        break;
    case (6):
        ind=4;
        break;
    case (8):
        ind=5;
        break;
    case (10):
        ind=6;
        break;
    case (16):
        ind=7;
        break;
    case (20):
        ind=8;
        break;
    case (25):
        ind=9;
        break;
    default:
        ind=6;
        break;
    }
    ui->cB_AlturaAntena->setCurrentIndex(ind);

    setting.endGroup();
    ui->lE_GradLon->setAlignment(Qt::AlignCenter);
    ui->lE_MinLon->setAlignment(Qt::AlignCenter);
    ui->lE_SegLon->setAlignment(Qt::AlignCenter);
    ui->lE_GradLat->setAlignment(Qt::AlignCenter);
    ui->lE_MinLat->setAlignment(Qt::AlignCenter);
    ui->lE_SegLat->setAlignment(Qt::AlignCenter);

    auto *intValGradlon = new QIntValidator( 60, 100, this );
    ui->lE_GradLon->setValidator(intValGradlon);

    auto *intValGradlat = new QIntValidator( 10, 35, this );
    ui->lE_GradLat->setValidator(intValGradlat);


    auto *intValMinSeg = new QIntValidator(0, 59, this );
    ui->lE_MinLon->setValidator(intValMinSeg);
    ui->lE_SegLon->setValidator(intValMinSeg);
    ui->lE_MinLat->setValidator(intValMinSeg);
    ui->lE_SegLat->setValidator(intValMinSeg);

    angulositio = 0;
}

DVDPEntrada::~DVDPEntrada()
{
    delete ui;
}

void DVDPEntrada::selecionArma()
{
    ui->label_6->setVisible(false);
    ui->comboBox->setVisible(false);

    if(arma == 1)
    {
        ui->lB_CalibreCamara->setVisible(true);
        ui->lB_CalibreCamara->setText("Cámara(Km)");
        ui->cB_Camara->setVisible(true);
        ui->lB_AlturaAntenasCannon->setText("Altura del equipo(m)");
    }
    else if(arma == 0)
    {
        ui->lB_AlturaAntenasCannon->setText("Altura del equipo(m)");
        ui->lB_CalibreCamara->setVisible(false);
        ui->cB_Camara->setVisible(false);
        medio = 0;
    }
    else
    {
        ui->lB_CalibreCamara->setVisible(false);
        ui->cB_Camara->setVisible(false);
        //        medio = 40;

        if(medio == 41)
        {
            ui->cB_AlturaAntena->clear();
            ui->cB_AlturaAntena->addItem("4",QVariant("4"));
        }
        else if(medio == 48)
        {
            ui->cB_AlturaAntena->clear();
            ui->cB_AlturaAntena->addItem("4",QVariant("4"));
            ui->label_6->setVisible(true);
            ui->comboBox->setVisible(true);

        }
        else if(medio == 46)
        {
            ui->cB_AlturaAntena->clear();
            ui->cB_AlturaAntena->addItem("4",QVariant("4"));
        }
        else
        {
            ui->cB_AlturaAntena->clear();
            ui->cB_AlturaAntena->addItem("2",QVariant("2"));
            ui->cB_AlturaAntena->addItem("4",QVariant("4"));
            ui->cB_AlturaAntena->addItem("6",QVariant("6"));
            ui->cB_AlturaAntena->addItem("8",QVariant("8"));
            ui->cB_AlturaAntena->addItem("10",QVariant("10"));
            ui->cB_AlturaAntena->addItem("12",QVariant("12"));
            ui->cB_AlturaAntena->addItem("16",QVariant("16"));
            ui->cB_AlturaAntena->addItem("20",QVariant("20"));
            ui->cB_AlturaAntena->addItem("25",QVariant("25"));
        }
    }
}

void DVDPEntrada::on_pB_Aceptar_clicked()
{
    if(medio == 48)
        angulositio = ui->comboBox->currentText().toInt();
    else
        angulositio=0;
    camara = ui->cB_Camara->currentText().toInt();
    if(arma == 1)
    {
        if(camara == 20)
            medio = 11;
        if(camara == 30)
            medio = 12;
        if(camara == 40)
            medio = 13;
    }
    AlAntena = ui->cB_AlturaAntena->currentText().toFloat();//altura de la antena o cannon o rampa

    // CoefK=ui->lineEdit_8->text().toDouble();
    gradlong=ui->lE_GradLon->text().toInt();
    minlong=ui->lE_MinLon->text().toInt();
    seglong=ui->lE_SegLon->text().toInt();

    gradlat=ui->lE_GradLat->text().toInt();
    minlat=ui->lE_MinLat->text().toInt();
    seglat=ui->lE_SegLat->text().toInt();
    //file=ui->lE_GradLat->text()+ui->lE_MinLat->text()+ui->lE_SegLat->text()+"_"+ui->lineEdit->text()+ui->lineEdit_2->text()+ui->lineEdit_3->text()+"_"+ui->lineEdit_7->text();

    file = "ZVD_"+QString::number(medio)+"_"+QString("%1%2%3_%4%5%6_%7")
            .arg(gradlat, 2, 10, QLatin1Char('0'))
            .arg(minlat, 2, 10, QLatin1Char('0'))
            .arg(seglat, 2, 10, QLatin1Char('0'))
            .arg(gradlong, 2, 10, QLatin1Char('0'))
            .arg(minlong, 2, 10, QLatin1Char('0'))
            .arg(seglong, 2, 10, QLatin1Char('0'))
            .arg(AlAntena, 2, 'g',-1, QLatin1Char('0'));

    if ((gradlong<74)||(gradlong>84))
    {
        QMessageBox mens;
        mens.setText("Los grados de longitud deben estar entre 74 y 86.");
        mens.exec();
        return;
    }
    if ((minlong<0)||(minlong>59))
    {
        QMessageBox mens;
        mens.setText("Los minutos deben estar entre 0 y 59.");
        mens.exec();
        return;
    }
    if ((seglong<0)||(seglong>59))
    {
        QMessageBox mens;
        mens.setText("Los segundos deben estar entre 0 y 59.");
        mens.exec();
        return;
    }
    if ((gradlat<19)||(gradlat>24))
    {
        QMessageBox mens;
        mens.setText("Los grados de latitud deben estar entre 19 y 24.");
        mens.exec();
        return;
    }
    if ((minlat<0)||(minlat>59))
    {
        QMessageBox mens;
        mens.setText("Los minutos deben estar entre 0 y 59.");
        mens.exec();
        return;
    }
    if ((seglat<0)||(seglat>59))
    {
        QMessageBox mens;
        mens.setText("Los segundos deben estar entre 0 y 59.");
        mens.exec();
        return;
    }


    QSettings setting("trabaltura.ini", QSettings::IniFormat);
    setting.beginGroup("ZVD");
    setting.setValue("cooperacion",0);
    setting.setValue("gradlong",gradlong);
    setting.setValue("minlong",minlong);
    setting.setValue("seglong",seglong);
    setting.setValue("gradlat",gradlat);
    setting.setValue("minlat",minlat);
    setting.setValue("seglat",seglat);
    setting.setValue("antena",AlAntena);
    setting.setValue("camara",camara);
    setting.setValue("medio",medio);
    setting.endGroup();
    accept();
}

void DVDPEntrada::on_pB_Cancelar_clicked()
{
    reject();
}
