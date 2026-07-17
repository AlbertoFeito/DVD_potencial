#include "c_perfizc.h"
#include "ui_c_perfizc.h"
#include <QDebug>
C_PerfiZC::C_PerfiZC(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::C_PerfiZC)
{
  ui->setupUi(this);
//  setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
  ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                              QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectItems);
  //ui->widget->setMinimumSize(700,700);
  ui->widget->plotLayout()->insertRow(0);
  //	plottitle = new QCPPlotTitle(ui->widget);
  plottitle = new QCPTextElement(ui->widget);
  ui->widget->plotLayout()->addElement(0, 0,plottitle );
  plottitle2 = new QCPTextElement(ui->widget);
  ui->widget->plotLayout()->addElement(2, 0,plottitle2 );

  ui->widget->addLayer("angulositio",nullptr,QCustomPlot::limAbove);
  ui->widget->layer("angulositio")->setVisible(false);

  connect(ui->widget, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel(QWheelEvent*)));
  connect(ui->widget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));

  connect(ui->widget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
  connect(ui->widget, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
  connect(ui->widget, SIGNAL(itemClick(QCPAbstractItem*,QMouseEvent*)), this, SLOT(itemClicked(QCPAbstractItem*,QMouseEvent*)));
  connect(ui->widget, SIGNAL(legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendItemClicked(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)));


  DistanciaSelected = 0;

  Disvuelo = 0;
}

C_PerfiZC::~C_PerfiZC()
{
  delete ui;
}

void C_PerfiZC::initGraph()
{
  ui->widget->addLayer("Distancias",nullptr,QCustomPlot::limBelow);
  ui->widget->layer("Distancias")->setVisible(false);

  ui->widget->addLayer("Recta Auxiliar",nullptr,QCustomPlot::limBelow);
  ui->widget->layer("Recta Auxiliar")->setVisible(false);

  //    ui->label->setText(fichero);
  ui->widget->addGraph(); //perfil del terreno
  ui->widget->graph(0)->setPen(QColor(0,100,0));
  ui->widget->graph(0)->setBrush(QBrush(QColor(0,100,0,50)));

  ui->widget->addGraph();//curvatura de la tierra
  ui->widget->graph(1)->setPen(QColor(100,0,0)); //color del trazo
  ui->widget->graph(0)->setChannelFillGraph(ui->widget->graph(1));

  ui->widget->addGraph();//mastil
  ui->widget->graph(2)->setPen(QColor(0,0,255)); //mastil

  ui->widget->addGraph();//base hasta dismax horizontal
  ui->widget->graph(3)->setPen(QColor(0,0,0));

  ui->widget->addGraph();//recta que pasa por las distancias
  ui->widget->graph(4)->setPen(QColor(255,0,0)); //
  ui->widget->graph(4)->setLayer ("Recta Auxiliar"); //


  //***********angulos de sitio *************//
  ui->widget->addGraph();
  ui->widget->graph(5)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(5)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(6)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(6)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(7)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(7)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(8)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(8)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(9)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(9)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(10)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(10)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(11)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(11)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(12)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(12)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(13)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(13)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(14)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(14)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(15)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(15)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(16)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(16)->setLayer ("angulositio");

  ui->widget->addGraph();
  ui->widget->graph(17)->setPen(QPen(Qt::black,1,Qt::DashLine));
  ui->widget->graph(17)->setLayer ("angulositio");
  /**************************************************************/
  ui->widget->addGraph();
  ui->widget->graph(18)->setPen(QPen(Qt::yellow));
  ui->widget->graph(18)->setLineStyle(QCPGraph::lsNone);
  ui->widget->graph(18)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));
  ui->widget->graph(18)->setName("Plazoleta en el terreno");

  ui->widget->addGraph();
  ui->widget->graph(19)->setPen(QPen(Qt::green));
  ui->widget->graph(19)->setLineStyle(QCPGraph::lsNone);
  ui->widget->graph(19)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));
  ui->widget->graph(19)->setName("Dimensiones teóricas de la plazoleta");

  ui->widget->addGraph();
  ui->widget->graph(20)->setPen(QPen(Qt::blue));
  ui->widget->graph(20)->setLineStyle(QCPGraph::lsNone);
  ui->widget->graph(20)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));
  ui->widget->graph(20)->setName("Obstáculos en el terreno");


  ui->widget->addGraph();//recta que pasa por las distancias
  ui->widget->graph(21)->setPen(QColor(255,0,0)); //
  ui->widget->graph(21)->setLayer ("Distancias"); //
  /***************************************************************/

  ui->widget->xAxis->setLabel("Distancia (m)");//etiqueta para las abscisas
  ui->widget->yAxis->setLabel("Altura (m)");//etiqueta para las abscisas
  ui->widget->yAxis->setRange(-100,1000);//se estabelece rango de y

  ui->widget->xAxis->setRange(-20,30000/*distancia*1000*/);

  QPen pen;
  pen.setWidth(2);
  pen.setColor(Qt::black);
  ui->widget->xAxis->setBasePen(pen);
  ui->widget->yAxis->setBasePen(pen);

  ui->widget->graph(0)->setName("Perfil de Alturas Azimut: "+QString::number(aazimut)+"°");
  ui->widget->graph(1)->setName("Curvatura de la tierra");
  ui->widget->graph(2)->setName("Altura de la posición + mastil de Antena: "+QString::number(posicion)+"m.");
  ui->widget->graph(3)->setName("Horizontal desde la posición hasta la distancia máxima");
  ui->widget->graph(4)->setName("Recta que une los puntos de la plazoleta con proyección en el terreno");
  ui->widget->graph(21)->setName("Recta de visibilidad auxiliar");
  ui->widget->graph(5)->setName("0°");
  ui->widget->graph(6)->setName("10'");
  ui->widget->graph(7)->setName("30'");
  ui->widget->graph(8)->setName("1°");
  ui->widget->graph(9)->setName("2°");
  ui->widget->graph(10)->setName("3°");
  ui->widget->graph(11)->setName("4°");

  ui->widget->graph(12)->setName("-10'");
  ui->widget->graph(13)->setName("-30'");
  ui->widget->graph(14)->setName("-1°");
  ui->widget->graph(15)->setName("-2°");
  ui->widget->graph(16)->setName("-3°");
  ui->widget->graph(17)->setName("-4°");
}

