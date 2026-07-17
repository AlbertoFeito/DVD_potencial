#include "c_grafplazoleta.h"
#include "ui_c_grafplazoleta.h"
#include <QDebug>
#include <QTextDocument>
#include <QDir>
#include <QFile>
#include <QTextDocumentWriter>
#include <QTextCursor>
#include <QTextTable>
c_GrafPlazoleta::c_GrafPlazoleta(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::c_GrafPlazoleta)
{
    ui->setupUi(this);
//    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectItems);
    //    ui->widget->setGeometry();
    danca = new DanCalc();
    ui->widget->setMinimumSize(600,600);
    //item = new QCPAbstractItem();
    ui->widget->plotLayout()->insertRow(0);
    //	plottitle = new QCPPlotTitle(ui->widget);
    plottitle = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(0, 0,plottitle );
    connect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
    connect(ui->widget, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel(QWheelEvent*)));

    auto *mainLayout = new QHBoxLayout;
    auto *botonLayout = new QHBoxLayout;
    auto *layout = new QGridLayout;
    dialogTabla = new QDialog();
    dialogTabla->setWindowFlags(Qt::WindowMinMaxButtonsHint);

    dialogTabla->setWindowFlags(dialogTabla->windowFlags() |= Qt::WindowCloseButtonHint);
    connect(dialogTabla,SIGNAL(finished(int)),this,SLOT(cerrartabla(int)));

    salvarTabla = new QPushButton("Salvar Tabla",dialogTabla);
    connect(salvarTabla,SIGNAL(clicked(bool)),this,SLOT(salvarTablas(/*bool*/)));
    tabla = new QTableWidget(dialogTabla);

    mainLayout->addWidget(tabla);
    botonLayout->addWidget(salvarTabla);
    botonLayout->addSpacing(dialogTabla->width()-10);
    layout->addLayout(botonLayout, 0, 0);
    botonLayout->addSpacing(10);
    layout->addLayout(mainLayout, 1, 0);

    dialogTabla->setLayout(layout);
}

c_GrafPlazoleta::~c_GrafPlazoleta()
{   
    delete ui;
}

void c_GrafPlazoleta::initGraph()
{
    ui->widget->yAxis->setRange(-700,700);//se estabelece rango de y
    ui->widget->xAxis->setRange(-700,700);
    ui->widget->xAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->yAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->xAxis->setVisible(false);
    ui->widget->yAxis->setVisible(false);

    if(banda == 0)
    {
        dialogTabla->setWindowTitle("Caracterización de la plazoleta. Banda Métrica-Posición: "+fichero);
        tabla->setWindowTitle("Caracterización de la plazoleta. Banda Métrica-Posición: "+fichero);
        plottitle->setText("Caracterización de la plazoleta. Banda Métrica-Posición: "+fichero);
    }
    else
    {
        dialogTabla->setWindowTitle("Caracterización de la plazoleta. Banda Decimétrica-Posición: "+fichero);
        tabla->setWindowTitle("Plazoleta Zona Cercana-Banda Decimétrica-Posición: "+fichero);
        plottitle->setText("Plazoleta Zona Cercana-Banda Decimétrica-Posición: "+fichero);
    }
}

