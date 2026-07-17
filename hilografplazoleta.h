#ifndef HILOGRAFPLAZOLETA_H
#define HILOGRAFPLAZOLETA_H

#include <QObject>
#include <QThread>
#include "ccalculos.h"
#include "defcomunes.h"
class HiloGrafPlazoleta : public QThread
{
	Q_OBJECT
public:
	explicit HiloGrafPlazoleta(QObject *parent = 0);
	~HiloGrafPlazoleta();

	CCalculos *calculos{};
	QString fichero;

	int banda{};//metrica = 0, decimetrica = 1
	double AltPos;
	double posicion;

	QList<double> listdist;//lista de interceptos para cada perfil los puntos de la horizontal

	QList<double> listdistplazoleta[360];//lista de los interceptos entre el perfil y los puntos de la horizontal en los 360 grados
	QList<double> InterceptoDisCono();//intercepto entre el perfil y los puntos de la horizontal
	QList<double> distanciasPlazoleta;//lista de distancia segun tabla
	QList<double> distanciasPZ(double antena);//distancia segun tabla
	QStringList  extensionPlazoleta[360];
    QList<double> extension;


    QList<double> InterceptoAnguloyPerfil();//intercepto entre el perfil y los angulos de cierre
	QList<double> disLimites[360];
	QList<double> angulosincidencia[360];
	QList<double> angulosincidencias;

    QStringList deformaciones[360];
    QStringList DeformacionTerreno;
    double DHword{};

	int azimut{};
	bool boton{};
	double Disvuelo{};
	double anguloincidencia(double mcu, double mre);
	double anguloINCidencia;
signals:
	void progreso(int c,QString mensaje);
	void findehilo(bool fin);
	void findehiloPerfil(bool fin);

protected:
	void run();
};

#endif // HILOGRAFPLAZOLETA_H
