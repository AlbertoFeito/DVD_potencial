#include "campos.h"
#include "ui_campos.h"

Campos::Campos(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Campos)
{
    ui->setupUi(this);
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables|
                                QCP::iSelectItems | QCP::iSelectLegend);

    ui->widget->plotLayout()->insertRow(0);
    plottitle = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(0,0,plottitle );
    plottitle->setText ("Campo de zonas");
    plottitle2 = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(2,0,plottitle2 );

    ui->widget->yAxis->setRange(15,30);//se estabelece rango de y
    ui->widget->xAxis->setRange(-70,-90);
    ui->widget->xAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->yAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->xAxis->setVisible(false);
    ui->widget->yAxis->setVisible(false);

    ui->widget->addLayer ("AzimutDist",nullptr,QCustomPlot::limAbove);
    ui->widget->addLayer ("ZM",nullptr,QCustomPlot::limAbove);
    ui->widget->addLayer ("Zonas",nullptr,QCustomPlot::limAbove);
    ui->widget->addLayer ("Sector",nullptr,QCustomPlot::limAbove);
    ui->widget->addLayer("SectorPrincipal",nullptr,QCustomPlot::limBelow);
    ui->widget->layer("SectorPrincipal")->setVisible(false);
    ui->widget->replot();

    cont = 0;
    puntos = new PUNTO*[360];
    for (int a = 0; a < 360; a++)
    {
        puntos[a] = new PUNTO[360];
        for (int d = 0; d < 360; d++)
        {
            QUtiles::PolarToReal(a, (d + 1), PM_0_90, &puntos[a][d].X, &puntos[a][d].Y);
        }
    }

    ui->pB_Sector->setDisabled (true);
    ui->pB_SectorPrincipal->setDisabled (true);

    connect(ui->widget, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int,QMouseEvent*)));
    connect(ui->widget, SIGNAL(itemClick(QCPAbstractItem*,QMouseEvent*)), this, SLOT(itemClick(QCPAbstractItem*,QMouseEvent*)));
    connect(ui->widget, SIGNAL(selectionChangedByUser()), this, SLOT(widgetClik()));

    connect(ui->cB_ListaZonas,SIGNAL(activated(QString)),this,SLOT(deterProbabilidad(QString)));

    probabilidad = 0;
    probabilidadAux = false;
}

Campos::~Campos()
{
    for (int i = 0; i < 360; ++i)
    {
        ZonaMuertaMuis[i].clear();
        ZonaMuertaMuis2[i].clear();
    }
    delete ui;
}

void Campos::pintar()
{
    Centros.Lat = lat;
    Centros.Lon = lon;
    QString nomCurva;
    prob = "_P:"+ui->cB_Probabilidad->currentText ();
    if(file.contains ("ZVD"))
    {
        pen1 = QPen(Qt::green,2);
        if(file.contains ("Rx"))
            pen1 = QPen(Qt::red,2);

        if(medio >= 21 )
        {
            nomCurva = file + prob;
            Centros.name = "Centro_" + file + prob;
        }
        else
        {
            nomCurva = file;
            Centros.name = "Centro_" + file;
        }
    }
    else
    {
        pen1 = QPen(Qt::red,2);
        nomCurva = file;
        Centros.name = "Centro_" + file;
    }


    ListCentros.append (Centros);

    Av = alvuelo(file);
    vel = velocidad (file);
    CustomPlotF customplotf;
    customplotf.pintaCurva (ui->widget,"Zonas", nomCurva, pen1,lon,lat,QCPScatterStyle::ssCross);
    customplotf.pintaCurva (ui->widget,"Zonas", nomCurva, pen1,QColor(150,0,150,100),xV,yV);
    if(!xV1.isEmpty ())
        customplotf.pintaCurva (ui->widget,"Zonas", nomCurva, QPen(QColor(10,100,120),2),QColor(150,0,150,100),xV1,yV1);
    if(!xV2.isEmpty ())
        customplotf.pintaCurva (ui->widget,"Zonas", nomCurva, QPen(Qt::red,2),QColor(150,0,150,100),xV2,yV2);

    if(!xZM1.isEmpty ())
        customplotf.pintaCurva (ui->widget,"Zonas", nomCurva, QPen(Qt::blue,2),QColor(150,0,150,100),xZM1,yZM1);
    if(!xZM2.isEmpty ())
        customplotf.pintaCurva (ui->widget,"Zonas", nomCurva, QPen(QColor(200,0,200),2),QColor(150,0,150,100),xZM2,yZM2);

    ui->widget->layer ("Zonas")->replot ();

    pintarZonaMuerta(zonaMuerta (file)/1000,lat,lon);
}