void c_GrafPlazoleta::pintar()
{
    ui->widget->clearGraphs();

    initGraph();

    pintaLinea(ui->widget);

    /*********distancias conocidas*********/
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::blue);
    pen.setStyle(Qt::DashDotLine);
    auto *elipseMax = new QCPItemEllipse(ui->widget);
    elipseMax->setPen(pen);
    double disMax = distanciasPlazoleta.last()*0.04;
    elipseMax->topLeft->setCoords(-disMax,disMax);
    elipseMax->bottomRight->setCoords(disMax,-disMax);

    QPen pen1;
    pen1.setWidth(2);
    pen1.setColor(QColor(47,116,232));
    pen1.setStyle(Qt::DashDotLine);
    auto *elipseMin = new QCPItemEllipse(ui->widget);
    elipseMin->setPen(pen1);
    double disMin = distanciasPlazoleta.first()*0.04;
    elipseMin->topLeft->setCoords(-disMin,disMin);
    elipseMin->bottomRight->setCoords(disMin,-disMin);

    QPen pen2;
    pen2.setWidth(2);
    pen2.setColor(QColor(50,220,250));
    pen2.setStyle(Qt::DashDotLine);
    auto *elipseMedia = new QCPItemEllipse(ui->widget);
    elipseMedia->setPen(pen2);
    double disMedia = distanciasPlazoleta.at(2)*0.04;
    elipseMedia->topLeft->setCoords(-disMedia,disMedia);
    elipseMedia->bottomRight->setCoords(disMedia,-disMedia);
    /**************************/

    bool aux = false;
    bool aux2 = true;
    for (int i = 40; i <= 1200; i+=40)
    {
        if(i==200 || i==400 ||i==600 ||i==800 ||i==1000 || i==1200)
        {
            aux=true;
            aux2=false;
        }
        else
        {
            aux=false;
            aux2=true;
        }
        pintaElipses(ui->widget,i,aux,QString::number(i/40),aux2);
    }
}
void c_GrafPlazoleta::pintaElipses(QCustomPlot *customplot, int coord, bool text, const QString &t, bool align)
{
    auto *elipse=new QCPItemEllipse(customplot);

    elipse->topLeft->setCoords(-coord,coord);
    elipse->bottomRight->setCoords(coord,-coord);
    if(text)
    {
        auto *distancia = new QCPItemText(customplot);
        distancia->setText(t +"Km");
        distancia->position->setCoords(0,coord);
        if(align)
            distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
        else
            distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);
    }
}
void c_GrafPlazoleta::pintaLinea(QCustomPlot *customPlot)
{
    double x;
    double y;
    for (int i = 0; i < 359; i+=30)
    {
        x = 1210*sin(i*RADIAN);//lineas
        y = 1210*cos(i*RADIAN);

        auto  *line = new QCPItemLine(customPlot);
        auto *text = new QCPItemText(customPlot);

        line->start->setCoords(0,0);
        line->end->setCoords(x,y);

        text->setText(QString::number(i));
        text->position->setCoords(x,y);
        switch (i) {
        case 0:
            text->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
            break;
        case 30: case 60: case 90:
            text->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
            break;
        case 120: case 150:
            text->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
            break;
        case 180: case 210: case 240:
            text->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
            break;
        case 270: case 300: case 330:
            text->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);
            break;
        default:
            break;
        }
        text->setFont(QFont(font().family(), 10));
    }
}
void c_GrafPlazoleta::pintarDistancias()
{
    QCPLineEnding head;
    head.setStyle( QCPLineEnding::esBar);
    head.setWidth(3);
    head.setLength(3);

    QVector<double> x1;
    QVector<double>  y1;
    QVector<double> x2;
    QVector<double> y2;
    QVector<double> x3;
    QVector<double> y3;
    QVector<double> xlast;
    QVector<double> ylast;

    for (int a = 0; a <= 360; a++)
    {
        ContSumDisInterna = 0;
        if(a < 360)
        {
            if(listdistplazoleta[a].length() == 5)
            {
                x1.append(listdistplazoleta[a].at(0)* 0.04 * sin(a * RADIAN));
                y1.append(listdistplazoleta[a].at(0)* 0.04 * cos(a * RADIAN));

                x3.append(listdistplazoleta[a].at(2)* 0.04 * sin(a * RADIAN));
                y3.append(listdistplazoleta[a].at(2)* 0.04 * cos(a * RADIAN));

                xlast.append(listdistplazoleta[a].at(4)* 0.04 * sin(a * RADIAN));
                ylast.append(listdistplazoleta[a].at(4)* 0.04 * cos(a * RADIAN));

                /////////////////////////////////////////////////

                for (int i = 0; i < InterceptoAnguloyPerfil[a].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        if(InterceptoAnguloyPerfil[a].at(i) <= listdistplazoleta[a].at(4))
                        {
                            double xIn2 = InterceptoAnguloyPerfil[a].at(i)* 0.04 * sin(a * RADIAN);
                            double yIn2 = InterceptoAnguloyPerfil[a].at(i)* 0.04 * cos(a * RADIAN);
                            auto  * linein = new QCPItemLine(ui->widget);
                            linein->setPen(QPen(Qt::red,1));
                            linein->setHead(head);
                            linein->setTail(head);
                            linein->start->setCoords(xIn2,yIn2);
                            if(i < InterceptoAnguloyPerfil[a].length()-1)
                            {
                                if(InterceptoAnguloyPerfil[a].at(i+1) <= listdistplazoleta[a].at(4))
                                {
                                    double xIn1 = InterceptoAnguloyPerfil[a].at(i+1)* 0.04 * sin(a * RADIAN);
                                    double yIn1 = InterceptoAnguloyPerfil[a].at(i+1)* 0.04 * cos(a * RADIAN);

                                    linein->end->setCoords(xIn1,yIn1);
                                }
                                else
                                    linein->end->setCoords(listdistplazoleta[a].at(4)* 0.04 * sin(a * RADIAN),listdistplazoleta[a].at(4)* 0.04 * cos(a * RADIAN));
                                if(InterceptoAnguloyPerfil[a].at(i) <= listdistplazoleta[a].at(2))
                                    ContSumDisInterna += InterceptoAnguloyPerfil[a].at(i+1)-InterceptoAnguloyPerfil[a].at(i);
                            }
                            else
                            {
                                linein->end->setCoords(listdistplazoleta[a].at(4)* 0.04 * sin(a * RADIAN),listdistplazoleta[a].at(4)* 0.04 * cos(a * RADIAN));
                                if(InterceptoAnguloyPerfil[a].at(i) <= listdistplazoleta[a].at(2))
                                    ContSumDisInterna +=listdistplazoleta[a].at(2)-InterceptoAnguloyPerfil[a].at(i);
                            }
                        }
                    }
                }
                /************************************/
            }
            else if(listdistplazoleta[a].length() >= 3)
            {
                x1.append(listdistplazoleta[a].at(0)* 0.04 * sin(a * RADIAN));
                y1.append(listdistplazoleta[a].at(0)* 0.04 * cos(a * RADIAN));

                x3.append(listdistplazoleta[a].at(2)* 0.04 * sin(a * RADIAN));
                y3.append(listdistplazoleta[a].at(2)* 0.04 * cos(a * RADIAN));

                auto *curDistancias5 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
                curDistancias5->setName("Distancias");
                curDistancias5->setData(xlast, ylast);
                //                curDistancias5->setPen(QPen(Qt::blue,2));
                if(xlast.length()==1)
                {
                    curDistancias5->setPen(QPen(Qt::blue,1));
                    curDistancias5->setScatterStyle(QCPScatterStyle::ssDisc);
                }
                else
                    curDistancias5->setPen(QPen(Qt::blue,2));

                xlast.clear();
                ylast.clear();
                //////////////////////////////////////////////////////////////////////
                for (int i = 0; i < InterceptoAnguloyPerfil[a].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        if(InterceptoAnguloyPerfil[a].at(i) <= listdistplazoleta[a].at(2))
                        {
                            double xIn2 = InterceptoAnguloyPerfil[a].at(i)* 0.04 * sin(a * RADIAN);
                            double yIn2 = InterceptoAnguloyPerfil[a].at(i)* 0.04 * cos(a * RADIAN);
                            auto  * linein = new QCPItemLine(ui->widget);

                            linein->setPen(QPen(Qt::red,1));
                            linein->setHead(head);
                            linein->setTail(head);
                            linein->start->setCoords(xIn2,yIn2);
                            if(i < InterceptoAnguloyPerfil[a].length()-1)
                            {
                                if(InterceptoAnguloyPerfil[a].at(i+1) <= listdistplazoleta[a].at(2))
                                {
                                    double xIn1 = InterceptoAnguloyPerfil[a].at(i+1)* 0.04 * sin(a * RADIAN);
                                    double yIn1 = InterceptoAnguloyPerfil[a].at(i+1)* 0.04 * cos(a * RADIAN);

                                    linein->end->setCoords(xIn1,yIn1);
                                }
                                else
                                    linein->end->setCoords(listdistplazoleta[a].at(2)* 0.04 * sin(a * RADIAN),listdistplazoleta[a].at(2)* 0.04 * cos(a * RADIAN));
                                if(InterceptoAnguloyPerfil[a].at(i) <= listdistplazoleta[a].at(2))
                                    ContSumDisInterna += InterceptoAnguloyPerfil[a].at(i+1)-InterceptoAnguloyPerfil[a].at(i);
                            }
                            else
                            {
                                linein->end->setCoords(listdistplazoleta[a].at(2)* 0.04 * sin(a * RADIAN),listdistplazoleta[a].at(2)* 0.04 * cos(a * RADIAN));
                                if(InterceptoAnguloyPerfil[a].at(i) <= listdistplazoleta[a].at(2))
                                    ContSumDisInterna += listdistplazoleta[a].at(2)-InterceptoAnguloyPerfil[a].at(i);
                            }
                        }
                    }
                }
                //********************************************//
            }
            else if(listdistplazoleta[a].length() >= 1)
            {
                x1.append(listdistplazoleta[a].at(0)* 0.04 * sin(a * RADIAN));
                y1.append(listdistplazoleta[a].at(0)* 0.04 * cos(a * RADIAN));

                auto *curDistancias3 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
                curDistancias3->setName("Distancias");
                curDistancias3->setData(x3, y3);
                //                curDistancias3->setPen(QPen(QColor(45,220,234),2));

                if(x3.length()==1)
                {
                    curDistancias3->setPen(QPen(QColor(45,220,234),1));
                    curDistancias3->setScatterStyle(QCPScatterStyle::ssDisc);
                }
                else
                    curDistancias3->setPen(QPen(QColor(45,220,234),2));


                x3.clear();
                y3.clear();
                for (int i = 0; i < InterceptoAnguloyPerfil[a].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        if(InterceptoAnguloyPerfil[a].at(i) <= listdistplazoleta[a].at(0))
                        {
                            double xIn2 = InterceptoAnguloyPerfil[a].at(i)* 0.04 * sin(a * RADIAN);
                            double yIn2 = InterceptoAnguloyPerfil[a].at(i)* 0.04 * cos(a * RADIAN);
                            auto  * linein = new QCPItemLine(ui->widget);
                            linein->setPen(QPen(Qt::red,1));
                            linein->setHead(head);
                            linein->setTail(head);
                            linein->start->setCoords(xIn2,yIn2);
                            if(i < InterceptoAnguloyPerfil[a].length()-1)
                            {
                                if(InterceptoAnguloyPerfil[a].at(i+1) <= listdistplazoleta[a].at(0))
                                {
                                    double xIn1 = InterceptoAnguloyPerfil[a].at(i+1)* 0.04 * sin(a * RADIAN);
                                    double yIn1 = InterceptoAnguloyPerfil[a].at(i+1)* 0.04 * cos(a * RADIAN);

                                    linein->end->setCoords(xIn1,yIn1);
                                }
                                else
                                    linein->end->setCoords(listdistplazoleta[a].at(0)* 0.04 * sin(a * RADIAN),listdistplazoleta[a].at(0)* 0.04 * cos(a * RADIAN));

                                if(InterceptoAnguloyPerfil[a].at(i) <= listdistplazoleta[a].at(0))
                                    ContSumDisInterna += InterceptoAnguloyPerfil[a].at(i+1)-InterceptoAnguloyPerfil[a].at(i);
                            }
                            else
                            {
                                linein->end->setCoords(listdistplazoleta[a].at(0)* 0.04 * sin(a * RADIAN),listdistplazoleta[a].at(0)* 0.04 * cos(a * RADIAN));

                                if(InterceptoAnguloyPerfil[a].at(i) <= listdistplazoleta[a].at(0))
                                    ContSumDisInterna +=listdistplazoleta[a].at(0)-InterceptoAnguloyPerfil[a].at(i);
                            }
                        }
                    }
                }

                /************************************/
            }

        }//azimut 360
        else
        {
            if(listdistplazoleta[0].length() == 5)
            {
                x1.append(listdistplazoleta[0].at(0)* 0.04 * sin(0 * RADIAN));
                y1.append(listdistplazoleta[0].at(0)* 0.04 * cos(0 * RADIAN));

                x3.append(listdistplazoleta[0].at(2)* 0.04 * sin(0 * RADIAN));
                y3.append(listdistplazoleta[0].at(2)* 0.04 * cos(0 * RADIAN));

                xlast.append(listdistplazoleta[0].at(4)* 0.04 * sin(0 * RADIAN));
                ylast.append(listdistplazoleta[0].at(4)* 0.04 * cos(0 * RADIAN));

                /////////////////////////////////////////////////

                for (int i = 0; i < InterceptoAnguloyPerfil[0].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        if(InterceptoAnguloyPerfil[0].at(i) <= listdistplazoleta[0].at(4))
                        {
                            double xIn2 = InterceptoAnguloyPerfil[0].at(i)* 0.04 * sin(0 * RADIAN);
                            double yIn2 = InterceptoAnguloyPerfil[0].at(i)* 0.04 * cos(0 * RADIAN);
                            auto  * linein = new QCPItemLine(ui->widget);
                            linein->setPen(QPen(Qt::red,1));
                            linein->setHead(head);
                            linein->setTail(head);
                            linein->start->setCoords(xIn2,yIn2);
                            if(i < InterceptoAnguloyPerfil[0].length()-1)
                            {
                                if(InterceptoAnguloyPerfil[0].at(i+1) <= listdistplazoleta[0].at(4))
                                {
                                    double xIn1 = InterceptoAnguloyPerfil[0].at(i+1)* 0.04 * sin(0 * RADIAN);
                                    double yIn1 = InterceptoAnguloyPerfil[0].at(i+1)* 0.04 * cos(0 * RADIAN);

                                    linein->end->setCoords(xIn1,yIn1);
                                }
                                else
                                    linein->end->setCoords(listdistplazoleta[0].at(4)* 0.04 * sin(0 * RADIAN),listdistplazoleta[0].at(4)* 0.04 * cos(0 * RADIAN));

                                if(InterceptoAnguloyPerfil[0].at(i) <= listdistplazoleta[0].at(2))
                                    ContSumDisInterna += InterceptoAnguloyPerfil[0].at(i+1)-InterceptoAnguloyPerfil[0].at(i);
                            }
                            else
                            {
                                linein->end->setCoords(listdistplazoleta[0].at(4)* 0.04 * sin(0 * RADIAN),listdistplazoleta[0].at(4)* 0.04 * cos(0 * RADIAN));

                                if(InterceptoAnguloyPerfil[0].at(i) <= listdistplazoleta[0].at(2))
                                    ContSumDisInterna +=listdistplazoleta[0].at(2)-InterceptoAnguloyPerfil[0].at(i);
                            }
                        }
                    }
                }

                /************************************/
            }
            else if(listdistplazoleta[0].length() >= 3)
            {
                x1.append(listdistplazoleta[0].at(0)* 0.04 * sin(0 * RADIAN));
                y1.append(listdistplazoleta[0].at(0)* 0.04 * cos(0 * RADIAN));

                x3.append(listdistplazoleta[0].at(2)* 0.04 * sin(0 * RADIAN));
                y3.append(listdistplazoleta[0].at(2)* 0.04 * cos(0 * RADIAN));

                auto *curDistancias5 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
                curDistancias5->setName("Distancias");
                curDistancias5->setData(xlast, ylast);
                //                curDistancias5->setPen(QPen(Qt::blue,2));
                if(xlast.length()==1)
                {
                    curDistancias5->setPen(QPen(Qt::blue,1));
                    curDistancias5->setScatterStyle(QCPScatterStyle::ssDisc);
                }
                else
                    curDistancias5->setPen(QPen(Qt::blue,2));



                xlast.clear();
                ylast.clear();

                //////////////////////////////////////////////////////////////////////
                for (int i = 0; i < InterceptoAnguloyPerfil[0].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        if(InterceptoAnguloyPerfil[0].at(i) <= listdistplazoleta[0].at(2))
                        {
                            double xIn2 = InterceptoAnguloyPerfil[0].at(i)* 0.04 * sin(0 * RADIAN);
                            double yIn2 = InterceptoAnguloyPerfil[0].at(i)* 0.04 * cos(0 * RADIAN);
                            auto  * linein = new QCPItemLine(ui->widget);

                            linein->setPen(QPen(Qt::red,1));
                            linein->setHead(head);
                            linein->setTail(head);
                            linein->start->setCoords(xIn2,yIn2);
                            if(i < InterceptoAnguloyPerfil[0].length()-1)
                            {
                                if(InterceptoAnguloyPerfil[0].at(i+1) <= listdistplazoleta[0].at(2))
                                {
                                    double xIn1 = InterceptoAnguloyPerfil[0].at(i+1)* 0.04 * sin(0 * RADIAN);
                                    double yIn1 = InterceptoAnguloyPerfil[0].at(i+1)* 0.04 * cos(0 * RADIAN);

                                    linein->end->setCoords(xIn1,yIn1);
                                }
                                else
                                    linein->end->setCoords(listdistplazoleta[0].at(2)* 0.04 * sin(0 * RADIAN),listdistplazoleta[0].at(2)* 0.04 * cos(0 * RADIAN));
                                if(InterceptoAnguloyPerfil[0].at(i) <= listdistplazoleta[0].at(2))
                                    ContSumDisInterna += InterceptoAnguloyPerfil[0].at(i+1)-InterceptoAnguloyPerfil[0].at(i);
                            }
                            else
                            {
                                linein->end->setCoords(listdistplazoleta[0].at(2)* 0.04 * sin(0 * RADIAN),listdistplazoleta[0].at(2)* 0.04 * cos(0 * RADIAN));

                                if(InterceptoAnguloyPerfil[0].at(i) <= listdistplazoleta[0].at(2))
                                    ContSumDisInterna += listdistplazoleta[0].at(2)-InterceptoAnguloyPerfil[0].at(i);
                            }
                        }
                    }
                }
                //********************************************//
            }
            else if(listdistplazoleta[0].length() >= 1)
            {
                x1.append(listdistplazoleta[0].at(0)* 0.04 * sin(0 * RADIAN));
                y1.append(listdistplazoleta[0].at(0)* 0.04 * cos(0 * RADIAN));

                auto *curDistancias3 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
                curDistancias3->setName("Distancias");
                curDistancias3->setData(x3, y3);
                if(x3.length()==1)
                {
                    curDistancias3->setPen(QPen(QColor(45,220,234),1));
                    curDistancias3->setScatterStyle(QCPScatterStyle::ssDisc);
                }
                else
                    curDistancias3->setPen(QPen(QColor(45,220,234),2));               

                x3.clear();
                y3.clear();
                for (int i = 0; i < InterceptoAnguloyPerfil[0].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        if(InterceptoAnguloyPerfil[0].at(i) <= listdistplazoleta[0].at(0))
                        {
                            double xIn2 = InterceptoAnguloyPerfil[0].at(i)* 0.04 * sin(0 * RADIAN);
                            double yIn2 = InterceptoAnguloyPerfil[0].at(i)* 0.04 * cos(0 * RADIAN);
                            auto  * linein = new QCPItemLine(ui->widget);
                            linein->setPen(QPen(Qt::red,1));
                            linein->setHead(head);
                            linein->setTail(head);
                            linein->start->setCoords(xIn2,yIn2);
                            if(i < InterceptoAnguloyPerfil[0].length()-1)
                            {
                                if(InterceptoAnguloyPerfil[0].at(i+1) <= listdistplazoleta[0].at(0))
                                {
                                    double xIn1 = InterceptoAnguloyPerfil[0].at(i+1)* 0.04 * sin(0 * RADIAN);
                                    double yIn1 = InterceptoAnguloyPerfil[0].at(i+1)* 0.04 * cos(0 * RADIAN);

                                    linein->end->setCoords(xIn1,yIn1);
                                }
                                else
                                    linein->end->setCoords(listdistplazoleta[0].at(0)* 0.04 * sin(0 * RADIAN),listdistplazoleta[0].at(0)* 0.04 * cos(0 * RADIAN));
                                if(InterceptoAnguloyPerfil[0].at(i) <= listdistplazoleta[0].at(0))
                                    ContSumDisInterna += InterceptoAnguloyPerfil[0].at(i+1)-InterceptoAnguloyPerfil[0].at(i);
                            }
                            else
                            {
                                linein->end->setCoords(listdistplazoleta[0].at(0)* 0.04 * sin(0 * RADIAN),listdistplazoleta[0].at(0)* 0.04 * cos(0 * RADIAN));

                                if(InterceptoAnguloyPerfil[0].at(i) <= listdistplazoleta[0].at(0))
                                    ContSumDisInterna +=listdistplazoleta[0].at(0)-InterceptoAnguloyPerfil[0].at(i);
                            }
                        }
                    }
                }

                /************************************/
            }
        }
        sumDisInternas.append(ContSumDisInterna);
        //		qDebug()<<a<<sumDisInternas.at(a);
    }

    auto *curDistancias1 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curDistancias1->setName("Distancias");
    curDistancias1->setData(x1, y1);
    if(x1.length()==1)
    {
        curDistancias1->setPen(QPen(QColor(47,116,232),1));
        curDistancias1->setScatterStyle(QCPScatterStyle::ssDisc);
    }
    else
        curDistancias1->setPen(QPen(QColor(47,116,232),2));

    auto *curDistancias3 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curDistancias3->setName("Distancias");
    curDistancias3->setData(x3, y3);
    if(x3.length()==1)
    {
        curDistancias3->setPen(QPen(QColor(45,220,234),1));
        curDistancias3->setScatterStyle(QCPScatterStyle::ssDisc);
    }
    else
        curDistancias3->setPen(QPen(QColor(45,220,234),2));

    auto *curDistancias5 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curDistancias5->setName("Distancias");
    curDistancias5->setData(xlast, ylast);
    if(xlast.length()==1)
    {
        curDistancias5->setPen(QPen(Qt::blue,1));
        curDistancias5->setScatterStyle(QCPScatterStyle::ssDisc);
    }
    else
        curDistancias5->setPen(QPen(Qt::blue,2));
}

