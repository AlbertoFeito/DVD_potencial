#include "graficaperfil.h"
#include "ui_graficaperfil.h"
#include <QDebug>
Graficaperfil::Graficaperfil(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graficaperfil)
{
    ui->setupUi(this);
    //  setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectItems);

    angulositio = 0;
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->widget->legend->setFont(legendFont);
    ui->widget->legend->setSelectedFont(legendFont);
    ui->widget->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
    //make legend wrap text in two columns
    ui->widget->legend->setWrap(2);
    ui->widget->legend->setRowSpacing(1);
    ui->widget->legend->setColumnSpacing(2);
    ui->widget->legend->setFillOrder(QCPLayoutGrid::FillOrder::foColumnsFirst,true);

    ui->widget->addLayer("angulositio",nullptr,QCustomPlot::limBelow);
    ui->widget->layer("angulositio")->setVisible(false);

    ui->widget->addLayer("Dismax",nullptr,QCustomPlot::limBelow);
    ui->widget->layer("Dismax")->setVisible(false);

    ui->widget->addLayer("Recta Auxiliar",nullptr,QCustomPlot::limBelow);
    ui->widget->layer("Recta Auxiliar")->setVisible(false);

    //  ui->widget->addLayer("Legend",0,QCustomPlot::limAbove);
    //  ui->widget->layer ("Legend")->setVisible(false);
    //  ui->widget->legend->setLayer ("Legend");

    ui->widget->plotLayout()->insertRow(0);
    plottitle = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(0, 0,plottitle );
    plottitle2 = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(2, 0,plottitle2 );
    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(ui->widget, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));


    connect(ui->widget, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel(QWheelEvent*)));
    connect(ui->widget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));

    connect(ui->widget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(ui->widget, SIGNAL(itemClick(QCPAbstractItem*,QMouseEvent*)), this, SLOT(itemClicked(QCPAbstractItem*,QMouseEvent*)));
    connect(ui->widget, SIGNAL(legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendItemClicked(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)));

    cont = 0;
}