void C_PerfiZC::pintar()
{
  if(anguloINCidencia !=0)
    ui->label->setText("Ángulo de Incidencia: "+QString::number(anguloINCidencia));
  else
    ui->label->setText("No hay Ángulo de Incidencia.");
  // set title of plot:
  if(banda == 0)
    plottitle->setText("Perfil Zona Cercana-Banda Métrica-Az: "+QString::number(aazimut)+"°-Posición: "+/*fichero*/calculos->label);
  else
    plottitle->setText("Perfil Zona Cercana-Banda Decimétrica-Az: "+QString::number(aazimut)+"°-Posición: "+/*fichero*/calculos->label);

  ui->widget->clearGraphs();
  for (auto i : listpTracerListaDisPZ)
    {
      ui->widget->removeItem(i);
    }
  listpTracerListaDisPZ.clear();

  for (auto i : listphaseTracer)
    {
      ui->widget->removeItem(i);
    }
  listphaseTracer.clear();

  for (auto listpTracerIntercepto : listpTracerInterceptos)
    {
      ui->widget->removeItem(listpTracerIntercepto);
    }
  listpTracerInterceptos.clear();

  for (auto i : Listtextgrado)
    {
      ui->widget->removeItem (i);
    }
  Listtextgrado.clear ();
  ui->widget->clearItems ();
  initGraph();

  double d=0.0;//distancia de 90++
  double curva = 0.0;//curvatura de la tierra
  double altu = 0.0;

  QList<CDatoTerreno*> *datosterreno = &calculos->DatosAzimut[aazimut].datosterreno;
  QList<CDatoAnguloCierre*> *datosAngulosCierre = &calculos->DatosAzimut[aazimut].datosAngulosCierre;

  altu = AltPos;
  ui->widget->graph(2)->addData(0,AltPos);
  ui->widget->graph(2)->addData(0,posicion);
  ui->widget->graph(3)->addData(0,AltPos);
  ui->widget->graph(3)->addData(30060,AltPos);

  for (int i=0; i <= /*Disvuelo/90*/30060/90; ++i)
    {
      ui->widget->graph(0)->addData(d,altu); //se adiciona el dato calculado al trazo
      ui->widget->graph(1)->addData(d,curva);
      d+=90;
      altu = datosterreno->at(i)->altura;
      curva= -(pow(d,2))/(17000000);
    }

  for (double i : listdist)//distancias min 1/4 1/2 3/4 dismax
    {
      pTracerInterceptos = new QCPItemTracer(ui->widget);

      pTracerInterceptos->setGraph(ui->widget->graph(0));
      pTracerInterceptos->setSelectable(true);
      pTracerInterceptos->setSelectedPen(QPen(Qt::yellow));
      pTracerInterceptos->setSelectedBrush(Qt::red);
      pTracerInterceptos->setGraphKey(i);
      pTracerInterceptos->setInterpolating(true);
      pTracerInterceptos->setStyle(QCPItemTracer::tsCircle);
      pTracerInterceptos->setPen(QPen(Qt::yellow));
      pTracerInterceptos->setBrush(Qt::yellow);
      pTracerInterceptos->setSize(7);
      pTracerInterceptos->setObjectName (QString("Plazoleta en el terreno, distancia: %1 metros.").arg(i));
      listpTracerInterceptos.append(pTracerInterceptos);
    }
  //************
  for (double i : distanciasPlazoleta)//distancias min 1/4 1/2 3/4 dismax
    {

      pTracerListaDisPZ = new QCPItemTracer(ui->widget);

      connect(pTracerListaDisPZ,SIGNAL(selectionChanged(bool)),this,SLOT(seleccionDis(bool)));

      pTracerListaDisPZ->setGraph(ui->widget->graph(3));
      pTracerListaDisPZ->setSelectable(true);
      pTracerListaDisPZ->setSelectedPen(QPen(Qt::green));
      pTracerListaDisPZ->setSelectedBrush(Qt::red);
      pTracerListaDisPZ->setGraphKey(i);
      pTracerListaDisPZ->setInterpolating(true);
      pTracerListaDisPZ->setStyle(QCPItemTracer::tsCircle);
      pTracerListaDisPZ->setPen(QPen(Qt::green));
      pTracerListaDisPZ->setBrush(Qt::green);
      pTracerListaDisPZ->setSize(7);
      QString obName= QString("Dimensiones teóricas de la plazoleta, distancia: %1 metros.").arg(i);
      pTracerListaDisPZ->setObjectName (obName);
      listpTracerListaDisPZ.append(pTracerListaDisPZ);
      //		ui->widget->addItem(listpTracerListaDisPZ.at(i));
    }
  for (auto i : *datosAngulosCierre)//pintar ac
    {
      if(i->distancia <= 30060)
        {
          phaseTracer = new QCPItemTracer(ui->widget);

          connect(phaseTracer,SIGNAL(selectionChanged(bool)),this,SLOT(seleccionAC(bool)));

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
          phaseTracer->setObjectName (QString("Obstáculo del terreno, distancia: %1 metros.").arg (i->distancia));
          listphaseTracer.append(phaseTracer);
          //			ui->widget->addItem(listphaseTracer.at(i));
        }
      else
        break;

    }

  /**angulos de sitio**/
  ui->widget->graph(5)->addData(0,posicion); //0min
  //  text0min->setText("0°");
  //  text0min->position->setCoords(10000,posicion);
  //  text0min->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  text0min->setFont(QFont(font().family(), 10));

  ui->widget->graph(6)->addData(0,posicion); //10min
  //  text10min->setText("10'");
  //  text10min->position->setCoords(10000,posicion + 37.08);
  //  text10min->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  text10min->setFont(QFont(font().family(), 10));

  ui->widget->graph(7)->addData(0,posicion); //30min
  //  text30min->setText("30'");
  //  text30min->position->setCoords(10000,posicion + 95.2);
  //  text30min->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  text30min->setFont(QFont(font().family(), 10));

  ui->widget->graph(8)->addData(0,posicion); //1grado
  //  text1grado->setText("1°");
  //  text1grado->position->setCoords(10000,posicion + 182.50);
  //  text1grado->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  text1grado->setFont(QFont(font().family(), 10));

  ui->widget->graph(9)->addData(0,posicion); //2grados
  //  text2grado->setText("2°");
  //  text2grado->position->setCoords(10000,posicion + 356.95);
  //  text2grado->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  text2grado->setFont(QFont(font().family(), 10));

  ui->widget->graph(10)->addData(0,posicion); //3grados
  //  text3grado->setText("3°");
  //  text3grado->position->setCoords(10000,posicion + 531.33);
  //  text3grado->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  text3grado->setFont(QFont(font().family(), 10));

  ui->widget->graph(11)->addData(0,posicion); //4grados
  //  text4grado->setText("4°");
  //  text4grado->position->setCoords(10000,posicion + 705.52);
  //  text4grado->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  text4grado->setFont(QFont(font().family(), 10));


  ui->widget->graph(12)->addData(0,posicion);
  //  textNeg10min->setText("-10'");
  //  textNeg10min->position->setCoords(10000,posicion - 37.08);
  //  textNeg10min->setPositionAlignment(Qt::AlignTop|Qt::AlignCenter);
  //  textNeg10min->setFont(QFont(font().family(), 10));

  ui->widget->graph(13)->addData(0,posicion);
  //  textNeg30min->setText("-30'");
  //  textNeg30min->position->setCoords(10000,posicion - 95.2);
  //  textNeg30min->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  textNeg30min->setFont(QFont(font().family(), 10));

  ui->widget->graph(14)->addData(0,posicion);
  //  textNeg1grado->setText("-1°");
  //  textNeg1grado->position->setCoords(10000,posicion - 182.50);
  //  textNeg1grado->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
  //  textNeg1grado->setFont(QFont(font().family(), 10));

  ui->widget->graph(15)->addData(0,posicion);
  //  textNeg2grado->setText("-2°.");
  //  textNeg2grado->position->setCoords(10000,posicion - 356.95);
  //  textNeg2grado->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  textNeg2grado->setFont(QFont(font().family(), 10));

  ui->widget->graph(16)->addData(0,posicion);
  //  textNeg3grado->setText("-3°.");
  //  textNeg3grado->position->setCoords(10000,posicion - 531.33);
  //  textNeg3grado->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  textNeg3grado->setFont(QFont(font().family(), 10));

  ui->widget->graph(17)->addData(0,posicion);
  //  textNeg4grado->setText("-4°");
  //  textNeg4grado->position->setCoords(10000,posicion - 705.52);
  //  textNeg4grado->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
  //  textNeg4grado->setFont(QFont(font().family(), 10));


  double dato320 [13][6] = {{-5585.621,0,5585.6208,11174.646,16770.489,22376.58},
                            {-4658.499,927.12149,6512.7423,12101.768,17697.611,23303.701},
                            {-2793.023,2792.5977,8378.2185,13967.244,19563.087,25169.178},
                            {0,5585.6208,11171.242,16760.267,22376.58,27962.201},
                            {5589.0255,11174.646,16760.267,22349.292,27945.136,33551.226},
                            {11184.869,16770.489,22356.11,27945.136,33540.979,39147.069},
                            {16790.959,22376.58,27962.201,33551.226,39147.069,44753.16},
                            {-6512.742,-927.121,4658.499,10247.525,15843.368,21449.459},
                            {-8378.218,-2792.597,2793.023,8381.049,13977.892,19583.983},
                            {-11174.646,-5585.621,0,5885.621,11174.646,16770.489},
                            {-16770.489,-11174.646,-5585.621,0,5585.621,11174.646},
                            {-22376.58,-16770.489,-11174.646,-5585.621,0,5585.621},
                            {-27962.201,-22376.58,-16770.489,-11174.646,-5585.621,0}};

  double dato10 [13][6] = {{-174.5506,0,174.55065,349.20769,524.07779,699.26812},
                           {-145.5780534,	28.97254665,	203.5231966,	378.1802366,	553.0503366,	728.2406666},
                           {-87.28192209,	87.26867791,	261.8193279,	436.4763679,	611.3464679,	786.5367979},
                           {0,	174.5506493,	349.20769,	524.07779,	699.26812,	874.8866353},
                           {174.55065,	349.2076949,	524.07779,	699.26812,	874.8866353,	1051.0424},
                           {349.20769,	524.0777928,	699.26812,	874.8866353,	1051.0424,	1227.845609},
                           {524.07779,	699.2681194,	874.8866353,	1051.042353,	1227.845609,	1405.408347},
                           {-203.5231466,	-28.97254665,	145.5781034,	320.2351434,	495.1052434,	670.2955734},
                           {-261.8192779,	-87.26867791,	87.28197209,	261.9390121,	436.8091121,	611.9994421},
                           {-349.2077,	-174.5506493,	0,	174.55065,	349.20769,	524.07779},
                           {-524.0778,	-349.2076949,	-174.5506,	0,	174.55065,	349.20769},
                           {-699.2681,	-524.0777928,	-349.2077,	-174.5506,	0,	174.55065},
                           {-874.8866,	-699.2681194,	-524.0778,	-349.2077,	-174.5506,	0}};

  double dato150 [13][6] = {{-2618.259739,	0,	2618.259739,	5238.115424,	7861.166892,	10489.02179},
                            {-2183.67154,	434.5881997,	3052.847939,	5672.703624,	8295.755092,	10923.60999},
                            {-1309.229571,	1309.030169,	3927.289908,	6547.145593,	9170.197061,	11798.05196},
                            {0,	2618.259739,	5238.115424,	7861.166892,	10489.02179,	13123.29953},
                            {2618.259739,	5238.115424,	7861.166892,	10489.02179,	13123.29953,	15765.635},
                            {5238.115424,	7861.166892,	10489.02179,	13123.300,	15765.635,	18417.68414},
                            {7861.166892,	10489.02179,	13123.29953,	15765.63529,	18417.68414,	21081.12521},
                            {-3052.8479,	-434.5882,	2183.6715,	4803.5272,	7426.5787,	10054.4336},
                            {-3927.289908,	-1309.030169,	1309.229571,	3929.085255,	6552.136723,	9179.991621},
                            {-5238.115424,	-2618.259739,	0,	2618.259739,	5238.115424,	7861.166892},
                            {-7861.166892,	-5238.115424,	-2618.259739,	0,	2618.259739,	5238.115424},
                            {-10489.02179,	-7861.166892,	-5238.115424,	-2618.259739,	0,	2618.259739},
                            {-13123.29953,	-10489.02179,	-7861.166892,	-5238.115424,	-2618.259739,	0}};



  ui->widget->graph(5)->addData(320000,posicion + dato320[0][1]);//0
  ui->widget->graph(6)->addData(320000,posicion + dato320[1][1]);//10
  ui->widget->graph(7)->addData(320000,posicion + dato320[2][1]);//30
  ui->widget->graph(8)->addData(320000,posicion + dato320[3][1]);//1
  ui->widget->graph(9)->addData(320000,posicion + dato320[4][1]);//2
  ui->widget->graph(10)->addData(320000,posicion + dato320[5][1]);//3
  ui->widget->graph(11)->addData(320000,posicion + dato320[6][1]);//4

  ui->widget->graph(12)->addData(320000,posicion + dato320[7][1]);//-10
  ui->widget->graph(13)->addData(320000,posicion + dato320[8][1]);//-30
  ui->widget->graph(14)->addData(320000,posicion + dato320[9][1]);//-1
  ui->widget->graph(15)->addData(320000,posicion + dato320[10][1]);//-2
  ui->widget->graph(16)->addData(320000,posicion + dato320[11][1]);//-3
  ui->widget->graph(17)->addData(320000,posicion + dato320[12][1]);//-4


  pintartexto("0°",10000,posicion + dato10[0][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("0°",150000,posicion + dato150[0][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("0°",320000,posicion + dato320[0][1],Qt::AlignTop|Qt::AlignLeft);

  pintartexto("10'",320000,posicion + dato320[1][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("30'",320000,posicion + dato320[2][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("1°",320000,posicion + dato320[3][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("2°",320000,posicion + dato320[4][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("3°",320000,posicion + dato320[5][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("4°",320000,posicion + dato320[6][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-10'",320000,posicion + dato320[7][1],Qt::AlignTop|Qt::AlignCenter);
  pintartexto("-30'",320000,posicion + dato320[8][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-1°",320000,posicion + dato320[9][1],Qt::AlignBottom|Qt::AlignLeft);
  pintartexto("-2°",320000,posicion + dato320[10][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-3°",320000,posicion + dato320[11][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-4°",320000,posicion + dato320[12][1],Qt::AlignTop|Qt::AlignLeft);

  pintartexto("10'",10000,posicion + dato10[1][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("30'",10000,posicion + dato10[2][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("1°",10000,posicion + dato10[3][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("2°",10000,posicion + dato10[4][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("3°",10000,posicion + dato10[5][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("4°",10000,posicion + dato10[6][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-10'",10000,posicion + dato10[7][1],Qt::AlignTop|Qt::AlignCenter);
  pintartexto("-30'",10000,posicion + dato10[8][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-1°",10000,posicion + dato10[9][1],Qt::AlignBottom|Qt::AlignLeft);
  pintartexto("-2°",10000,posicion + dato10[10][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-3°",10000,posicion + dato10[11][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-4°",10000,posicion + dato10[12][1],Qt::AlignTop|Qt::AlignLeft);

  pintartexto("10'",150000,posicion + dato150[1][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("30'",150000,posicion + dato150[2][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("1°",150000,posicion + dato150[3][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("2°",150000,posicion + dato150[4][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("3°",150000,posicion + dato150[5][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("4°",150000,posicion + dato150[6][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-10'",150000,posicion + dato150[7][1],Qt::AlignTop|Qt::AlignCenter);
  pintartexto("-30'",150000,posicion + dato150[8][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-1°",150000,posicion + dato150[9][1],Qt::AlignBottom|Qt::AlignLeft);
  pintartexto("-2°",150000,posicion + dato150[10][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-3°",150000,posicion + dato150[11][1],Qt::AlignTop|Qt::AlignLeft);
  pintartexto("-4°",150000,posicion + dato150[12][1],Qt::AlignTop|Qt::AlignLeft);



  /********************/
  ui->widget->replot();
}
void C_PerfiZC::pintartexto(const QString& t, double x, double y, Qt::Alignment alignment)
{
  textgrado = new QCPItemText(ui->widget);
  textgrado->setText(t);
  textgrado->position->setCoords(x,y);
  textgrado->setPositionAlignment(alignment);
  textgrado->setFont(QFont(font().family(), 10));
  textgrado->setLayer("angulositio");
  Listtextgrado.append (textgrado);
}
void C_PerfiZC::seleccionDis(bool cambio)
{
  double m = 0;
  QVector<double> x;
  QVector<double> y;
  x.append(0);
  y.append(posicion);
  if(cambio)
    {
      ui->widget->graph(4)->data().clear();

      for (int i = 0; i < listpTracerInterceptos.length(); ++i)
        {
          if(listpTracerListaDisPZ.at(i)->selected())
            {
              x.append(listpTracerInterceptos.at(i)->position->key());
              y.append(listpTracerInterceptos.at(i)->position->value());
              m = (AltPos - posicion)/(distanciasPlazoleta.at(i));
              DistanciaSelected = m * listpTracerInterceptos.at(i)->position->key() + posicion;
              x.append(listpTracerInterceptos.at(i)->graphKey());
              y.append(DistanciaSelected);
              ui->widget->graph(4)->setData(x,y);
              ui->widget->layer ("Recta Auxiliar")->setVisible (true);
              break;
            }
        }
    }
}

void C_PerfiZC::seleccionAC(bool cambio)
{
  QList<CDatoAnguloCierre*> *datosAngulosCierre = &calculos->DatosAzimut[aazimut].datosAngulosCierre;
  double    DisAngulocierre;
  QVector<double> x;
  QVector<double> y;
  x.append(0);
  y.append(posicion);
  if(cambio)
    {
      ui->widget->graph(21)->data().clear();

      for (int i = 0; i < listphaseTracer.length(); ++i)
        {
          if(listphaseTracer.at(i)->selected())
            {
              x.append(listphaseTracer.at(i)->position->key());
              y.append(listphaseTracer.at(i)->position->value());

              DisAngulocierre=datosAngulosCierre->at(i)->tangente *30060 + posicion;

              x.append(30060);
              y.append(DisAngulocierre);
              ui->widget->graph(21)->setData(x,y);
              ui->widget->layer ("Distancias")->setVisible (true);
              break;
            }
        }
    }
}

void C_PerfiZC::selectionChanged()
{
  if(ui->widget->graph (4)->selected ())
    ui->widget->layer ("Recta Auxiliar")->setVisible (true);
  else
    ui->widget->layer ("Recta Auxiliar")->setVisible (false);

  if(ui->widget->graph (21)->selected ())
    ui->widget->layer ("Distancias")->setVisible (true);
  else
    ui->widget->layer ("Distancias")->setVisible (false);
}
void C_PerfiZC::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
    Q_UNUSED(dataIndex);
  QString message = QString("A seleccionado la curva '%1'.").arg(plottable->name())/*.arg(dataIndex).arg(dataValue)*/;
  plottitle2->setText (message);
  QCPPlottableLegendItem *item = ui->widget->legend->itemWithPlottable(plottable);
  item->setSelected(true);
}

void C_PerfiZC::itemClicked(QCPAbstractItem *item, QMouseEvent *event)
{
    Q_UNUSED(event);
  QString message = item->objectName ();
  plottitle2->setText (message);
  for (auto i : listphaseTracer)
    {
      if(i->objectName () == item->objectName ())
        {
          ui->widget->legend->item (20)->setSelected (true);
          ui->widget->layer ("Distancias")->setVisible (true);
        }
    }

  for (auto i : listpTracerListaDisPZ)
    {
      if(i->objectName () == item->objectName ())
        {
          ui->widget->legend->item (19)->setSelected (true);
          ui->widget->layer ("Recta Auxiliar")->setVisible (true);
        }
    }
  for (auto listpTracerIntercepto : listpTracerInterceptos)
    {
      if(listpTracerIntercepto->objectName () == item->objectName ())
        ui->widget->legend->item (18)->setSelected (true);
    }
}

void C_PerfiZC::legendItemClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event)
{
    Q_UNUSED(legend);
    Q_UNUSED(event);
  for (int i=0; i < ui->widget->graphCount(); ++i)//seleccionar grafica
    {
      QCPGraph *graph = ui->widget->graph(i);
      if(legendItem == ui->widget->legend->itemWithPlottable(graph))
        {
          graph->setSelection (QCPDataSelection(graph->data()->dataRange()));
          if(legendItem == ui->widget->legend->item (21))
            ui->widget->layer ("Distancias")->setVisible (true);
          else
            ui->widget->layer ("Distancias")->setVisible (false);
          if(legendItem == ui->widget->legend->item (4))
            ui->widget->layer ("Recta Auxiliar")->setVisible (true);
          else
            ui->widget->layer ("Recta Auxiliar")->setVisible (false);
          break;
        }
    }

  if(legendItem == ui->widget->legend->item (20))
    listphaseTracer.last ()->setSelected (true);

  if(legendItem == ui->widget->legend->item (19))
    listpTracerListaDisPZ.last ()->setSelected (true);

  if(legendItem == ui->widget->legend->item (18))
    listpTracerInterceptos.last ()->setSelected (true);
}

void C_PerfiZC::widgetClik()
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

void C_PerfiZC::mouseWheel(QWheelEvent * event)
{
  Q_UNUSED (event);
  if (ui->widget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->widget->axisRect()->setRangeZoom(ui->widget->xAxis->orientation());
  else if (ui->widget->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->widget->axisRect()->setRangeZoom(ui->widget->yAxis->orientation());
  else
    ui->widget->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void C_PerfiZC::mouseMove(QMouseEvent *event)
{
  x=ui->widget->xAxis->pixelToCoord(event->pos().x());
  y=ui->widget->yAxis->pixelToCoord(event->pos().y());
  QString punto="D:"+QString::number(x)+"m; H:"+QString::number(y)+"m";
  ui->widget->setToolTip(punto);
}

void C_PerfiZC::on_checkBox_toggled(bool checked)//obs
{
  foreach (QCPItemTracer *item, listphaseTracer)
    {
      item->setVisible(checked);
    }
  ui->widget->replot();
}

void C_PerfiZC::on_checkBox_2_toggled(bool checked)
{
  foreach (QCPItemTracer *item, listpTracerListaDisPZ)
    {
      item->setVisible(checked);
    }
  ui->widget->replot();
}

void C_PerfiZC::on_pB_Zoommas_clicked()
{
  CustomPlotF customplotf;
  customplotf.zoommas(ui->widget);
}

void C_PerfiZC::on_pB_Zoommen_clicked()
{
  CustomPlotF customplotf;
  customplotf.zoommenos(ui->widget);
}
void C_PerfiZC::filePrintPdf()
{
  QString nom;
  QString aux;
  aux = /*fichero*/calculos->label;
  aux.replace(":", "" );
  nom=(QDir::currentPath()+"/GraficasPDF/"+aux+"_az_"+QString::number(aazimut)+"_perPlaz");

  CustomPlotF customplotf;
  customplotf.filePrintPdf(ui->widget,nom);
}
void C_PerfiZC::on_pushButton_4_clicked()
{
  filePrintPdf();
}

void C_PerfiZC::on_cheB_leyenda_toggled(bool checked)
{

  ui->widget->legend->setVisible(checked);

  ui->widget->replot();
}

void C_PerfiZC::on_checkBox_3_toggled(bool checked)
{
  ui->widget->layer("angulositio")->setVisible(checked);
  ui->widget->layer("angulositio")->replot();
}
