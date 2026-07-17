#include "angcierre1.h"
#include "ui_angcierre1.h"

#include "QDebug"

angcierre1::angcierre1(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::angcierre1)
{
  ui->setupUi(this);
  //    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
  ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                              QCP::iSelectLegend | QCP::iSelectPlottables);
  
  ui->widget->plotLayout()->insertRow(0);
  plottitle = new QCPTextElement(ui->widget);
  ui->widget->plotLayout()->addElement(0, 0,plottitle );
  
  ui->widget->setZoomMode(true);
  
  connect(ui->widget, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel(QWheelEvent*)));
  connect(ui->widget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
}

angcierre1::~angcierre1()
{
  delete ui;
}
void angcierre1::pintar()
{
//  int min = -90;
  ui->label->setText("Ángulo de cierre Lineal. Posición: "+ calculos->fichero);
  ui->widget->clearGraphs();
  initGraph();
  determDistanCiclo(calculos->grupo,calculos->arma,calculos->medio);
  for (int i=0; i < 360; ++i)
    {
      double ac = 0.0;
      for (int j = 0; j < calculos->DatosAzimut[i].datosAngulosCierre.length(); ++j)
        {
          if(calculos->DatosAzimut[i].datosAngulosCierre.length()>=1)
            {
              if(calculos->DatosAzimut[i].datosAngulosCierre.at(j)->distancia > distanciaArma)
                {
                  ac = calculos->DatosAzimut[i].datosAngulosCierre.at(j-1)->angulo;
                  break;
                }


                  ac = calculos->DatosAzimut[i].datosAngulosCierre.last()->angulo;

            }
//          else
//            {
//              if(calculos->DatosAzimut[i].datosAngulosCierre.last()->distancia > distanciaArma)
//                {
//                  ac = -3440*calculos->AlAnt/1.5;
//                }
//              else
//                ac = calculos->DatosAzimut[i].datosAngulosCierre.last()->angulo;
//            }
        }
      ui->widget->graph(0)->addData(i,ac); //se adiciona el dato calculado al trazo
      ui->widget->graph(1)->addData(i,-30); //se adiciona el dato calculado al trazo
      ui->widget->graph(2)->addData(i,0); //se adiciona el dato calculado al trazo
      ui->widget->graph(3)->addData(i,30); //se adiciona el dato calculado al trazo
    }
  ui->widget->replot();//dibujo la grafica
}

void angcierre1::determDistanCiclo(int g, int a, int m)
{
    if(g == 1)
    {
        if(a == 0)//POV
        {
            distanciaArma = 12060;
        }
        if(a == 1)//POP
        {
            if(m == 11)
                distanciaArma = 20070;
            if(m == 12)
                distanciaArma = 30060;
            if(m == 13)
                distanciaArma = 40050;
        }
        //se agrego un grupo 2
        if(a ==  2)//AAA
        {
            //21 22 23 24 25 26 27
            if(m == 21)
                distanciaArma = 8010;
            if(m == 22)
                distanciaArma = 8010;
            if(m == 23)
                distanciaArma = 8010;
            if(m == 24)
                distanciaArma = 8010;
            if(m == 25)
                distanciaArma = 8010;
            if(m == 26)
                distanciaArma = 8010;
            if(m == 27)
                distanciaArma = 8010;
        }
        if(a == 3)//CAA
        {
            // if(m == 46)
            // distanciaArma = 30060;
            // if(m == 47)
            // distanciaArma = 60030;
            if(m == 31)
                distanciaArma = 8010;
            if(m == 32)
                distanciaArma = 8010;
            if(m == 33)
                distanciaArma = 8010;
            if(m == 34)
                distanciaArma = 8010;
            if(m == 35)
                distanciaArma = 8010;
            if(m == 36)
                distanciaArma = 45000;
            if(m == 37)
                distanciaArma = 65070;
            if(m == 38)
                distanciaArma = 80010;
            if(m == 39)
                distanciaArma = 150030;
        }
        if(a == 4)//ERL
        {
            if(m == 40)
                distanciaArma = 190080;
            if(m == 41 || m == 48 || m == 49)
                distanciaArma = 185040;
            if(m == 42)
                distanciaArma = 60030;
            if(m == 43)
                distanciaArma = 18000;
            if(m == 44)
                distanciaArma = 22050;
            if(m == 45)
                distanciaArma = 120060;
            if(m == 46)
                distanciaArma = 30060;
            if(m == 47)
                distanciaArma = 60030;
            if(m == 50)
                distanciaArma = 190080;
        }
    }
    if(g ==  2)
    {
        //21 22 23 24 25 26 27
        if(m == 21)
            distanciaArma = 1530;
        if(m == 22)
            distanciaArma = 2520;
        if(m == 23 || m == 44)
            distanciaArma = 2520;
        if(m == 24)
            distanciaArma = 3060;
        if(m == 25)
            distanciaArma = 4050;
        if(m == 26 || m == 43)
            distanciaArma = 6030;
        if(m == 27 || m == 42)
            distanciaArma = 12060;
    }
    if(g == 3)
    {
      //31 32 33 34 35 36 37 38 39
      if(m == 31)
        distanciaArma = 4050;
      if(m == 32)
        distanciaArma = 2070;
      if(m == 33)
        distanciaArma = 2520;
      if(m == 34)
        distanciaArma = 3060;
      if(m == 35)
        distanciaArma = 5040;
      if(m == 36)
        distanciaArma = 10080;
      if(m == 37)
        distanciaArma = 20070;
      if(m == 38)
        distanciaArma = 17010;
      if(m == 39)
        distanciaArma = 42840;
    }
}
void angcierre1::mouseMove(QMouseEvent *event)
{
  double x=ui->widget->xAxis->pixelToCoord(event->pos().x());
  double y=ui->widget->yAxis->pixelToCoord(event->pos().y());
  QString punto="Az:"+QString::number(x)+"°; AC:"+QString::number(y)+"min";
  ui->widget->setToolTip(punto);
}

