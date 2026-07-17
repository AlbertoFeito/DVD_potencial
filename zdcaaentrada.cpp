#include "zdcaaentrada.h"
#include "ui_zdcaaentrada.h"


#include <QIntValidator>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
ZDCAAEntrada::ZDCAAEntrada(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZDCAAEntrada)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowCloseButtonHint);
    QSettings setting("trabaltura.ini", QSettings::IniFormat);
    setting.beginGroup("ZPD");
    ui->lineEdit->setText(QString ("%1").arg(setting.value("gradlong",82).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_2->setText(QString ("%1").arg(setting.value("minlong",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_3->setText(QString ("%1").arg(setting.value("seglong",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_4->setText(QString ("%1").arg(setting.value("gradlat",23).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_5->setText(QString ("%1").arg(setting.value("minlat",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_6->setText(QString ("%1").arg(setting.value("seglat",0).toInt(), 2, 10, QLatin1Char('0')));

    //    int al;
    //    int ind;
    //    al=setting.value("alrampa",4).toFloat();
    //    switch (al) {
    //    case (1):
    //        ind=0;
    //        break;
    //    case (2):
    //        ind=1;
    //        break;
    //    default:
    //        ind=1;
    //        break;
    //    }
    //    ui->comboBox->setCurrentIndex(ind);

    QString aux;
    aux=setting.value("cohete","C1").toString();
    if(aux=="C1")
        ui->lineEdit_7->setText("2");
    else if(aux=="C2")
        ui->lineEdit_7->setText("1.6");
    else if(aux=="C3")
        ui->lineEdit_7->setText("1.6");
    else if(aux=="Igla")
        ui->lineEdit_7->setText("1.6");   
    else if(aux=="C10")
        ui->lineEdit_7->setText("2");
    else if(aux=="OCA-AKM")
        ui->lineEdit_7->setText("5");
    else if(aux=="Cuadrado")
        ui->lineEdit_7->setText("6");
    else if(aux=="Pechora")
        ui->lineEdit_7->setText("4");
    else if(aux=="Volga")
        ui->lineEdit_7->setText("6");

    setting.endGroup();
    ui->comboBox_2->setCurrentIndex(8);

    ui->lineEdit->setAlignment(Qt::AlignCenter);
    ui->lineEdit_2->setAlignment(Qt::AlignCenter);
    ui->lineEdit_3->setAlignment(Qt::AlignCenter);
    ui->lineEdit_4->setAlignment(Qt::AlignCenter);
    ui->lineEdit_5->setAlignment(Qt::AlignCenter);
    ui->lineEdit_6->setAlignment(Qt::AlignCenter);

    auto *intValGradlon = new QIntValidator( 60, 100, this );
    ui->lineEdit->setValidator(intValGradlon);

    auto *intValGradlat = new QIntValidator( 10, 35, this );
    ui->lineEdit_4->setValidator(intValGradlat);


    auto *intValMinSeg = new QIntValidator(0, 59, this );
    ui->lineEdit_2->setValidator(intValMinSeg);
    ui->lineEdit_3->setValidator(intValMinSeg);
    ui->lineEdit_5->setValidator(intValMinSeg);
    ui->lineEdit_6->setValidator(intValMinSeg);

}

ZDCAAEntrada::~ZDCAAEntrada()
{
    delete ui;
}

int ZDCAAEntrada::medios()
{
    if(grupo == 3)
    {
        switch (ui->comboBox_2->currentIndex()) {
        case (0):
            medio = 31;
            break;
        case (1):
            medio = 32;
            break;
        case (2):
            medio = 33;
            break;
        case (3):
            medio = 34;
            break;
        case (4):
            medio = 35;
            break;
        case (5):
            medio = 36;
            break;
        case (6):
            medio = 37;
            break;
        case (7):
            medio = 38;
            break;
        case (8):
            medio = 39;
            break;

        }
    }
    else
    {
//        switch (ui->comboBox_2->currentIndex()) {
//        case (0):
//            medio = 36;
//            break;
//        case (1):
//            medio = 37;
//            break;
//        case (2):
//            medio = 38;
//            break;
//        case (3):
//            medio = 39;
//            break;
//        }
        switch (ui->comboBox_2->currentIndex()) {
        case (0):
            medio = 31;
            break;
        case (1):
            medio = 32;
            break;
        case (2):
            medio = 33;
            break;
        case (3):
            medio = 34;
            break;
        case (4):
            medio = 35;
            break;
        case (5):
            medio = 36;
            break;
        case (6):
            medio = 37;
            break;
        case (7):
            medio = 38;
            break;
        case (8):
            medio = 39;
            break;

        }
    }
    return medio;
}

void ZDCAAEntrada::grupos(int g)
{
    if(g == 1)
    {
        ui->comboBox_2->clear();
        ui->comboBox_2->addItem("C1",QVariant("C1"));
        ui->comboBox_2->addItem("C2",QVariant("C2"));
        ui->comboBox_2->addItem("C3",QVariant("C3"));
        ui->comboBox_2->addItem("Igla",QVariant("Igla"));
        ui->comboBox_2->addItem("C10",QVariant("C10"));
        ui->comboBox_2->addItem("OCA_AKM",QVariant("OCA_AKM"));
        ui->comboBox_2->addItem("Cuadrado",QVariant("Cuadrado"));
        ui->comboBox_2->addItem("Pechora",QVariant("Pechora"));
        ui->comboBox_2->addItem("Volga",QVariant("Volga"));


    }
    else
    {
        ui->comboBox_2->clear();
        ui->comboBox_2->addItem("C1",QVariant("C1"));
        ui->comboBox_2->addItem("C2",QVariant("C2"));
        ui->comboBox_2->addItem("C3",QVariant("C3"));
        ui->comboBox_2->addItem("Igla",QVariant("Igla"));
        ui->comboBox_2->addItem("C10",QVariant("C10"));
        ui->comboBox_2->addItem("OCA_AKM",QVariant("OCA_AKM"));
        ui->comboBox_2->addItem("Cuadrado",QVariant("Cuadrado"));
        ui->comboBox_2->addItem("Pechora",QVariant("Pechora"));
        ui->comboBox_2->addItem("Volga",QVariant("Volga"));

    }
}
void ZDCAAEntrada::on_comboBox_2_currentIndexChanged(int index)
{
    if(grupo == 3)
    {
        switch (index) {
        case (0):
            ui->lineEdit_7->setText("2");
            break;
        case (1):
            ui->lineEdit_7->setText("1.6");
            break;
        case (2):
            ui->lineEdit_7->setText("1.6");
            break;
        case (3):
            ui->lineEdit_7->setText("1.6");
            break;
        case (4):
            ui->lineEdit_7->setText("2");
            break;
        case (5):
            ui->lineEdit_7->setText("5");
            break;
        case (6):
            ui->lineEdit_7->setText("6");
            break;
        case (7):
            ui->lineEdit_7->setText("4");
            break;
        case (8):
            ui->lineEdit_7->setText("6");
            break;      
        }
    }
    else
    {
//        switch (index) {
//        case (0):
//            ui->lineEdit_7->setText("5");
//            break;
//        case (1):
//            ui->lineEdit_7->setText("6");
//            break;
//        case (2):
//            ui->lineEdit_7->setText("4");
//            break;
//        case (3):
//            ui->lineEdit_7->setText("6");
//            break;
//        }
        switch (index) {
        case (0):
            ui->lineEdit_7->setText("2");
            break;
        case (1):
            ui->lineEdit_7->setText("1.6");
            break;
        case (2):
            ui->lineEdit_7->setText("1.6");
            break;
        case (3):
            ui->lineEdit_7->setText("1.6");
            break;
        case (4):
            ui->lineEdit_7->setText("2");
            break;
        case (5):
            ui->lineEdit_7->setText("5");
            break;
        case (6):
            ui->lineEdit_7->setText("6");
            break;
        case (7):
            ui->lineEdit_7->setText("4");
            break;
        case (8):
            ui->lineEdit_7->setText("6");
            break;
        }
    }
}

void ZDCAAEntrada::on_pushButton_clicked()
{
    medios();
//    if(grupo == 1 && ( medio == 31 || medio == 32 || medio == 33 || medio == 34 || medio == 35))
//    {
//        QMessageBox mens;
//        mens.setText("El medio seleccionado no tiene radar");
//        mens.exec();
//        reject();
//    }
//    else
//    {
        Alrampa=ui->lineEdit_7->text().toDouble();//altura de la rampa
        cohete=ui->comboBox_2->currentText();
        gradlong=ui->lineEdit->text().toInt();
        minlong=ui->lineEdit_2->text().toInt();
        seglong=ui->lineEdit_3->text().toInt();

        gradlat=ui->lineEdit_4->text().toInt();
        minlat=ui->lineEdit_5->text().toInt();
        seglat=ui->lineEdit_6->text().toInt();
//        int aux;
//        if (medio == 32 || medio == 33 || medio == 34)
//            aux = 2;
//        else
//            aux = Alrampa;
        file = "ZPD_"+QString("%1%2%3_%4%5%6_%7_%8")
                .arg(gradlat, 2, 10, QLatin1Char('0'))
                .arg(minlat, 2, 10, QLatin1Char('0'))
                .arg(seglat, 2, 10, QLatin1Char('0'))
                .arg(gradlong, 2, 10, QLatin1Char('0'))
                .arg(minlong, 2, 10, QLatin1Char('0'))
                .arg(seglong, 2, 10, QLatin1Char('0'))
                .arg(/*aux*/Alrampa, 2, 'g', -1, QLatin1Char('0'))
                .arg(cohete);
        qDebug()<<"ZDCAAEntrada file"<<file;
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
        setting.beginGroup("ZPD");
        setting.setValue("gradlong",gradlong);
        setting.setValue("minlong",minlong);
        setting.setValue("seglong",seglong);
        setting.setValue("gradlat",gradlat);
        setting.setValue("minlat",minlat);
        setting.setValue("seglat",seglat);
//        setting.setValue("antena",Alrampa);
        setting.setValue("cohete",cohete);
        setting.setValue("medio",medio);
        setting.setValue("cooperacion",0);
        setting.endGroup();
        accept();
//    }
}

void ZDCAAEntrada::on_pushButton_2_clicked()
{
    reject();
}

//void ZDCAAEntrada::on_cB_Velocidad_currentTextChanged(const QString &arg1)
//{
//    velocidad = arg1.toInt ();
//    velocidad = 300;
//    tiempo = 25;
//}

//void ZDCAAEntrada::on_cB_Tiempo_currentTextChanged(const QString &arg1)
//{
//   tiempo = arg1.toInt ();
//   velocidad = 300;
//   tiempo = 25;
//}
