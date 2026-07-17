#include "qinicial.h"
#include "ui_qinicial.h"
#include "dlgselecpo.h"
#include <QDir>
#include <QSettings>
#include <QMessageBox>


QInicial::QInicial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QInicial)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
    f.close();

    QFicheroINI fileIni;
    fileIni.readInicio();

    QDir Dir;//para crear las carpetas donde se almacenan los ficheros.
//    Dir.mkdir(QDir::currentPath() + "/BD");//si no existe la carpeta la creo
    Dir.mkdir(QDir::currentPath() + "/Temporal");//si no existe la carpeta la creo
    Dir.mkdir(QDir::currentPath() + "/Zonas");//si no existe la carpeta la creo
    Dir.mkdir(QDir::currentPath() + "/GraficasPDF");//si no existe la carpeta la creo
    Dir.mkdir(QDir::currentPath() + "/GraficasPDF/Campos");//si no existe la carpeta la creo
    Dir.mkdir(QDir::currentPath() + "/TablasPDF");//si no existe la carpeta la creo
    Dir.mkdir(QDir::currentPath() + "/AngulosCierre");//si no existe la carpeta la creo
    Dir.mkdir(QDir::currentPath() + "/CalificaciónERL");//si no existe la carpeta la creo

    SASA("ZVD");
    //    SASA("ZPD");
    //    SASA("ZPF");

    /**********sasa***********/

//    trabsasa.InicioProceso();
//    trabsasa.HabilitaConectserver();

    connect(&trabsasa,&TrabSasa::si_trabIniciado,this,&QInicial::trabIniciado);
    connect(&trabsasa,&TrabSasa::si_comienzaCalculo,this,&QInicial::estadoSasa);

    estadoCalculo = true;
//    connect(&timerSASA,SIGNAL(timeout()),this,SLOT(slTimerSASA()));
//    timerSASA.singleShot(60000,this,SLOT(slTimerSASA()));
    //    while (estadoCalculo) {
    //        qDebug()<<"SASA";
            qDebug()<<"SASA"<< timerSASA.isActive();
    //        slTimerSASA();

    //    }

    /****************************/
}

QInicial::~QInicial()
{

    delete ui;
}
void QInicial::estadoSasa(bool estado)
{
    CCalculos calculos;
    QFicheroINI fileIni;
    calculos.grupo = 1;
    if(estado)
    {
        calculos.latPos.G = trabsasa.LatFte.G;
        calculos.latPos.M = trabsasa.LatFte.M;
        calculos.latPos.S = trabsasa.LatFte.S;
        calculos.lonPos.G = trabsasa.LongFte.G;
        calculos.lonPos.M = trabsasa.LongFte.M;
        calculos.lonPos.S = trabsasa.LongFte.S;
        calculos.medio = trabsasa.TipoMedio;

        if(calculos.medio != 49)
        {
            calculos.AlAnt = fileIni.DatDelMedio(calculos.medio);
            calculos.arma = fileIni.arma;
        }
        else
        {
            calculos.AlAnt = fileIni.AlAntena;
            calculos.arma = 4;
        }

        calculos.camara = 20/*fileIni.camara*/;
        calculos.calcularDatos();

        QThread thread;
        calculos.moveToThread(&thread);
        thread.start();

        AnguCierre2 angCierre;
        angCierre.calculos = &calculos;
        angCierre.determDistanCiclo(calculos.grupo,calculos.arma,calculos.medio);
        angCierre.determAC();
        //        int velocidad[] ={150,300,420,640,950};
        QList<int> v = velocidad(calculos.medio);

        for (int i = 0; i < 12; ++i)
        {
            distanThread disthread;
            disthread.Vuelo = alturaVuelo (i,calculos.grupo,calculos.medio);
            disthread.probabilidad = 0;
            if(v.size()>0)
            {
                for (int j : v)
                {

                    disthread.velocidad = j;

                    if(calculos.medio == 35  || calculos.medio == 34  || calculos.medio == 33  || calculos.medio == 32  || calculos.medio == 31|| calculos.medio == 27 || calculos.medio == 26 || calculos.medio == 25  || calculos.medio == 24  || calculos.medio == 23  || calculos.medio == 22  || calculos.medio == 21)
                        disthread.tiempo = 5;
                    else if(calculos.medio == 36)
                        disthread.tiempo = 16;
                    else if(calculos.medio == 37)
                        disthread.tiempo = 24;
                    else if(calculos.medio == 38)
                        disthread.tiempo = 15;
                    else if(calculos.medio == 39)
                        disthread.tiempo = 25;
                    else if(calculos.medio == 42 ||  calculos.medio == 43 || calculos.medio == 44)
                        disthread.tiempo = 10;
                    else
                        disthread.tiempo = 25;

                    if(disthread.Vuelo != 1)
                    {
                        disthread.calculos = &calculos;
                        disthread.fichero = calculos.fichero;
                        disthread.determDistanCiclo();
                        disthread.start();

                        if(!disthread.isFinished())
                        {
                            disthread.wait();
                            //                  if(disthread.pintarDistancias ())
                            //                    disthread.guardar ();
                        }
                    }
                }/**************/
            }
            else
            {
                if(disthread.Vuelo != 1)
                {
                    disthread.calculos = &calculos;
                    disthread.fichero = calculos.fichero;
                    disthread.determDistanCiclo();
                    disthread.start();

                    if(!disthread.isFinished())
                    {
                        disthread.wait();
                        //                  if(disthread.pintarDistancias ())
                        //                    disthread.guardar ();
                    }
                }
            }
        }
        if(trabsasa.ClientePE != NULL)
            trabsasa.SendDatoClientePE(PACKET_FIN_CALCV);
        else
        {
            exit(0);
        }
    }
}

