#ifndef DISTANTHREAD_H
#define DISTANTHREAD_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QGeoCoordinate>

#include "ccalculos.h"
#include "dancalc.h"
//#include "basedatos/cbasedatos.h"

const QStringList listaCamposPOV  = {"azimut","lat","long","DVD05","SNV"} ;

const QStringList listaCamposERL = {"azimut","lat","long","DVD05","SNV"
                                    ,"DVD06","DVD07","DVD08","DVD09"} ;

const QStringList listaCampos  = { "azimut","lat","long","DVD05","SNV","ZM1pos"
                                   ,"SNV_ZM1pos","ZM1neg","SNV_ZM1neg"
                                   ,"DVD06","DVD07","DVD08","DVD09"
                                   ,"ZDL05","ZDL06","ZDL07","ZDL08","ZDL09"
                                   ,"ZDR05","ZDR06","ZDR07","ZDR08","ZDR09"
                                   ,"LatZDR","LonZDR" } ;

// Estructura para pasar datos Sigma sin modificar variables globales
struct DatosSigma {
    QList<double> disVDirecta[360];
    QList<double> disInternas[360];
    QList<double> ZonaMuertaMuis[360];
    QList<double> ZonaMuertaMuis2[360];
};

class distanThread : public QThread
{
    Q_OBJECT
public:
    explicit distanThread(QObject *parent = 0);
    ~distanThread();
    bool alcance;
    QGeoCoordinate geoPos;
    double LatOR{} ;
    double LonOR{};
    CCalculos *calculos{};
    QList<double> disVDirecta[360];
    QList<double> disInternas[360];
    QList<double> ZonaMuertaMuis[360];
    QList<double> ZonaMuertaMuis2[360];
    QList<double> ZonaMuertaCCAA[360];
    int tablaVelocidad[75][15]{};
    QString fichero;
    QString label;
    bool parar{};
    bool detener;
    bool muis;
    int Vuelo;
    bool ObjLoc;
    double Disvuelo{};
    QString nombreAux;
    int probabilidad;
    int distancia;
    double distanciamax;
    double distanciamaxOficial;
    QList<double> disMax;
    double zonamuerta;
    int angulositio{};
    double tang1{},tang2{};
    QList<QPointF> LPinterceptosRecta;
    QList<QPointF> LPinterceptosAS1;
    QList<QPointF> LPinterceptosAS2;
    QList<QPointF> listaASTodos;
    bool guarda{};
    int velocidad{};
    int velocidadCohete{};
    int tiempo{};
    int parametro{};

    double DLCmin = 0;
    double DLCmax = 0;

    double DRCmin = 0;
    double DRCmax = 0;

    double DECCM = 0;//

    QString strSigma;

    void guardar();
    void guardarDB();

    bool leer();
    bool leerDB();

    void stop();

    void calcDistancia();

    void determDistanCiclo();

    void tangente(int ang);

    QList<QPointF> Pointinterceptos(bool recttierra, int a, QList<double> datosCurva, QList<double> datosRecta);

    QList<QPointF> listaPuntos( QList<QPointF> ListaPuntos);//ordena lista de puntos
    QList<double> listaNumeros( QList<double> ListaNumeros);//ordena lista de numeros
    bool pintarDistancias();

    bool comprobarECC(int t, int alturaVuelo, double dvdecc);
    bool comprobarDlanza(double *dl, double dmaxAzzi);
    bool comprobarDZDR(double *dzdr);
    bool comprobarDlanzaSegmentosNV(double *dl, int a);
    bool comprobarDlanzaSegmentosNVzm(double *dl, int a);
    bool comprobarDlanzaDzr(double *dl, double dr, int a);
    void calcularDlDzr(double *dl, double *dr, int a);
    int velocidadCohetes(int hb, double dlanz);
    void leerVelocidad();

    bool creaTablaBaseDatos(const QString &tablaName);

    void corrige(int a);

    // Setters para configuración desde la UI
    void setSigmaSeleccionado(double sigma);
    void setTipoOnda(const QString& tipo);

    // Getters
    double getSigmaSeleccionado() const;
    QString getTipoOnda() const;

    QString limpiarSufijosNombre(const QString &nombre);
signals:
    void progreso(int c,QString mensaje);
    void findehilo(bool fin);

public slots:

protected:
    void run();
private:
    DanCalc danca;
    //    CBaseDatos *m_basedatos;
    //    QStringList listaCampos;


    int DlanzMin;
    int DlanzMax;
    int DdestMin;
    int DdestMax;
    void estDistanMinMax();


    double sigmaSeleccionado;  // 1.0 = normal, 5.0 = _S5, 0.5 = _S05, 0.1 = _S01
    QString tipoOnda;           // ""=ninguno, "M", "DM", "CM"
    // Hash de coeficientes Sigma - AHORA CON QString como clave
    // Claves: "36", "38", "39", "40_M", "40_DM", "40_CM", "42", "43", "44"
    QHash<QString, QHash<double, double>> coeficientesSigma;
    void inicializarCoeficientesSigma();
    void calcularYGuardarSigma(int medio, int grupo, int vuelo, const QList<double> &disMax, const QString &nombreBase);
    QString obtenerExtension(int vuelo);


    void guardarFicheroSigma(const QString &nombreFichero, const QList<double> &disMaxSigma, double sigma, int indiceSigma, const DatosSigma &datos);
    void guardarFicheroSigma(const QString &nombreFichero, const QList<double> &disMaxSigma, double sigma, int indiceSigma, QList<double> disVDirectaMod[], QList<double> disInternasMod[], QList<double> ZonaMuertaMuisMod[], QList<double> ZonaMuertaMuis2Mod[]);
    double obtenerDistanciaMaxOficial(int medio, const QString &tipoOnda);
};

#endif // DISTANTHREAD_H
