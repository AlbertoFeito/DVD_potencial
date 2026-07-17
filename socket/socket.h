#ifndef SOCKET_H
#define SOCKET_H

#include<QIODevice>
#include <QAbstractSocket>
#include <QTcpSocket>
#include<QTcpServer>
#include<QUdpSocket>
#include<QMessageBox>
#include <QHostAddress>
#include <QObject>

class HostAddres;

#define DEFAULT_SOCKBUF_SIZE	8192

struct PEER_INFO //estructura de datos del servidor
{
    int nPeerPort;
    QString strPeerAddr;
    QString strPeerName;

    PEER_INFO()
    {
        nPeerPort=0;
        strPeerAddr="";
        strPeerName="";
    }
};

struct INFO //estructura de datos del mismo cliente
{
    int nPort;
    QString strAddr;

    INFO()
    {
        nPort=0;
        strAddr="";
    }
};



//---------------------Class TcpClient--------------------------//

#ifndef QSOCKET_TCPCLIENTE_H
#define QSOCKET_TCPCLIENTE_H

class QSocket_TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit QSocket_TcpClient(QObject *parent = nullptr,QTcpSocket* newClient=nullptr);

    bool Create(QString strBindAddr,int nRecvBufsize,int nsendBufSize);
    void connectedToServer(QString address,int port);  //conectar al servidor
    void disconnectToServer();                         //desconectar del servidor

    PEER_INFO getPeer_Info(){return Peer_Info;}        //devuelve la estructura
    INFO getInfo(){return Info;}                       //devuelve la estructura
    QTcpSocket* getClient(){return client;}            //devuelve el socket cliente
    int getType(){return nType;}                       // devuelve el tipo de cliente
    quint8 getnRed(){return nRed;}
    int getError(){return iError;}
    int readInfo(void* data,int size);                 //lee la informacion del buffer
    QByteArray readAllInfo();                              //lee toda la información del buffer del socket
    int sendInfo(void* data,int size);                 //Envia la informacion de data al server
    void close();

public:
    quint8 nRed;                                        //número de orden del cliente que conecta
    int iError;
private:
    PEER_INFO Peer_Info;
    INFO Info;
    QTcpSocket* client;
    int nType;                                          //1:Client,2:Server

signals:
    void accepted(QSocket_TcpClient*);                     //signal que se emite cuando el server acepto la conexion
    void errorConection(QSocket_TcpClient*);               //signal que se emite cuando ocurre un error en la conexion al server
    void disconnected(QSocket_TcpClient*);                 //signal que se emite cuando pierde la conexion del server
    void readyRead(QSocket_TcpClient*);                    //signal que se emite cuando le llega datos al socket
    void destroyedClient(QSocket_TcpClient*, bool);        //signal que se emite cuando se elimina el socket

public slots:
    void copyInfoServer();                              //se llena los datos del server (Peer_info)
    void error(QAbstractSocket::SocketError);           //se emite la signal de errorConection con el error
    void infoNotice();                                  //se emite la signal de readReady con el socket
    void destroyed();                                   //se emite la signal de destroyedClient con el socket eliminado

};

#endif // QSOCKET_TCPCLIENT_H

//---------------------Class TcpServer--------------------------//

#ifndef QSOCKET_TCPSERVER_H
#define QSOCKET_TCPSERVER_H

class QSocket_TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit QSocket_TcpServer(QObject *parent = nullptr);

    bool create(int port);                                          //pone en escucha al servidor por el puerto pasado por parametro
    void close();                                                   //desconecta todos los socket y cierra el server
    void disconnectAllSocket();                                     // desconecta todos los socket
    int clientCount();                                              //devuelve la cantidad de socket conectados
    int port();                                                     //devuelve el puerto de escucha del server

    int sendToRequest(QObject data,int size,QSocket_TcpClient client); //hacer
    int sendInfo(char* data,qint64 maxSize,QSocket_TcpClient* client); //manda datos a un cliente especificado
    bool Listen(){return server.isListening();}                     //devuelve el estado de escucha del server

    QList<QSocket_TcpClient*>* getListClient(){return listClient;}     //devuelve la lista de socket asociados al server

private:
    QTcpServer server;
    QList<QSocket_TcpClient*>* listClient;

signals:
    void newConection(QSocket_TcpClient*);                             //signal que se emite cuando hay una nueva conexion
    void closeConection(QSocket_TcpClient*);                           //signal que se emite cuando se cierra una conexion
    void readyRead(QSocket_TcpClient*);                                //signal que se emite cuando le llega datos

public slots:
    void newClient();                                               //se emite la signal de newConection
    void deleteClient(QSocket_TcpClient*, bool);                             //se emite la signal de closeConection
    void infoNotice(QSocket_TcpClient*);                               //se emite la signal de readyRead


};
#endif //QSOCKET_TCPSERVER_H

//---------------------Class UdpServer--------------------------//
#ifndef QSOCKET_UDPSERVER_H
#define QSOCKET_UDPSERVER_H

class QSocket_UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit QSocket_UdpServer(QObject *parent = nullptr);
//    bool Escucha_A(const QHostAddress &address, quint16 port = 0,BingMode mode = DefaultForPlatform);

private:
    QUdpSocket *socketUdp;

signals:

public slots:

};

#endif // QSOCKET_UDPSERVER_H


#endif // SOCKET_H


//ConnectionRefusedError,              /*0 la conexion fue rechazada o time out*/
//RemoteHostClosedError,               /*1 el servidor remoto cerro la conexion*/
//HostNotFoundError,                   /*2 la direccion del servidor no fue encontrada*/
//SocketAccessError,                   /*3 la operacion del socket fallo porque la aplicacion no tiene los privilegios requeridos*/
//SocketResourceError,                 /*4  el sistema local supero los recursos,(ej. muchos sockets)*/
//SocketTimeoutError,                  /*5 la operacion del socket esta fuera de tiempo*/
//DatagramTooLargeError,               /*6 el datagrama es mayor que el limite del sistema opreativo (8192 bytes)*/
//NetworkError,                        /*7 a ocurrido un error en la Red (ej,se desconecto el cable de Red)*/
//AddressInUseError,                   /*8 la direccion especificada siempre esta en uso y esta declarada como exclusiva*/
//SocketAddressNotAvailableError,      /*9 la direcion especificada no pertenece al servidor */
//UnsupportedSocketOperationError,     /*10 la operacion demandada por el socket no es soportada por el sistema operativo */
//UnfinishedSocketOperationError,      /*11 la ultima operacion no ha terminado*/
//OperationError,                      /*19  la operacion se frusto mientras el socket estaba en un estado que no le permitia atender a esta*/
//TemporaryError,                      /*22 Ocurrio un error temporal  (ej.,la operacion puede ser bloqueada y el socket es non-blocking)*/

//UnknownSocketError = -1,             /*-1 ocurrio un error indefinido*/
//NoError = -2                         /*-2 no ha ocurrido ningun error*/
