#include "dlgseleczpfozvd.h"
#include "ui_dlgseleczpfozvd.h"

dlgSelecZPFoZVD::dlgSelecZPFoZVD(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgSelecZPFoZVD)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    seleccion = false;
}

dlgSelecZPFoZVD::~dlgSelecZPFoZVD()
{
    delete ui;
}

void dlgSelecZPFoZVD::on_pB_Aceptar_clicked()
{
    if(ui->rB_ZPFAAA->isChecked())
        seleccion = true;
    else
        seleccion = false;
    accept();
}

void dlgSelecZPFoZVD::on_pB_Cancelar_clicked()
{
    reject();
}