void c_GrafPlazoleta::mouseMove(QMouseEvent *event)
{
    int AzmObj = 0;

    double x = (ui->widget->xAxis->pixelToCoord(event->pos().x()));
    double y = (ui->widget->yAxis->pixelToCoord(event->pos().y()));

    double distancia = danca->Distancia(x,y)*1000/40;

    double xx = fabs(x);
    double yy = fabs(y);
    double cita = (180/M_PI)*atan2(yy,xx);
    if(x >= 0 && y < 0)//cuadrante 2
        AzmObj =(90 + cita);
    if(x > 0.0 && y >= 0.0)//cuadrante 1
        AzmObj =(90 - cita);
    if(x < 0.0 && y >= 0.0)//cuadrante 4
        AzmObj =(270 + cita);
    if(x < 0.0 && y <= 0.0)//cuadrante 3
        AzmObj =(270 - cita);

    QString punto = "Distancia:"+QString::number(distancia)+"m; Azimut:"+QString::number(AzmObj)+"°";
    ui->widget->setToolTip(punto);
}

void c_GrafPlazoleta::cerrartabla(int number)
{
    Q_UNUSED(number);
    ui->checkBox->setChecked(false);
}

void c_GrafPlazoleta::salvarTablas(/*bool salvar*/)
{
    QString aux;
    aux = fichero;
    aux.replace(":", "" );
    //    QString eliover;
    //    eliover = "<table><tr></tr>";
    QString heathers;
    QString titulo;
    if(banda == 0)
        titulo = "Caracterización de la plazoleta. Banda Métrica-Posición: "+fichero;

    else
        titulo = "Caracterización de la plazoleta. Banda Decimétrica-Posición: "+fichero;

    QString nom;

    if(banda == 0)
        nom = (QDir::currentPath()+"/CalificaciónERL/"+aux+"BandaMétrica");
    else
        nom = (QDir::currentPath()+"/CalificaciónERL/"+aux+"BandaDecimétrica");

    QString fileName = QFileDialog::getSaveFileName(this, "Exportar a DOC",nom,"*.doc",nullptr,nullptr);
    if (!fileName.isEmpty())
    {
        QFile f(fileName);
        if(f.open(QFile::WriteOnly))
        {
            QTextStream arr(&f);

            arr << "<html>" <<
                   "<body>" <<
                   "<table align=center border=1 bordercolor=0 cellspacing=0 cellpadding=3 style='border-collapse:collapse;font-size:9.0pt;font-family:Microsoft Sans Serif'>" <<
                   "<tr>" <<
                   "<td align=center colspan=10><b>" + titulo + "</b></td>" <<
                   "</tr>";
            heathers = "Azimut";
            arr << "<tr>" <<
                   "<td align=center><b>" + heathers + "</b></td>";

            heathers = "Extensión de la plazoleta";
            arr << "<td align=center><b>" + heathers;
            heathers = "(X <= 2*DmáxTeórico)";
            arr << "<br>"+ heathers + "</b></td>";

            heathers = "Calificación";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "Participación del terreno";
            arr << "<td align=center><b>" +heathers;
            heathers = "(X >= 70%)";
            arr << "<br>"+ heathers + "</b></td>";

            heathers = "Calificación";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "Ángulos de incidencia";
            arr << "<td align=center><b>" + heathers;
            heathers = "(X <= 4°)";
            arr << "<br>"+ heathers + "</b></td>";

            heathers = "Calificación";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "Uniformidad del terreno";
            arr << "<td align=center><b>" + heathers;
            heathers = "(X >= 80%)";
            arr << "<br>"+ heathers + "</b></td>";

            heathers = "Calificación";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "Calificación general";
            arr << "<td align=center><b>" + heathers + "</b></td>" <<
                   "</tr>" ;

            for (int i = 0; i < tabla->rowCount(); ++i)
            {
                arr << "<tr>" <<
                       "<td align=center>" + tabla->item(i,0)->text() + "</td>" <<
                       "<td align=center>" + tabla->item(i,1)->text() + "</td>" <<
                       "<td align=center>" + tabla->item(i,2)->text() + "</td>" <<
                       "<td align=center>" + tabla->item(i,3)->text() + "</td>" <<
                       "<td align=center>" + tabla->item(i,4)->text() + "</td>" <<
                       "<td align=center>" + tabla->item(i,5)->text() + "</td>" <<
                       "<td align=center>" + tabla->item(i,6)->text() + "</td>" <<
                       "<td align=center>" + tabla->item(i,7)->text() + "</td>" <<
                       "<td align=center>" + tabla->item(i,8)->text() + "</td>" <<
                       "<td align=center>" + tabla->item(i,9)->text() + "</td>" <<
                       "</tr>" ;
            }
            arr <<  "</table>" <<
                    "</body>" <<
                    "</html>";
            f.close();
        }
    }
}
void c_GrafPlazoleta::mouseWheel(QWheelEvent *event)
{
    Q_UNUSED(event);
    if (ui->widget->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->widget->axisRect()->setRangeZoom(ui->widget->xAxis->orientation());
    else if (ui->widget->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->widget->axisRect()->setRangeZoom(ui->widget->yAxis->orientation());
    else
        ui->widget->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void c_GrafPlazoleta::on_pB_ZoomMas_clicked()
{
  CustomPlotF customplotf;
  customplotf.zoommas(ui->widget);
}

void c_GrafPlazoleta::on_pB_ZoomMen_clicked()
{
  CustomPlotF customplotf;
  customplotf.zoommenos(ui->widget);
}
void c_GrafPlazoleta::filePrintPdf(bool cual)
{
    QString nom;
    QString aux;
    aux = fichero;
    aux.replace(":", "" );
    if(cual )
    {
        nom = (QDir::currentPath()+"/GraficasPDF/"+aux+"_GPlaz");
        CustomPlotF customplotf;
        customplotf.filePrintPdf(ui->widget,nom);
    }
}

void c_GrafPlazoleta::llenartabla(bool llenar)
{
    int ang;
    if(llenar)
    {
        califGeneral.clear();
        tabla->setColumnCount(10);
        tabla->setRowCount(360);
        tabla->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

        tabla->verticalHeader()->setVisible(false);
        tabla->setEditTriggers(nullptr);
        for (int i = 0; i < 360; i++)
        {
            double porciento = 0;
            tablaitem = new QTableWidgetItem();//azimut
            tablaitem->setText(QString::number(i));
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,0,tablaitem);

            //extension de la plazoleta listdistplazoleta[360]***********************
            QString extensionplazoleta = extensionPlazoleta[i].at(0);
            QStringList aux = extensionplazoleta.split(",");
            const QString &extencion = aux.at(0);
           const QString &calificacion = aux.at(1);
            tablaitem = new QTableWidgetItem();
            tablaitem->setText(extencion);
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,1,tablaitem);

            tablaitem = new QTableWidgetItem();//calificacion
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tablaitem->setText(calificacion);
            tabla->setItem(i,2,tablaitem);

            //participacion del terreno-- sumar las distancias de los segmentos de no participación
            tablaitem = new QTableWidgetItem();
            tablaitem->setTextAlignment(Qt::AlignCenter);
            if(listdistplazoleta[i].length()>=3)
            {
                porciento = (listdistplazoleta[i].at(2) - sumDisInternas.at(i))/listdistplazoleta[i].at(2)*100;
                if(porciento >= 70)
                    tablaitem->setText(QString::number(porciento));
                else
                    tablaitem->setText(QString::number(porciento));
            }
            else
                tablaitem->setText("--");


            tabla->setItem(i,3,tablaitem);

            tablaitem = new QTableWidgetItem();//calificacion
            tablaitem->setTextAlignment(Qt::AlignCenter);
            if(listdistplazoleta[i].length()>=3)
            {
                porciento = (listdistplazoleta[i].at(2) - sumDisInternas.at(i))/listdistplazoleta[i].at(2)*100;
                if(porciento >= 70)
                    tablaitem->setText(" Corresponde");
                else
                    tablaitem->setText(" No corresponde");
            }
            else
                tablaitem->setText(" No corresponde");

            tabla->setItem(i,4,tablaitem);

            //angulos de incidencia**************************************
            tablaitem = new QTableWidgetItem();
            tablaitem->setTextAlignment(Qt::AlignCenter);
            if(!anguloincidencia[i].isEmpty())
            {
                tablaitem->setText(QString::number(anguloincidencia[i].at(0)));
            }
            else
                tablaitem->setText("--");
            tabla->setItem(i,5,tablaitem);

            tablaitem = new QTableWidgetItem();//calificacion
            tablaitem->setTextAlignment(Qt::AlignCenter);
            if(!anguloincidencia[i].isEmpty())
            {
                if(banda == 0)
                {
                    if(antena<=2)
                        ang = 6;
                    else if(antena<=8)
                        ang = 4;
                    else
                        ang = 2;
                    if(anguloincidencia[i].at(0) <=ang)
                        tablaitem->setText("Corresponde");
                    else
                        tablaitem->setText("No corresponde");
                }
                else
                {
                    ang = 2;
                    if(anguloincidencia[i].at(0) <=ang)
                        tablaitem->setText("Corresponde");
                    else
                        tablaitem->setText("No corresponde");
                }
            }
            else
                tablaitem->setText("No corresponde");

            tabla->setItem(i,6,tablaitem);

            //uniformidad del terreno*******************************

            QString deformacionaux = deformaciones[i].at(0);
            QStringList auxdeformacion = deformacionaux.split(",");
            const QString &deformacion = auxdeformacion.at(0);
            const QString &calificacionDeform = auxdeformacion.at(1);

            tablaitem = new QTableWidgetItem();
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tablaitem->setText(deformacion);
            tabla->setItem(i,7,tablaitem);

            tablaitem = new QTableWidgetItem();//calificacion
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tablaitem->setText(calificacionDeform);
            tabla->setItem(i,8,tablaitem);

            //calificación general*****************************************
            tablaitem = new QTableWidgetItem();
            tablaitem->setTextAlignment(Qt::AlignCenter);
            if(tabla->item(i,2)->text().contains("No corresponde") || tabla->item(i,4)->text().contains("No corresponde") || tabla->item(i,6)->text().contains("No corresponde") || tabla->item(i,8)->text().contains("Irregular"))
            {
                tablaitem->setText("No corresponde");
                califGeneral.append("No corresponde");
            }
            else
            {
                tablaitem->setText("Corresponde");
                califGeneral.append("Corresponde");
            }
            tabla->setItem(i,9,tablaitem);
        }
        QFont font;
        font.setBold(true);
        font.setFamily("Cursive");
        font.setPointSize(5);
        //		tabla->setFont(font);
        //		tabla->horizontalHeader()->setFont(font);
        tabla->setHorizontalHeaderLabels(QString("Azimut;Extensión de la plazoleta\n X <= 2*DmáxTeórico;Calificación;Participación del terreno\n X >= 70%;Calificación;Ángulos de incidencia\n X <="+QString::number(ang)+"°;Calificación;Uniformidad del terreno\n X >= 80%;Calificación;Calificación general").split(";"));
        tabla->resizeColumnsToContents();
        //        qDebug()<<"califGeneral"<<califGeneral;
    }
    else
    {
        tabla->clear();
        //        tabla->close();
        dialogTabla->close();
    }
}

void c_GrafPlazoleta::pintarCalifGeneral()
{
    QVector<double> x1;
    QVector<double> y1;
    for (int a = 0; a <= 360; a++)
    {
        if(a < 360)
        {
            if(listdistplazoleta[a].length() >= 3)
            {
                if(califGeneral.at(a) == "Corresponde")
                {
                    x1.append(listdistplazoleta[a].at(2)* 0.04 * sin(a * RADIAN));
                    y1.append(listdistplazoleta[a].at(2)* 0.04 * cos(a * RADIAN));
                }
                else
                {
                    auto *curCorresponde = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
                    curCorresponde->setName("Corresponde");
                    curCorresponde->setData(x1, y1);
                    if(x1.length()==1)
                    {
                        curCorresponde->setPen(QPen(QColor(0,220,0),2));
                        curCorresponde->setScatterStyle(QCPScatterStyle::ssDisc);
                    }
                    else
                        curCorresponde->setPen(QPen(QColor(0,220,0),5));

                    x1.clear();
                    y1.clear();
                }
            }
            else
            {
                auto *curCorresponde = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
                curCorresponde->setName("Corresponde");
                curCorresponde->setData(x1, y1);
                if(x1.length()==1)
                {
                    curCorresponde->setPen(QPen(QColor(0,220,0),2));
                    curCorresponde->setScatterStyle(QCPScatterStyle::ssCircle);
                }
                else
                    curCorresponde->setPen(QPen(QColor(0,220,0),5));

                x1.clear();
                y1.clear();
            }
        }//360
        else
        {
            if(listdistplazoleta[0].length() >= 3)
            {
                if(califGeneral.at(0) == "Corresponde")
                {
                    x1.append(listdistplazoleta[0].at(2)* 0.04 * sin(0 * RADIAN));
                    y1.append(listdistplazoleta[0].at(2)* 0.04 * cos(0 * RADIAN));
                }
                else
                {
                    auto *curCorresponde = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
                    curCorresponde->setName("Corresponde");
                    curCorresponde->setData(x1, y1);

                    if(x1.length()==1)
                    {
                        curCorresponde->setPen(QPen(QColor(0,220,0),2));
                        curCorresponde->setScatterStyle(QCPScatterStyle::ssCircle);
                    }
                    else
                        curCorresponde->setPen(QPen(QColor(0,220,0),5));


                    x1.clear();
                    y1.clear();
                }
            }
            else
            {
                auto *curCorresponde = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
                curCorresponde->setName("Corresponde");
                curCorresponde->setData(x1, y1);
                if(x1.length()==1)
                {
                    curCorresponde->setPen(QPen(QColor(0,220,0),2));
                    curCorresponde->setScatterStyle(QCPScatterStyle::ssCircle);
                }
                else
                    curCorresponde->setPen(QPen(QColor(0,220,0),5));

                x1.clear();
                y1.clear();
            }
        }
    }
    auto *curCorresponde = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curCorresponde->setName("Corresponde");
    curCorresponde->setData(x1, y1);
    if(x1.length()==1)
    {
        curCorresponde->setPen(QPen(QColor(0,220,0),2));
        curCorresponde->setScatterStyle(QCPScatterStyle::ssCircle);
    }
    else
        curCorresponde->setPen(QPen(QColor(0,220,0),5));

    x1.clear();
    y1.clear();

    //	for (int i = 0; i < plotables.length(); ++i)
    //	{
    //		ui->widget->addPlottable(plotables.at(i));
    //	}
    //	for (int i = 0; i < item.length(); ++i)
    //	{
    //		ui->widget->addItem(item.at(i));
    //	}

    ui->widget->replot();
}
void c_GrafPlazoleta::on_pB_SalvarGrafica_clicked()
{
    filePrintPdf(true);
}

void c_GrafPlazoleta::on_checkBox_toggled(bool checked)
{
    if(checked)
    {
        dialogTabla->show ();
    }
    else
        dialogTabla->close();
}

void c_GrafPlazoleta::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    for (auto & deformacione: deformaciones)//    for(int a = 0; a < 360; a++)
    {
        for (int i = 0; i < deformacione.length(); ++i)
        {
            deformacione[i].clear();
        }
    }
    llenartabla(false);
    ui->checkBox->setChecked(false);
}
