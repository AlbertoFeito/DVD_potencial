#ifndef TRABSASA_H
#define TRABSASA_H

#include <QObject>
#include "socket/socket.h"
#include "socket/gmsgeocoord.h"


#define ENCABEZADO              3;   //Bytes de comienzo = COD_INIC+Cant de Bytes en el paquete+Tipo de paquete
#define COD_INIC                170  //0xAA    //Inicio de Paquete
#define PACKET_AUTENTICACION    185  //0xB9    //Paquete de autenticacion
#define PACKET_DATO_VISIB       199  //0xC7    //Paquete de datos para el calculo de la visibilidad
#define PACKET_FIN_CALCV        211  //0xD3    //Paquete de datos de fin del calculo de la visibilidad Directa
#define CRITICAL_ERROR          221  //0xDD    //Paquete de Error crítico en la aplicación de Visibilidad
#define PACKET_FIN_WORK         231  //0xE7    //Fin del trabajo

class TrabSasa : public QObject
{
    Q_OBJECT
public:
    explicit TrabSasa(QObject *parent = nullptr);

    QSocket_TcpServer server;
    QSocket_TcpClient *ClientePE;
    QByteArray ArrDatosClientePE;

    QString m_strDriveTrabajo,m_strPathApp;
    // Buffer para la transmisión de paquetes de información por Socket
    quint8* spack;
    // Buffer para la recepción de paquetes de información por Socket
    quint8* rpack;

    QString LocalHostPE;
    quint16 Puerto_PE;
    QString PassW_PE;

    bool bFinWork;

    //variables para el calculo del punto de encuentro
    //del Calulador viene
    quint8 NoObjEnc;
    GMSG LatFte;           //Coord Geo del a posicion de la fuente en G,M,S
    GMSG LongFte;           //Coord Geo del a posicion de la fuente en G,M,S
    QString AlturaAntena;   //Altura de la antena del medio
    quint8 TipoMedio;       //tipo de medio segun datos de la Visibilida Directa
    bool serverConect;


    static QString CodificaString(QString s);
    static QString DecodificaString(QString s);
    void InicioProceso();
    void HabilitaConectserver();
    void SendDatoClientePE(quint8);

private slots:
    //slots comunes (cliente)
    //    void ErrorConnection(QSocket_TcpClient*);
    void PierdeConnection(QSocket_TcpClient*);
    //slots del server
    void onNuevoCliente(QSocket_TcpClient*);
    void ReadDatosClientePE();

signals:
    void si_comienzaCalculo(bool);
    void si_trabIniciado();
};

#endif // TRABSASA_H
