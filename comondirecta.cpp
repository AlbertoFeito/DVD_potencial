#include "comondirecta.h"
#include "ui_comondirecta.h"

ComonDirecta::ComonDirecta(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComonDirecta)
{
    ui->setupUi(this);
     setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);

    RelAsp = ui->widget->width()/ui->widget->height();
//ui->widget->setMinimumSize(700,700);
    ui->widget->plotLayout()->insertRow(0);
//    plottitle = new QCPPlotTitle(ui->widget);
    plottitle = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(0, 0,plottitle );

    connect(ui->widget, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
    connect(ui->widget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
    connect(ui->widget, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    distancia = 0;
    AlPos = 0;
    Antena1 = 0;
    Antena2 = 0;
    azimut = 0;
}

ComonDirecta::~ComonDirecta()
{
    delete ui;
}

/**********************construir graficos de perfil y anngulo de cierre 1************************************************/
void ComonDirecta::initGraph()
{
    // set title of plot:
    plottitle->setText("Comunicación directa. Posición1: "+ fichero);

    ui->widget->addGraph();//Comunicacines
    ui->widget->graph(0)->setPen(QColor(0,100,0)); //color del trazo
    ui->widget->graph(0)->setBrush(QBrush(QColor(0,100,0,50))); //color bajo la curva

    ui->widget->addGraph();//Comunicacines
    ui->widget->graph(1)->setPen(QColor(255,0,0)); //color del trazo
    ui->widget->graph(1)->setBrush(QBrush(QColor(255,0,0,0))); //color bajo la curva


    ui->widget->addGraph();//Comunicacines
    ui->widget->graph(2)->setPen(QColor(0,0,255)); //color del trazo
    ui->widget->graph(2)->setBrush(QBrush(QColor(0,0,255,0))); //color bajo la curva

    ui->widget->addGraph();//Comunicacines
    ui->widget->graph(3)->setPen(QColor(0,0,255)); //color del trazo
    ui->widget->graph(3)->setBrush(QBrush(QColor(0,0,255,0))); //color bajo la curva

    ui->widget->addGraph();//tierra
    ui->widget->graph(4)->setPen(QColor(0,0,0)); //color del trazo
    ui->widget->graph(4)->setBrush(QBrush(QColor(0,0,0,0))); //color bajo la curva
    ui->widget->graph(0)->setChannelFillGraph(ui->widget->graph(4));

    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::black);

    ui->widget->xAxis->setLabel("Distancia mts");//etiqueta para las abscisas
    ui->widget->yAxis->setLabel("Altura en mts.");//etiqueta para las abscisas
    ui->widget->yAxis->setRange(-100,1000);//se estabelece rango de y
    ui->widget->xAxis->setRange(-20,50000);
    ui->widget->xAxis->setBasePen(pen);
    ui->widget->yAxis->setBasePen(pen);
}

void ComonDirecta::pintarComones()
{
    double d = 0;
    pos1 = 0;
    double pos2 = 0;
    double curva = 0;
    double baseA1=0;
    double puntaA1 = 0;
    double baseA2=0;
    double puntaA2 = 0;
    double y = 0;
    double x = 0;

    ui->widget->clearGraphs();
    initGraph();

    if(fmod(distancia,90) == 0)
    {
        pos2=datosterreno.at(int(distancia/90));
    }
    else
    {
        int p1 = int(distancia/90);//punto distancia multiplo de 90
        double m =(datosterreno.at(p1+1)-datosterreno.at(p1))/((p1+1)*90-(p1)*90);//pendiente de la recta que une a p1 con p1+1
        double n = (datosterreno.at(p1)-m*p1*90);//n de la recta

        pos2 = (m*distancia+n);//ecuacion de la recta y=mx+n
    }
    AlPos = datosterreno.at(0);
    baseA2 = distancia;
    puntaA2 = pos2;
    puntaA1 = AlPos;
    y = AlPos +Antena1;
    for (double i : datosterreno)
    {
        ui->widget->graph(0)->addData(d,i); //se adiciona el dato calculado al trazo
        ui->widget->graph(4)->addData(d,curva);
        d+=90;
        curva= -(pow(d,2))/(17000000);
    }
    ui->widget->graph(1)->addData(x,y); //recta de visibilidad
    ui->widget->graph(2)->addData(baseA2,puntaA2); //mastil 2
    ui->widget->graph(3)->addData(baseA1,puntaA1); //mastil 1

    puntaA2 = pos2 + Antena2;
    puntaA1 = AlPos + Antena1;
    y = puntaA2;
    x = distancia;
    ui->widget->graph(1)->addData(x,y); //recta de visibilidad
    ui->widget->graph(2)->addData(baseA2,puntaA2); //mastil 2
    ui->widget->graph(3)->addData(baseA1,puntaA1); //mastil 1

    ui->widget->graph(0)->setName("Perfil de Alturas Azimut: "+QString::number(azimut));//nombre para la leyenda
    ui->widget->graph(1)->setName("Recta de visibilidad Antena");//nombre para la leyenda
    ui->widget->graph(2)->setName("Mastil antena 2: "+ QString::number(Antena2)+ "m");//nombre para la leyenda
    ui->widget->graph(3)->setName("Mastil antena 1: "+ QString::number(Antena1)+ "m");//nombre para la leyenda
    ui->widget->graph(4)->setName("Curvatura de la tierra");
    ui->widget->legend->setVisible(true); //muestra la leyenda

    ui->widget->replot();      //pinto todas las graficas
}

void ComonDirecta::mouseWheel()
{
    if (ui->widget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->widget->axisRect()->setRangeZoom(ui->widget->xAxis->orientation());
    else if (ui->widget->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->widget->axisRect()->setRangeZoom(ui->widget->yAxis->orientation());
    else
        ui->widget->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
void ComonDirecta::mouseMove(QMouseEvent *event)
{
    double x=ui->widget->xAxis->pixelToCoord(event->pos().x());
    double y=ui->widget->yAxis->pixelToCoord(event->pos().y());
    QString punto="D:"+QString::number(x)+"m; A:"+QString::number(y)+"m";
    ui->widget->setToolTip(punto);
}

void ComonDirecta::on_pB_Zoommas_clicked()
{
  CustomPlotF customplotf;
  customplotf.zoommas(ui->widget);
}

void ComonDirecta::on_pB_Zoommen_clicked()
{
  CustomPlotF customplotf;
  customplotf.zoommenos(ui->widget);
}
void ComonDirecta::filePrintPdf()
{
    QString nom;
    QString aux;
    aux = fichero;
    aux.replace(":", "" );

    nom=(QDir::currentPath()+"/GraficasPDF/"+aux+"_pap");
    CustomPlotF customplotf;
    customplotf.filePrintPdf(ui->widget,nom);
}
void ComonDirecta::on_pushButton_4_clicked()
{
   filePrintPdf();
}
void ComonDirecta::selectionChanged()
{
    for (int i=0; i < ui->widget->graphCount(); ++i)
    {
        QCPGraph *graph = ui->widget->graph(i);

        QCPPlottableLegendItem *item = ui->widget->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setSelection (QCPDataSelection(graph->data()->dataRange()));
        }
    }
}
