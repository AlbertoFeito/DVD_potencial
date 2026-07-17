#include "dlgselcterl.h"
#include "ui_dlgselcterl.h"

DlgSelctERL::DlgSelctERL(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DlgSelctERL)
{
  ui->setupUi(this);
  setWindowFlags(Qt::WindowCloseButtonHint);
}

DlgSelctERL::~DlgSelctERL()
{
  delete ui;
}

void DlgSelctERL::on_pB_Aceptar_clicked()
{
  if(ui->rB_ERL->isChecked())
    {
      medio = 40;
    }
  if(ui->rB_MUIS->isChecked())
    {
      medio = 41;
    }
  if(ui->rB_Biestatico18->isChecked())
    {
      medio = 45;
    }
  if(ui->rB_BiestaticoP->isChecked())
    {
      medio = 46;
    }
  if(ui->rB_BiestaticoM->isChecked())
    {
      medio = 47;
    }
  if(ui->rB_HE->isChecked())
    {
      medio = 48;
    }
  if(ui->rB_Codem->isChecked())
    {
      medio = 49;
    }
  if(ui->rB_P14->isChecked())
    {
      medio = 50;
    }

  accept();
}

void DlgSelctERL::on_pB_Cancelar_clicked()
{
  reject();
}
