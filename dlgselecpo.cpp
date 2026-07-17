#include "dlgselecpo.h"
#include "ui_dlgselecpo.h"

dlgSelecPO::dlgSelecPO(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgSelecPO)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    seleccion = false;
}

dlgSelecPO::~dlgSelecPO()
{
    delete ui;
}

void dlgSelecPO::on_pushButton_clicked()
{
    if(ui->rB_POV->isChecked())
        seleccion = true;
    else
        seleccion = false;
    accept();
}

void dlgSelecPO::on_pushButton_2_clicked()
{
    reject();
}
