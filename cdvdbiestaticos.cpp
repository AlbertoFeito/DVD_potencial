#include "cdvdbiestaticos.h"
#include "ui_cdvdbiestaticos.h"

const int length1p = 360;//distancia 320km para el calculo del perfil del terreno

CDVDBiestaticos::CDVDBiestaticos(int m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDVDBiestaticos)
{
    ui->setupUi(this);
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables|
                                QCP::iSelectItems | QCP::iSelectLegend);
    ui->widget->plotLayout()->insertRow(0);
    plottitle = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(0, 0,plottitle );

    //  leerZonas();
    ui->pB_Centro->setEnabled (false);
    ui->pB_Salvar->setEnabled (false);
    ui->pB_Sector->setEnabled (false);
    ui->pB_Zoommas->setEnabled (false);
    ui->pB_Zoommenos->setEnabled (false);
    ui->chB_VerSector->setEnabled (false);
    ui->chB_VerObjetivo->setEnabled (false);
    medio = m;

    initGraph();
    ui->widget->addLayer("Sector",nullptr,QCustomPlot::limAbove);
    ui->widget->addLayer("SectorRx",nullptr,QCustomPlot::limAbove);
    ui->widget->addLayer("SectorObj",nullptr,QCustomPlot::limAbove);
    ui->widget->addLayer("AzimutDist",nullptr,QCustomPlot::limAbove);

    ui->widget->addLayer("Zonas",nullptr,QCustomPlot::limAbove);

    puntos = new PUNTO*[360];
    for (int a = 0; a < 360; a++)
    {
        puntos[a] = new PUNTO[length1p];
        for (int d = 0; d < length1p; d++)
        {
            QUtiles::PolarToReal(a, (d + 1), PM_0_90, &puntos[a][d].X, &puntos[a][d].Y);
        }
    }
}

CDVDBiestaticos::~CDVDBiestaticos()
{
    delete ui;
}
void CDVDBiestaticos::initGraph()
{
    ui->widget->yAxis->setRange(15,30);//se estabelece rango de y
    ui->widget->xAxis->setRange(-70,-90);
    ui->widget->xAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->yAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->xAxis->setVisible(false);
    ui->widget->yAxis->setVisible(false);

    ui->widget->replot();
}

void CDVDBiestaticos::pintar(QString file)
{
    if(!file.at (5).isNumber ())
        file.insert (5,"0");

    latPos.G = file.midRef (7,2).toInt ();
    latPos.M = file.midRef (9,2).toInt ();
    latPos.S = file.midRef (11,2).toInt ();

    lonPos.G = file.midRef (14,2).toInt ();
    lonPos.M = file.midRef (16,2).toInt ();
    lonPos.S = file.midRef (18,2).toInt ();


    lat = QUtiles::GmsToGrados(latPos);
    lon = QUtiles::GmsToGrados(lonPos)*-1;
    Centros.Lat = lat;
    Centros.Lon = lon;
    Centros.name = "Centro_" +file;

    ListCentros.append (Centros);

    if(file.contains ("Tx"))
        pen1 = QPen(Qt::green,2);
    else
        pen1 = QPen(Qt::red,2);

    CustomPlotF customplotf;
    customplotf.pintaCurva (ui->widget,"Zonas","Centro_" + file,pen1,lon,lat,QCPScatterStyle::ssCross);
    customplotf.pintaCurva (ui->widget,"Zonas",file,pen1,QColor(150,0,150,100),xV,yV);

    ui->widget->layer ("Zonas")->replot ();

    QString fileAux1 = fileTX;
    QString fileAux2 = fileRX;
    fileAux1.replace (4,2,aux);
    fileAux2.replace (4,2,aux);
    plottitle->setText ("D.V.D.P a:"+QString::number (alvuelo (fileTX))+"m de Altura de Vuelo.\n Posición TX: "+ fileAux1 +". Posición RX:  "+fileAux2);

}

