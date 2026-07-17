#include "angulocierre.h"
#include "ui_angulocierre.h"

angulocierre::angulocierre(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::angulocierre)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    barra = new QStatusBar(this);
    barra->showMessage("Escoja que Forma desea ver");
    ui->layautBarra->addWidget(barra);
}

angulocierre::~angulocierre()
{
    delete barra;
    delete ui;
}
void angulocierre::closeEvent(QCloseEvent *event)
{
    Q_UNUSED (event);
    for (auto & var : lisangcierre1)
    {
        var->close();
    }
    lisangcierre1.clear();
    for (auto & var : lisAnguCierre2)
    {
        var->close();
    }
    lisAnguCierre2.clear();
   emit  cerro();
}
void angulocierre::on_pushButton_2_clicked()//cerrar
{
    emit  cerro();
    this->close();
}
void angulocierre::on_pushButton_clicked()//aceptar
{
    if (ui->radioButton->isChecked())
    {
        forma1=new angcierre1();
//        forma1->medio = medio;
//        forma1->arma = arma;
//        forma1->grupo = grupo;
        forma1->calculos = calculos;
//        forma1->fichero = fichero;
        forma1->pintar();
        forma1->showMaximized();
        lisangcierre1.append(forma1);
    }
    else
    {
        forma2 = new AnguCierre2();
//        forma2->grupo = grupo;
//        forma2->arma = arma;
//        forma2->medio = medio;
        forma2->calculos = calculos;
//        forma2->Fichero = fichero;
        forma2->pintar();
        forma2->showMaximized();
        lisAnguCierre2.append(forma2);
    }
}