int Campos::alvuelo(const QString &strFile)
{
    int alvue = 0;

    if(strFile.endsWith (".m00"))    alvue = 0;
    if(strFile.endsWith (".m25"))    alvue = 25;
    if(strFile.endsWith (".m50"))    alvue = 50;
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

int Campos::velocidad(const QString &strFile)
{
    int v = 0;
    //    int m = QString("%1%2").arg(strFile.at (4)).arg(strFile.at (5)).toInt ();
    if(strFile.contains ("ZVD"))
    {
        //        if(m >= 36 && m<40)
        //            v = QString("%1%2%3").arg(strFile.at (24)).arg(strFile.at (25)).arg(strFile.at (26)).toInt ();

    }
    return  v;
}

double Campos::zonaMuerta(const QString &strFile)
{
    int m = QString("%1%2").arg(strFile.at (4)).arg(strFile.at (5)).toInt ();
    double zonamuerta = 0;
    if(strFile.contains ("ZVD"))
    {
        switch (m)
        {
        case 40:zonamuerta = 2.25 * Av; break;
        case 45:zonamuerta = 2.25 * Av; break;
        case 46:zonamuerta = 3500; break;
        case 50:zonamuerta = 5 * Av;    break;
        }
        if(m >= 36)
        {
            ui->cB_Probabilidad->setEnabled (true);
            if(m<40)
                ui->cb_Velocidad->setEnabled (true);
        }
        else
        {
            ui->cB_Probabilidad->setEnabled (false);
            ui->cb_Velocidad->setEnabled (true);
        }
    }
    if(strFile.contains ("ZPD"))
    {
        switch (m)
        {
        case 31:zonamuerta = 500;break;
        case 32:zonamuerta = 500;break;
        case 33:zonamuerta = 500;break;
        case 34:
            switch (Av) {
            case 500: zonamuerta = 10;break;
            case 1000: zonamuerta = 20;break;
            case 2000: zonamuerta = 40; break;
            default:zonamuerta = 10;break;
            }
            break;
        case 35:zonamuerta = 800; break;
        case 36:zonamuerta = 1500;break;
        case 37:zonamuerta = 3500;break;
        case 38:zonamuerta = 3500;break;
        case 39:zonamuerta = 7000;break;
        }
    }
    if(strFile.contains ("ZPF"))
    {
        switch (m)
        {
        case 21:
            switch (Av) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 22:
            switch (Av) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 23:
            switch (Av) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 24:
            switch (Av) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 25:
            switch (Av) {
            case 100: zonamuerta = 5; break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100;break;
            default: zonamuerta = 5;break;
            }
            break;
        case 26:
            switch (Av) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100;break;
            case 4000: zonamuerta = 340;break;
            default:zonamuerta = 5;break;
            }
            break;
        case 27:
            switch (Av) {
            case 100: zonamuerta = 10;break;
            case 200: zonamuerta = 20;break;
            case 300: zonamuerta = 30;break;
            case 400: zonamuerta = 40;break;
            case 500: zonamuerta = 50;break;
            case 1000: zonamuerta = 100;break;
            case 2000: zonamuerta = 200;break;
            case 6000: zonamuerta = 680;break;
            case 10000: zonamuerta = 1300;break;
            default:zonamuerta = 10;break;
            }
            break;
        case 42:
            switch (Av) {
            case 100: zonamuerta = 10;break;
            case 200: zonamuerta = 20;break;
            case 300: zonamuerta = 30;break;
            case 400: zonamuerta = 40;break;
            case 500: zonamuerta = 50;break;
            case 1000: zonamuerta = 100;break;
            case 2000: zonamuerta = 200;break;
            case 6000: zonamuerta = 680;break;
            case 10000:zonamuerta = 1300;break;
            default:zonamuerta = 10;break;
            }
            break;
        case 43:
            switch (Av) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100; break;
            case 6000:
                zonamuerta = 340;
                break;
            case 10000:
                zonamuerta = 1300;
                break;
            default:zonamuerta = 5;break;
            }
            break;
        case 44:
            switch (Av) {
            case 100: zonamuerta = 5;break;
            case 200: zonamuerta = 10;break;
            case 300: zonamuerta = 15;break;
            case 400: zonamuerta = 20;break;
            case 500: zonamuerta = 25;break;
            case 1000: zonamuerta = 50;break;
            case 2000: zonamuerta = 100; break;
            default:zonamuerta = 5;break;
            }
            break;
        }
    }

    return zonamuerta;
}

void Campos::pintarZonaMuerta(double dedZone, double latitud, double longitud)
{
    double toplefx; double toplefy; double botrighX; double botrighy;
    QString nomCurva;
    if(medio >= 36)
    {
        nomCurva = file + prob;
    }
    else
    {
        nomCurva = file;
    }
    auto *ZonaMuerta=new QCPItemEllipse(ui->widget);
    ZonaMuerta->setLayer ("Zonas");
    ZonaMuerta->setObjectName ("ZM_" + nomCurva);
    ZonaMuerta->setSelectable (true);

    listaZM.append (ZonaMuerta);
    toplefy = dancal.Latitud_2 (dedZone,latitud);
    toplefx = dancal.Longitud_2 (dedZone,longitud,latitud);

    botrighy = dancal.Latitud_2 (-dedZone,latitud);
    botrighX = dancal.Longitud_2 (-dedZone,longitud,latitud);

    ZonaMuerta->topLeft->setCoords(toplefx,toplefy);
    ZonaMuerta->bottomRight->setCoords(botrighX,botrighy);
    ZonaMuerta->setPen(QPen(Qt::blue,2));
    ZonaMuerta->setSelectedPen (QPen(Qt::yellow,2));

    ui->widget->replot();
}

void Campos::pintarZonaMuertaMuisHaz()
{

}

void Campos::on_pB_Pintar_clicked()
{ 
    Av = alvuelo(file);

    vel = velocidad(file);
    bool existe = true;
    for(const auto & ListCentro : ListCentros)/* (int i = 0; i < ListCentros.length (); ++i)*/
    {
        if(ListCentro.name.contains ("Centro_" + file))
        {
            existe = false;
            break;
        }
    }
    if(existe || ListCentros.isEmpty () || probabilidadAux)
    {
        for (int i = 0; i < 360; ++i)
        {
            ZonaMuertaMuis[i].clear();
            ZonaMuertaMuis2[i].clear();
        }
        if(leer(file))
            pintar();
        else
            QMessageBox::information (nullptr,"Información","Fichero corrupto, calcule nuevamente.");
    }
}

void Campos::leerZonas()
{
    ui->cb_Velocidad->setEnabled (true);
    todo.clear ();
    ListCentros.clear ();
    todo = qfindfiles.getFileList (QDir::currentPath() + "/Zonas/");

    Av = ui->cB_AlturaObjetivo->currentText ().toInt ();
    vel = ui->cb_Velocidad->currentText ().toInt ();

    for (int i = 0; i < todo.length (); ++i)
    {
        QString zona = todo.at (i);
        if (Av == alvuelo(zona))
        {
            ui->cB_ListaZonas->addItem (todo.at (i));
        }
    }
    if(ui->cB_ListaZonas->count () > 0)
    {
        file = ui->cB_ListaZonas->currentText ();
        deterProbabilidad(file);
    }
    else
    {
        ui->cB_Probabilidad->setEnabled (false);
        //        ui->cb_Velocidad->setEnabled (false);
    }
}

void Campos::on_rB_ZVD_toggled(bool checked)
{
    ui->cb_Velocidad->setEnabled (true);
    ui->cB_ListaZonas->clear ();
    if(checked)
    {
        for (int i = 0; i < todo.length (); ++i)
        {
            QString zona = todo.at (i);
            if (zona.contains ("ZVD") && Av == alvuelo(zona))
            {
                if(vel == velocidad (zona) || velocidad(zona) == 0)
                    ui->cB_ListaZonas->addItem (todo.at (i));
            }
        }
        if(ui->cB_ListaZonas->count () > 0)
        {
            file = ui->cB_ListaZonas->currentText ();
            deterProbabilidad(file);
        }
        else
        {
            ui->cB_Probabilidad->setEnabled (false);
            //            ui->cb_Velocidad->setEnabled (false);
        }
    }
}

