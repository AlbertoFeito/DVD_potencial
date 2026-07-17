#include "dlgselectzpdozvd.h"
#include "ui_dlgselectzpdozvd.h"

dlgSelectZPDoZVD::dlgSelectZPDoZVD(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgSelectZPDoZVD)
{
    ui->setupUi(this);
}

dlgSelectZPDoZVD::~dlgSelectZPDoZVD()
{
    delete ui;
}

void dlgSelectZPDoZVD::on_pB_Aceptar_clicked()
{
    if(ui->rB_ZPD->isChecked())
        seleccion = true;
    else
        seleccion = false;
    accept();
}

void dlgSelectZPDoZVD::on_pB_Cancelar_clicked()
{
    reject();
}