Graficaperfil::~Graficaperfil()
{
    delete ui;
}
void Graficaperfil::initGraph()
{
    ui->widget->addGraph(); //perfil del terreno
    ui->widget->graph(0)->setPen(QColor(0,100,0));
    ui->widget->graph(0)->setBrush(QBrush(QColor(0,100,0,50)));

    ui->widget->addGraph();//curvatura de la tierra
    ui->widget->graph(1)->setPen(QColor(100,0,0)); //color del trazo
    ui->widget->graph(0)->setChannelFillGraph(ui->widget->graph(1));

    ui->widget->addGraph();//curva de visibilidad
    ui->widget->graph(2)->setPen(QColor(0,0,0)); //color del trazo

    ui->widget->addGraph();//Altura a 25 metros
    ui->widget->graph(3)->setPen(QColor(0,0,255)); //color del trazo

    ui->widget->addGraph();//Altura a 50 metros
    ui->widget->graph(4)->setPen(QColor(0,0,255)); //color del trazo

    ui->widget->addGraph();//Altura a 100 metros
    ui->widget->graph(5)->setPen(QColor(255,0,255)); //color del trazo

    ui->widget->addGraph();//Altura a 200 metros
    ui->widget->graph(6)->setPen(QColor(100,0,255)); //color del trazo

    ui->widget->addGraph();//Altura a 300 metros
    ui->widget->graph(7)->setPen(QColor(100,200,255)); //color del trazo

    ui->widget->addGraph();//Altura a 400 metros
    ui->widget->graph(8)->setPen(QColor(200,100,255)); //color del trazo

    ui->widget->addGraph();//Altura a 500 metros
    ui->widget->graph(9)->setPen(QColor(50,100,255));

    ui->widget->addGraph();//Altura a 1000 metros
    ui->widget->graph(10)->setPen(QColor(50,50,255));

    ui->widget->addGraph();//recta de visibilidad
    ui->widget->graph(11)->setPen(QColor(255,0,0)); //color del trazo

    ui->widget->addGraph();//mastil
    ui->widget->graph(12)->setPen(QColor(10,10,255));

    //***********angulos de sitio *************//
    ui->widget->addGraph();
    ui->widget->graph(13)->setPen(QPen(Qt::black,1,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(14)->setPen(QPen(Qt::black,1,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(15)->setPen(QPen(Qt::black,1,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(16)->setPen(QPen(Qt::yellow,2,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(17)->setPen(QPen(Qt::black,1,Qt::DashDotLine));

    ui->widget->addGraph();
    ui->widget->graph(18)->setPen(QPen(Qt::black,1,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(19)->setPen(QPen(Qt::black,1,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(20)->setPen(QPen(Qt::black,1,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(21)->setPen(QPen(Qt::black,1,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(22)->setPen(QPen(Qt::yellow,2,Qt::DashDotLine));

    ui->widget->addGraph();
    ui->widget->graph(23)->setPen(QPen(Qt::black,1,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(24)->setPen(QPen(Qt::black,1,Qt::DashLine));

    ui->widget->addGraph();
    ui->widget->graph(25)->setPen(QPen(Qt::black,1,Qt::DashLine));



    ui->widget->addGraph();
    ui->widget->graph(26)->setPen(QPen(Qt::blue));
    ui->widget->graph(26)->setLineStyle(QCPGraph::lsNone);
    ui->widget->graph(26)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));

    //******************************************************//

    ui->widget->addGraph();//recta de visibilidad
    ui->widget->graph(27)->setPen(QColor(255,0,0)); //color del trazo
    ui->widget->graph(27)->setName("Recta de visibilidad directa auxiliar");
    ui->widget->graph (27)->setLayer ("Recta Auxiliar");

    /*******************/

    /********** >1000*********/
    ui->widget->addGraph();//"Altura de vuelo a 2000m sobre el perfil del terreno"
    ui->widget->graph(28)->setPen(QColor(0,0,250)); //color del trazo
    ui->widget->graph(28)->setName("Altura de vuelo a 2000m sobre el perfil del terreno");

    ui->widget->addGraph();//"Altura de vuelo a 4000m sobre el perfil del terreno"
    ui->widget->graph(29)->setPen(QColor(0,0,250)); //color del trazo
    ui->widget->graph(29)->setName("Altura de vuelo a 4000m sobre el perfil del terreno");

    ui->widget->addGraph();//"Altura de vuelo a 6000m sobre el perfil del terreno"
    ui->widget->graph(30)->setPen(QColor(0,0,250)); //color del trazo
    ui->widget->graph(30)->setName("Altura de vuelo a 6000m sobre el perfil del terreno");

    ui->widget->addGraph();//"Altura de vuelo a 10000m sobre el perfil del terreno"
    ui->widget->graph(31)->setPen(QColor(0,0,250)); //color del trazo
    ui->widget->graph(31)->setName("Altura de vuelo a 10000m sobre el perfil del terreno");

    ui->widget->addGraph();//"Altura de vuelo a 17000m sobre el perfil del terreno"
    ui->widget->graph(32)->setPen(QColor(0,0,250)); //color del trazo
    ui->widget->graph(32)->setName("Altura de vuelo a 17000m sobre el perfil del terreno");

    ui->widget->addGraph();//"Altura de vuelo a 30000m sobre el perfil del terreno"
    ui->widget->graph(33)->setPen(QColor(0,0,250)); //color del trazo
    ui->widget->graph(33)->setName("Altura de vuelo a 30000m sobre el perfil del terreno");


    ui->widget->addGraph();
    ui->widget->graph(34)->setPen(QPen(Qt::black,1,Qt::DashLine));//5

    ui->widget->addGraph();
    ui->widget->graph(35)->setPen(QPen(Qt::black,1,Qt::DashLine));//-5



    ui->widget->addGraph();
    ui->widget->graph(36)->setPen(QPen(Qt::blue));
    ui->widget->graph(36)->setLineStyle(QCPGraph::lsNone);
    ui->widget->graph(36)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,3));//dvdp
    /************************/
    ui->widget->xAxis->setLabel("Distancia (m)");//etiqueta para las abscisas
    ui->widget->yAxis->setLabel("Altura (m)");//etiqueta para las abscisas
    ui->widget->yAxis->setRange(-100,1000);//se estabelece rango de y

    ui->widget->xAxis->setRange(-20,50000/*distancia*1000*/);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::black);
    ui->widget->xAxis->setBasePen(pen);
    ui->widget->yAxis->setBasePen(pen);

    //***nombre para la leyenda****//
    ui->widget->graph(0)->setName("Perfil de Alturas Azimut: "+QString::number(aazimut)+"°");
    ui->widget->graph(1)->setName("Curvatura de la tierra");
    ui->widget->graph(2)->setName("Curva de visibilidad directa");
    ui->widget->graph(3)->setName("Altura de vuelo a 25 sobre el perfil del terreno");
    ui->widget->graph(4)->setName("Altura de vuelo a 50m sobre el perfil del terreno");
    ui->widget->graph(5)->setName("Altura de vuelo a 100m sobre el perfil del terreno");
    ui->widget->graph(6)->setName("Altura de vuelo a 200m sobre el perfil del terreno");
    ui->widget->graph(7)->setName("Altura de vuelo a 300m sobre el perfil del terreno");
    ui->widget->graph(8)->setName("Altura de vuelo a 400m sobre el perfil del terreno");
    ui->widget->graph(9)->setName("Altura de vuelo a 500m sobre el perfil del terreno");
    ui->widget->graph(10)->setName("Altura de vuelo a 1000m sobre el perfil del terreno");

    ui->widget->graph(11)->setName("Recta de visibilidad");
    if(calculos->medio == 48)
    {
        ui->widget->graph(13)->setName("E.F");
        ui->widget->graph(13)->setPen (QPen(Qt::magenta,2,Qt::DashLine));
    }
    else
        ui->widget->graph(13)->setName("0°");

    ui->widget->graph(14)->setName("10'");
    ui->widget->graph(15)->setName("30'");
    ui->widget->graph(16)->setName("1°");
    ui->widget->graph(17)->setName("2°");
    ui->widget->graph(18)->setName("3°");
    ui->widget->graph(19)->setName("4°");
    ui->widget->graph(34)->setName("5°");

    ui->widget->graph(20)->setName("-10'");
    ui->widget->graph(21)->setName("-30'");
    ui->widget->graph(22)->setName("-1°");
    ui->widget->graph(23)->setName("-2°");
    ui->widget->graph(24)->setName("-3°");
    ui->widget->graph(25)->setName("-4°");
    ui->widget->graph(35)->setName("-5°");

    ui->widget->graph(26)->setName("Obstáculos en el terreno");
    ui->widget->graph(36)->setName("D.V.D.P para cada altura de vuelo");

    ui->widget->graph(13)->setLayer("angulositio");
    ui->widget->graph(14)->setLayer("angulositio");
    ui->widget->graph(15)->setLayer("angulositio");
    ui->widget->graph(16)->setLayer("angulositio");
    ui->widget->graph(17)->setLayer("angulositio");
    ui->widget->graph(18)->setLayer("angulositio");
    ui->widget->graph(19)->setLayer("angulositio");
    ui->widget->graph(20)->setLayer("angulositio");
    ui->widget->graph(21)->setLayer("angulositio");
    ui->widget->graph(22)->setLayer("angulositio");
    ui->widget->graph(23)->setLayer("angulositio");
    ui->widget->graph(24)->setLayer("angulositio");
    ui->widget->graph(25)->setLayer("angulositio");
    ui->widget->graph(34)->setLayer("angulositio");
    ui->widget->graph(35)->setLayer("angulositio");
}

void Graficaperfil::pintar()
{  
    // set title of plot:
    plottitle->setText("Perfil de Alturas Azimut: "+QString::number(aazimut)+"°. Posición: "+calculos->label);

    double AltPos = 0.0;
    posicion = 0.0;//Altura de la posicion  + altura de la antena
    double d=0.0;//distancia de 90++
    double curva = 0.0;//curvatura de la tierra
    double DisAngulocierre = 0.0;//distancia del angulo de cierre

    double Disvuelo = 0.0;
    double AltVuelo = 0.0;
    double AlvueloCT = 0.0;
    double altu = 0.0;
    double punta = 0.0;

    ui->widget->clearGraphs();
    for (auto i : listphaseTracer)
    {
        ui->widget->removeItem(i);
    }
    for (auto listphaseTracerDi : listphaseTracerDis)
    {
        ui->widget->removeItem(listphaseTracerDi);
    }
    for (auto i : Listtextgrado)
    {
        ui->widget->removeItem (i);
    }

    ui->widget->clearItems ();
    listphaseTracer.clear();
    listphaseTracerDis.clear();
    Listtextgrado.clear ();

    initGraph();

    AltPos = calculos->datoterrenoMiPos.altura;
    posicion = AltPos+calculos->AlAnt;

    AlvueloCT = posicion;
    punta = AltPos;

    DisAngulocierre = posicion;

    ui->widget->graph(2)->addData(Disvuelo,AlvueloCT); //curva de visibilidad directa

    ui->widget->graph(13)->addData(0,posicion); //0min
    ui->widget->graph(14)->addData(0,posicion); //10min
    ui->widget->graph(15)->addData(0,posicion); //30min
    ui->widget->graph(16)->addData(0,posicion); //1grado
    ui->widget->graph(17)->addData(0,posicion); //2grados
    ui->widget->graph(18)->addData(0,posicion); //3grados
    ui->widget->graph(19)->addData(0,posicion); //4grados
    ui->widget->graph(20)->addData(0,posicion);
    ui->widget->graph(21)->addData(0,posicion);
    ui->widget->graph(22)->addData(0,posicion);
    ui->widget->graph(23)->addData(0,posicion);
    ui->widget->graph(24)->addData(0,posicion);
    ui->widget->graph(25)->addData(0,posicion);
    ui->widget->graph(34)->addData(0,posicion);
    ui->widget->graph(35)->addData(0,posicion);

    QList<CDatoTerreno*> *datosterreno = &calculos->DatosAzimut[aazimut].datosterreno;
    QList<CDatoAnguloCierre*> *datosAngulosCierre = &calculos->DatosAzimut[aazimut].datosAngulosCierre;

    altu = AltPos;
    distanciaCiclo = 0;
    distanciaAC = 0;

    //  if(calculos->medio == 40 /*|| calculos->medio == 50*/ ||(calculos->medio == 45 && calculos->tipoBiesta == "Rx"))
    //    {
    //      distanciaCiclo = determDistanCiclo(calculos->grupo, calculos->arma, calculos->medio)/90;
    ////      distanciaAC = datosAngulosCierre->last()->distancia;
    //    }
    //  else
    distanciaCiclo = determDistanCiclo(calculos->grupo, calculos->arma, calculos->medio)/90 + 25;

    for (int i = 0; i < datosAngulosCierre->length (); ++i)
    {
        if(datosAngulosCierre->at (i)->distancia > distanciaCiclo)
        {
            datosAngulosCierre->removeAt (i);
            i--;
        }

    }

    int ciclo = 5112;
    if(calculos->medio == 50)
        ciclo = 7777;
    for (int i=0; i < ciclo; ++i)
    {
        if(i < distanciaCiclo)
        {
            ui->widget->graph(0)->addData(d,altu); //se adiciona el dato calculado al trazo
            ui->widget->graph(1)->addData(d,curva);
            ui->widget->graph(3)->addData(d,(altu+25)); //Altura a 25 metros
            ui->widget->graph(4)->addData(d,(altu+50)); //Altura a 50 metros
            ui->widget->graph(5)->addData(d,(altu+100)); //Altura a 200 metros
            ui->widget->graph(6)->addData(d,(altu+200)); //Altura a 300 metros
            ui->widget->graph(7)->addData(d,(altu+300)); //Altura a 400 metros
            ui->widget->graph(8)->addData(d,(altu+400)); //Altura a 500 metros
            ui->widget->graph(9)->addData(d,(altu+500)); //Altura a 1000 metros
            ui->widget->graph(10)->addData(d,(altu+1000)); //Altura a 1000 metros(altu+1000)
            ui->widget->graph(12)->addData(0,punta); //Mastil
            punta=posicion;
            ui->widget->graph(12)->setName("Altura de la posición + mastil de Antena: "+QString::number(punta)+"m.");
            if(i < 3556 )
                altu = datosterreno->at(i)->altura;
            else
                altu = curva;


            //**********sin perfil*****************/

            ui->widget->graph(28)->addData(d,curva + 2000);
            ui->widget->graph(29)->addData(d,curva + 4000);
            ui->widget->graph(30)->addData(d,curva + 6000);
            ui->widget->graph(31)->addData(d,curva + 10000);
            ui->widget->graph(32)->addData(d,curva + 17000);
            ui->widget->graph(33)->addData(d,curva + 30000);
            //********//

            d+=90;

            curva= -(pow(d,2))/(17000000);

            if(AltVuelo < 1000)
            {
                AltVuelo+=20;
                k=1;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);

            }
            else if( AltVuelo < 1500)
            {
                AltVuelo+=500;
                if(calculos->medio != 50)
                    k=0.995;
                else
                    k=1;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 2000)
            {
                AltVuelo+=500;
                if(calculos->medio != 50)
                    k=0.98;
                else
                    k=1;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 2500)
            {
                AltVuelo+=500;
                if(calculos->medio != 50)
                    k=0.97;
                else
                    k=1;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 3000)
            {
                AltVuelo+=500;
                if(calculos->medio != 50)
                    k=0.96;
                else
                    k=1;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 4000)
            {
                AltVuelo+=1000;
                if(calculos->medio != 50)
                    k=0.95;
                else
                    k=1;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 5000)
            {
                AltVuelo+=1000;
                if(calculos->medio != 50)
                    k=0.93;
                else
                    k=1;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 6000)
            {
                AltVuelo+=1000;
                if(calculos->medio != 50)
                    k=0.9;
                else
                    k=0.99;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 8000)
            {
                AltVuelo+=2000;
                if(calculos->medio != 50)
                    k=0.85;
                else
                    k=0.965;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 10000)
            {
                AltVuelo+=2000;
                if(calculos->medio != 50)
                    k=0.8;
                else
                    k=0.94;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 12000)
            {
                AltVuelo+=2000;
                if(calculos->medio != 50)
                    k=0.76;
                else
                    k=0.93;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 14000)
            {
                AltVuelo+=2000;
                if(calculos->medio != 50)
                    k=0.73;
                else
                    k=0.92;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 17000)
            {
                AltVuelo+=3000;
                if(calculos->medio != 50)
                    k=0.7;
                else
                    k=0.91;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));

                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 20000)
            {
                AltVuelo+=3000;
                if(calculos->medio != 50)
                    k=0.68;
                else
                    k=0.907;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));
                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if(AltVuelo < 25000)
            {
                AltVuelo+=5000;
                if(calculos->medio != 50)
                    k=0.64;
                else
                    k=0.902;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));
                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            else if (AltVuelo < 30000)
            {
                AltVuelo+=5000;
                if(calculos->medio != 50)
                    k=0.6;
                else
                    k=0.9;
                Disvuelo=1000*(4.12*k*(sqrt(AltVuelo)+sqrt(posicion)));
                AlvueloCT=AltVuelo -(pow(Disvuelo,2))/(17000000);
            }
            ui->widget->graph(2)->addData(Disvuelo,AlvueloCT);
        }
    }
    double dato320 [15][7] = {{-5585.621,   0,            5585.6208,    11174.646,    16770.489,    22376.58,     27996.372},//0

                              {-4658.499,   927.12149,    6512.7423,    12101.768,    17697.611,    23303.701,    28923.493},//10min
                              {-2793.023,   2792.5977,    8378.2185,    13967.244,    19563.087,    25169.178,    30788.97002},//30min
                              {0,           5585.6208,    11171.242,    16760.267,    22376.58,     27962.201,    33581.993},//1
                              {5589.0255,   11174.646,    16760.267,    22349.292,    27945.136,    33551.226,    39171.019},//2
                              {11184.869,   16770.489,    22356.11,     27945.136,    33540.979,    39147.069,    44766.861},//3
                              {16790.959,   22376.58,     27962.201,    33551.226,    39147.069,    44753.16,     50372.952},//4

                              {22410.751,   27996.372,    33581.993,    39171.019,    44766.861,    50372.952,    55992.745},//5

                              {-6512.742,   -927.121,     4658.499,     10247.525,    15843.368,    21449.459,    22410.751},//-10min
                              {-8378.218,   -2792.597,    2793.023,     8381.049,     13977.892,    19583.983,    21449.459},//-30min
                              {-11174.646,  -5585.621,    0,            5885.621,     11174.646,    16770.489,    19583.983},//-1
                              {-16770.489,  -11174.646,   -5585.621,    0,            5585.621,     11174.646,    16770.489},//-2
                              {-22376.58,   -16770.489,   -11174.646,   -5585.621,    0,            5585.621,     11174.646},//-3
                              {-27962.201,  -22376.58,    -16770.489,   -11174.646,   -5585.621,    0,            5585.621},//-4

                              {-33633.355,  -27996.372,   -22376.57982, -16770.489,   -11174.646,   -5585.621,    0}};//-5

    double dato10 [15][7] = {{-174.5506,    0,            174.55065,    349.20769,    524.07779,699.26812,874.8866353},//0

                             {-145.5780534,   28.97254665,	203.5231966,	378.1802366,	553.0503366,	728.2406666,  903.8591819},//10min
                             {-87.28192209,   87.26867791,	261.8193279,	436.4763679,	611.3464679,	786.5367979,  962.1553132},//30min
                             {0,	            174.5506493,	349.20769,	524.07779,	699.26812,	874.8866353,  1051.0424},//1
                             {174.55065,	    349.2076949,	524.07779,	699.26812,	874.8866353,	1051.0424,    1227.845609},//2
                             {349.20769,	    524.0777928,	699.26812,	874.8866353,	1051.0424,	1227.845609,  1405.408347},//3
                             {524.07779,	    699.2681194,	874.8866353,	1051.042353,	1227.845609,	1405.408347,  1583.844403},//4
                             {699.26812,      874.8866353,	1051.042353,	1227.845609,	1405.408347,	1583.844403,  1763.269807},//5

                             {-203.5231466,   -28.97254665,	145.5781034,	320.2351434,	495.1052434,	670.2955734,  845.9140887},//-10min
                             {-261.8192779,   -87.26867791,	87.28197209,	261.9390121,	436.8091121,	611.9994421,  787.6179574},//-30min
                             {-349.2077,	    -174.5506493,	0,	        174.55065,	349.20769,	524.07779,    699.26812},//-1
                             {-524.0778,	    -349.2076949,	-174.5506,	0,	        174.55065,	349.20769,    524.07779},//-2
                             {-699.2681,	    -524.0777928,	-349.2077,	-174.5506,	0,	        174.55065,    349.20769},//-3
                             {-874.8866,	    -699.2681194,	-524.0778,	-349.2077,	-174.5506,	0,            174.55065},//-4
                             {-1051.042353,   -874.8866353,	-699.2681,	-524.0778,	-349.2077,	-174.5506,    0}};//-5

    double dato150 [15][7] = {{-2618.259739,  0,	                2618.259739,	5238.115424,	7861.166892,	10489.02179,13123.29953},

                              {-2183.67154,	434.5881997,	3052.847939,	5672.703624,	8295.755092,	10923.60999,   13557.88773},
                              {-1309.229571,	1309.030169,	3927.289908,	6547.145593,	9170.197061,	11798.05196,   14432.3297},
                              {0,                 2618.259739,	5238.115424,	7861.166892,	10489.02179,	13123.29953,   15765.635},
                              {2618.259739,	5238.115424,	7861.166892,	10489.02179,	13123.29953,	15765.635,     18417.68414},
                              {5238.115424,	7861.166892,	10489.02179,	13123.300,	15765.635,	18417.68414,   21081.12521},
                              {7861.166892,	10489.02179,	13123.29953,	15765.63529,	18417.68414,	21081.12521,   23757.66605},
                              {10489.02179,	13123.29953,	15765.63529,	18417.68414,	21081.12521,	23757.66605,   26449.04711},

                              {-3052.8479,	-434.5882,	2183.6715,	4803.5272,	7426.5787,	10054.4336,   12688.7113},
                              {-3927.289908,	-1309.030169,	1309.229571,	3929.085255,	6552.136723,	9179.991621,  11814.26936},
                              {-5238.115424,	-2618.259739,	0,	        2618.259739,	5238.115424,	7861.166892,  10489.02179},
                              {-7861.166892,	-5238.115424,	-2618.259739,	0,	        2618.259739,	5238.115424,  7861.166892},
                              {-10489.02179,	-7861.166892,	-5238.115424,	-2618.259739,	0,	        2618.259739,  5238.115424},//-3
                              {-13123.29953,	-10489.02179,	-7861.166892,	-5238.115424,	-2618.259739,	0,            2618.259739},
                              {-15765.63529,	-13123.29953,	-10489.02179,	-7861.166892,	-5238.115424,	-2618.259739,	0}};

    ui->widget->graph(13)->addData(320000,posicion + dato320[0][angulositio+1]);//0
    ui->widget->graph(14)->addData(320000,posicion + dato320[1][angulositio+1]);//10
    ui->widget->graph(15)->addData(320000,posicion + dato320[2][angulositio+1]);//30
    ui->widget->graph(16)->addData(320000,posicion + dato320[3][angulositio+1]);//1
    ui->widget->graph(17)->addData(320000,posicion + dato320[4][angulositio+1]);//2
    ui->widget->graph(18)->addData(320000,posicion + dato320[5][angulositio+1]);//3
    ui->widget->graph(19)->addData(320000,posicion + dato320[6][angulositio+1]);//4
    ui->widget->graph(34)->addData(320000,posicion + dato320[7][angulositio+1]);//5

    ui->widget->graph(20)->addData(320000,posicion + dato320[8][angulositio+1]);//-10
    ui->widget->graph(21)->addData(320000,posicion + dato320[9][angulositio+1]);//-30
    ui->widget->graph(22)->addData(320000,posicion + dato320[10][angulositio+1]);//-1
    ui->widget->graph(23)->addData(320000,posicion + dato320[11][angulositio+1]);//-2
    ui->widget->graph(24)->addData(320000,posicion + dato320[12][angulositio+1]);//-3
    ui->widget->graph(25)->addData(320000,posicion + dato320[13][angulositio+1]);//-4
    ui->widget->graph(35)->addData(320000,posicion + dato320[14][angulositio+1]);//-5

    if(calculos->medio == 48)
    {
        pintartexto("E.F",320000,posicion + dato320[0][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::magenta);
        pintartexto("E.F",10000,posicion + dato10[0][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::magenta);
        pintartexto("E.F",150000,posicion + dato150[0][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::magenta);
    }
    else
    {
        pintartexto("0°",10000,posicion + dato10[0][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
        pintartexto("0°",150000,posicion + dato150[0][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
        pintartexto("0°",320000,posicion + dato320[0][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    }
    pintartexto("10'",320000,posicion + dato320[1][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("30'",320000,posicion + dato320[2][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("1°",320000,posicion + dato320[3][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("2°",320000,posicion + dato320[4][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("3°",320000,posicion + dato320[5][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("4°",320000,posicion + dato320[6][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("5°",320000,posicion + dato320[7][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-10'",320000,posicion + dato320[8][angulositio+1],Qt::AlignTop|Qt::AlignCenter,Qt::black);
    pintartexto("-30'",320000,posicion + dato320[9][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-1°",320000,posicion + dato320[10][angulositio+1],Qt::AlignBottom|Qt::AlignLeft,Qt::black);
    pintartexto("-2°",320000,posicion + dato320[11][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-3°",320000,posicion + dato320[12][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-4°",320000,posicion + dato320[13][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-5°",320000,posicion + dato320[14][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);

    pintartexto("10'",10000,posicion + dato10[1][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("30'",10000,posicion + dato10[2][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("1°",10000,posicion + dato10[3][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("2°",10000,posicion + dato10[4][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("3°",10000,posicion + dato10[5][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("4°",10000,posicion + dato10[6][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("5°",10000,posicion + dato10[7][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-10'",10000,posicion + dato10[8][angulositio+1],Qt::AlignTop|Qt::AlignCenter,Qt::black);
    pintartexto("-30'",10000,posicion + dato10[9][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-1°",10000,posicion + dato10[10][angulositio+1],Qt::AlignBottom|Qt::AlignLeft,Qt::black);
    pintartexto("-2°",10000,posicion + dato10[11][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-3°",10000,posicion + dato10[12][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-4°",10000,posicion + dato10[13][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-5°",10000,posicion + dato10[14][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);

    pintartexto("10'",150000,posicion + dato150[1][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("30'",150000,posicion + dato150[2][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("1°",150000,posicion + dato150[3][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("2°",150000,posicion + dato150[4][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("3°",150000,posicion + dato150[5][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("4°",150000,posicion + dato150[6][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("5°",150000,posicion + dato150[7][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-10'",150000,posicion + dato150[8][angulositio+1],Qt::AlignTop|Qt::AlignCenter,Qt::black);
    pintartexto("-30'",150000,posicion + dato150[9][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-1°",150000,posicion + dato150[10][angulositio+1],Qt::AlignBottom|Qt::AlignLeft,Qt::black);
    pintartexto("-2°",150000,posicion + dato150[11][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-3°",150000,posicion + dato150[12][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-4°",150000,posicion + dato150[13][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);
    pintartexto("-5°",150000,posicion + dato150[14][angulositio+1],Qt::AlignTop|Qt::AlignLeft,Qt::black);

    ui->widget->graph(11)->addData(0,posicion); //recta de visibilidad
    if(datosAngulosCierre->length () >= 1)
    {
        for (auto i : *datosAngulosCierre)
        {
            phaseTracer = new QCPItemTracer(ui->widget);

            connect(phaseTracer,SIGNAL(selectionChanged(bool)),this,SLOT(seleccionAC(bool)));
            if(i->distancia < distanciaAC)
            {
                phaseTracer->setGraph(ui->widget->graph(0));
                phaseTracer->setSelectable(true);
                phaseTracer->setSelectedPen(QPen(Qt::blue));
                phaseTracer->setSelectedBrush(Qt::red);
                phaseTracer->setGraphKey(i->distancia);
                phaseTracer->setInterpolating(true);
                phaseTracer->setStyle(QCPItemTracer::tsCircle);
                phaseTracer->setPen(QPen(Qt::blue));
                phaseTracer->setBrush(Qt::blue);
                phaseTracer->setSize(7);
                QString message = QString("Ángulo de sitio a %1 metros de distancia.").arg(i->distancia);
                phaseTracer->setObjectName (message);
                listphaseTracer.append(phaseTracer);
                DisAngulocierre=i->tangente * d + posicion;
            }
        }
        ui->widget->graph(11)->addData(d,DisAngulocierre); //recta de visibilidad
        ui->widget->replot();
    }
    //  else
    //    {
    //      phaseTracer = new QCPItemTracer(ui->widget);
    //      connect(phaseTracer,SIGNAL(selectionChanged(bool)),this,SLOT(seleccionAC(bool)));
    //      qDebug()<<"ssssssssssssssssss";

    //      phaseTracer->setGraph(ui->widget->graph(0));
    //      phaseTracer->setSelectable(true);
    //      phaseTracer->setSelectedPen(QPen(Qt::blue));
    //      phaseTracer->setSelectedBrush(Qt::red);
    //      phaseTracer->setGraphKey(1.5);
    //      phaseTracer->setInterpolating(true);
    //      phaseTracer->setStyle(QCPItemTracer::tsCircle);
    //      phaseTracer->setPen(QPen(Qt::blue));
    //      phaseTracer->setBrush(Qt::blue);
    //      phaseTracer->setSize(7);

    //      double anC = -3440 * calculos->AlAnt/1.5;
    //      DisAngulocierre = anC/3440 *1.5 + posicion;

    //      QString message = QString("Ángulo de sitio a %1 metros de distancia.").arg(1.5);
    //      phaseTracer->setObjectName (message);
    //      listphaseTracer.append(phaseTracer);
    //      //      break;
    //      ui->widget->graph(11)->addData(1.5,DisAngulocierre); //recta de visibilidad
    //      ui->widget->replot();
    //    }

}

void Graficaperfil::pintartexto(const QString& t, double x, double y, Qt::Alignment alignment,const QColor& color)
{
    textgrado = new QCPItemText(ui->widget);
    textgrado->setText(t);
    textgrado->position->setCoords(x,y);
    textgrado->setPositionAlignment(alignment);
    textgrado->setFont(QFont(font().family(), 10,QFont::Bold));
    textgrado->setColor (color);
    textgrado->setLayer("angulositio");
    textgrado->setSelectable (false);
    Listtextgrado.append (textgrado);
}

void Graficaperfil::closeEvent(QCloseEvent *event)
{
    Q_UNUSED (event);
    ui->checkBox->setChecked (false);
}

int Graficaperfil::determDistanCiclo(int g, int a, int m)
{
    switch (g)
    {
    case 1:
        switch (a)
        {
        case 0: distanciaAC = 12060; break;
        case 1:
            switch (m)
            {
            case 11: distanciaAC = 20070; break;
            case 12: distanciaAC = 30060; break;
            case 13: distanciaAC = 40050; break;
            }
            break;
        case 2:
            switch (m)
            {
            case 21: distanciaAC = 8010; break;
            case 22: distanciaAC = 8010; break;
            case 23: distanciaAC = 8010; break;
            case 24: distanciaAC = 8010; break;
            case 25: distanciaAC = 8010; break;
            case 26: distanciaAC = 8010; break;
            case 27: distanciaAC = 8010; break;
            }
            break;
        case 3:
            switch (m)
            {

            case 31: distanciaAC = 8010; break;
            case 32: distanciaAC = 8010; break;
            case 33: distanciaAC = 8010; break;
            case 34: distanciaAC = 8010; break;
            case 35: distanciaAC = 8010; break;
            case 36: distanciaAC = 45000; break;
            case 37: distanciaAC = 65000; break;
            case 38: distanciaAC = 80010; break;
            case 39: distanciaAC = 150030; break;
            }
            break;
        case 4:
            switch (m)
            {
            case 40: distanciaAC = 320040; break;
            case 41: distanciaAC = 185040; break;
            case 42: distanciaAC = 60030; break;
            case 43: distanciaAC = 18000; break;
            case 44: distanciaAC = 22050; break;

            case 45: distanciaAC = 120060; break;
            case 46: distanciaAC = 30060; break;
            case 47: distanciaAC = 60030; break;

            case 48: distanciaAC = 185040; break;
            case 49: distanciaAC = 185040; break;
            case 50: distanciaAC = 700020; break;
            }
            break;
        }
        break;
    case 2:
        switch (m)
        {
        case 21: distanciaAC = 1530; break;
        case 22: distanciaAC = 2520; break;
        case 23: distanciaAC = 2520; break;
        case 44: distanciaAC = 2520; break;
        case 24: distanciaAC = 3060; break;
        case 25: distanciaAC = 4050; break;
        case 26: distanciaAC = 6030; break;
        case 43: distanciaAC = 6030; break;
        case 27: distanciaAC = 12060; break;
        case 42: distanciaAC = 12060; break;
        }
        break;
    case 3:
        switch (m)
        {
        case 31: distanciaAC = 4050; break;
        case 32: distanciaAC = 2070; break;
        case 33: distanciaAC = 2520; break;
        case 34: distanciaAC = 3060; break;
        case 35: distanciaAC = 5040; break;
        case 36: distanciaAC = 10080; break;
        case 37: distanciaAC = 20070; break;
        case 38: distanciaAC = 17000; break;
        case 39: distanciaAC = 42840; break;
        }
        break;
    }
    return distanciaAC;
}

void Graficaperfil::mouseWheel(QWheelEvent *event)
{
    Q_UNUSED (event);
    if (ui->widget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->widget->axisRect()->setRangeZoom(ui->widget->xAxis->orientation());
    else if (ui->widget->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->widget->axisRect()->setRangeZoom(ui->widget->yAxis->orientation());
    else
        ui->widget->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void Graficaperfil::mouseMove(QMouseEvent *event)
{
    x=ui->widget->xAxis->pixelToCoord(event->pos().x());
    y=ui->widget->yAxis->pixelToCoord(event->pos().y());
    QString punto="D:"+QString::number(x)+"m; H:"+QString::number(y)+"m";
    ui->widget->setToolTip(punto);
}

void Graficaperfil::on_cheB_AngulosDeSitio_toggled(bool checked)
{  
    ui->widget->layer("angulositio")->setVisible(checked);
    //    ui->widget->layer("angulositio")->removeChild ();
    ui->widget->layer("angulositio")->replot();
}

void Graficaperfil::on_cheB_leyenda_toggled(bool checked)
{
    ui->widget->legend->setVisible(checked);

    ui->widget->replot();
}

void Graficaperfil::on_pB_Zoommas_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommas(ui->widget);
}

void Graficaperfil::on_pB_Zoommen_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommenos(ui->widget);
}

void Graficaperfil::filePrintPdf()
{
    QString nom;
    QString aux;
    aux = calculos->fichero;
    aux.replace(":", "" );
    nom=(QDir::currentPath()+"/GraficasPDF/"+aux+"_az_"+QString::number(aazimut)+"_per");
    CustomPlotF customplotf;
    customplotf.filePrintPdf(ui->widget,nom);
}

void Graficaperfil::on_pushButton_4_clicked()
{
    filePrintPdf();
}

void Graficaperfil::seleccionAC(bool cambio)//dibujo la recta de visibilidad auxiliar
{
    QList<CDatoAnguloCierre*> *datosAngulosCierre = &calculos->DatosAzimut[aazimut].datosAngulosCierre;
    double    DisAngulocierre;
    QVector<double> x;
    QVector<double> y;
    x.append(0);
    y.append(posicion);
    if(cambio)
    {
        for (int i = 0; i < listphaseTracer.length()-1; ++i)
        {
            if(listphaseTracer.at(i)->selected())
            {
                x.append(listphaseTracer.at(i)->position->key());
                y.append(listphaseTracer.at(i)->position->value());

                DisAngulocierre=datosAngulosCierre->at(i)->tangente *listphaseTracer.at(i+1)->graphKey() + posicion;

                x.append(listphaseTracer.at(i+1)->graphKey());
                y.append(DisAngulocierre);
                ui->widget->graph(27)->setData(x,y);
                ui->widget->layer ("Recta Auxiliar")->setVisible (true);
                break;
            }
        }
    }
}

void Graficaperfil::selectionChanged()
{
    if(ui->widget->graph (27)->selected ())
        ui->widget->layer ("Recta Auxiliar")->setVisible (true);
    else
        ui->widget->layer ("Recta Auxiliar")->setVisible (false);

}

void Graficaperfil::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
    Q_UNUSED (dataIndex);
    QString message = QString("A seleccionado la curva '%1'.").arg(plottable->name())/*.arg(dataIndex).arg(dataValue)*/;
    plottitle2->setText (message);
    QCPPlottableLegendItem *item = ui->widget->legend->itemWithPlottable(plottable);
    item->setSelected(true);
}

void Graficaperfil::itemClicked(QCPAbstractItem *item, QMouseEvent *event)
{
    Q_UNUSED(event);
    QString message = item->objectName ();
    plottitle2->setText (message);
    for (auto i : listphaseTracer)
    {
        if(i->objectName () == item->objectName ())
        {
            ui->widget->legend->item (26)->setSelected (true);
            ui->widget->layer ("Recta Auxiliar")->setVisible (true);
        }
    }

    for (auto listphaseTracerDi : listphaseTracerDis)
    {
        if(listphaseTracerDi->objectName () == item->objectName ())
        {
            ui->widget->legend->item (36)->setSelected (true);
            ui->widget->layer ("Recta Auxiliar")->setVisible (true);
        }
    }
}

void Graficaperfil::legendItemClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event)
{
    Q_UNUSED(legend);
    Q_UNUSED(event);
    for (int i=0; i < ui->widget->graphCount(); ++i)//seleccionar grafica
    {
        QCPGraph *graph = ui->widget->graph(i);
        if(legendItem == ui->widget->legend->itemWithPlottable(graph))
        {
            graph->setSelection (QCPDataSelection(graph->data()->dataRange()));
            if(legendItem == ui->widget->legend->item (27))
                ui->widget->layer ("Recta Auxiliar")->setVisible (true);
            else
                ui->widget->layer ("Recta Auxiliar")->setVisible (false);
            break;
        }
    }

    if(legendItem == ui->widget->legend->item (26))
        listphaseTracer.last ()->setSelected (true);

    if(legendItem == ui->widget->legend->item (36))
    {
        if(!listphaseTracerDis.isEmpty ())
            for (auto listphaseTracerDi : listphaseTracerDis) {
                listphaseTracerDi->setSelected (true);
            }
    }
}

void Graficaperfil::widgetClik()
{
    for (int i = 0; i < ui->widget->plottableCount (); ++i)
    {
        auto *graph = dynamic_cast<QCPGraph*>(ui->widget->plottable (i));
        if(graph->selected ())
            plottitle2->setText (graph->name ());
        else
            plottitle2->setText ("");
    }
}

void Graficaperfil::on_checkBox_toggled(bool checked)
{
    if(checked)
    {
        //      bool existe = false;
        if(leer (".m00",0))
        {
            //          existe = true;
        }
        if(leer (".m25",1))
        {
            //          existe = true;
        }
        if(leer (".m50",2))
        {
            //          existe = true;
        }
        if(leer (".001",3))
        {
            //          existe = true;
        }
        if(leer (".002",4))
        {
            //          existe = true;
        }
        if(leer (".003",5))
        {
            //          existe = true;
        }
        if(leer (".004",6))
        {
            //          existe = true;
        }
        if(leer (".005",7))
        {
            //          existe = true;
        }
        if(leer (".010",8))
        {
            //          existe = true;
        }
        if(leer (".020",9))
        {
            //          existe = true;
        }
        if(leer (".040",10))
        {
            //          existe = true;
        }
        if(leer (".060",11))
        {
            //          existe = true;
        }
        if(leer (".100",12))
        {
            //          existe = true;
        }
        if(leer (".170",13))
        {
            //          existe = true;
        }
        if(leer (".300",14))
        {
            //          existe = true;
        }
    }
    else
    {
        //borrar
        for (auto & i : disVDirecta2)
        {
            i.clear ();
        }
    }
    ui->widget->layer("Dismax")->setVisible(checked);
    ui->widget->layer("Dismax")->replot();
}

bool Graficaperfil::leer(const QString& extencion, int curva)
{
    for (auto & i : disVDirecta)
    {
        i.clear();
    }

    bool entra=false;
    bool listo = false;
    QDir temp=QDir(QDir::currentPath() + "/Zonas");//direccion de los ficheros de coordenadas geograficas
    QFileInfoList Fich= temp.entryInfoList();

    foreach (const QFileInfo info, Fich)
    {
        QString nombfich= info.fileName();
        if (calculos->fichero+extencion==nombfich)
        {
            entra=true;
            break;
        }
    }
    if(entra)
    {
        QStringList todo;
        QFile file(QDir::currentPath() + "/Zonas/"+calculos->fichero+extencion);
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
            for (int i = 0;i < 360; ++i)
            {
                QStringList grande=todo.at(i).split("*");

                QStringList visibilidad =grande.at(1).split(",");

                disVDirecta[i].append(visibilidad.at(0).toDouble());

            }
            for (auto & j : disVDirecta)
            {
                disVDirecta2[curva].append (j.at (0));
            }

            ////////////////////////////////
            int graf = 0;
            switch (curva) {
            case 0:         graf = 0;        break;
            case 1:         graf = 3;        break;
            case 2:         graf = 4;        break;
            case 3:         graf = 5;        break;
            case 4:         graf = 6;        break;
            case 5:         graf = 7;        break;
            case 6:         graf = 8;        break;
            case 7:         graf = 9;        break;
            case 8:         graf = 10;       break;
            case 9:         graf = 28;       break;
            case 10:        graf = 29;       break;
            case 11:        graf = 30;       break;
            case 12:        graf = 31;       break;
            case 13:        graf = 32;       break;
            case 14:        graf = 33;       break;
            default:
                break;
            }

            auto *phaseTracerDist = new QCPItemTracer(ui->widget);

            phaseTracerDist->setGraph(ui->widget->graph(graf));
            phaseTracerDist->setSelectable(true);
            phaseTracerDist->setSelectedPen(QPen(Qt::red));
            phaseTracerDist->setSelectedBrush(Qt::red);
            phaseTracerDist->setLayer ("Dismax");
            phaseTracerDist->setInterpolating(true);
            phaseTracerDist->setStyle(QCPItemTracer::tsCircle);
            phaseTracerDist->setPen(QPen(Qt::blue));
            phaseTracerDist->setBrush(Qt::black);
            phaseTracerDist->setSize(3);

            phaseTracerDist->setGraphKey(disVDirecta[aazimut].at (0));

            //          phaseTracerDist->setObjectName (QString::number (graf));
            QString message = QString("DVDP a %1 metros de distancia.").arg(disVDirecta2[curva].at (aazimut));
            phaseTracerDist->setObjectName (message);
            listphaseTracerDis.append (phaseTracerDist);
            ///***************
        }
        else
        {
            listo = false;
            file.remove();
        }
    }
    return listo;
}