void Campos::on_rB_ZPD_toggled(bool checked)
{
    ui->cb_Velocidad->setEnabled (true);
    ui->cB_ListaZonas->clear ();
    if(checked)
    {
        for (int i = 0; i < todo.length (); ++i)
        {
            QString zona = todo.at (i);
            if (zona.contains ("ZPD") && Av == alvuelo(zona))
            {
                if(vel == velocidad (zona) || velocidad(zona) == 0)
                    ui->cB_ListaZonas->addItem (todo.at (i));
            }
        }
        if(ui->cB_ListaZonas->count () > 0)
        {
            file = ui->cB_ListaZonas->currentText ();
            deterProbabilidad(file);
            //      Av = alvuelo(file);
        }
        else
        {
            ui->cB_Probabilidad->setEnabled (false);
            //            ui->cb_Velocidad->setEnabled (false);
        }
    }
}

void Campos::on_rB_ZPF_toggled(bool checked)
{
    ui->cb_Velocidad->setEnabled (true);
    ui->cB_ListaZonas->clear ();
    if(checked)
    {
        for (int i = 0; i < todo.length (); ++i)
        {
            QString zona = todo.at (i);
            if (zona.contains ("ZPF") && Av == alvuelo(zona))
            {
                if(vel == velocidad (zona) || velocidad(zona) == 0)
                    ui->cB_ListaZonas->addItem (todo.at (i));
            }
        }
        if(ui->cB_ListaZonas->count () > 0)
        {
            file = ui->cB_ListaZonas->currentText ();
            deterProbabilidad(file);
            //      Av = alvuelo(file);
        }
        else
        {
            ui->cB_Probabilidad->setEnabled (false);
            //            ui->cb_Velocidad->setEnabled (false);
        }
    }
}

void Campos::on_rB_Todos_toggled(bool checked)
{
    ui->cb_Velocidad->setEnabled (true);
    ui->cB_ListaZonas->clear ();
    if(checked)
    {
        for (int i = 0; i < todo.length (); ++i)
        {
            QString zona = todo.at (i);
            if (Av == alvuelo(zona))
            {
                if(vel == velocidad (zona) || velocidad(zona) == 0)
                    ui->cB_ListaZonas->addItem (todo.at (i));
            }
        }
        if(ui->cB_ListaZonas->count () > 0)
        {
            file = ui->cB_ListaZonas->currentText ();
            deterProbabilidad(file);
        }
        else
        {
            ui->cB_Probabilidad->setEnabled (false);
            //            ui->cb_Velocidad->setEnabled (false);
        }
    }
}

