#include "perfil.h"
#include "ui_perfil.h"
#include <QSettings>
#include <QIntValidator>
#include <QDebug>
Perfil::Perfil(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Perfil)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    grafperfil = new Graficaperfil();

    barra = new QStatusBar(this);
    barra->showMessage("Entre el Azimut para ver el perfil");
    ui->layautBarra->addWidget(barra);

    QSettings setting("trabaltura.ini", QSettings::IniFormat);
    setting.beginGroup("Perfil");
    ui->lineEdit->setText(QString ("%1").arg(setting.value("azimut",0).toInt()));
    setting.endGroup();
    ui->lineEdit->setAlignment(Qt::AlignCenter);
    auto *intValPerfil=new QIntValidator(0,359,this);
    ui->lineEdit->setValidator(intValPerfil);
    delete intValPerfil;
    arma = 0;
    camara = 0;
}

Perfil::~Perfil()
{

    delete ui;
}
void Perfil::closeEvent(QCloseEvent *event)
{
    Q_UNUSED (event);
    grafperfil->close();
//    delete grafperfil;
//    delete barra;
    emit  cerro();
}
void Perfil::on_pushButton_2_clicked()//cancelar
{    
    emit  cerro();
    grafperfil->close();
    this->close();
}

void Perfil::on_pushButton_clicked()//aceptar
{
    grafperfil->close();

//    grafperfil->fichero = fichero;
//    grafperfil->arma = arma;
//    grafperfil->camara = camara;
//    grafperfil->calibre = calibre;
//    grafperfil->cohete = cohete;

    int a=ui->lineEdit->text().toInt();
    if ((a>=0) && (a<=359))
    {
        barra->showMessage("Mostrando grafica del perfil deseado. Un momento por favor");

        QSettings setting("trabaltura.ini", QSettings::IniFormat);
        setting.beginGroup("Perfil");
        setting.setValue("azimut",a);
        setting.endGroup();

        grafperfil->angulositio = angulositio;
        grafperfil->calculos = calculos;
        grafperfil->aazimut = a;
        grafperfil->pintar();

        grafperfil->showMaximized();
        barra->showMessage("Entre el Azimut para ver el perfil");
    }
    else
    {
        QMessageBox mens;
        mens.setText("El Azimut debe estar entre 0° y 359°");
        mens.exec();
    }
}