void CDVDBiestaticos::borrar()
{
    ListCentros.clear ();
    cont = 0;
    ui->widget->clearPlottables ();
    for (auto i : listaElipse)
    {
        ui->widget->removeItem(i);
    }
    listaElipse.clear ();
    for (auto i : listaZM)
    {
        ui->widget->removeItem(i);
    }
    listaZM.clear ();
    for (auto i : listaText)
    {
        ui->widget->removeItem(i);
    }
    listaText.clear ();
    for (auto i : listaLine)
    {
        ui->widget->removeItem(i);
    }
    listaLine.clear ();
    ui->widget->replot ();
}

void CDVDBiestaticos::leerZonas()
{
    todoZonas.clear ();
    todoZonas = qfindfiles.getFileList (QDir::currentPath() + "/Zonas/");

    for (int i = 0; i < todoZonas.length (); ++i)
    {
        QString zona = todoZonas.at (i);
        int m = QString("%1%2").arg(zona.at (4)).arg(zona.at (5)).toInt ();
        if(m == medio)
        {
            if(zona.contains ("Tx"))
                ui->cB_Transmisor->addItem (zona);
            if(zona.contains ("Rx"))
                ui->cB_Receptores->addItem (zona);
        }
    }
    fileTX = ui->cB_Transmisor->currentText ();
    fileRX = ui->cB_Receptores->currentText ();
}

bool CDVDBiestaticos::leer(const QString& fichero)
{
    yV.clear ();
    xV.clear ();

    bool entra=false;
    bool listo = false;
    QDir temp=QDir(QDir::currentPath() + "/Zonas");//direccion de los ficheros de coordenadas geograficas
    QFileInfoList Fich= temp.entryInfoList();

    foreach (const QFileInfo info, Fich)
    {
        QString nombfich= info.fileName();
        if (fichero==nombfich)
        {
            entra=true;
            break;
        }
    }
    if(entra)
    {
        QStringList todo;
        QFile file(QDir::currentPath() + "/Zonas/"+fichero);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            todo.append(line);
        }
        if( todo.length() == 360)
        {
            listo = true;
            for (int i = 0;i <= 360; ++i)
            {
                if(i<360)
                {
                    QStringList grande=todo.at(i).split(",");

                    xV.append (grande.at(0).toDouble());
                    yV.append (grande.at(1).toDouble());
                }
                else
                {
                    QStringList grande=todo.at(0).split(",");

                    xV.append (grande.at(0).toDouble());
                    yV.append (grande.at(1).toDouble());
                }
            }
        }
        else
        {
            listo = false;
            file.remove();
        }
    }
    return listo;
}

bool CDVDBiestaticos::existe(const QString& zona)
{
    bool existe = true;
    for (const auto & ListCentro : ListCentros)
    {
        if(ListCentro.name.contains ("Centro_" + zona))
        {
            existe = false;
            break;
        }
    }
    return existe;
}

double CDVDBiestaticos::DeterDis(double LGO, double LGD, double LTD, double LTO)
{
    double Lx;
    double Ly;
    double D;

    double dLatMedia=(LTO+LTD)/2.0;
    double dKfc=111.42325*cos(dLatMedia*RADIAN);
    if(LGO>LGD)
        Lx=(double)((LGO-LGD)*dKfc);
    else
        Lx=(double)((LGD-LGO)*dKfc);

    if(LTO>LTD)
        Ly=(double)((LTO-LTD)*(110.26477+(dLatMedia*0.0186644)));
    else
        Ly=(double)((LTD-LTO)*(110.26477+(dLatMedia*0.0186644)));
    D = dancal.Distancia(Lx,Ly);//km
    return D;
}

