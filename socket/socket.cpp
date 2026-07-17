
#include "socket.h"
//#include "../General/listas.h"
//#include "clientesconectados.h"



QSocket_TcpClient::QSocket_TcpClient(QObject *parent, QTcpSocket *newClient) :
    QObject(parent)
{

    if(newClient!=nullptr)
    {
        client=newClient;
        nType=2;//cuando lo crea el server

        connect(client,SIGNAL(disconnected()),
                client,SLOT(deleteLater()));

        connect(client,SIGNAL(destroyed()),
                this,SLOT(destroyed()));


        Peer_Info.nPeerPort=client->peerPort();
        Peer_Info.strPeerAddr=client->peerAddress().toString();
        Peer_Info.strPeerName=client->peerName();

    }
    else
    {
        client=new QTcpSocket();
        nType=1;//cuando es un cliente creado por el sistema

        connect(client,SIGNAL(connected()),
                this,SLOT(copyInfoServer()));

        connect(client,SIGNAL(error(QAbstractSocket::SocketError)),
                this,SLOT(error(QAbstractSocket::SocketError)));

//        connect(client,SIGNAL(disconnected()),
//                this,SIGNAL(disconnected()));
    }

    connect(client,SIGNAL(readyRead()),
            this,SLOT(infoNotice()));

    Info.nPort=client->localPort();
    Info.strAddr=client->localAddress().toString();
}

void QSocket_TcpClient::copyInfoServer()//slot
{
     Peer_Info.nPeerPort=client->peerPort();
     Peer_Info.strPeerAddr=client->peerAddress().toString();
     Peer_Info.strPeerName=client->peerName();

     emit accepted(this);
}

void QSocket_TcpClient::error(QAbstractSocket::SocketError err)//slot
{
    iError=err;
    if(err==QAbstractSocket::RemoteHostClosedError)
        emit disconnected(this);
    else
        emit errorConection(this);
}

void QSocket_TcpClient::infoNotice()//slot
{
    emit readyRead(this);
}

int QSocket_TcpClient::readInfo(void* data,int size)
{
    if(data!=nullptr)
    {
        int a=client->read((char*)data,size);
        return a;
    }

    return 0;
}

QByteArray QSocket_TcpClient::readAllInfo()
{
    QByteArray DatoArray = client->readAll();
    return DatoArray;
}

int QSocket_TcpClient::sendInfo(void* data,int size)
{
    if(data!=nullptr)
        return client->write((char*)data,size);

    return 0;
}

void QSocket_TcpClient::close()
{
    client->close();
    emit destroyedClient(this, false);
}

void QSocket_TcpClient::destroyed()//slot
{
    emit destroyedClient(this, true);
}

void QSocket_TcpClient::connectedToServer(QString address,int port)
{
    client->connectToHost(address,port,QIODevice::ReadWrite);
}

void QSocket_TcpClient::disconnectToServer()
{
    client->disconnectFromHost();
}

//---------------------Class Server--------------------------//

QSocket_TcpServer::QSocket_TcpServer(QObject *parent) :
    QObject(parent)
{
    listClient=new QList<QSocket_TcpClient*>();

    connect(&server,SIGNAL(newConnection()),
            this,SLOT(newClient()));
}

 bool QSocket_TcpServer::create(int port)
 {
     return server.listen(QHostAddress::Any,port);
 }

  void QSocket_TcpServer::close()
  {
      disconnectAllSocket();

      server.close();
  }

  void  QSocket_TcpServer::newClient()//slot
  {
      QSocket_TcpClient* client_new=new QSocket_TcpClient(0,server.nextPendingConnection());

      connect(client_new,SIGNAL(destroyedClient(QSocket_TcpClient*, bool)),
              this,SLOT(deleteClient(QSocket_TcpClient*, bool)));

      connect(client_new,SIGNAL(readyRead(QSocket_TcpClient*)),
              this,SLOT(infoNotice(QSocket_TcpClient*)));

      listClient->append(client_new);

      emit newConection(client_new);
  }

  void QSocket_TcpServer::deleteClient(QSocket_TcpClient* client, bool emitevent)//slot
  {
      for(int i=0;i<listClient->count();i++)
      {
          if(listClient->at(i)==client)
          {
            listClient->removeAt(i);
            if(emitevent)
                emit closeConection(client);
            break;
          }
      }
  }

  void QSocket_TcpServer::disconnectAllSocket()
  {
      QList<QObject*> lista = server.children();

      while(lista.count()>1)
          lista.takeLast()->deleteLater();
  }

  int QSocket_TcpServer::clientCount()
  {
      return listClient->count();
  }

  int QSocket_TcpServer::port()
  {
      return server.serverPort();
  }

  int QSocket_TcpServer::sendToRequest(QObject data,int size,QSocket_TcpClient client)
  {
      return 0;
  }

  int QSocket_TcpServer::sendInfo(char* data,qint64 maxSize,QSocket_TcpClient* client)
  {
     if(data!=nullptr)
     {
         return client->sendInfo(data,maxSize);
     }

     return 0;
  }

  void QSocket_TcpServer::infoNotice(QSocket_TcpClient* client)//slot
  {
      emit readyRead(client);
  }


  //---------------------Class UDP--------------------------//

//  bool QSocket_UdpServer::Escucha_A(const QHostAddress &address, quint16 port)
//  {
//      return(socketUdp->bind(&address,port,mode));
//  }