void QInicial::trabIniciado()
{
    estadoCalculo = false;
    if(trabsasa.ClientePE == NULL)
    {
        show();
    }


}

void QInicial::slTimerSASA()
{    
    qDebug()<<"timer";
    if(trabsasa.ClientePE)
    {
        trabsasa.ClientePE->close();
        delete trabsasa.ClientePE;
        trabsasa.ClientePE = NULL;

    }
    if(estadoCalculo)
        show();
}
/****************************************/
void QInicial::SASA(const QString& group)
{
    CCalculos calculos;
    if(group == "ZVD")
        calculos.grupo = 1;

    if(group == "ZPF")
        calculos.grupo = 2;

    if(group == "ZPD")
        calculos.grupo = 3;

    QFicheroINI fileIni;
    fileIni.readIni(group);

    if(fileIni.cooperacion == 1)
    {
        calculos.latPos.G = (short)fileIni.gradlat;
        calculos.latPos.M = fileIni.minlat;
        calculos.latPos.S = fileIni.seglat;
        calculos.lonPos.G = fileIni.gradlong;
        calculos.lonPos.M = fileIni.minlong;
        calculos.lonPos.S = fileIni.seglong;
        calculos.medio = fileIni.medio;

        if(calculos.medio != 49)
        {
            calculos.AlAnt = fileIni.DatDelMedio(fileIni.medio);
            calculos.arma = fileIni.arma;
        }
        else
        {
            calculos.AlAnt = fileIni.AlAntena;
            calculos.arma = 4;
        }

        calculos.camara = fileIni.camara;
        calculos.calcularDatos();

        QThread thread;
        calculos.moveToThread(&thread);
        thread.start();

        AnguCierre2 angCierre;
        angCierre.calculos = &calculos;
        angCierre.determDistanCiclo(calculos.grupo,calculos.arma,calculos.medio);
        angCierre.determAC();
        //        int velocidad[] ={150,300,420,640,950};
        QList<int> v = velocidad(calculos.medio);

        for (int i = 0; i < 12; ++i)
        {
            distanThread disthread;
            disthread.Vuelo = alturaVuelo (i,calculos.grupo,calculos.medio);
            disthread.probabilidad = 0;
            if(v.size()>0)
            {
                for (int j : v)
                {

                    disthread.velocidad = j;

                    if(calculos.medio == 35  || calculos.medio == 34  || calculos.medio == 33  || calculos.medio == 32  || calculos.medio == 31|| calculos.medio == 27 || calculos.medio == 26 || calculos.medio == 25  || calculos.medio == 24  || calculos.medio == 23  || calculos.medio == 22  || calculos.medio == 21)
                        disthread.tiempo = 5;
                    else if(calculos.medio == 36)
                        disthread.tiempo = 16;
                    else if(calculos.medio == 37)
                        disthread.tiempo = 24;
                    else if(calculos.medio == 38)
                        disthread.tiempo = 15;
                    else if(calculos.medio == 39)
                        disthread.tiempo = 25;
                    else if(calculos.medio == 42 ||  calculos.medio == 43 || calculos.medio == 44)
                        disthread.tiempo = 10;
                    else
                        disthread.tiempo = 25;

                    if(disthread.Vuelo != 1)
                    {
                        disthread.calculos = &calculos;
                        disthread.fichero = calculos.fichero;
                        disthread.determDistanCiclo();
                        disthread.start();

                        if(!disthread.isFinished())
                        {
                            disthread.wait();
                            //                  if(disthread.pintarDistancias ())
                            //                    disthread.guardar ();
                        }
                    }
                }/**************/
            }
            else
            {
                if(disthread.Vuelo != 1)
                {
                    disthread.calculos = &calculos;
                    disthread.fichero = calculos.fichero;
                    disthread.determDistanCiclo();
                    disthread.start();

                    if(!disthread.isFinished())
                    {
                        disthread.wait();
                        //                  if(disthread.pintarDistancias ())
                        //                    disthread.guardar ();
                    }
                }
            }
        }

        fileIni.Salir(group);
        exit(0);
    }
}