bool Campos::leer(const QString &fichero)
{
    Vuelo = ui->cB_AlturaObjetivo->currentText ().toInt ();
    yV.clear ();
    xV.clear ();
    yV1.clear ();
    xV1.clear ();
    yV2.clear ();
    xV2.clear ();

    yZM1.clear ();
    xZM1.clear ();

    yZM2.clear ();
    xZM2.clear ();
    for(auto & i : disVDirecta)/* (int i = 0; i < 360; ++i)*/
    {
        i.clear ();
    }
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
        QFile f(QDir::currentPath() + "/Zonas/"+fichero);

        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
        while (!f.atEnd())
        {
            QByteArray line = f.readLine();
            todo.append(line);
        }
        if( todo.length() == 360)
        {
            if(file.startsWith ("ZVD"))grupo = 1;
            if(file.startsWith ("ZPD"))grupo = 3;
            if(file.startsWith ("ZPF"))grupo = 2;

            if(!file.at (5).isNumber ())
            {
                medio = 0;
                file.insert (5,"0");
            }
            else
                medio = file.midRef (4,2).toInt ();
            if(medio != 48)
            {
                angulositio = 0;
                latPos.G = (short)file.midRef (7,2).toInt ();
                latPos.M = (short)file.midRef (9,2).toInt ();
                latPos.S = (short)file.midRef (11,2).toInt ();

                lonPos.G = (short)file.midRef (14,2).toInt ();
                lonPos.M = (short)file.midRef (16,2).toInt ();
                lonPos.S = (short)file.midRef (18,2).toInt ();
            }
            else
            {
                if(!file.at (7).isNumber () )
                {
                    angulositio = file.midRef (7,2).toInt ();
                    latPos.G = file.midRef (10,2).toInt ();
                    latPos.M = file.midRef (12,2).toInt ();
                    latPos.S = file.midRef (14,2).toInt ();

                    lonPos.G = file.midRef (17,2).toInt ();
                    lonPos.M = file.midRef (19,2).toInt ();
                    lonPos.S = file.midRef (21,2).toInt ();
                }
                else
                {
                    angulositio = file.midRef (7,1).toInt ();
                    latPos.G = file.midRef (9,2).toInt ();
                    latPos.M = file.midRef (11,2).toInt ();
                    latPos.S = file.midRef (13,2).toInt ();

                    lonPos.G = file.midRef (16,2).toInt ();
                    lonPos.M = file.midRef (18,2).toInt ();
                    lonPos.S = file.midRef (20,2).toInt ();
                }
            }

            lat = QUtiles::GmsToGrados(latPos);
            lon = QUtiles::GmsToGrados(lonPos)*-1;

            listo = true;
            QStringList grande;
            QStringList visibilidad;
            //          double visib = 0;
            double disLanzamiento = 0;
            double disZDR = 0;
            for (int i = 0;i <= 360; ++i)
            {
                if(i<360)
                {
                    grande=todo.at(i).split("*");

                    if(probabilidad == 0)
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            visibilidad = grande.at (8).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (13).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at (1).split(",");//0.5
                        disVDirecta[i].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[i].append(disLanzamiento);
                            disVDirecta[i].append(disZDR);
                            double Ykm = disVDirecta[i].at (1)/1000 * cos(i * RADIAN);
                            double Xkm = disVDirecta[i].at (1)/1000 * sin(i * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[i].at (2)/1000 * cos(i * RADIAN);
                            Xkm = disVDirecta[i].at (2)/1000 * sin(i * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[i].at (0)/1000 * cos(i * RADIAN);
                        double Xkm = disVDirecta[i].at (0)/1000 * sin(i * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }
                    else if(probabilidad == 1)
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            visibilidad = grande.at (9).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (14).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at(4).split(",");//0.6
                        disVDirecta[i].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[i].append(disLanzamiento);
                            disVDirecta[i].append(disZDR);
                            double Ykm = disVDirecta[i].at (1)/1000 * cos(i * RADIAN);
                            double Xkm = disVDirecta[i].at (1)/1000 * sin(i * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[i].at (2)/1000 * cos(i * RADIAN);
                            Xkm = disVDirecta[i].at (2)/1000 * sin(i * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[i].at (0)/1000 * cos(i * RADIAN);
                        double Xkm = disVDirecta[i].at (0)/1000 * sin(i * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }
                    else if(probabilidad == 2)
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            visibilidad = grande.at (10).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (15).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at(5).split(",");//0.7
                        disVDirecta[i].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[i].append(disLanzamiento);
                            disVDirecta[i].append(disZDR);
                            double Ykm = disVDirecta[i].at (1)/1000 * cos(i * RADIAN);
                            double Xkm = disVDirecta[i].at (1)/1000 * sin(i * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[i].at (2)/1000 * cos(i * RADIAN);
                            Xkm = disVDirecta[i].at (2)/1000 * sin(i * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[i].at (0)/1000 * cos(i * RADIAN);
                        double Xkm = disVDirecta[i].at (0)/1000 * sin(i * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }
                    else if(probabilidad == 3)
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        { visibilidad = grande.at (11).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (16).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at(6).split(",");//0.8
                        disVDirecta[i].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[i].append(disLanzamiento);
                            disVDirecta[i].append(disZDR);
                            double Ykm = disVDirecta[i].at (1)/1000 * cos(i * RADIAN);
                            double Xkm = disVDirecta[i].at (1)/1000 * sin(i * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[i].at (2)/1000 * cos(i * RADIAN);
                            Xkm = disVDirecta[i].at (2)/1000 * sin(i * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[i].at (0)/1000 * cos(i * RADIAN);
                        double Xkm = disVDirecta[i].at (0)/1000 * sin(i * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }
                    else
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            visibilidad = grande.at (12).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (17).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at(7).split(",");//0.9
                        disVDirecta[i].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[i].append(disLanzamiento);
                            disVDirecta[i].append(disZDR);
                            double Ykm = disVDirecta[i].at (1)/1000 * cos(i * RADIAN);
                            double Xkm = disVDirecta[i].at (1)/1000 * sin(i * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[i].at (2)/1000 * cos(i * RADIAN);
                            Xkm = disVDirecta[i].at (2)/1000 * sin(i * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[i].at (0)/1000 * cos(i * RADIAN);
                        double Xkm = disVDirecta[i].at (0)/1000 * sin(i * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }
                }
                else
                {
                    grande=todo.at(0).split("*");

                    if(probabilidad == 0)
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            visibilidad = grande.at (8).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (13).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at (1).split(",");//0.5
                        disVDirecta[0].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[0].append(disLanzamiento);
                            disVDirecta[0].append(disZDR);
                            double Ykm = disVDirecta[0].at (1)/1000 * cos(0 * RADIAN);
                            double Xkm = disVDirecta[0].at (1)/1000 * sin(0 * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[0].at (2)/1000 * cos(0 * RADIAN);
                            Xkm = disVDirecta[0].at (2)/1000 * sin(0 * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[0].at (0)/1000 * cos(i * RADIAN);
                        double Xkm = disVDirecta[0].at (0)/1000 * sin(i * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }
                    else if(probabilidad == 1)
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            visibilidad = grande.at (9).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (14).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at(4).split(",");//0.6
                        disVDirecta[0].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[0].append(disLanzamiento);
                            disVDirecta[0].append(disZDR);
                            double Ykm = disVDirecta[0].at (1)/1000 * cos(0 * RADIAN);
                            double Xkm = disVDirecta[0].at (1)/1000 * sin(0 * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[0].at (2)/1000 * cos(0 * RADIAN);
                            Xkm = disVDirecta[0].at (2)/1000 * sin(0 * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);
                        double Xkm = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }
                    else if(probabilidad == 2)
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            visibilidad = grande.at (10).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (15).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at(5).split(",");//0.7
                        disVDirecta[0].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[0].append(disLanzamiento);
                            disVDirecta[0].append(disZDR);
                            double Ykm = disVDirecta[0].at (1)/1000 * cos(0 * RADIAN);
                            double Xkm = disVDirecta[0].at (1)/1000 * sin(0 * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[0].at (2)/1000 * cos(0 * RADIAN);
                            Xkm = disVDirecta[0].at (2)/1000 * sin(0 * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);
                        double Xkm = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }
                    else if(probabilidad == 3)
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        { visibilidad = grande.at (11).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (16).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at(6).split(",");//0.8
                        disVDirecta[0].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[0].append(disLanzamiento);
                            disVDirecta[0].append(disZDR);
                            double Ykm = disVDirecta[0].at (1)/1000 * cos(0 * RADIAN);
                            double Xkm = disVDirecta[0].at (1)/1000 * sin(0 * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[0].at (2)/1000 * cos(0 * RADIAN);
                            Xkm = disVDirecta[0].at (2)/1000 * sin(0 * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);
                        double Xkm = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }
                    else
                    {
                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            visibilidad = grande.at (12).split(",");
                            disLanzamiento = visibilidad.at(0).toDouble();

                            visibilidad = grande.at (17).split(",");
                            disZDR = visibilidad.at(0).toDouble();
                        }
                        visibilidad = grande.at(7).split(",");//0.9
                        disVDirecta[0].append(visibilidad.at(0).toDouble());

                        if(((medio == 39 && Vuelo >= 100) || ((medio == 38 || medio == 37 || medio == 36) && Vuelo >= 25)) && grupo == 1)
                        {
                            disVDirecta[0].append(disLanzamiento);
                            disVDirecta[0].append(disZDR);
                            double Ykm = disVDirecta[0].at (1)/1000 * cos(0 * RADIAN);
                            double Xkm = disVDirecta[0].at (1)/1000 * sin(0 * RADIAN);
                            double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV1.append (-LonOR);
                            yV1.append (LatOR);
                            Ykm = disVDirecta[0].at (2)/1000 * cos(0 * RADIAN);
                            Xkm = disVDirecta[0].at (2)/1000 * sin(0 * RADIAN);
                            LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                            LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                            xV2.append (-LonOR);
                            yV2.append (LatOR);
                        }
                        double Ykm = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);
                        double Xkm = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales
                        xV.append (-LonOR);
                        yV.append (LatOR);
                    }

                }
                //*******************zona muerta muis y haz estrecho**********************//
                if(medio == 48 || medio == 41)
                {
                    QStringList muis1 =grande.at(2).split(",");
                    for (int j = 0; j < muis1.length(); ++j)
                    {
                        if(probabilidad > 0)
                        {
                            if(visibilidad.at (0).toDouble() > muis1.at (j).toDouble())
                                ZonaMuertaMuis[i].append(muis1.at(j).toDouble());
                        }
                        else
                            ZonaMuertaMuis[i].append(muis1.at(j).toDouble());
                    }
                    QStringList muis2 =grande.at(3).split(",");
                    for (int j = 0; j < muis2.length(); ++j)
                    {
                        if(probabilidad > 0)
                        {
                            if(visibilidad.at (0).toDouble() > muis2.at (j).toDouble())
                                ZonaMuertaMuis2[i].append(muis2.at(j).toDouble());
                        }
                        else
                            ZonaMuertaMuis2[i].append(muis2.at(j).toDouble());
                    }
                    if(ZonaMuertaMuis[i].isEmpty ())
                        ZonaMuertaMuis[i].append (0);
                    if(ZonaMuertaMuis2[i].isEmpty ())
                        ZonaMuertaMuis2[i].append (0);

                    if(i<360)
                    {
                        double Ykm = ZonaMuertaMuis[i].at(0)/1000 * cos(i * RADIAN);
                        double Xkm = ZonaMuertaMuis[i].at(0)/1000 * sin(i * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales

                        double Ykm2 = ZonaMuertaMuis2[i].at(0)/1000 * cos(i * RADIAN);
                        double Xkm2 = ZonaMuertaMuis2[i].at(0)/1000 * sin(i * RADIAN);
                        double LatOR2 = dancal.Latitud_2(Ykm2,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR2 = dancal.Longitud_2(Xkm2,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales

                        if(angulositio == -1 || angulositio == 0)
                        {
                            if(Av == 0)
                            {
                                //xZM2
                                xZM2.append (-LonOR2);
                                yZM2.append (LatOR2);
                            }
                            else
                            {
                                //xZM1
                                xZM1.append (-LonOR);
                                yZM1.append (LatOR);
                            }
                        }
                        else
                        {
                            if(Av == 0)
                            {
                                //xZM2
                                xZM2.append (-LonOR2);
                                yZM2.append (LatOR2);
                            }
                            else
                            {
                                //xZM1
                                xZM1.append (-LonOR);
                                yZM1.append (LatOR);
                            }
                        }
                    }
                    else
                    {
                        double Ykm = ZonaMuertaMuis[0].at(0)/1000 * cos(0 * RADIAN);
                        double Xkm = ZonaMuertaMuis[0].at(0)/1000 * sin(0 * RADIAN);
                        double LatOR = dancal.Latitud_2(Ykm,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR = dancal.Longitud_2(Xkm,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales

                        double Ykm2 = ZonaMuertaMuis2[0].at(0)/1000 * cos(0 * RADIAN);
                        double Xkm2 = ZonaMuertaMuis2[0].at(0)/1000 * sin(0 * RADIAN);
                        double LatOR2 = dancal.Latitud_2(Ykm2,lat);//determinar la latitud del objetivo con respecto al radar en grados decimales
                        double LonOR2 = dancal.Longitud_2(Xkm2,-lon,lat);//determinar la longitud del objetivo con respecto al radar en grados decimales

                        if(angulositio == -1 || angulositio == 0)
                        {
                            if(Av == 0)
                            {
                                //xZM2

                                xZM2.append (-LonOR2);
                                yZM2.append (LatOR2);
                            }
                            else
                            {
                                //xZM1
                                xZM1.append (-LonOR);
                                yZM1.append (LatOR);
                            }
                        }
                        else
                        {
                            if(Av == 0)
                            {
                                //xZM2
                                xZM2.append (-LonOR2);
                                yZM2.append (LatOR2);
                            }
                            else
                            {
                                //xZM1
                                xZM1.append (-LonOR);
                                yZM1.append (LatOR);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            listo = false;
            f.remove();
        }
    }
    return listo;
}

void Campos::graphClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent* event)
{
    Q_UNUSED(event);
    Q_UNUSED(dataIndex);
    QString message = plottable->name();

    // GUARDAR la curva seleccionada actualmente
    curvaSeleccionadaActual = plottable->name();

    for (auto j : listaZM)
    {
        if(j->objectName ().contains (message))
        {
            j->setSelected (true);
        }
    }

    for (int i = 0; i < ui->widget->plottableCount (); ++i)
    {
        auto *curve = dynamic_cast<QCPCurve*>(ui->widget->plottable (i));
        if(curve->selected ())
        {
            if(medio == 45 && curve->name().contains ("Tx"))
            {
                xVTx.clear ();
                yVTx.clear ();
                for (int var = 0; var < curve->dataCount (); ++var)
                {
                    xVTx.append (curve->dataMainKey (var));
                    yVTx.append (curve->dataMainValue (var));
                }
                for (const auto & ListCentro : ListCentros)
                {
                    if(ListCentro.name.contains ("Centro_" + curve->name ()))
                    {
                        lat = ListCentro.Lat;
                        lon = ListCentro.Lon;
                        break;
                    }
                }
                ui->pB_Sector->setEnabled (true);
            }
            if(medio == 47 && curve->name().contains ("Rx"))
            {
                xVRx.clear ();
                yVRx.clear ();
                for (int var = 0; var < curve->dataCount (); ++var)
                {
                    xVRx.append (curve->dataMainKey (var));
                    yVRx.append (curve->dataMainValue (var));
                }
                for (const auto & ListCentro : ListCentros)
                {
                    if(ListCentro.name.contains ("Centro_" + curve->name ()))
                    {
                        lat = ListCentro.Lat;
                        lon = ListCentro.Lon;
                        break;
                    }
                }
                ui->pB_Sector->setEnabled (true);
            }
            //para todos
            xVSectorP.clear ();
            yVSectorP.clear ();
            for (int var = 0; var < curve->dataCount (); ++var)
            {
                xVSectorP.append (curve->dataMainKey (var));
                yVSectorP.append (curve->dataMainValue (var));
            }
            for (const auto & ListCentro : ListCentros)
            {
                if(ListCentro.name.contains ("Centro_" + curve->name ()))
                {
                    lat = ListCentro.Lat;
                    lon = ListCentro.Lon;
                    break;
                }
            }
            ui->pB_SectorPrincipal->setEnabled (true);
        }

        if(plottable->name ().contains (curve->name ()))
        {
            curve->setSelection (QCPDataSelection(curve->data()->dataRange()));
        }
    }
    plottitle2->setText (message);

    // Mostrar el sector existente si lo hay para esta curva
    int idx = encontrarSectorPorCurva(curvaSeleccionadaActual);
    if (idx != -1) {
        ui->widget->layer("SectorPrincipal")->setVisible(true);
        ui->widget->replot();
    }
}

void Campos::itemClick(QCPAbstractItem *item, QMouseEvent *event)
{
    Q_UNUSED(event);

    QString message = item->objectName ();

    for (int i = 0; i < ui->widget->plottableCount (); ++i)
    {
        auto *curve = dynamic_cast<QCPCurve*>(ui->widget->plottable (i));
        if(message.contains (curve->name ()))
        {
            curve->setSelection (QCPDataSelection(curve->data()->dataRange()));
            message = curve->name ();
        }
    }
    plottitle2->setText (message);
}

void Campos::widgetClik()
{
    for (int i = 0; i < ui->widget->plottableCount (); ++i)
    {
        auto *curve = dynamic_cast<QCPCurve*>(ui->widget->plottable (i));
        if(!curve->selected ())
        {
            plottitle2->setText ("");
            ui->pB_Sector->setEnabled (false);
            ui->pB_SectorPrincipal->setEnabled (false);
        }
    }
}

void Campos::on_pB_Borrar_clicked()
{
    QMessageBox msgBox(QMessageBox::Warning, tr("Atención"),"Usted va a borrar las zonas representadas", nullptr, this);
    msgBox.addButton(tr("Si"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("No"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
        borrarTodo();
}

void Campos::on_pB_Centro_clicked()
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
            customplotf.pintaElipses (ui->widget,ListCentros.at (cont).Lat,ListCentros.at (cont).Lon,&listaElipse,&listaText);
            customplotf.pintaLineaAzimut (ui->widget,ListCentros.at (cont).Lat,ListCentros.at (cont).Lon,&listaLine,&listaText);
            cont++;
        }
        else
            cont=0;
    }
    ui->widget->layer ("AzimutDist")->replot ();
}

void Campos::borrarSeleccion(QCPAbstractPlottable *plottable)
{
    // Eliminar el sector asociado a esta curva ANTES de borrar la curva
    eliminarSectorDeCurva(plottable->name());

    // Si era la curva seleccionada actualmente, limpiar
    if (curvaSeleccionadaActual == plottable->name()) {
        curvaSeleccionadaActual.clear();
    }

    for (int i = 0; i < ListCentros.length (); ++i)
    {
        //        qDebug()<<ListCentros.at (i).name<<plottable->name ();
        if(ListCentros.at (i).name.contains (plottable->name ()))
        {
            ListCentros.removeAt (i);
            i--;
        }
    }
    for (int i = 0; i < listaZM.length (); ++i)
    {
        if(listaZM.at (i)->objectName ().contains (plottable->name ()))
        {
            ui->widget->removeItem (listaZM.at (i));
            listaZM.removeAt (i);
            i--;
        }
    }
    for (int i = 0; i < ui->widget->plottableCount (); ++i)
    {
        QString plotvname = plottable->name ();
        if(plotvname.contains ("Centro_"))
            plotvname.remove ("Centro_");
        auto *curve = dynamic_cast<QCPCurve*>(ui->widget->plottable (i));
        QString curvname = curve->name ();
        if(curvname.contains ("Centro_"))
            curvname.remove ("Centro_");
        if(plotvname == curvname)
        {
            ui->widget->removePlottable (curve);
            i--;
        }
    }
    plottitle2->setText ("");
    ui->widget->layer ("Zonas")->replot ();
    if(ui->cB_ListaZonas->count () > 0)
    {
        file = ui->cB_ListaZonas->currentText ();
        deterProbabilidad(file);
    }
    else
    {
        ui->cB_Probabilidad->setEnabled (false);
        //        ui->cb_Velocidad->setEnabled (false);
    }
}

void Campos::borrarTodo()
{
    ui->pB_Sector->setDisabled (true);
    ui->pB_SectorPrincipal->setDisabled (true);
    ListCentros.clear ();
    cont = 0;
    ui->widget->clearPlottables ();
    //****************//
    //    for (int i = 0; i < ui->widget->plottableCount (); ++i)
    //    {
    //        QString plotvname =ui->widget->plottable (i)->name ();
    //        if(plotvname.contains (prob))
    //        {
    //            ui->widget->removePlottable (i);
    //            i--;
    //        }
    //    }
    //***************//
    for (auto i : listaElipse)
    {
        ui->widget->removeItem(i);
    }
    listaElipse.clear ();
    for (auto i : listaZM)
    {
        //        if(listaZM.at (i)->objectName ().contains (prob))
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
    plottitle2->setText ("");
    ui->widget->replot ();
    if(ui->cB_ListaZonas->count () > 0)
    {
        file = ui->cB_ListaZonas->currentText ();
        deterProbabilidad(file);
    }
    else
    {
        ui->cB_Probabilidad->setEnabled (false);
        //        ui->cb_Velocidad->setEnabled (false);
    }
}

void Campos::on_pB_Salvar_clicked()
{
    //  filePrintPdf();
    QString nom;

    nom = (QDir::currentPath()+"/GraficasPDF/Campos/");
    CustomPlotF customplotf;
    customplotf.filePrintPdf (ui->widget,nom);
}

void Campos::on_pB_BorrarSelecction_clicked()
{
    QMessageBox msgBox(QMessageBox::Warning, tr("Atención"),"Usted va a borrar la zona seleccionada", nullptr, this);
    msgBox.addButton(tr("Si"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("No"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole)
    {
        for (int i = 0; i < ui->widget->plottableCount (); ++i)
        {
            auto *curve = dynamic_cast<QCPCurve*>(ui->widget->plottable (i));
            if(curve->selected ())
                borrarSeleccion(curve);
        }
        for (int i = 0; i < ui->widget->itemCount (); ++i)
        {
            auto *itemElipse = dynamic_cast<QCPItemEllipse*>(ui->widget->item (i));
            if(itemElipse->selected ())
            {
                for (int i = 0; i < ui->widget->plottableCount (); ++i)
                {
                    auto *curve = dynamic_cast<QCPCurve*>(ui->widget->plottable (i));
                    if(itemElipse->objectName ().contains (curve->name ()))
                        borrarSeleccion(curve);
                }
            }
        }
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
        cont=0;
        ui->widget->layer ("AzimutDist")->replot ();
    }
}

void Campos::on_pB_Sector_clicked()
{
    for (auto i : sectorCurva)
    {
        ui->widget->removePlottable (i);
    }
    sectorCurva.clear ();
    XSector.clear ();
    YSector.clear ();
    DlgSectorBiestatico dlgsectorbiestatico;
    dlgsectorbiestatico.xVRx = xVRx;
    dlgsectorbiestatico.yVRx = yVRx;
    dlgsectorbiestatico.xVTx = xVTx;
    dlgsectorbiestatico.yVTx = yVTx;
    dlgsectorbiestatico.latTX = lat;
    dlgsectorbiestatico.lonTX = lon;
    dlgsectorbiestatico.medio = medio;
    if(dlgsectorbiestatico.exec () == QDialog::Accepted)
    {
        int angDer = dlgsectorbiestatico.angDer;
        int angIzq = dlgsectorbiestatico.angIzq;
        if(medio == 47)
        {
            XSector.append (lon);//inicio
            YSector.append (lat);//inicio
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
            XSector.append (lon);//fin
            YSector.append (lat);//inicio
        }
        else//m == 45
        {
            XSector.append (lon);//inicio
            YSector.append (lat);//inicio
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
            XSector.append (lon);//fin
            YSector.append (lat);//inicio
        }

        CustomPlotF customplotf;
        customplotf.pintaCurva (ui->widget,"Sector","Sector Tx",QPen(Qt::blue,2),QColor(150,0,150,100),XSector,YSector,&sectorCurva);

        ui->widget->layer ("Sector")->setVisible (true);
        ui->widget->layer ("Sector")->replot ();
    }
    else
    {
        ui->widget->layer ("Sector")->replot ();
    }
}

void Campos::on_pB_ZoomMas_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommas(ui->widget);
}

void Campos::on_pB_ZoomMen_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommenos(ui->widget);
}

void Campos::deterProbabilidad(const QString& strFile)
{
    file = strFile;//fichero seleccionado
    int m = QString("%1%2").arg(strFile.at (4)).arg(strFile.at (5)).toInt ();//veo el medio
    if(strFile.contains ("ZVD"))
    {
        if(m >= 36)
        {
            ui->cB_Probabilidad->setEnabled (true);
            probabilidad = ui->cB_Probabilidad->currentIndex ();
            if(m<40)
            {
                ui->cb_Velocidad->setEnabled (true);
                //                ui->cb_Velocidad->setCurrentText (QString::number (vel));
            }
        }
        else
        {
            ui->cB_Probabilidad->setEnabled (false);
            probabilidad = 0;
            probabilidadAux  = false;

            //            ui->cb_Velocidad->setEnabled (false);
        }
    }
    else
    {
        ui->cB_Probabilidad->setEnabled (false);
        probabilidad = 0;
        probabilidadAux  = true;
        //        ui->cb_Velocidad->setEnabled (false);
    }
    //     ui->cb_Velocidad->setCurrentText (QString::number (vel));
}

void Campos::on_cB_Probabilidad_currentIndexChanged(int index)
{
    if(ListCentros.length ()>0)
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("Atención"),"Usted va a borrar las zonas que no poseen la probabilidad seleccionada", nullptr, this);
        msgBox.addButton(tr("Si"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("No"), QMessageBox::RejectRole);
        if (msgBox.exec() == QMessageBox::AcceptRole)
        {
            //    borrarTodo ();
            for (int i = 0; i < ui->widget->plottableCount (); ++i)
            {
                auto *curve = dynamic_cast<QCPCurve*>(ui->widget->plottable (i));
                int m = QString("%1%2").arg(curve->name ().at (4)).arg(curve->name ().at (5)).toInt ();//veo el medio
                if(/*curve->name ().contains (QString::number (vel))*/m>=36 )
                    //                if(curve->name ().contains (prob))
                {
                    borrarSeleccion(curve);
                    i--;
                }
            }
            probabilidad = index;
            probabilidadAux = true;
            prob =  "_P:"+ui->cB_Probabilidad->currentText ();
        }
    }
    else
    {
        probabilidad = index;
        probabilidadAux = false;
        prob =  "_P:"+ui->cB_Probabilidad->currentText ();
    }
}

void Campos::on_cB_AlturaObjetivo_currentTextChanged(const QString &arg1)
{
    if(ListCentros.length ()>0)
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("Atención"),"Usted va a borrar las zonas representadas", nullptr, this);
        msgBox.addButton(tr("Si"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("No"), QMessageBox::RejectRole);
        if (msgBox.exec() == QMessageBox::AcceptRole)
        {
            borrarTodo();
            ui->cB_ListaZonas->clear ();
            Av = arg1.toInt ();
            for (int i = 0; i < todo.length (); ++i)
            {
                QString zona = todo.at (i);
                if (Av == alvuelo(zona))
                {
                    ui->cb_Velocidad->setCurrentIndex (0);
                    ui->cB_ListaZonas->addItem (todo.at (i));
                }
            }
            if(ui->cB_ListaZonas->count () > 0)
            {
                file = ui->cB_ListaZonas->currentText ();
                deterProbabilidad(file);
            }
            else
                ui->cB_Probabilidad->setEnabled (false);
            ui->rB_Todos->setChecked (true);
        }
        else//mensaje no
        {
            //            if(Av != arg1)
            ui->cB_AlturaObjetivo->setCurrentText (QString::number (Av));

        }
    }
    else
    {
        ui->cB_ListaZonas->clear ();
        Av = arg1.toInt ();
        for (int i = 0; i < todo.length (); ++i)
        {
            QString zona = todo.at (i);
            if (Av == alvuelo(zona))
            {
                ui->cb_Velocidad->setCurrentIndex (0);
                ui->cB_ListaZonas->addItem (todo.at (i));
            }
        }
        if(ui->cB_ListaZonas->count () > 0)
        {
            file = ui->cB_ListaZonas->currentText ();
            deterProbabilidad(file);
        }
        else
        {
            ui->cB_Probabilidad->setEnabled (false);
        }
        ui->rB_Todos->setChecked (true);
    }
}

void Campos::on_cb_Velocidad_currentTextChanged(const QString &arg1)
{   
    if(ListCentros.length ()>0)
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("Atención"),"Usted va a borrar las zonas que no poseen la velocidad seleccionada", nullptr, this);
        msgBox.addButton(tr("Si"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("No"), QMessageBox::RejectRole);
        if (msgBox.exec() == QMessageBox::AcceptRole)
        {
            for (int i = 0; i < ui->widget->plottableCount (); ++i)//borro el que tenga otra velocidad
            {
                auto *curve = dynamic_cast<QCPCurve*>(ui->widget->plottable (i));
                qDebug()<<curve->name ();
                int m = QString("%1%2").arg(curve->name ().at (4)).arg(curve->name ().at (5)).toInt ();//veo el medio
                if(/*curve->name ().contains (QString::number (vel))*/m>=36 && m<40)
                {
                    borrarSeleccion(curve);
                    i--;
                }
            }
            ui->cB_ListaZonas->clear ();
            Av = ui->cB_AlturaObjetivo->currentText ().toInt ();
            vel = arg1.toInt ();
            for (int i = 0; i < todo.length (); ++i)
            {
                QString zona = todo.at (i);
                if(arg1 != "[]")
                {
                    if (vel == velocidad (zona) && Av == alvuelo (zona))
                        ui->cB_ListaZonas->addItem (todo.at (i));
                    else
                    {
                        if (Av == alvuelo (zona) && velocidad(zona) == 0)
                            ui->cB_ListaZonas->addItem (todo.at (i));
                    }
                }
                else
                {
                    if (Av == alvuelo (zona))
                        ui->cB_ListaZonas->addItem (todo.at (i));
                }
            }
            if(ui->cB_ListaZonas->count () > 0)
            {
                file = ui->cB_ListaZonas->currentText ();
                deterProbabilidad(file);
            }
            else
                ui->cB_Probabilidad->setEnabled (false);
            ui->rB_Todos->setChecked (true);
        }
        else//mensaje no
        {
            if(vel!=0)
                ui->cb_Velocidad->setCurrentText (QString::number (vel));
            else
                ui->cb_Velocidad->setCurrentIndex (0);
        }
    }
    else
    {
        ui->cB_ListaZonas->clear ();
        Av = ui->cB_AlturaObjetivo->currentText ().toInt ();
        vel = arg1.toInt ();
        for (int i = 0; i < todo.length (); ++i)
        {
            QString zona = todo.at (i);
            if(arg1 != "[]")
            {
                if (vel == velocidad (zona) && Av == alvuelo (zona))
                    ui->cB_ListaZonas->addItem (todo.at (i));
                else
                {
                    if (Av == alvuelo (zona) && velocidad(zona) == 0)
                        ui->cB_ListaZonas->addItem (todo.at (i));
                }
            }
            else
            {
                if (Av == alvuelo (zona)) {
                    ui->cB_ListaZonas->addItem (todo.at (i)); }
            }
        }
        if(ui->cB_ListaZonas->count () > 0)
        {
            file = ui->cB_ListaZonas->currentText ();
            deterProbabilidad(file);
        }
        else
            ui->cB_Probabilidad->setEnabled (false);
        ui->rB_Todos->setChecked (true);
    }
}

void Campos::on_pB_SectorPrincipal_clicked()
{
    if (curvaSeleccionadaActual.isEmpty()) {
        QMessageBox::information(this, "Información", "Primero seleccione una curva");
        return;
    }

    bool ok;
    int a1 = QInputDialog::getInt(this, tr("Azimut inicio"),
                                  tr("A1:"), 300, 0, 360, 1, &ok);
    if (!ok) return;

    int a2 = QInputDialog::getInt(this, tr("Azimut fin"),
                                  tr("A2:"), 10, 0, 360, 1, &ok);
    if (!ok) return;

    // Verificar si ya existe un sector para esta curva
    int idx = encontrarSectorPorCurva(curvaSeleccionadaActual);

    if (idx != -1) {
        // Actualizar sector existente - eliminar el anterior
        for (auto* plottable : sectoresPorCurva[idx].sectores) {
            ui->widget->removePlottable(plottable);
        }
        sectoresPorCurva[idx].sectores.clear();
        sectoresPorCurva[idx].a1 = a1;
        sectoresPorCurva[idx].a2 = a2;
    } else {
        // Crear nuevo sector
        SectorInfo nuevoSector;
        nuevoSector.nombreCurva = curvaSeleccionadaActual;
        nuevoSector.a1 = a1;
        nuevoSector.a2 = a2;
        sectoresPorCurva.append(nuevoSector);
        idx = sectoresPorCurva.size() - 1;
    }

    // Construir el sector con los ángulos
    QVector<double> XSectorTemp, YSectorTemp;

    int i1 = a1 % 360;
    int i2 = a2 % 360;
    if (i1 < 0) i1 += 360;
    if (i2 < 0) i2 += 360;

    if (i1 <= i2) {
        for (int i = i1; i <= i2; i++) {
            if (i < xVSectorP.size()) {
                XSectorTemp.append(xVSectorP[i]);
                YSectorTemp.append(yVSectorP[i]);
            }
        }
    } else {
        for (int i = i1; i < 360 && i < xVSectorP.size(); i++) {
            XSectorTemp.append(xVSectorP[i]);
            YSectorTemp.append(yVSectorP[i]);
        }
        for (int i = 0; i <= i2 && i < xVSectorP.size(); i++) {
            XSectorTemp.append(xVSectorP[i]);
            YSectorTemp.append(yVSectorP[i]);
        }
    }

    // Cerrar con el centro
    XSectorTemp.prepend(lon);
    YSectorTemp.prepend(lat);
    XSectorTemp.append(lon);
    YSectorTemp.append(lat);

    // Pintar el nuevo sector
    CustomPlotF customplotf;
    QList<QCPCurve *> nuevosSectores;

    customplotf.pintaCurva(ui->widget, "SectorPrincipal",
                           "Sector_" + curvaSeleccionadaActual,
                           QPen(Qt::blue, 2), QColor(0, 150, 0, 100),
                           XSectorTemp, YSectorTemp, &nuevosSectores);


    // Guardar los plottables del sector
    sectoresPorCurva[idx].sectores = nuevosSectores;

    ui->widget->layer("SectorPrincipal")->setVisible(true);
    ui->widget->replot();
}

// Función para encontrar el índice del sector asociado a una curva
int Campos::encontrarSectorPorCurva(const QString& nombreCurva)
{
    for (int i = 0; i < sectoresPorCurva.size(); ++i) {
        if (sectoresPorCurva[i].nombreCurva == nombreCurva) {
            return i;
        }
    }
    return -1;  // No existe
}

// Función para eliminar el sector de una curva específica
void Campos::eliminarSectorDeCurva(const QString& nombreCurva)
{
    int idx = encontrarSectorPorCurva(nombreCurva);
    if (idx != -1) {
        // Eliminar los plottables del sector
        for (auto* plottable : sectoresPorCurva[idx].sectores) {
            ui->widget->removePlottable(plottable);
        }
        sectoresPorCurva.removeAt(idx);
    }
}


