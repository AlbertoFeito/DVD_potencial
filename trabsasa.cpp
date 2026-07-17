#include "trabsasa.h"
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
TrabSasa::TrabSasa(QObject *parent) : QObject(parent)
{
    NoObjEnc = 0;
    serverConect=false;
    ClientePE = nullptr;

//    InicioProceso();
//    HabilitaConectserver();
}

void TrabSasa::InicioProceso()
{
    QDir dir;
#ifdef Q_OS_WIN
    m_strDriveTrabajo=QDir:: rootPath();
    m_strPathApp = dir.absolutePath();
#else
    m_strDriveTrabajo=QDir:: homePath();
    m_strDriveTrabajo+="/SISTEMA/";
#endif
    QString sTemp = m_strPathApp;
    QStringList sLtemp = sTemp.split("/");//0=root, 1=Dir Sasa
//    m_strPathApp = sLtemp.at(0)+"/"+sLtemp.at(1)+"/";

    QCoreApplication::setOrganizationName("CID 3 - QtSASA");
    QCoreApplication::setApplicationName("ZVisibilidad");
    QSettings CalcIni(m_strPathApp+"/trabaltura.ini",QSettings::IniFormat);

    LocalHostPE = CalcIni.value("Calculador/IP","").toString();
    Puerto_PE = static_cast<quint16>(CalcIni.value("Calculador/Puerto","").toInt());
    PassW_PE = CalcIni.value("Calculador/Password","").toString();

    quint8 TamaPaq = 50;
    spack = new quint8[TamaPaq];
    rpack = new quint8[TamaPaq];
}

void TrabSasa::HabilitaConectserver()
{
    serverConect = server.create(Puerto_PE);

    connect(&server,SIGNAL(newConection(QSocket_TcpClient*)),
                     this,SLOT(onNuevoCliente(QSocket_TcpClient*)));
    connect(&server,SIGNAL(closeConection(QSocket_TcpClient*)),
                     this,SLOT(PierdeConnection(QSocket_TcpClient*)));
    connect(&server,SIGNAL(readyRead(QSocket_TcpClient*)),
                     this,SLOT(ReadDatosClientePE()));
}

void TrabSasa::SendDatoClientePE(quint8 Paquete)
{
    switch (Paquete) {
    case PACKET_AUTENTICACION:
    {
        spack[0] = COD_INIC;
        spack[1] = PACKET_AUTENTICACION;

        QByteArray paq = PassW_PE.toLocal8Bit();
        spack[3] = static_cast<quint8>(paq.size());
        quint8 cont;
        for( cont=4 ; cont< paq.size()+4 ; cont++)
            spack[cont] = static_cast<quint8>(paq[cont-4]);
        spack[2] = cont;
        ClientePE->sendInfo(spack,cont);

    }
        break;
    case PACKET_FIN_CALCV:
    {
        spack[0] = COD_INIC;
        quint8 CantByte = 4;
        spack[1] = PACKET_FIN_CALCV;
        spack[2] = CantByte;
        spack[3] = NoObjEnc;
        ClientePE->sendInfo(spack,CantByte);
        //este paquete es la respuesta de que ya calculo
    }
        break;
    default:
        break;
    }
}

void TrabSasa::PierdeConnection(QSocket_TcpClient *clientDesc)
{
    if(clientDesc->getType()==2)
    {
        delete clientDesc;
        clientDesc=nullptr;
    }
}

void TrabSasa::onNuevoCliente(QSocket_TcpClient * Cliente)
{
    QString strIP=Cliente->getPeer_Info().strPeerAddr;
    QStringList Ltemp = strIP.split(":");
    if(Ltemp.length()>1)
        strIP = Ltemp.last();

    if(strIP.compare(LocalHostPE)!=0)
        Cliente->close();
    else
        ClientePE = Cliente;
}

void TrabSasa::ReadDatosClientePE()
{
    ArrDatosClientePE = ClientePE->readAllInfo();
    if(ArrDatosClientePE.size()>0)
    {
        QVector<quint8>DatoObj;
        for (int i=0 ;i<ArrDatosClientePE.size() ;i++)
            DatoObj.append(static_cast<quint8>(ArrDatosClientePE.at(i)));

        quint8 Dato0 = static_cast<quint8>(DatoObj.at(0));
        quint8 CantByte = DatoObj.at(2);

        if((Dato0 == COD_INIC) && (CantByte == DatoObj.size()))
        {
            quint8 valor = DatoObj.at(1);
            switch (valor) {
            case (PACKET_AUTENTICACION)://185  B9
            {
                int cont = ArrDatosClientePE.at(3);
                QString StrAut="";
                for(int j=0;j<cont;j++)
                {
                    StrAut.append(DatoObj.at(j+4));
                }
                if(StrAut.compare(PassW_PE)!=0)
                    PierdeConnection(ClientePE);
                else
                {
                    SendDatoClientePE(PACKET_AUTENTICACION);
                    //aqui es el indicador de que estas trab por proceso
                    emit si_trabIniciado();
                }
            }
                break;
            case(PACKET_DATO_VISIB)://213 0xD5
            {
                NoObjEnc = DatoObj.at(3);
                LatFte.G =  static_cast<short>(DatoObj.at(4));
                LatFte.M =  static_cast<uchar>(DatoObj.at(5));
                LatFte.S =  static_cast<uchar>(DatoObj.at(6));
                LongFte.G =  static_cast<short>(DatoObj.at(7));
                LongFte.M =  static_cast<uchar>(DatoObj.at(8));
                LongFte.S =  static_cast<uchar>(DatoObj.at(9));
                TipoMedio = DatoObj.at(10);
                //aqui comienza el calculo
                emit si_comienzaCalculo(true);
            }
                break;
            case(PACKET_FIN_WORK):
            {
                int fin = DatoObj.at(3);
                if(fin == PACKET_FIN_WORK)
//                    destroy(true);
                    emit si_comienzaCalculo(false);
//                    exit(0);
                //aqui es el indicador de que termina el proceso


            }
                break;
            default:
                break;
            }
        }
    }
}
