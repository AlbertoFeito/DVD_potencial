#ifndef DLGBIESTATICO_H
#define DLGBIESTATICO_H

#include <QDialog>
//#include "cdistanciabiestatico.h"
#include "dlgbiestaticoentrada.h"
#include "cdvdbiestaticos.h"
#include "qventanainicialerlypov.h"
#include "zpdcaaa.h"
//#include "ccalculos.h"
namespace Ui {
  class DlgBiestatico;
}

class DlgBiestatico : public QDialog
{
  Q_OBJECT

public:
  explicit DlgBiestatico(QWidget *parent = 0);
  ~DlgBiestatico();
  QString tipo;
  bool estado{};
  bool estados();
  //  CCalculos *calculos;
  QString fileTX;
  QString fileRX;
  int medio{};
  int numVentana;
  int numVen{};
signals:
  void cierra(int);
public slots:
  void limpiarlista2(int num);

private slots:
  void on_pB_Transmisor_clicked();

  void on_pB_Receptor_clicked();

  void on_pB_Cancelar_clicked();

  void on_pB_Biestatico_clicked();
protected:
    void closeEvent(QCloseEvent *event);
private:
  Ui::DlgBiestatico *ui;
//  CDistanciaBiestatico *DistBiesta;
  CDVDBiestaticos *DistBiesta{};
//  DlgBiestaticoEntrada DlgBiestEntrada;

//  QVentanaInicialERLyPOV *VentanaInicialBiestatico ;
  QList<QVentanaInicialERLyPOV*> lisVentaInicialBiesta;
  QList<ZPDCAAA*> lisVentaInicialBiestaP;
  QList<CDVDBiestaticos*> lisDistBiesta;

};

#endif // DLGBIESTATICO_H
