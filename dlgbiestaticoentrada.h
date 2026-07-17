#ifndef DLGBIESTATICOENTRADA_H
#define DLGBIESTATICOENTRADA_H

#include <QDialog>
#include <QDebug>
#include <QSettings>

namespace Ui {
  class DlgBiestaticoEntrada;
}

class DlgBiestaticoEntrada : public QDialog
{
  Q_OBJECT

public:
  explicit DlgBiestaticoEntrada(QWidget *parent = 0);
  ~DlgBiestaticoEntrada();

  QString fileTx;
  QString fileRx;
  QString ext;
  int medio{};
  int alvue;
  int GlatRX{},MlatRX{},SlatRX{},GlonRX{},MlonRX{},SlonRX{};
  int GlatTX{},MlatTX{},SlatTX{},GlonTX{},MlonTX{},SlonTX{};
  int AntenaTX{},AntenaRX{};
  void guardaConfiguracion();
  void leeConfiguracion();
  void FicherosNombres(bool cambio);
public slots:
  void editTX(const QString& a);

private slots:
  void on_pB_Aceptar_clicked();

  void on_pB_Cancelar_clicked();

  void on_cB_AltVuelo_currentIndexChanged(int index);

private:
  Ui::DlgBiestaticoEntrada *ui;
};

#endif // DLGBIESTATICOENTRADA_H
