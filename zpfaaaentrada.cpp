#include "zpfaaaentrada.h"
#include "ui_zpfaaaentrada.h"

#include <QIntValidator>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

ZPFAAAEntrada::ZPFAAAEntrada(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZPFAAAEntrada)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowCloseButtonHint);
    cooperacion = 0;
    QSettings setting("trabaltura.ini", QSettings::IniFormat);
    setting.beginGroup("ZPF");
    ui->lineEdit->setText(QString ("%1").arg(setting.value("gradlong",82).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_2->setText(QString ("%1").arg(setting.value("minlong",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_3->setText(QString ("%1").arg(setting.value("seglong",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_4->setText(QString ("%1").arg(setting.value("gradlat",23).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_5->setText(QString ("%1").arg(setting.value("minlat",0).toInt(), 2, 10, QLatin1Char('0')));
    ui->lineEdit_6->setText(QString ("%1").arg(setting.value("seglat",0).toInt(), 2, 10, QLatin1Char('0')));
    int calib;
    int ind;
    calib=setting.value("calibre",100).toInt();
    switch (calib) {
    case (12):
        ind=0;
        ui->lineEdit_7->setText("1.5");
        break;
    case (14):
        ind=1;
        ui->lineEdit_7->setText("1.5");
        break;
    case (23):
        ind=2;
        ui->lineEdit_7->setText("2");
        break;
    case (30):
        ind=3;
        ui->lineEdit_7->setText("2");
        break;
    case (37):
        ind=4;
        ui->lineEdit_7->setText("2");
        break;
    case (57):
        ind=5;
        ui->lineEdit_7->setText("2.5");
        break;
    case (100):
        ind=6;
        ui->lineEdit_7->setText("2.5");
        break;
    default:
        ind=6;
        break;
    }
    ui->comboBox->setCurrentIndex(ind);

    setting.endGroup();
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

    //   QIntValidator *intValAlcannon=new QIntValidator(0,10,this);
    //   ui->lineEdit_7->setValidator(intValAlcannon);

}

ZPFAAAEntrada::~ZPFAAAEntrada()
{
    delete ui;
}

int ZPFAAAEntrada::medios()
{
    if(grupo == 2)
    {
        arma = 2;
        switch (ui->comboBox->currentIndex()) {
        case (0):
            medio = 21;
            break;
        case (1):
            medio = 22;
            break;
        case (2):
            medio = 23;
            break;
        case (3):
            medio = 24;
            break;
        case (4):
            medio = 25;
            break;
        case (5):
            medio = 26;
            break;
        case (6):
            medio = 27;
            break;
        }
    }
    else
    {
        switch (ui->comboBox->currentIndex()) {
        case (0):
            medio = 21;
            break;
        case (1):
            medio = 22;
            break;
        case (2):
            medio = 23;
            break;
        case (3):
            medio = 24;
            break;
        case (4):
            medio = 25;
            break;

        case (5):
            medio = 26;
            break;
        case (6):
            medio = 27;
            break;

        case (7):
            medio = 42;
            arma = 4;
            break;
        case (8):
            medio = 43;
            arma = 4;
            break;
        case (9):
            medio = 44;
            arma = 4;
            break;
        }
    }
    return medio;
}

void ZPFAAAEntrada::grupos(int g)
{
    if(g == 1)
    {
        ui->comboBox->clear();
        ui->comboBox->addItem("12.7",QVariant("12.7"));
        ui->comboBox->addItem("14.5",QVariant("14.5"));
        ui->comboBox->addItem("23",QVariant("23"));
        ui->comboBox->addItem("30",QVariant("30"));
        ui->comboBox->addItem("37",QVariant("37"));

        ui->comboBox->addItem("57",QVariant("57"));
        ui->comboBox->addItem("100",QVariant("100"));

        ui->comboBox->addItem("RPK1",QVariant("RPK1"));
        ui->comboBox->addItem("RPK2",QVariant("RPK2"));
        ui->comboBox->addItem("CHILKA",QVariant("CHILKA"));
    }
    else
    {
        ui->comboBox->clear();
        ui->comboBox->addItem("12.7",QVariant("12.7"));
        ui->comboBox->addItem("14.5",QVariant("14.5"));
        ui->comboBox->addItem("23",QVariant("23"));
        ui->comboBox->addItem("30",QVariant("30"));
        ui->comboBox->addItem("37",QVariant("37"));
        ui->comboBox->addItem("57",QVariant("57"));
        ui->comboBox->addItem("100",QVariant("100"));
//        ui->comboBox->addItem("RPK1",QVariant("RPK1"));
//        ui->comboBox->addItem("RPK2",QVariant("RPK2"));
//        ui->comboBox->addItem("CHILKA",QVariant("CHILKA"));
    }
}
void ZPFAAAEntrada::on_comboBox_currentIndexChanged(int index)
{
    if(grupo == 2)//zpf
    {
        switch (index) {
        case (0):
            ui->lineEdit_7->setText("1.5");
            break;
        case (1):
            ui->lineEdit_7->setText("1.5");
            break;
        case (2):
            ui->lineEdit_7->setText("2");
            break;
        case (3):
            ui->lineEdit_7->setText("2");
            break;
        case (4):
            ui->lineEdit_7->setText("2");
            break;
        case (5):
            ui->lineEdit_7->setText("2.5");
            break;
        case (6):
            ui->lineEdit_7->setText("2.5");
            break;
//        case (7):
//            ui->lineEdit_7->setText("2.5");
//            break;
//        case (8):
//            ui->lineEdit_7->setText("2.5");
//            break;
//        case (9):
//            ui->lineEdit_7->setText("2");
//            break;
        }
    }
    else//zvd
    {
        switch (index) {
        case (0):
            ui->lineEdit_7->setText("1.5");
            break;
        case (1):
            ui->lineEdit_7->setText("1.5");
            break;
        case (2):
            ui->lineEdit_7->setText("2");
            break;
        case (3):
            ui->lineEdit_7->setText("2");
            break;
        case (4):
            ui->lineEdit_7->setText("2");
            break;
        case (5):
            ui->lineEdit_7->setText("2.5");
        case (6):
            ui->lineEdit_7->setText("2.5");
        case (7):
            ui->lineEdit_7->setText("3.7");
            break;
        case (8):
            ui->lineEdit_7->setText("3.7");
            break;
        case (9):
            ui->lineEdit_7->setText("2.5");
            break;
        }
    }
}

void ZPFAAAEntrada::on_pushButton_clicked()
{
    medios();
//    if(grupo == 1 && ( medio == 21 || medio == 22 || medio == 23 || medio == 24 || medio == 25))
//    {
//        QMessageBox mens;
//        mens.setText("El medio seleccionado no tiene radar");
//        mens.exec();
//        reject();
//    }
//    else
//    {
        AlCannon=ui->lineEdit_7->text().toDouble();
        calibre=ui->comboBox->currentText().toFloat();
        gradlong=ui->lineEdit->text().toInt();
        minlong=ui->lineEdit_2->text().toInt();
        seglong=ui->lineEdit_3->text().toInt();

        gradlat=ui->lineEdit_4->text().toInt();
        minlat=ui->lineEdit_5->text().toInt();
        seglat=ui->lineEdit_6->text().toInt();
//        int aux;
//        if (medio == 21 || medio == 22 || medio == 26 || medio == 27 )
//            aux = 2;
//        else
//            aux = AlCannon;
        file = "ZPF_"+QString("%1%2%3_%4%5%6_%7_%8")
                .arg(gradlat, 2, 10, QLatin1Char('0'))
                .arg(minlat, 2, 10, QLatin1Char('0'))
                .arg(seglat, 2, 10, QLatin1Char('0'))
                .arg(gradlong, 2, 10, QLatin1Char('0'))
                .arg(minlong, 2, 10, QLatin1Char('0'))
                .arg(seglong, 2, 10, QLatin1Char('0'))
                .arg(/*aux*/AlCannon, 2,'g', -1, QLatin1Char('0'))
                .arg(calibre, 4, 'g', -1, QLatin1Char('0'));

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
        setting.beginGroup("ZPF");

        setting.setValue("cooperacion",0);
        setting.setValue("gradlong",gradlong);
        setting.setValue("minlong",minlong);
        setting.setValue("seglong",seglong);
        setting.setValue("gradlat",gradlat);
        setting.setValue("minlat",minlat);
        setting.setValue("seglat",seglat);
        setting.setValue("calibre",calibre);
//        setting.setValue("altcannon",AlCannon);
        setting.setValue("medio",medio);
        setting.endGroup();
        accept();
//    }
}

void ZPFAAAEntrada::on_pushButton_2_clicked()
{
    reject();
}