double CDVDBiestaticos::AZimut(double LGO, double LGD, double LTD, double LTO)
{
    double AZ = 0;
    double Lx = 0.0;
    double Ly = 0.0;

    Lx = (LGO-LGD)*111.42325*cos((LTO+LTD)/2*RADIAN);
    Ly = (LTD-LTO)*(110.26477+((LTO+LTD)/2)*0.0186644);

    double cita = 0.0;
    cita = (180.0/M_PI)*(atan2(Ly,Lx));
    if((Ly>0.0F)&(Lx>0.0F))   //primer cuadrante
        AZ=90-cita;
    else if((Ly>0.0F)&(Lx<0.0F))   //segungo cuadrante
        AZ=450-cita;//270
    else if((Ly<0.0F)&(Lx<0.0F))   //tercer cuadrante
        AZ=90-cita;//180
    else    //cuarto cuadrante
    {
        AZ=90-cita;  //90
    }
    if(AZ == -90)
    {
        AZ = 270;
    }
    return AZ;
}

int CDVDBiestaticos::alvuelo(const QString& strFile)
{
    int alvue = 0;

    if(strFile.endsWith (".m00"))    alvue = 0;
    if(strFile.endsWith (".m25"))    alvue = 25;
    if(strFile.endsWith (".m05"))    alvue = 50;
    if(strFile.endsWith (".001"))    alvue = 100;
    if(strFile.endsWith (".002"))    alvue = 200;
    if(strFile.endsWith (".003"))    alvue = 300;
    if(strFile.endsWith (".004"))    alvue = 400;
    if(strFile.endsWith (".005"))    alvue = 500;
    if(strFile.endsWith (".010"))    alvue = 1000;
    if(strFile.endsWith (".020"))    alvue = 2000;
    if(strFile.endsWith (".040"))    alvue = 4000;
    if(strFile.endsWith (".060"))    alvue = 6000;
    if(strFile.endsWith (".100"))    alvue = 10000;
    if(strFile.endsWith (".170"))    alvue = 17000;
    if(strFile.endsWith (".300"))    alvue = 30000;

    return  alvue;
}
/**********botones**********/
void CDVDBiestaticos::on_pB_Pintar_clicked()
{
    borrar ();
    xVRx.clear ();
    yVRx.clear ();
    yVTx.clear ();
    xVTx.clear ();

    if(existe (fileTX) || ListCentros.isEmpty ())
    {
        if(leer(fileTX))
        {
            pintar(fileTX);
            yVTx = yV;
            xVTx = xV;
            latTX = lat;
            lonTX = lon;
            ui->pB_Centro->setEnabled (true);
            ui->pB_Salvar->setEnabled (true);
            ui->pB_Sector->setEnabled (true);
            ui->pB_Zoommas->setEnabled (true);
            ui->pB_Zoommenos->setEnabled (true);
            //          ui->chB_VerSector->setEnabled (true);
        }
        else
        {
            QMessageBox::information (nullptr,"Información","Fichero corrupto, calcule nuevamente.");
            ui->pB_Centro->setEnabled (false);
            ui->pB_Salvar->setEnabled (false);
            ui->pB_Sector->setEnabled (false);
            ui->pB_Zoommas->setEnabled (false);
            ui->pB_Zoommenos->setEnabled (false);
            ui->chB_VerSector->setEnabled (false);
            ui->chB_VerObjetivo->setEnabled (false);
        }
    }
    if(existe (fileRX) || ListCentros.isEmpty ())
    {
        if(leer(fileRX))
        {
            pintar(fileRX);
            yVRx = yV;
            xVRx = xV;
            latRX = lat;
            lonRX = lon;
        }
        else
            QMessageBox::information (nullptr,"Información","Fichero corrupto, calcule nuevamente.");
    }
}

void CDVDBiestaticos::on_pB_Borrar_clicked()
{
    borrar ();
    ui->pB_Centro->setEnabled (false);
    ui->pB_Salvar->setEnabled (false);
    ui->pB_Sector->setEnabled (false);
    ui->pB_Zoommas->setEnabled (false);
    ui->pB_Zoommenos->setEnabled (false);
    ui->chB_VerSector->setEnabled (false);
    ui->chB_VerObjetivo->setEnabled (false);
    ui->chB_Leyenda->setChecked (false);
}