void angcierre1::initGraph()
{
  QPen pen;
  pen.setWidth(3);
  pen.setColor(Qt::red);
  // set title of plot:
  plottitle->setText("Ángulo de cierre Lineal. Posición: "+ calculos->label);
  
  ui->widget->addGraph();//Angulos de cierre
  ui->widget->graph(0)->setPen(pen); //color del trazo
  ui->widget->graph(0)->setBrush(QBrush(QColor(255,0,0,0))); //color bajo la curva
  
  ui->widget->addGraph();//-30
  ui->widget->graph(1)->setPen(QColor(0,255,0)); //color del trazo
  ui->widget->graph(1)->setBrush(QBrush(QColor(0,255,0,0))); //color bajo la curva
  
  ui->widget->addGraph();//0
  ui->widget->graph(2)->setPen(QColor(0,0,255)); //color del trazo
  ui->widget->graph(2)->setBrush(QBrush(QColor(0,0,255,0))); //color bajo la curva
  
  ui->widget->addGraph();//30
  ui->widget->graph(3)->setPen(QColor(255,0,255)); //color del trazo
  ui->widget->graph(3)->setBrush(QBrush(QColor(255,255,0,0))); //color bajo la curva
  
  pen.setWidth(2);
  pen.setColor(Qt::black);
  
  ui->widget->xAxis->setBasePen(pen);
  ui->widget->yAxis->setBasePen(pen);
  
  ui->widget->xAxis->setLabel("Azimut.(grados)");//etiqueta para las abscisas
  ui->widget->yAxis->setLabel("Ángulos de cierre.(minutos)");//etiqueta para las abscisas
  ui->widget->yAxis->setRange(-500,500);//se estabelece rango de y
  ui->widget->xAxis->setRange(0,360);
}
void angcierre1::mouseWheel(QWheelEvent *event)
{
  Q_UNUSED (event);
  if (ui->widget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->widget->axisRect()->setRangeZoom(ui->widget->xAxis->orientation());
  else if (ui->widget->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->widget->axisRect()->setRangeZoom(ui->widget->yAxis->orientation());
  else
    ui->widget->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
  
}

void angcierre1::on_pB_Zoommas_clicked()
{
  CustomPlotF customplotf;
  customplotf.zoommas(ui->widget);
}

void angcierre1::on_pB_Zoommen_clicked()
{
  CustomPlotF customplotf;
  customplotf.zoommenos (ui->widget);
}


void angcierre1::on_pushButton_4_clicked()
{
  QString nom;
  QString aux;
  aux = calculos->fichero;
  aux.replace(":", "" );
  
  nom=(QDir::currentPath()+"/GraficasPDF/"+aux+"_ac1");
  CustomPlotF customplotf;
  customplotf.filePrintPdf(ui->widget,nom);
}

QCustomPlotZoom::QCustomPlotZoom(QWidget *parent)
  : QCustomPlot(parent)
  , mZoomMode(false)
  , mRubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{
  
}

QCustomPlotZoom::~QCustomPlotZoom()
= default;

void QCustomPlotZoom::setZoomMode(bool mode)
{
  mZoomMode = mode;
}

