#include "dlgobjetivop.h"
#include "ui_dlgobjetivop.h"

dlgObjetivoP::dlgObjetivoP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgObjetivoP)
{
    ui->setupUi(this);
}

dlgObjetivoP::~dlgObjetivoP()
{
    delete ui;
}

void dlgObjetivoP::on_pB_Aceptar_clicked()
{
    accept();
}