void CDVDBiestaticos::on_pB_Centro_clicked()
{
    if(!ListCentros.isEmpty ())
    {
        for (auto i : listaElipse)
        {
            ui->widget->removeItem(i);
        }
        listaElipse.clear ();
        for (auto i : listaText)
        {
            ui->widget->removeItem(i);
        }
        listaText.clear ();
        for (auto i : listaLine)
        {
            ui->widget->removeItem(i);
        }
        listaLine.clear ();

        if(cont < ListCentros.length ())
        {
            CustomPlotF customplotf;
            customplotf.pintaLineaAzimut (ui->widget,ListCentros.at (cont).Lat,ListCentros.at (cont).Lon,&listaLine,&listaText);
            customplotf.pintaElipses (ui->widget,ListCentros.at (cont).Lat,ListCentros.at (cont).Lon,&listaElipse,&listaText);
            cont++;
        }
        else
            cont=0;
    }
    ui->widget->layer ("AzimutDist")->replot ();
}

void CDVDBiestaticos::on_pB_Sector_clicked()
{
    for (auto i : sectorCurva)
    {
        ui->widget->removePlottable (i);
    }
    sectorCurva.clear ();
    XSector.clear ();
    YSector.clear ();
    XSector2.clear ();
    YSector2.clear ();
    XSectorObj.clear ();
    YSectorObj.clear ();
    DlgSectorBiestatico dlgsectorbiestatico;
    dlgsectorbiestatico.xVRx = xVRx;
    dlgsectorbiestatico.yVRx = yVRx;
    dlgsectorbiestatico.xVTx = xVTx;
    dlgsectorbiestatico.yVTx = yVTx;
    dlgsectorbiestatico.latRX = latRX;
    dlgsectorbiestatico.lonRX = lonRX;
    dlgsectorbiestatico.latTX = latTX;
    dlgsectorbiestatico.lonTX = lonTX;
    dlgsectorbiestatico.medio = medio;
    if(dlgsectorbiestatico.exec () == QDialog::Accepted)
    {
        ui->chB_VerSector->setEnabled (true);
        ui->chB_VerSector->setChecked (true);
        int angDer = dlgsectorbiestatico.angDer;
        int angIzq = dlgsectorbiestatico.angIzq;
        int angDer2 = dlgsectorbiestatico.angDer2;
        int angIzq2 = dlgsectorbiestatico.angIzq2;

        int angulomedio = dlgsectorbiestatico.angulo;
        if(medio == 47)
        {

            XSector.append (lonRX);//inicio
            YSector.append (latRX);//inicio

            int a = angIzq;
            int b = 0;

            for (int i= 0; i < 90; ++i)
            {
                if( a < 360 && a != angDer)
                {

                    XSector.append (xVRx.at (a));
                    YSector.append (yVRx.at (a));
                    a++;
                }
                else
                {

                    if( b <= angDer)
                    {
                        XSector.append (xVRx.at (b));
                        YSector.append (yVRx.at (b));
                        b++;
                    }
                }
            }
            XSector.append (xVRx.at (angDer));//fin
            YSector.append (yVRx.at (angDer));//inicio
            XSector.append (lonRX);//fin
            YSector.append (latRX);//inicio
        }
        else
        {
            if(medio == 45)
            {
                XSector.append (lonTX);//inicio
                YSector.append (latTX);//inicio
                int a = angIzq;
                int b = 0;

                for (int i= 0; i < 30; ++i)
                {
                    if( a < 360 && a != angDer)
                    {

                        XSector.append (xVTx.at (a));
                        YSector.append (yVTx.at (a));
                        a++;
                    }
                    else
                    {

                        if( b <= angDer)
                        {
                            XSector.append (xVTx.at (b));
                            YSector.append (yVTx.at (b));
                            b++;
                        }
                    }
                }
                XSector.append (xVTx.at (angDer));//fin
                YSector.append (yVTx.at (angDer));//inicio
                XSector.append (lonTX);//fin
                YSector.append (latTX);//inicio
            }
            else
            {
                /////////////////////////biestatico pechora//****************************************

                int angDerObj = dlgsectorbiestatico.angDerObj;
                int angIzqObj = dlgsectorbiestatico.angIzqObj;

                ui->chB_VerObjetivo->setEnabled(true);
                XSector.append (lonRX);//inicio
                YSector.append (latRX);//inicio
                int a = angIzq;
                int b = 0;

                XSector2.append (lonTX);//inicio
                YSector2.append (latTX);//inicio
                XSectorObj.append (lonTX);//inicio
                YSectorObj.append (latTX);//inicio
                int a2 = angIzq2;
                int b2 = 0;
                int aObj = angIzqObj;
                int bObj = 0;

                for (int i= 0; i < 4; ++i)
                {
                    if( a < 360 && a != angDer)
                    {

                        XSector.append (xVRx.at (a));
                        YSector.append (yVRx.at (a));
                        a++;
                    }
                    else
                    {

                        if( b <= angDer)
                        {
                            XSector.append (xVRx.at (b));
                            YSector.append (yVRx.at (b));
                            b++;
                        }
                    }

                    if( a2 < 360 && a2 != angDer2)
                    {

                        XSector2.append (xVTx.at (a2));
                        YSector2.append (yVTx.at (a2));
                        a2++;
                    }
                    else
                    {
                        if( b2 <= angDer)
                        {
                            XSector2.append (xVTx.at (b2));
                            YSector2.append (yVTx.at (b2));
                            b2++;
                        }
                    }
                    if( aObj < 360 && aObj != angDerObj)
                    {

                        XSectorObj.append (xVTx.at (aObj));
                        YSectorObj.append (yVTx.at (aObj));
                        aObj++;
                    }
                    else
                    {
                        if( bObj <= angDer)
                        {
                            XSectorObj.append (xVTx.at (bObj));
                            YSectorObj.append (yVTx.at (bObj));
                            bObj++;
                        }
                    }
                }
                XSector.append (xVRx.at (angDer));//fin
                YSector.append (yVRx.at (angDer));//inicio
                XSector.append (lonRX);//fin
                YSector.append (latRX);//inicio

                XSector2.append (xVTx.at (angDer2));//fin
                YSector2.append (yVTx.at (angDer2));//inicio
                XSector2.append (lonTX);//fin
                YSector2.append (latTX);//inicio

                XSectorObj.append (xVTx.at (angDerObj));//fin
                YSectorObj.append (yVTx.at (angDerObj));//inicio
                XSectorObj.append (lonTX);//fin
                YSectorObj.append (latTX);//inicio

                double distancia = DeterDis(xVRx.at (angulomedio)*-1,lonRX*-1,yVRx.at (angulomedio),latRX);

                ui->label->setText ("Distancia maxima: "+QString::number (distancia,'f',2)+"Km");
                ui->label->setVisible (true);
            }
        }

        CustomPlotF customplotf;

        if(medio == 46)
        {
            customplotf.pintaCurva (ui->widget,"SectorRx","Sector Rx",QPen(Qt::blue,2),QBrush(QColor(150,0,150,100)),XSector,YSector,&sectorCurva);

            customplotf.pintaCurva (ui->widget,"SectorObj","Sector TxObj",QPen(Qt::blue,2),QBrush(QColor(150,0,150,100)),XSectorObj,YSectorObj,&sectorCurva);

            customplotf.pintaCurva (ui->widget,"Sector","Sector Tx",QPen(Qt::blue,2),QBrush(QColor(150,0,150,100)),XSector2,YSector2,&sectorCurva);
            //pintar objetivo
            customplotf.pintaCurva(ui->widget,"SectorObj","objetivo",QPen(Qt::black,2),dlgsectorbiestatico.geocoordinateObj.longitude(),dlgsectorbiestatico.geocoordinateObj.latitude(),QCPScatterStyle::ssCrossCircle,&sectorCurva);
        }
        else
            customplotf.pintaCurva (ui->widget,"Sector","Sector Tx",QPen(Qt::blue,2),QBrush(QColor(150,0,150,100)),XSector,YSector,&sectorCurva);

        if(ui->chB_VerSector->isChecked ())
        {
            ui->widget->layer ("Sector")->setVisible (true);
            ui->widget->layer ("Sector")->replot ();
            ui->widget->layer ("SectorRx")->setVisible (true);
            ui->widget->layer ("SectorRx")->replot ();
        }
        else
        {
            ui->widget->layer("SectorObj")->setVisible(false);
            ui->widget->layer ("SectorObj")->replot ();
        }
    }
    else
    {
        ui->chB_VerSector->setEnabled (false);
        ui->chB_VerObjetivo->setEnabled (false);
        ui->chB_VerSector->setChecked (false);
        ui->chB_VerObjetivo->setChecked (false);
        ui->widget->layer ("Sector")->replot ();
    }
}