int QInicial::alturaVuelo(int i, int g, int m)
{
    int alvuelo = 1;
    switch (g) {//zvd
    case 1:
        switch (i) {
        case 0://30000
            if( m == 39)//solo volga
                alvuelo = 30000;
            break;
        case 1://17000
            if( m == 39 || m == 38)//volga y pechora
                alvuelo = 17000;
            break;
        case 2://10000
            if( m != 41  && m !=49 && m !=36 && m !=35 && m !=34 && m !=33 && m !=32 && m !=31  &&  m !=21 && m !=22 && m !=23 && m !=24 && m !=25 && m !=26   && m !=43 && m !=44)
                alvuelo = 10000;
            break;

        case 3://6000
            if( m !=36 && m !=35 && m !=34 && m !=33 && m !=32 && m !=31  &&  m !=21 && m !=22 && m !=23 && m !=24 && m !=25  && m !=44)
                alvuelo = 6000;
            break;

        case 4://4000
            if(m !=31 && m !=32 && m !=33 && m !=34 && m !=35 && m !=21 && m !=22 && m !=23 && m !=24 && m !=44)
                alvuelo = 4000;
            break;
        case 5://2000
            if(m !=21)
                alvuelo = 2000;
            break;
        case 6://1000
            alvuelo = 1000;
            break;
        case 7://500
            alvuelo = 500;
            break;
        case 8://100
            alvuelo = 100;
            break;
        case 9://50
            //            if(m !=39 )
            alvuelo = 50;
            break;
        case 10://0
            //            if(m !=39 )
            alvuelo = 25;
            break;
        case 11://0
            // if(m !=39 && m !=38 && m !=37 && m !=36 && m !=35 && m !=34 && m !=33 && m !=32 && m !=31  &&  m !=21 && m !=22 && m !=23 && m !=24 && m !=25 && m !=26  && m !=27 && m !=42 && m !=43 && m !=44)
            alvuelo = 0;
            break;
        default:
            break;
        }
        break;
    case 2://ZPF
        switch (i) {
        case 0://10000
            if(m !=21 && m !=22 && m !=23 && m !=24 && m !=25 && m !=26 )
                alvuelo = 10000;
            break;
        case 1://6000
            if(m !=21 && m !=22 && m !=23 && m !=24 && m !=25)
                alvuelo = 6000;
            break;
        case 3://2000
            if(m !=21)
                alvuelo = 2000;
            break;
        case 4://1000
            alvuelo = 1000;
            break;
        case 5://500
            alvuelo = 500;
            break;
        case 6://100
            alvuelo = 100;
            break;
        default:
            break;
        }
        break;
    case 3://zpd
        switch (i) {
        case 0://10000
            if(m !=31 && m !=32 && m !=33 && m !=34 && m !=35 && m !=36)
                alvuelo = 10000;
            break;
        case 2://4000
            if(m !=31 && m !=32 && m !=33 && m !=34 && m !=35 )
                alvuelo = 4000;
            break;
        case 3://2000
            alvuelo = 2000;
            break;
        case 4://1000
            alvuelo = 1000;
            break;
        case 5://500
            alvuelo = 500;
            break;
        case 6://100
            if(m !=39)
                alvuelo = 100;
            break;
        case 7://100
            if(m !=39)
                alvuelo = 50;

            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return alvuelo;
}

QList<int> QInicial::velocidad(int m)
{
    QList<int> v;
    switch (m) {
    case 39:
        v ={150,300,420,640,950};
        break;
    case 38:
    case 37:
        v ={150,300,420,640};
        break;
    case 36:
    case 35:
    case 34:
    case 33:
    case 32:
    case 31:
    case 27:
    case 26:
    case 25:
    case 24:
    case 23:
    case 22:
    case 21:
    case 42:
        v ={150,300,420};
        break;
    default:
        break;
    }
    return v;

}


//******   POV o POP   ***********//
void QInicial::on_pB_PO_clicked()
{
    dlgSelecPO dlgselPO;

    if(dlgselPO.exec() == QDialog::Accepted)//si acepto
    {
        VentanaInicialERLyPOV = new QVentanaInicialERLyPOV();
        VentanaInicialERLyPOV->grupo = 1;
        if(dlgselPO.seleccion)
        {
            VentanaInicialERLyPOV->arma = 0;
            VentanaInicialERLyPOV->medio = 0;
            VentanaInicialERLyPOV->setWindowTitle("Ventana inicial de trabajo P.O.V.");
        }
        else
        {
            VentanaInicialERLyPOV->arma = 1;

            VentanaInicialERLyPOV->setWindowTitle("Ventana inicial de trabajo P.O.P.");
        }
        VentanaInicialERLyPOV->seleccionArma(false);
        VentanaInicialERLyPOV->show();
    }
}
//***********   AAA   ***********//
void QInicial::on_pB_AAA_clicked()
{
    dlgSelecZPFoZVD dlgZPFoZVD;

    if(dlgZPFoZVD.exec() == QDialog::Accepted)//si acepto
    {
        if(dlgZPFoZVD.seleccion)
        {
            zpfaaa = new ZPFAAA();
            zpfaaa->grupo = 2;
            zpfaaa->setWindowTitle("Ventana inicial de trabajo Z.P.F.A.A.A.");
            zpfaaa->seleccion(true);
            zpfaaa->show();
        }
        else
        {
            zpfaaa = new ZPFAAA();
            zpfaaa->grupo = 1;
            zpfaaa->setWindowTitle("Ventana inicial de trabajo Z.V.D.A.A.A.");
            zpfaaa->seleccion(false);
            zpfaaa->show();
        }

    }
}
//************   CCAA   ***********//
void QInicial::on_pB_CCAA_clicked()
{
    dlgSelectZPDoZVD dlgZPDoZVD;

    if(dlgZPDoZVD.exec() == QDialog::Accepted)//si acepto
    {
        if(dlgZPDoZVD.seleccion)
        {
            zpdcaaa = new ZPDCAAA();
            zpdcaaa->grupo = 3;
            zpdcaaa->arma = 3;
            zpdcaaa->setWindowTitle("Ventana inicial de trabajo Z.P.D.C.C.A.A.");
            zpdcaaa->seleccion(true);
            zpdcaaa->show();
        }
        else
        {
            zpdcaaa = new ZPDCAAA();
            zpdcaaa->grupo = 1;
            zpdcaaa->arma = 3;
            zpdcaaa->setWindowTitle("Ventana inicial de trabajo Z.V.D.C.C.A.A.");
            zpdcaaa->seleccion(false);
            zpdcaaa->show();
        }

    }
}
//***********   ERL   ***********//
void QInicial::on_pB_ERL_clicked()
{
    DlgSelctERL dlgselerl;

    if(dlgselerl.exec() == QDialog::Accepted)//si acepto
    {
        medio = dlgselerl.medio;
        if(medio == 45 || medio == 46 || medio == 47)
        {
            auto *dlgbiestatico = new DlgBiestatico();
            dlgbiestatico->medio = medio;
            dlgbiestatico->numVentana = 0;
            dlgbiestatico->show ();
        }
        else
        {
            VentanaInicialERLyPOV = new QVentanaInicialERLyPOV();
            VentanaInicialERLyPOV->grupo = 1;
            VentanaInicialERLyPOV->medio = dlgselerl.medio;

            VentanaInicialERLyPOV->arma = 4;

            VentanaInicialERLyPOV->setWindowTitle("Ventana inicial de trabajo E.R.L");
            if(dlgselerl.medio == 40 || dlgselerl.medio == 50)
            {
                VentanaInicialERLyPOV->seleccionArma(true);
                VentanaInicialERLyPOV->show();
            }
            else
            {
                VentanaInicialERLyPOV->seleccionArma(false);
                VentanaInicialERLyPOV->show();
            }
        }
    }
}
//***********   PuntoAPunto   ***********//
void QInicial::on_pB_PuntoAPunto_clicked()
{
    comunicacion.show();
}
//***********   Ayuda   ***********//
void QInicial::on_pB_Ayuda_clicked()
{
    auto * ayuda = new QProcess;
    QString ayudaDir="C:/WINDOWS/hh.exe "+QDir::currentPath()+"/AyudaVisibDirecta.chm";

    if(ayuda->isOpen())
        ayuda->close();
    ayuda->start(ayudaDir);

}

void QInicial::on_pB_Cerrar_clicked()
{
    this->close();
}

void QInicial::closeEvent(QCloseEvent *event)
{
    Q_UNUSED (event);
    QApplication::closeAllWindows();
}

void QInicial::on_pB_Campo_clicked()
{
    campo = new Campos();
    campo->leerZonas();
    //  campo->configInterface ();
    campo->showMaximized ();
}