void CDVDBiestaticos::on_pB_Zoommas_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommas (ui->widget);
}

void CDVDBiestaticos::on_pB_Zoommenos_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommenos (ui->widget);
}

void CDVDBiestaticos::on_pB_Salvar_clicked()
{
    QString nom;

    QString auxtx = fileTX.replace (".","_G");
    QString auxrx = fileRX.replace (".","_G");

    nom = (QDir::currentPath()+"/GraficasPDF/"+auxtx+"_"+auxrx);

    CustomPlotF customplotf;
    customplotf.filePrintPdf (ui->widget,nom);
}

void CDVDBiestaticos::on_cB_Transmisor_currentIndexChanged(const QString &arg1)
{
    fileTX = arg1;
}

void CDVDBiestaticos::on_cB_Receptores_currentIndexChanged(const QString &arg1)
{
    fileRX = arg1;
}
/////////***********************///////////////////
void CDVDBiestaticos::on_chB_Leyenda_toggled(bool checked)
{
    ui->widget->legend->setVisible (checked);
    ui->widget->replot ();
}

void CDVDBiestaticos::on_chB_VerSector_toggled(bool checked)
{
    if(checked)
    {
        ui->chB_VerObjetivo->setChecked(false);
        ui->widget->layer("SectorObj")->setVisible(false);
        ui->widget->layer ("SectorObj")->replot ();

        ui->widget->layer ("Sector")->setVisible (true);
        ui->widget->layer ("Sector")->replot ();
        ui->widget->layer("SectorRx")->setVisible(true);
        ui->widget->layer ("SectorRx")->replot ();
    }
    else
    {
        ui->widget->layer("SectorRx")->setVisible(false);
        ui->widget->layer ("SectorRx")->replot ();
        ui->widget->layer ("Sector")->setVisible (false);
        ui->widget->layer ("Sector")->replot ();
    }
}


void CDVDBiestaticos::on_chB_VerObjetivo_toggled(bool checked)
{
    if(checked)
    {
        ui->chB_VerSector->setChecked(false);
        ui->widget->layer("SectorObj")->setVisible(true);
        ui->widget->layer ("SectorObj")->replot ();
        ui->widget->layer("SectorRx")->setVisible(true);
        ui->widget->layer ("SectorRx")->replot ();
        ui->widget->layer ("Sector")->setVisible (false);
        ui->widget->layer ("Sector")->replot ();
    }
    else
    {
        ui->widget->layer("SectorRx")->setVisible(false);
        ui->widget->layer ("SectorRx")->replot ();
        ui->widget->layer("SectorObj")->setVisible(false);
        ui->widget->layer ("SectorObj")->replot ();
    }
}
