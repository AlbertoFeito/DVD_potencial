#include "distanciavd.h"
#include "ui_distanciavd.h"

DistanciaVD::DistanciaVD(int dismax, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DistanciaVD),
    m_dismax(dismax)
{
    ui->setupUi(this);
    //  setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    alcance = false;
    alvue = 0;

    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );
    ui->widget->plotLayout()->insertRow(0);
    plottitle = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(0, 0,plottitle );

    ui->chB_ZMuerta1->setVisible(false);
    ui->chB_ZMuerta2->setVisible(false);
    ui->chB_DLanzamiento->setVisible(false);
    ui->chB_DLanzMin->setVisible(false);
    ui->chB_DZDRealizanle->setVisible(false);
    ui->chB_DZDReaMin->setVisible(false);
    ui->chB_DECCMin->setVisible(false);
    connect(ui->widget,SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));

    auto *QHBbotonLayout = new QHBoxLayout;
    auto *QGridlayout = new QGridLayout;
    dialogTabla = new QDialog();
    dialogTabla->setWindowFlags(Qt::WindowMinMaxButtonsHint);
    dialogTabla->setWindowFlags(dialogTabla->windowFlags() |= Qt::WindowCloseButtonHint);
    connect(dialogTabla,SIGNAL(finished(int)),this,SLOT(cerrartabla(int)));
    headerView = new QHeaderView(Qt::Horizontal);
    tabla = new QTableWidget(dialogTabla);
    headerView->resizeSection (1,10);
    tabla->setHorizontalHeader(headerView);

    salvarTabla = new QPushButton("Salvar Tabla",dialogTabla);
    salvarTabla->setMaximumSize (100,30);
    salvarTabla->setMinimumSize (100,30);
    connect(salvarTabla,SIGNAL(clicked(bool)),this,SLOT(filePrintPdf(bool)));

    QHBbotonLayout->insertSpacing (1,dialogTabla->width()-salvarTabla->width ());
    QHBbotonLayout->addWidget(salvarTabla);
    QHBbotonLayout->insertSpacing (-1,dialogTabla->width()-salvarTabla->width ());

    QGridlayout->addLayout(QHBbotonLayout, 0, 0);
    QGridlayout->addWidget (tabla, 1, 0);

    dialogTabla->setLayout(QGridlayout);
    //tabla de probabilidad********************
    auto *QHBbotonLayoutP = new QHBoxLayout;
    auto *QGridlayoutP = new QGridLayout;
    dialogTablaP = new QDialog();
    dialogTablaP->setWindowFlags(Qt::WindowMinMaxButtonsHint);
    dialogTablaP->setWindowFlags(dialogTablaP->windowFlags() |= Qt::WindowCloseButtonHint);

    connect(dialogTablaP,SIGNAL(finished(int)),this,SLOT(cerrartablaP(int)));
    headerViewP = new QHeaderView(Qt::Horizontal);
    tablaP = new QTableWidget(dialogTablaP);
    headerViewP->resizeSection (1,10);
    tablaP->setHorizontalHeader(headerViewP);

    salvarTablaP = new QPushButton("Salvar Tabla",dialogTablaP);
    salvarTablaP->setMaximumSize (100,30);
    salvarTablaP->setMinimumSize (100,30);
    connect(salvarTablaP,SIGNAL(clicked(bool)),this,SLOT(salvaTablaP(bool)));

    QHBbotonLayoutP->insertSpacing (1,dialogTablaP->width()-salvarTablaP->width ());
    QHBbotonLayoutP->addWidget(salvarTablaP);
    QHBbotonLayoutP->insertSpacing (-1,dialogTablaP->width()-salvarTablaP->width ());



    QGridlayoutP->addLayout(QHBbotonLayoutP, 0, 0,Qt::AlignCenter);
    QGridlayoutP->addWidget (tablaP, 1, 0,Qt::AlignCenter);

    dialogTablaP->setLayout(QGridlayoutP);
    //******************************************//

    ui->widget->addLayer("Segmentos",nullptr,QCustomPlot::limBelow);
    ui->widget->addLayer("DisLanzamiento",nullptr,QCustomPlot::limBelow);
    ui->widget->addLayer("DisLanzMin",nullptr,QCustomPlot::limBelow);
    ui->widget->addLayer("DisZonReaMin",nullptr,QCustomPlot::limBelow);
    ui->widget->addLayer("DisZonarealizable",nullptr,QCustomPlot::limBelow);
    ui->widget->addLayer("DisDEteccionMin",nullptr,QCustomPlot::limBelow);
    ui->widget->addLayer("SectorPrincipal",nullptr,QCustomPlot::limBelow);
    ui->widget->layer("SectorPrincipal")->setVisible(false);
    ui->widget->layer("DisLanzamiento")->setVisible(false);
    ui->widget->layer("DisLanzMin")->setVisible(false);
    ui->widget->layer("DisZonReaMin")->setVisible(false);
    ui->widget->layer("DisZonarealizable")->setVisible(false);
    ui->widget->layer("DisDEteccionMin")->setVisible(false);
    prob = "_P:0.5";
    dedZone = 0;
    Area = 0;
    AreaZM = 0;
    AreaZM2 = 0;
    D0 = 0;D1= 0;r= 0;r2= 0;r3 = 0;
}

DistanciaVD::~DistanciaVD()
{
    for (int i = 0; i < 360; ++i)
    {
        disVDirecta[i].clear();
        disInternas[i].clear();

        ZonaMuertaMuis[i].clear();
        ZonaMuertaMuis2[i].clear();
        zonaMuertaMuisInternas[i].clear();
        zonaMuertaMuisInternas2[i].clear();
    }
    delete ui;
}

void DistanciaVD::initGraph()
{
    ui->widget->yAxis->setRange(-600,600);//se estabelece rango de y
    ui->widget->xAxis->setRange(-600,600);
    ui->widget->xAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->yAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->xAxis->setVisible(false);
    ui->widget->yAxis->setVisible(false);

    for (int a = 0; a < 360; a++)
    {
        disInternas[a].clear();
        disVDirecta[a].clear();
    }

    customplotf.pintaLineaAzimut(ui->widget);

    customplotf.pintaElipses(ui->widget,5,true,false);
    customplotf.pintaElipses(ui->widget,10,true,true);
    customplotf.pintaElipses(ui->widget,20,true,false);
    bool aux = true;
    bool aux2 = true;

    for (int i = 40; i <= 700; i+=20)
    {
        customplotf.pintaElipses(ui->widget,i,aux,aux2);
        aux = !aux;
        if(aux)
            aux2 = !aux2;
    }

    ui->widget->replot();
}

bool DistanciaVD::pintarDistancias(int dis)
{
    QString auxF = fichero;
    if(alcance)
    {
        auxF.insert (6,"a");
    }

    if(grupo == 1)//pov pop erl
    {
        setWindowTitle("D.V.D.P. a "+ QString::number(alvue)+" m de Altura del Objetivo");
        if(medio >= 0 && medio <=35 )
        {
            plottitle->setText("D.V.D.P. a "+ QString::number(alvue)+"m de Altura del Objetivo Posición: "+label);
            titulo = "D.V.D.P a "+ QString::number(alvue)+"m de Altura del Objetivo\n Posición: "+label+"\n";}
        else
        {
            plottitle->setText("D.V.D.P. a "+ QString::number(alvue)+"m de Altura del Objetivo Posición: "+label + prob);
            titulo = "D.V.D.P a "+ QString::number(alvue)+"m de Altura del Objetivo\n Posición: "+label+ prob+"\n";
        }
        ui->label_2->setText("D.V.D.P. a "+ QString::number(alvue)+"m de Altura del Objetivo Posición: "+auxF);
    }
    if(grupo == 2)//aaa
    {
        setWindowTitle("Z.P.F.A.A.A. a "+ QString::number(alvue)+" m de Altura del Objetivo");
        plottitle->setText("Z.P.F.A.A.A. a "+ QString::number(alvue)+"m de Altura del Objetivo Posición: "+label);
        ui->label_2->setText("Z.P.F.A.A.A. a "+ QString::number(alvue)+"m de Altura del Objetivo Posición: "+auxF);
        titulo = "Z.P.F.A.A.A a "+ QString::number(alvue)+"m de Altura del Objetivo\n Posición: "+label+"\n";
    }

    if(grupo == 3)//caa
    {
        setWindowTitle("Z.P.D.C.A.A. a "+ QString::number(alvue)+" m de Altura del Objetivo");
        plottitle->setText("Z.P.D.C.A.A. a "+ QString::number(alvue)+"m de Altura del Objetivo Posición: "+label);
        ui->label_2->setText("Z.P.D.C.A.A. a "+ QString::number(alvue)+"m de Altura del Objetivo Posición: "+auxF);
        titulo = "Z.P.D.C.A.A. a "+ QString::number(alvue)+"m de Altura del Objetivo\n Posición: "+label+"\n";
    }

    QVector<double> x1; QVector<double> y1;
    QString datostabla;

    double xIni = 0;
    double yIni = 0;
    double xFin = 0;
    double yFin = 0;
    for (int a = 0; a <= 360; a++)
    {
        datostabla.clear();
        if(a < 360)
        {
            x1.append(disVDirecta[a].at(dis)/1000 * sin(a * RADIAN));
            y1.append(disVDirecta[a].at(dis)/1000 * cos(a * RADIAN));
            if(disInternas[a].length() >= 1)//al menos un punto interno
            {
                //                datostabla = QString::number(disVDirecta[a].at(dis)/1000)+ "----> DisInternas(km): ";

                for (int i = 0; i < disInternas[a].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        xIni= disInternas[a].at(i)/1000 * sin(a * RADIAN);
                        yIni = disInternas[a].at(i)/1000 * cos(a * RADIAN);

                        if(i < disInternas[a].length()-1 )
                        {
                            xFin = disInternas[a].at(i+1)/1000 * sin(a * RADIAN);
                            yFin = disInternas[a].at(i+1)/1000 * cos(a * RADIAN);
                        }
                        else
                        {
                            xFin=xIni;
                            yFin=yIni;
                            x1.remove (a);
                            y1.remove (a);
                            x1.append(disInternas[a].last ()/1000*sin(a * RADIAN));
                            y1.append(disInternas[a].last ()/1000*cos(a * RADIAN));
                            disVDirecta[a].replace (dis,disInternas[a].last ());
                            disInternas[a].removeLast ();
                            //                          qDebug()<<"aaaaaaaaaaaa distanciavd";
                            i--;
                        }
                        customplotf.pintaLinea (ui->widget,"Segmentos",QPen(QColor(254,154,46),2),xIni,yIni,xFin,yFin);
                    }
                    if(i < disInternas[a].length()-1)
                        datostabla = datostabla + QString::number(disInternas[a].at(i)/1000) +", ";
                    else
                    {
                        if(!disInternas[a].isEmpty ())
                            datostabla = datostabla + QString::number(disInternas[a].at(i)/1000 );
                    }
                }
                datostabla.prepend(QString::number(disVDirecta[a].at(dis)/1000)+ "----> DisInternas(km): ");
            }
            else
                datostabla = QString::number(disVDirecta[a].at(dis)/1000);

        }
        else//360
        {
            x1.append(disVDirecta[0].at(dis)/1000*sin(0 * RADIAN));
            y1.append(disVDirecta[0].at(dis)/1000*cos(0 * RADIAN));
            if(disInternas[0].length() >= 1)
            {
                //datostabla = QString::number(disVDirecta[0].at(dis)/1000)+ "----> DisInternas(km): ";

                for (int i = 0; i < disInternas[0].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        xIni= disInternas[0].at(i)/1000 * sin(0 * RADIAN);
                        yIni = disInternas[0].at(i)/1000 * cos(0 * RADIAN);
                        if(i < disInternas[0].length()-1 )
                        {
                            xFin = disInternas[0].at(i+1)/1000 * sin(0 * RADIAN);
                            yFin = disInternas[0].at(i+1)/1000 * cos(0 * RADIAN);
                        }
                        else
                        {
                            xFin=xIni;
                            yFin=yIni;
                            x1.remove (0);
                            y1.remove (0);
                            x1.append(disInternas[0].last ()/1000*sin(0 * RADIAN));
                            y1.append(disInternas[0].last ()/1000*cos(0 * RADIAN));
                            disVDirecta[0].replace (dis,disInternas[0].last ());
                            disInternas[0].removeLast ();
                            i--;
                        }
                        customplotf.pintaLinea (ui->widget,"Segmentos",QPen(QColor(254,154,46),2),xIni,yIni,xFin,yFin);
                    }
                    if(i < disInternas[0].length()-1)
                        datostabla = datostabla + QString::number(disInternas[0].at(i)/1000) +", ";
                    else
                    {
                        if(!disInternas[0].isEmpty ())
                            datostabla = datostabla + QString::number(disInternas[0].at(i)/1000 );
                    }
                }
                datostabla.prepend(QString::number(disVDirecta[0].at(dis)/1000)+ "----> DisInternas(km): ");
            }
            else
                datostabla = QString::number(disVDirecta[0].at(dis)/1000);


        }
    }

    if((grupo == 2) || (grupo==3) || (arma == 2 && (medio >=42 && medio <=44)))
    {
        customplotf.pintaCurva (ui->widget,"Distancias",QPen(Qt::red,2),x1,y1);
    }
    else if(dis == 1)
        customplotf.pintaCurva (ui->widget,"DisLanzamiento","DisLanzamiento",QPen(QColor(10,100,120),2),QBrush(QColor(0,0,0,0)),x1,y1);

    else if(dis == 2)
        customplotf.pintaCurva (ui->widget,"DisZonarealizable","DisZonarealizable",QPen(Qt::red,2),QBrush(QColor(0,0,0,0)),x1,y1);
    else
        customplotf.pintaCurva (ui->widget,"Distancias",QPen(Qt::green,2),x1,y1);

    //  calcularArea(dis);
    return true;
}

bool DistanciaVD::pintarObjetoLocal(int dis)
{
    setWindowTitle("Objetos locales");
    plottitle->setText("Objetos locales: Posición: "+label);
    titulo = "Objetos locales: Posición:  "+label+"\n";
    ui->label_2->setText("Objetos locales: Posición:  "+fichero);

    QVector<double> x1; QVector<double> y1;
    QString datostabla;

    double xIni = 0;
    double yIni = 0;
    double xFin = 0;
    double yFin = 0;

    for (int a = 0; a <= 360; a++)
    {
        datostabla.clear();
        if(a < 360)
        {
            x1.append(disVDirecta[a].at(dis)/1000 * sin(a * RADIAN));
            y1.append(disVDirecta[a].at(dis)/1000 * cos(a * RADIAN));
            if(disInternas[a].length() >= 1)//al menos un punto interno
            {
                //                datostabla = QString::number(disVDirecta[a].at(dis)/1000)+ "----> DisInternas(km): ";
                disInternas[a].removeFirst();
                disInternas[a].removeLast();
                for (int i = 0; i < disInternas[a].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        xIni= disInternas[a].at(i)/1000 * sin(a * RADIAN);
                        yIni = disInternas[a].at(i)/1000 * cos(a * RADIAN);

                        if(i < disInternas[a].length()-1 )
                        {
                            xFin = disInternas[a].at(i+1)/1000 * sin(a * RADIAN);
                            yFin = disInternas[a].at(i+1)/1000 * cos(a * RADIAN);
                        }
                        else
                        {
                            //                            xFin=disVDirecta[a].at(dis)/1000 * sin(a * RADIAN);
                            //                            yFin=disVDirecta[a].at(dis)/1000 * cos(a * RADIAN);
                            //                            x1.remove (a);
                            //                            y1.remove (a);
                            //                            x1.append(xFin);
                            //                            y1.append(yFin);
                            //                            disVDirecta[a].replace (dis,disInternas[a].last ());
                            //                            disInternas[a].removeLast ();
                            //                          qDebug()<<"aaaaaaaaaaaa distanciavd";
                            //                            i--;
                        }
                        customplotf.pintaLinea (ui->widget,"Segmentos",QPen(QColor(254,154,46),2),xIni,yIni,xFin,yFin);
                    }
                    if(i < disInternas[a].length()-1)
                        datostabla = datostabla + QString::number(disInternas[a].at(i)/1000) +", ";
                    else
                    {
                        if(!disInternas[a].isEmpty ())
                            datostabla = datostabla + QString::number(disInternas[a].at(i)/1000 );
                    }
                }
                datostabla.prepend(QString::number(disVDirecta[a].at(dis)/1000)+ "----> DisInternas(km): ");
            }
            else
                datostabla = QString::number(disVDirecta[a].at(dis)/1000);

        }
        else//360
        {
            x1.append(disVDirecta[0].at(dis)/1000*sin(0 * RADIAN));
            y1.append(disVDirecta[0].at(dis)/1000*cos(0 * RADIAN));
            if(disInternas[0].length() >= 1)
            {
                //                disInternas[a].removeFirst();
                //                disInternas[a].removeLast();
                //datostabla = QString::number(disVDirecta[0].at(dis)/1000)+ "----> DisInternas(km): ";

                for (int i = 1; i < disInternas[0].length(); i++)
                {
                    if((i)%2 == 0)
                    {
                        xIni= disInternas[0].at(i)/1000 * sin(0 * RADIAN);
                        yIni = disInternas[0].at(i)/1000 * cos(0 * RADIAN);
                        if(i < disInternas[0].length()-1 )
                        {
                            xFin = disInternas[0].at(i+1)/1000 * sin(0 * RADIAN);
                            yFin = disInternas[0].at(i+1)/1000 * cos(0 * RADIAN);
                        }
                        else
                        {
                            //                            xFin=disVDirecta[0].at(dis)/1000 * sin(0 * RADIAN);
                            //                            yFin=disVDirecta[0].at(dis)/1000 * cos(0 * RADIAN);
                            //                            x1.remove (0);
                            //                            y1.remove (0);
                            //                            x1.append(xFin);
                            //                            y1.append(yFin);
                            //                            disVDirecta[0].replace (dis,disInternas[0].last ());
                            //                            disInternas[0].removeLast ();
                            //                            i--;
                        }
                        customplotf.pintaLinea (ui->widget,"Segmentos",QPen(QColor(254,154,46),2),xIni,yIni,xFin,yFin);
                    }
                    if(i < disInternas[0].length()-1)
                        datostabla = datostabla + QString::number(disInternas[0].at(i)/1000) +", ";
                    else
                    {
                        if(!disInternas[0].isEmpty ())
                            datostabla = datostabla + QString::number(disInternas[0].at(i)/1000 );
                    }
                }
                datostabla.prepend(QString::number(disVDirecta[a].at(dis)/1000)+ "----> DisInternas(km): ");
            }
            else
                datostabla = QString::number(disVDirecta[0].at(dis)/1000);
        }
    }
    customplotf.pintaCurva (ui->widget,"Distancias",QPen(Qt::green,2),x1,y1);
    return true;
}

void DistanciaVD::salvaTablaP(bool b)
{
    Q_UNUSED (b);
    QString nom;
    QString aux;
    QString aux2;
    QString auxF = fichero;
    if(alcance)
    {
        auxF.insert (6,"a");
    }
    aux = auxF;
    aux2 = prob;

    aux.replace(":", "" );
    //    aux.replace(".", "_" );

    aux2.replace(":", "" );
    aux2.replace(".", "" );
    if(!(medio >= 0 && medio <=35 ))
        aux = aux+aux2;

    if(aux.at (4) =='0')//pov
        aux.insert (4,"0");
    QString heathers;
    QString datostabla;
    QStringList datostablalist;
    aux.insert (24,"A_");
    nom = (QDir::currentPath()+"/TablasPDF/"+aux);

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
                   "<tr>";
            arr << "<td align=center colspan=6><b> Comparación de áreas: "+aux+"</b></td>";
            arr << "</tr>";

            heathers = "Zona";
            arr << "<tr>" <<
                   "<td align=center><b>" + heathers + "</b></td>";

            heathers = "ÁreaCubierta";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "ÁreaCircunferencia";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "%";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "ÁreaCuadrado";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "%";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            arr << "</tr>" ;
            for (int i = 0; i < datosProbabilidad.length (); ++i)
            {
                datostabla.clear();
                datostablalist.clear ();
                datostabla = datosProbabilidad.at (i);
                datostablalist.append (datostabla.split (','));
                arr << "<tr>" <<
                       "<td align=center>" +datostablalist[0] + "</td>" <<
                       "<td align=center>" +datostablalist[1] + "</td>" <<
                       "<td align=center>" +datostablalist[2] + "</td>" <<
                       "<td align=center>" +datostablalist[3] + "</td>" <<
                       "<td align=center>" +datostablalist[4] + "</td>" <<
                       "<td align=center>" +datostablalist[5] + "</td>" ;
                arr << "</tr>" ;
            }
            arr <<  "</table>" <<
                    "</body>" <<
                    "</html>";
            f.close();
        }
    }
}

void DistanciaVD::filePrintPdf(bool cual)
{
    QString nom;
    QString aux;
    QString aux2;
    QString auxF = fichero;
    if(alcance)
    {
        auxF.insert (6,"a");
    }
    aux = auxF;
    aux2 = prob;

    aux.replace(":", "" );
    //    aux.replace(".", "_" );

    aux2.replace(":", "" );
    aux2.replace(".", "" );

    if(!(medio >= 0 && medio <=35 ))
        aux = aux+aux2;

    if(aux.at (4) =='0')//pov
        aux.insert (4,"0");
    QString heathers;
    QString datostabla;

    if(cual == 0)
    {
        aux.insert (24,"T");
        nom = (QDir::currentPath()+"/TablasPDF/"+aux);

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
                       "<tr>";
                if(medio == 41 || medio == 48)
                    arr << "<td align=center colspan=7><b>" + titulo + "</b></td>";

                else  if((medio == 39 && alvue >=100) && grupo == 1)
                    arr << "<td align=center colspan=7><b>" + titulo + "</b></td>";
                else  if((medio == 38 && alvue >=25) && grupo == 1)
                    arr << "<td align=center colspan=7><b>" + titulo + "</b></td>";
                else  if((((medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33
                            || medio == 32 || medio == 31)&& alvue >=25) || medio == 42 || medio == 43
                          || medio == 44) && grupo == 1)
                    arr << "<td align=center colspan=5><b>" + titulo + "</b></td>";

                else
                    arr << "<td align=center colspan=3><b>" + titulo + "</b></td>";
                arr << "</tr>";

                heathers = "Azimut(°)";
                arr << "<tr>" <<
                       "<td align=center><b>" + heathers + "</b></td>";

                heathers = "Distancia Máxima (Km)";
                arr << "<td align=center><b>" + heathers + "</b></td>";

                heathers = "S.N.V. (Km)";
                arr << "<td align=center><b>" + heathers + "</b></td>";
                //*************************//
                if((medio == 39 && alvue >=100) && grupo == 1)
                {
                    heathers = "ZonaMuerta -3°(Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "S.N.V.Z.M. -3°(Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "D.Z.L. (Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "D.Z.R. (Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";
                }
                if((medio == 38 && alvue >=25) && grupo == 1)//38
                {
                    heathers = "ZonaMuerta -2°(Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "S.N.V.Z.M. -2°(Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "D.Z.L. (Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "D.Z.R. (Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";
                }
                if(((medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 27 || medio == 26 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21  || medio == 42 || medio == 43 || medio == 44)&& alvue >=25) && grupo == 1)//37 y 36
                {
                    //                   heathers = "ZonaMuerta1";
                    //                   arr << "<td align=center><b>" + heathers + "</b></td>";

                    //                   heathers = "S.N.V.Z.M.1";
                    //                   arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "D.Z.L. (Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "D.Z.R. (Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";
                }
                /**************************************************************************/
                if(medio == 41 || medio == 48)
                {
                    heathers = "ZonaMuerta 1°(Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "S.N.V.Z.M. 1°(Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "ZonaMuerta -1°(Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";

                    heathers = "S.N.V.Z.M. -1°(Km)";
                    arr << "<td align=center><b>" + heathers + "</b></td>";
                }
                /**************************************************************************/
                arr << "</tr>" ;

                for (int a = 0; a < 360; ++a)
                {
                    datostabla.clear();
                    arr << "<tr>" <<
                           "<td align=center>" +QString::number(a) + "</td>" <<
                           "<td align=center>" + QString::number(disVDirecta[a].at(0)/1000)+ "</td>";
                    for (int i = 0; i < disInternas[a].length(); i++)
                    {
                        if(i < disInternas[a].length()-1)
                            datostabla = datostabla + QString::number(disInternas[a].at(i)/1000) +", ";
                        else
                            datostabla = datostabla + QString::number(disInternas[a].at(i)/1000 );
                    }
                    arr << "<td align=center>" + datostabla + "</td>";
                    datostabla.clear();
                    //********************//
                    if((medio == 39 && alvue >=100) && grupo == 1)
                    {
                        arr <<"<td align=center>" + QString::number(ZonaMuertaMuis2[a].at(0)/1000)+ "</td>";
                        if(ZonaMuertaMuis2[a].length()>=2)
                        {
                            datostabla = QString::number(ZonaMuertaMuis2[a].at(1)/1000) ;
                            for (int j = 2; j < ZonaMuertaMuis2[a].length(); j++)
                            {
                                if(j < ZonaMuertaMuis2[a].length()-1)
                                    datostabla = datostabla+", " + QString::number(ZonaMuertaMuis2[a].at(j)/1000) +", ";
                                else
                                    datostabla = datostabla+", " + QString::number(ZonaMuertaMuis2[a].at(j)/1000 );
                            }
                        }
                        else
                            datostabla = "--";
                        arr << "<td align=center>" + datostabla + "</td>";
                        //dzl y dlr***********************************
                        arr << "<td align=center>" + QString::number(disVDirecta[a].at(1)/1000)+ "</td>"<<
                               "<td align=center>" + QString::number(disVDirecta[a].at(2)/1000)+ "</td>";
                    }
                    if((medio == 38 && alvue >=25) && grupo == 1)
                    {
                        arr <<"<td align=center>" + QString::number(ZonaMuertaMuis2[a].at(0)/1000)+ "</td>";
                        if(ZonaMuertaMuis2[a].length()>=2)
                        {
                            datostabla = QString::number(ZonaMuertaMuis2[a].at(1)/1000) ;
                            for (int j = 2; j < ZonaMuertaMuis2[a].length(); j++)
                            {
                                if(j < ZonaMuertaMuis2[a].length()-1)
                                    datostabla = datostabla +", "+ QString::number(ZonaMuertaMuis2[a].at(j)/1000) +", ";
                                else
                                    datostabla = datostabla+", " + QString::number(ZonaMuertaMuis2[a].at(j)/1000 );
                            }
                        }
                        else
                            datostabla = "--";
                        arr << "<td align=center>" + datostabla + "</td>";
                        //dzl y dlr*************************************
                        arr << "<td align=center>" + QString::number(disVDirecta[a].at(1)/1000)+ "</td>"<<
                               "<td align=center>" + QString::number(disVDirecta[a].at(2)/1000)+ "</td>";
                    }
                    if(((medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 27 || medio == 26|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21 || medio == 42 || medio == 43 || medio == 44)&& alvue >=25) && grupo == 1)//37 y 36
                    {
                        arr << "<td align=center>" + QString::number(disVDirecta[a].at(1)/1000)+ "</td>"<<//dzl y dlr
                               "<td align=center>" + QString::number(disVDirecta[a].at(2)/1000)+ "</td>";
                    }

                    /**************************************************************************/
                    if(medio == 41 || medio == 48)
                    {
                        arr <<"<td align=center>" + QString::number(ZonaMuertaMuis[a].at(0)/1000)+ "</td>";
                        if(ZonaMuertaMuis[a].length()>=2)
                        {
                            datostabla = QString::number(ZonaMuertaMuis[a].at(1)/1000) +", ";
                            for (int j = 2; j < ZonaMuertaMuis[a].length(); j++)
                            {
                                if(j < ZonaMuertaMuis[a].length()-1)
                                    datostabla = datostabla + QString::number(ZonaMuertaMuis[a].at(j)/1000) +", ";
                                else
                                    datostabla = datostabla + QString::number(ZonaMuertaMuis[a].at(j)/1000 );
                            }
                        }
                        else
                            datostabla = "--";
                        arr << "<td align=center>" + datostabla + "</td>";
                        datostabla.clear();

                        arr <<"<td align=center>" + QString::number(ZonaMuertaMuis2[a].at(0)/1000)+ "</td>";
                        if(ZonaMuertaMuis2[a].length()>=2)
                        {
                            datostabla = QString::number(ZonaMuertaMuis2[a].at(1)/1000) +", ";
                            for (int j = 2; j < ZonaMuertaMuis2[a].length(); j++)
                            {
                                if(j < ZonaMuertaMuis2[a].length()-1)
                                    datostabla = datostabla + QString::number(ZonaMuertaMuis2[a].at(j)/1000) +", ";
                                else
                                    datostabla = datostabla + QString::number(ZonaMuertaMuis2[a].at(j)/1000 );
                            }
                        }
                        else
                            datostabla = "--";
                        arr << "<td align=center>" + datostabla + "</td>";
                    }
                    /**************************************************************************/
                    arr << "</tr>" ;
                }
                arr <<  "</table>" <<
                        "</body>" <<
                        "</html>";
                f.close();
            }
        }
    }
    else
    {
        if( aux.contains ("Tx") || aux.contains("Rx") )
            aux.insert (26,"G");
        else
            aux.insert (24,"G");
        nom = (QDir::currentPath()+"/GraficasPDF/"+aux);

        CustomPlotF customplotf;
        customplotf.filePrintPdf (ui->widget,nom);
    }
}

void DistanciaVD::pintarZonaMuerta(double zonamuerta)
{  
    dedZone = zonamuerta/1000;
    if(dedZone>0)
    {
        customplotf.pintaElipses (ui->widget,dedZone,"ZM: "+ QString::number(zonamuerta)+"m",QColor(0,0,255),false,QPen(Qt::blue,2));
    }
}

void DistanciaVD::pintarZonaMuertaMuis()
{

    ui->widget->addLayer("ZMMuis1",nullptr,QCustomPlot::limAbove);
    ui->widget->layer("ZMMuis1")->setVisible(false);
    ui->widget->addLayer("ZMMuis-1",nullptr,QCustomPlot::limAbove);
    ui->widget->layer("ZMMuis-1")->setVisible(false);

    QVector<double> x1; QVector<double> y1; QVector<double> x2; QVector<double> y2;
    double xIni = 0;
    double yIni = 0;
    double xFin = 0;
    double yFin = 0;

    AreaZM = 0;
    AreaZM2 = 0;

    for (int a = 0; a <= 360; a++)
    {
        if(a < 360)
        {
            for (int i = 0; i < ZonaMuertaMuis[a].length(); ++i)
            {
                if(ZonaMuertaMuis[a].at(i)>disVDirecta[a].at (0))
                {
                    ZonaMuertaMuis[a].removeAt(i);
                    i--;
                    if(ZonaMuertaMuis[a].isEmpty())
                        ZonaMuertaMuis[a].append(0);
                }
                else
                {
                    if(i>0)
                        zonaMuertaMuisInternas[a].append(ZonaMuertaMuis[a].at(i));
                }
            }
            for (int i = 0; i < ZonaMuertaMuis2[a].length(); ++i)
            {
                if(ZonaMuertaMuis2[a].at(i)>disVDirecta[a].at (0))
                {
                    ZonaMuertaMuis2[a].removeAt(i);
                    i--;
                    if(ZonaMuertaMuis2[a].isEmpty())
                        ZonaMuertaMuis2[a].append(0);
                }
                else
                {
                    if(i>0)
                        zonaMuertaMuisInternas2[a].append(ZonaMuertaMuis2[a].at(i));
                }
            }
            /////////////********ver angulositio*************************////////
            if(angulositio == -1 || angulositio == 0)
            {
                if(alvue == 0)
                {
                    ///////**************** +1 alvue ==0 *****************/////////////
                    if(zonaMuertaMuisInternas[a].length() >= 1)//al menos un punto interno
                    {

                        for (int i = 0; i < zonaMuertaMuisInternas[a].length(); i++)
                        {
                            if((i)%2 == 0)
                            {
                                xIni = zonaMuertaMuisInternas[a].at(i)/1000 * sin(a * RADIAN);
                                yIni = zonaMuertaMuisInternas[a].at(i)/1000 * cos(a * RADIAN);
                                //                                xFin = 0;
                                //                                yFin = 0;

                                if(i < zonaMuertaMuisInternas[a].length()-1 )
                                {
                                    xFin = zonaMuertaMuisInternas[a].at(i+1)/1000 * sin(a * RADIAN);
                                    yFin = zonaMuertaMuisInternas[a].at(i+1)/1000 * cos(a * RADIAN);
                                }
                                else
                                {
                                    xFin = zonaMuertaMuisInternas[a].last()/1000 * sin(a * RADIAN);
                                    yFin = zonaMuertaMuisInternas[a].last()/1000 * cos(a * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis1",QPen(Qt::black,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }

                    ////////////******** -1***********/////////////

                    x2.append(ZonaMuertaMuis2[a].at(0)/1000 * sin(a * RADIAN));
                    y2.append(ZonaMuertaMuis2[a].at(0)/1000 * cos(a * RADIAN));

                    if(a < 359)
                        D1 = ZonaMuertaMuis2[a+1].at (0)/1000;
                    else
                        D1 = ZonaMuertaMuis2[0].at (0)/1000;
                    r3 = (D0+D1)/2;

                    if(zonaMuertaMuisInternas2[a].length() >= 1 && zonaMuertaMuisInternas2[a].at(0) > 0)//al menos un punto interno
                    {
                        for (int i = 0; i < zonaMuertaMuisInternas2[a].length(); i++)
                        {
                            if((i)%2 == 0)
                            {
                                xIni = zonaMuertaMuisInternas2[a].at(i)/1000 * sin(a * RADIAN);
                                yIni = zonaMuertaMuisInternas2[a].at(i)/1000 * cos(a * RADIAN);
                                //                                xFin = 0;
                                //                                yFin = 0;
                                if(i < zonaMuertaMuisInternas2[a].length()-1 )
                                {
                                    xFin  = zonaMuertaMuisInternas2[a].at(i+1)/1000 * sin(a * RADIAN);
                                    yFin  = zonaMuertaMuisInternas2[a].at(i+1)/1000 * cos(a * RADIAN);
                                }
                                else
                                {
                                    xFin = zonaMuertaMuisInternas2[a].last()/1000 * sin(a * RADIAN);
                                    yFin = zonaMuertaMuisInternas2[a].last()/1000 * cos(a * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }
                }

                else/////////alvue >0
                {
                    ///////////************** +1 alvue >0***********************************//
                    x1.append(ZonaMuertaMuis[a].at(0)/1000 * sin(a * RADIAN));
                    y1.append(ZonaMuertaMuis[a].at(0)/1000 * cos(a * RADIAN));

                    if(a < 359)
                        D1 = ZonaMuertaMuis[a+1].at (0)/1000;
                    else
                        D1 = ZonaMuertaMuis[0].at (0)/1000;
                    r2 = (D0+D1)/2;

                    if(zonaMuertaMuisInternas[a].length() >= 0)//al menos un punto interno
                    {

                        for (int i = 0; i < zonaMuertaMuisInternas[a].length(); i++)
                        {
                            if((i)%2 == 0)
                            {
                                xIni = zonaMuertaMuisInternas[a].at(i)/1000 * sin(a * RADIAN);
                                yIni = zonaMuertaMuisInternas[a].at(i)/1000 * cos(a * RADIAN);
                                //                                xFin = 0;
                                //                                yFin = 0;
                                if(i < zonaMuertaMuisInternas[a].length()-1 )
                                {
                                    xFin = zonaMuertaMuisInternas[a].at(i+1)/1000 * sin(a * RADIAN);
                                    yFin = zonaMuertaMuisInternas[a].at(i+1)/1000 * cos(a * RADIAN);

                                }
                                else
                                {
                                    xFin = zonaMuertaMuisInternas[a].last()/1000 * sin(a * RADIAN);
                                    yFin = zonaMuertaMuisInternas[a].last()/1000 * cos(a * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis1",QPen(Qt::black,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }

                    if(ZonaMuertaMuis2[a].length() >= 1 && ZonaMuertaMuis2[a].at(0) > 0)//al menos un punto interno
                    {
                        for (int i = 0; i < ZonaMuertaMuis2[a].length(); i++)
                        {
                            if((i)%2 == 0)
                            {
                                xIni = ZonaMuertaMuis2[a].at(i)/1000 * sin(a * RADIAN);
                                yIni = ZonaMuertaMuis2[a].at(i)/1000 * cos(a * RADIAN);
                                //                                xFin = 0;
                                //                                yFin = 0;
                                if(i < ZonaMuertaMuis2[a].length()-1 )
                                {
                                    xFin = ZonaMuertaMuis2[a].at(i+1)/1000 * sin(a * RADIAN);
                                    yFin = ZonaMuertaMuis2[a].at(i+1)/1000 * cos(a * RADIAN);
                                }
                                else
                                {
                                    xFin = ZonaMuertaMuis2[a].last()/1000 * sin(a * RADIAN);
                                    yFin = ZonaMuertaMuis2[a].last()/1000 * cos(a * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }
                }

            }
            else////////******************    angulositio >=1
            {
                if(alvue == 0)
                {
                    ///zona muerta -1 ///
                    if(ZonaMuertaMuis2[a].last() == 0)
                    {
                        x2.append(ZonaMuertaMuis2[a].at(0)/1000 * sin(a * RADIAN));
                        y2.append(ZonaMuertaMuis2[a].at(0)/1000 * cos(a * RADIAN));

                        if(a < 359)
                            D1 = ZonaMuertaMuis2[a+1].at (0)/1000;
                        else
                            D1 = ZonaMuertaMuis2[0].at (0)/1000;
                        r3 = (D0+D1)/2;

                        xFin = disVDirecta[a].at (0)/1000 * sin(a * RADIAN);
                        yFin = disVDirecta[a].at (0)/1000 * cos(a * RADIAN);
                        customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),0,0,xFin,yFin);
                    }
                    else
                    {
                        x2.append(ZonaMuertaMuis2[a].at(0)/1000 * sin(a * RADIAN));
                        y2.append(ZonaMuertaMuis2[a].at(0)/1000 * cos(a * RADIAN));

                        if(a < 359)
                            D1 = ZonaMuertaMuis2[a+1].at (0)/1000;
                        else
                            D1 = ZonaMuertaMuis2[0].at (0)/1000;
                        r3 = (D0+D1)/2;

                        for (int i = 1; i < ZonaMuertaMuis2[a].length(); i++)
                        {
                            if((i)%2 != 0)
                            {
                                xIni = ZonaMuertaMuis2[a].at(i)/1000 * sin(a * RADIAN);
                                yIni = ZonaMuertaMuis2[a].at(i)/1000 * cos(a * RADIAN);

                                if(i < ZonaMuertaMuis2[a].length()-1 )
                                {
                                    xFin = ZonaMuertaMuis2[a].at(i+1)/1000 * sin(a * RADIAN);
                                    yFin = ZonaMuertaMuis2[a].at(i+1)/1000 * cos(a * RADIAN);
                                }
                                else
                                {
                                    xFin = disVDirecta[a].at (0)/1000 * sin(a * RADIAN);
                                    yFin = disVDirecta[a].at (0)/1000 * cos(a * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }

                    ///zona muerta +1 ///
                    if(ZonaMuertaMuis[a].last() != 0)
                    {
                        for (int i = 0; i < ZonaMuertaMuis[a].length(); i++)
                        {
                            if((i)%2 != 0)
                            {
                                xIni = ZonaMuertaMuis[a].at(i)/1000 * sin(a * RADIAN);
                                yIni = ZonaMuertaMuis[a].at(i)/1000 * cos(a * RADIAN);
                                //                                xFin = 0;
                                //                                yFin = 0;
                                if(i < ZonaMuertaMuis[a].length()-1 )
                                {
                                    xFin = ZonaMuertaMuis[a].at(i+1)/1000 * sin(a * RADIAN);
                                    yFin = ZonaMuertaMuis[a].at(i+1)/1000 * cos(a * RADIAN);

                                }
                                else
                                {
                                    xFin = disVDirecta[a].at (0)/1000 * sin(a * RADIAN);
                                    yFin = disVDirecta[a].at (0)/1000 * cos(a * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis1",QPen(Qt::black,1),xIni,yIni,xFin,yFin);
                            }
                        }

                    }
                    else
                    {
                        ///no hay cono muerto
                    }
                }
                else
                {
                    //******************   alvue > 0 22/11/2017
                    ///zona muerta -1 ///
                    if(ZonaMuertaMuis2[a].last() != 0)
                    {
                        if(ZonaMuertaMuis2[a].length()>0)
                        {
                            for (int i = 0; i < ZonaMuertaMuis2[a].length(); i++)
                            {

                                if((i)%2 == 0)
                                {
                                    xIni = ZonaMuertaMuis2[a].at(i)/1000 * sin(a * RADIAN);
                                    yIni = ZonaMuertaMuis2[a].at(i)/1000 * cos(a * RADIAN);

                                    if(i < ZonaMuertaMuis2[a].length()-1 )
                                    {
                                        xFin = ZonaMuertaMuis2[a].at(i+1)/1000 * sin(a * RADIAN);
                                        yFin = ZonaMuertaMuis2[a].at(i+1)/1000 * cos(a * RADIAN);
                                    }
                                    else
                                    {
                                        xFin = disVDirecta[a].at (0)/1000 * sin(a * RADIAN);
                                        yFin = disVDirecta[a].at (0)/1000 * cos(a * RADIAN);
                                    }
                                    customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                                }
                            }
                        }
                        else
                        {
                            xIni = ZonaMuertaMuis2[a].at(0)/1000 * sin(a * RADIAN);
                            yIni = ZonaMuertaMuis2[a].at(0)/1000 * cos(a * RADIAN);
                            xFin = disVDirecta[a].at (0)/1000 * sin(a * RADIAN);
                            yFin = disVDirecta[a].at (0)/1000 * cos(a * RADIAN);
                            customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                        }
                    }
                    else
                    {
                        //no hay intercepto
                    }
                    ///**************** zona muerta +1 *******************///
                    x1.append(ZonaMuertaMuis[a].at(0)/1000 * sin(a * RADIAN));
                    y1.append(ZonaMuertaMuis[a].at(0)/1000 * cos(a * RADIAN));

                    D0 = ZonaMuertaMuis[a].at (0)/1000;
                    if(a < 359)
                        D1 = ZonaMuertaMuis[a+1].at (0)/1000;
                    else
                        D1 = ZonaMuertaMuis[0].at (0)/1000;
                    r2 = (D0+D1)/2;

                    if(ZonaMuertaMuis[a].length() > 0)
                    {
                        for (int i = 1; i < ZonaMuertaMuis[a].length(); i++)
                        {
                            if((i)%2 != 0)
                            {
                                xIni = ZonaMuertaMuis[a].at(i)/1000 * sin(a * RADIAN);
                                yIni = ZonaMuertaMuis[a].at(i)/1000 * cos(a * RADIAN);

                                if(i < ZonaMuertaMuis[a].length()-1 )
                                {
                                    xFin = ZonaMuertaMuis[a].at(i+1)/1000 * sin(a * RADIAN);
                                    yFin = ZonaMuertaMuis[a].at(i+1)/1000 * cos(a * RADIAN);
                                }
                                else
                                {
                                    xFin = disVDirecta[a].at (0)/1000 * sin(a * RADIAN);
                                    yFin = disVDirecta[a].at (0)/1000 * cos(a * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis1",QPen(Qt::black,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }
                }
            }
        }
        else/////////*************************       a == 360 para cerrar las curvas*****************************/////////////
        {
            if(angulositio == -1 || angulositio == 0)
            {
                if(alvue == 0)
                {
                    //////////////////************ +1 alvue == 0********************////////////////

                    if(zonaMuertaMuisInternas[0].length() >= 1)//al menos un punto interno
                    {

                        for (int i = 0; i < zonaMuertaMuisInternas[0].length(); i++)
                        {
                            if((i)%2 == 0)
                            {
                                xIni = zonaMuertaMuisInternas[0].at(i)/1000 * sin(0 * RADIAN);
                                yIni = zonaMuertaMuisInternas[0].at(i)/1000 * cos(0 * RADIAN);
                                //                                xFin = 0;
                                //                                yFin = 0;
                                if(i < zonaMuertaMuisInternas[0].length()-1 )
                                {
                                    xFin = zonaMuertaMuisInternas[0].at(i+1)/1000 * sin(0 * RADIAN);
                                    yFin = zonaMuertaMuisInternas[0].at(i+1)/1000 * cos(0 * RADIAN);
                                }
                                else
                                {
                                    xFin = zonaMuertaMuisInternas[0].last()/1000 * sin(0 * RADIAN);
                                    yFin = zonaMuertaMuisInternas[0].last()/1000 * cos(0 * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis1",QPen(Qt::black,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }

                    ///////////////********** -1 ************///////////

                    x2.append(ZonaMuertaMuis2[0].at(0)/1000 * sin(0 * RADIAN));
                    y2.append(ZonaMuertaMuis2[0].at(0)/1000 * cos(0 * RADIAN));

                    if(zonaMuertaMuisInternas2[0].length() >= 1 && zonaMuertaMuisInternas2[0].at(0) > 0)//al menos un punto interno
                    {
                        for (int i = 0; i < zonaMuertaMuisInternas2[0].length(); i++)
                        {
                            if((i)%2 == 0)
                            {
                                xIni = zonaMuertaMuisInternas2[0].at(i)/1000 * sin(0 * RADIAN);
                                yIni = zonaMuertaMuisInternas2[0].at(i)/1000 * cos(0 * RADIAN);
                                //                                xFin = 0;
                                //                                yFin = 0;
                                if(i < zonaMuertaMuisInternas2[0].length()-1 )
                                {
                                    xFin = zonaMuertaMuisInternas2[0].at(i+1)/1000 * sin(0 * RADIAN);
                                    yFin = zonaMuertaMuisInternas2[0].at(i+1)/1000 * cos(0 * RADIAN);
                                }
                                else
                                {
                                    xFin = zonaMuertaMuisInternas2[0].last()/1000 * sin(0 * RADIAN);
                                    yFin = zonaMuertaMuisInternas2[0].last()/1000 * cos(0 * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }
                }
                else
                {
                    //////////******************** +1 alvue>0*******************///////
                    x1.append(ZonaMuertaMuis[0].at(0)/1000*sin(0 * RADIAN));
                    y1.append(ZonaMuertaMuis[0].at(0)/1000*cos(0 * RADIAN));

                    if(zonaMuertaMuisInternas[0].length() >= 1)//al menos un punto interno
                    {

                        for (int i = 0; i < zonaMuertaMuisInternas[0].length(); i++)
                        {
                            if((i)%2 == 0)
                            {
                                xIni = zonaMuertaMuisInternas[0].at(i)/1000 * sin(0 * RADIAN);
                                yIni = zonaMuertaMuisInternas[0].at(i)/1000 * cos(0 * RADIAN);
                                //                                xFin=0;
                                //                                yFin=0;
                                if(i < zonaMuertaMuisInternas[0].length()-1 )
                                {
                                    xFin = zonaMuertaMuisInternas[0].at(i+1)/1000 * sin(0 * RADIAN);
                                    yFin = zonaMuertaMuisInternas[0].at(i+1)/1000 * cos(0 * RADIAN);
                                }
                                else
                                {
                                    xFin = zonaMuertaMuisInternas[0].last()/1000 * sin(0 * RADIAN);
                                    yFin = zonaMuertaMuisInternas[0].last()/1000 * cos(0 * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis1",QPen(Qt::black,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }
                    ///////////////********** -1 alvue > 0 ************///////////

                    if(ZonaMuertaMuis2[0].length() >= 1 && ZonaMuertaMuis2[0].at(0) > 0)//al menos un punto interno
                    {
                        for (int i = 0; i < ZonaMuertaMuis2[0].length(); i++)
                        {
                            if((i)%2 == 0)
                            {
                                xIni = ZonaMuertaMuis2[0].at(i)/1000 * sin(0 * RADIAN);
                                yIni = ZonaMuertaMuis2[0].at(i)/1000 * cos(0 * RADIAN);
                                //                                xFin =0;
                                //                                yFin =0;
                                if(i < ZonaMuertaMuis2[0].length()-1 )
                                {
                                    xFin = ZonaMuertaMuis2[0].at(i+1)/1000 * sin(0 * RADIAN);
                                    yFin = ZonaMuertaMuis2[0].at(i+1)/1000 * cos(0 * RADIAN);

                                }
                                else
                                {
                                    xFin = ZonaMuertaMuis2[0].last()/1000 * sin(0 * RADIAN);
                                    yFin = ZonaMuertaMuis2[0].last()/1000 * cos(0 * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }
                }
            }
            else/////////////////angulositio >=1
            {
                if(alvue == 0)
                {
                    ///-1 ///
                    if(ZonaMuertaMuis2[0].last() == 0)
                    {
                        x2.append(ZonaMuertaMuis2[0].at(0)/1000 * sin(0 * RADIAN));
                        y2.append(ZonaMuertaMuis2[0].at(0)/1000 * cos(0 * RADIAN));

                        xFin = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                        yFin = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);

                        customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),0,0,xFin,yFin);
                    }
                    else
                    {
                        x2.append(ZonaMuertaMuis2[0].at(0)/1000 * sin(0 * RADIAN));
                        y2.append(ZonaMuertaMuis2[0].at(0)/1000 * cos(0 * RADIAN));

                        for (int i = 1; i < ZonaMuertaMuis2[0].length(); i++)
                        {
                            if((i)%2 != 0)
                            {
                                xIni = ZonaMuertaMuis2[0].at(i)/1000 * sin(0 * RADIAN);
                                yIni = ZonaMuertaMuis2[0].at(i)/1000 * cos(0 * RADIAN);
                                //                                xFin=0;
                                //                                yFin=0;
                                if(i < ZonaMuertaMuis2[0].length()-1 )
                                {
                                    xFin = ZonaMuertaMuis2[0].at(i+1)/1000 * sin(0 * RADIAN);
                                    yFin = ZonaMuertaMuis2[0].at(i+1)/1000 * cos(0 * RADIAN);

                                }
                                else
                                {
                                    xFin = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                                    yFin = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }
                    ///zona muerta +1 ///
                    if(ZonaMuertaMuis[0].last() != 0)
                    {

                        for (int i = 0; i < ZonaMuertaMuis[0].length(); i++)
                        {
                            if((i)%2 != 0)
                            {
                                xIni = ZonaMuertaMuis[0].at(i)/1000 * sin(0 * RADIAN);
                                yIni = ZonaMuertaMuis[0].at(i)/1000 * cos(0 * RADIAN);
                                //                                xFin = 0;
                                //                                yFin = 0;
                                if(i < ZonaMuertaMuis[0].length()-1 )
                                {
                                    xFin= ZonaMuertaMuis[0].at(i+1)/1000 * sin(0 * RADIAN);
                                    yFin= ZonaMuertaMuis[0].at(i+1)/1000 * cos(0 * RADIAN);

                                }
                                else
                                {
                                    xFin = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                                    yFin = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis1",QPen(Qt::black,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }
                    else
                    {
                        ///no hay cono muerto
                    }
                }
                else
                {
                    //******************   alvue > 0 22/11/2017
                    ///zona muerta -1 ///
                    if(ZonaMuertaMuis2[0].last() != 0)
                    {
                        if(ZonaMuertaMuis2[0].length()>0)
                        {
                            for (int i = 0; i < ZonaMuertaMuis2[0].length(); i++)
                            {

                                if((i)%2 == 0)
                                {
                                    xIni = ZonaMuertaMuis2[0].at(i)/1000 * sin(0 * RADIAN);
                                    yIni = ZonaMuertaMuis2[0].at(i)/1000 * cos(0 * RADIAN);

                                    if(i < ZonaMuertaMuis2[0].length()-1 )
                                    {
                                        xFin = ZonaMuertaMuis2[0].at(i+1)/1000 * sin(0 * RADIAN);
                                        yFin = ZonaMuertaMuis2[0].at(i+1)/1000 * cos(0 * RADIAN);
                                    }
                                    else
                                    {
                                        xFin = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                                        yFin = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);
                                    }
                                    customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                                }
                            }
                        }
                        else
                        {
                            xIni = ZonaMuertaMuis2[0].at(0)/1000 * sin(0 * RADIAN);
                            yIni = ZonaMuertaMuis2[0].at(0)/1000 * cos(0 * RADIAN);
                            xFin = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                            yFin = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);
                            customplotf.pintaLinea(ui->widget,"ZMMuis-1",QPen(Qt::yellow,1),xIni,yIni,xFin,yFin);
                        }
                    }
                    else
                    {
                        //no hay intercepto
                    }
                    ///**************** zona muerta +1 *******************///
                    x1.append(ZonaMuertaMuis[0].at(0)/1000 * sin(0 * RADIAN));
                    y1.append(ZonaMuertaMuis[0].at(0)/1000 * cos(0 * RADIAN));
                    if(ZonaMuertaMuis[0].length() > 0)
                    {
                        for (int i = 1; i < ZonaMuertaMuis[0].length(); i++)
                        {
                            if((i)%2 != 0)
                            {
                                xIni = ZonaMuertaMuis[0].at(i)/1000 * sin(0 * RADIAN);
                                yIni = ZonaMuertaMuis[0].at(i)/1000 * cos(0 * RADIAN);

                                if(i < ZonaMuertaMuis[0].length()-1 )
                                {
                                    xFin = ZonaMuertaMuis[0].at(i+1)/1000 * sin(0 * RADIAN);
                                    yFin = ZonaMuertaMuis[0].at(i+1)/1000 * cos(0 * RADIAN);
                                }
                                else
                                {
                                    xFin = disVDirecta[0].at (0)/1000 * sin(0 * RADIAN);
                                    yFin = disVDirecta[0].at (0)/1000 * cos(0 * RADIAN);
                                }
                                customplotf.pintaLinea(ui->widget,"ZMMuis1",QPen(Qt::black,1),xIni,yIni,xFin,yFin);
                            }
                        }
                    }

                }
            }
        }
        AreaZM = AreaZM + 0.00873 * pow(r2,2);
        AreaZM2 = AreaZM2 + 0.00873 * pow(r3,2);
    }


    customplotf.pintaCurva (ui->widget,"ZMMuis1","Zona Muerta MUIS1",QPen(Qt::blue,2),QBrush(QColor(0, 0, 255, 100)),x1,y1);
    customplotf.pintaCurva (ui->widget,"ZMMuis-1","Zona Muerta MUIS-1",QPen(QColor(200,0,200),2),QBrush(QColor(200, 0, 200, 100)),x2,y2);

    ui->chB_ZMuerta1->setVisible(false);
    ui->chB_ZMuerta2->setVisible(false);
    ui->widget->layer("ZMMuis1")->setVisible(true);
    ui->widget->layer("ZMMuis1")->replot();
    ui->widget->layer("ZMMuis-1")->setVisible(true);
    ui->widget->layer("ZMMuis-1")->replot();
}

void DistanciaVD::pintarZonaMuertaCCAA()//pinto los limites cercanos de la dlanza y drealizable
{
    if(medio == 39 )
    {
        if(!alcance)
        {
            if(V == 150)
            {
                if(alvue <= 1000)
                {DLCmin = 10000;
                    DRCmin = 7000;
                    switch (T) {
                    case 0:DECCM = 10300; break;
                    case 14:DECCM = 12100;break;
                    case 25:DECCM = 13800; break;
                    case 35:DECCM = 15300; break;
                    }
                }
                if(alvue == 2000)
                {DLCmin = 110000;
                    DRCmin = 8000;
                    switch (T) {
                    case 0:DECCM = 11300; break;
                    case 14:DECCM = 13100;break;
                    case 25:DECCM = 14800; break;
                    case 35:DECCM = 16300; break;

                    }
                }
                if(alvue == 4000)
                {DLCmin = 12000;
                    DRCmin = 9000;
                    switch (T) {
                    case 0:DECCM = 12300; break;
                    case 14:DECCM = 14100;break;
                    case 25:DECCM = 15800; break;
                    case 35:DECCM = 17300; break;

                    }
                }
                if(alvue == 6000)
                {DLCmin = 14000;
                    DRCmin = 10300;
                    switch (T) {
                    case 0:DECCM = 14300; break;
                    case 14:DECCM = 16100;break;
                    case 25:DECCM = 17800; break;
                    case 35:DECCM = 19300; break;

                    }
                }
                if(alvue == 10000)
                {DLCmin = 16000;
                    DRCmin = 12046;
                    switch (T) {
                    case 0:DECCM = 16300; break;
                    case 14:DECCM = 18100;break;
                    case 25:DECCM = 19800; break;
                    case 35:DECCM = 21300; break;

                    }
                }
                if(alvue == 17000)
                {DLCmin = 22000;
                    DRCmin = 17326;
                    switch (T) {
                    case 0:DECCM = 22300; break;
                    case 14:DECCM = 24100;break;
                    case 25:DECCM = 25800; break;
                    case 35:DECCM = 27300; break;

                    }
                }
                if(alvue == 30000)
                {DLCmin = 29000;
                    DRCmin = 23682;
                    switch (T) {
                    case 0:DECCM = 29300; break;
                    case 14:DECCM = 31100;break;
                    case 25:DECCM = 32800; break;
                    case 35:DECCM = 34300; break;

                    }
                }
            }
            if(V == 300)
            {
                if(alvue <= 1000)
                {DLCmin = 12000;
                    DRCmin = 7000;
                    switch (T) {
                    case 0:DECCM = 12600; break;
                    case 14:DECCM = 16000;break;
                    case 25:DECCM = 19500; break;
                    case 35:DECCM = 22500; break;

                    }
                }
                if(alvue == 2000)
                {DLCmin = 12000;
                    DRCmin = 7000;
                    switch (T) {
                    case 0:DECCM = 12600; break;
                    case 14:DECCM = 16000;break;
                    case 25:DECCM = 19500; break;
                    case 35:DECCM = 22500; break;

                    }
                }
                if(alvue == 4000)
                {DLCmin = 13000;
                    DRCmin = 7500;
                    switch (T) {
                    case 0:DECCM = 13600; break;
                    case 14:DECCM = 17000;break;
                    case 25:DECCM = 20500; break;
                    case 35:DECCM = 23500; break;

                    }
                }
                if(alvue == 6000)
                {DLCmin = 16000;
                    DRCmin = 9500;
                    switch (T) {
                    case 0:DECCM = 16600; break;
                    case 14:DECCM = 20000;break;
                    case 25:DECCM = 23500; break;
                    case 35:DECCM = 26500; break;

                    }
                }
                if(alvue == 10000)
                {DLCmin = 19000;
                    DRCmin = 11500;
                    switch (T) {
                    case 0:DECCM = 19600; break;
                    case 14:DECCM = 23000;break;
                    case 25:DECCM = 26500; break;
                    case 35:DECCM = 29500; break;

                    }
                }
                if(alvue == 17000)
                {DLCmin = 26000;
                    DRCmin = 17000;
                    switch (T) {
                    case 0:DECCM = 26600; break;
                    case 14:DECCM = 30000;break;
                    case 25:DECCM = 33500; break;
                    case 35:DECCM = 36500; break;

                    }
                }
                if(alvue == 30000)
                {DLCmin = 34000;
                    DRCmin = 24000;
                    switch (T) {
                    case 0:DECCM = 34600; break;
                    case 14:DECCM = 37000;break;
                    case 25:DECCM = 41500; break;
                    case 35:DECCM = 44500; break;

                    }
                }
            }
            if(V == 420)
            {
                if(alvue <= 1000)
                {DLCmin = 15000;
                    DRCmin = 7759;
                    switch (T) {
                    case 0:DECCM = 15900; break;
                    case 14:DECCM = 20880;break;
                    case 25:DECCM = 25500; break;
                    case 35:DECCM = 29700; break;

                    }
                }
                if(alvue == 2000)
                {DLCmin = 16000;
                    DRCmin = 8300;
                    switch (T) {
                    case 0:DECCM = 16900; break;
                    case 14:DECCM = 21880;break;
                    case 25:DECCM = 25500; break;
                    case 35:DECCM = 21300; break;

                    }
                }
                if(alvue == 4000)
                {DLCmin = 18000;
                    DRCmin = 8783;
                    switch (T) {
                    case 0:DECCM = 18900; break;
                    case 14:DECCM = 23880;break;
                    case 25:DECCM = 26500; break;
                    case 35:DECCM = 32700; break;

                    }
                }
                if(alvue == 6000)
                {DLCmin = 19000;
                    DRCmin = 9500;
                    switch (T) {
                    case 0:DECCM = 19900; break;
                    case 14:DECCM = 24880;break;
                    case 25:DECCM = 29500; break;
                    case 35:DECCM = 33700; break;

                    }
                }
                if(alvue == 10000)
                {DLCmin = 19000;
                    DRCmin = 9500;
                    switch (T) {
                    case 0:DECCM = 19900; break;
                    case 14:DECCM = 24880;break;
                    case 25:DECCM = 29500; break;
                    case 35:DECCM = 33700; break;

                    }
                }
                if(alvue == 17000)
                {DLCmin = 26000;
                    DRCmin = 15000;
                    switch (T) {
                    case 0:DECCM = 26900; break;
                    case 14:DECCM = 31880;break;
                    case 25:DECCM = 36500; break;
                    case 35:DECCM = 40700; break;

                    }
                }
                if(alvue == 30000)
                {DLCmin = 34000;
                    DRCmin = 20000;
                    switch (T) {
                    case 0:DECCM = 34900; break;
                    case 14:DECCM = 39880;break;
                    case 25:DECCM = 44500; break;
                    case 35:DECCM = 48700; break;

                    }
                }
            }
            if(V == 640)
            {
                if(alvue <= 1000)
                {DLCmin = 17000;
                    DRCmin = 7330;
                    switch (T) {
                    case 0:DECCM = 18300; break;
                    case 14:DECCM = 26000;break;
                    case 25:DECCM = 33000; break;
                    case 35:DECCM = 39400; break;

                    }
                }
                if(alvue == 2000)
                {DLCmin = 17000;
                    DRCmin = 7330;
                    switch (T) {
                    case 0:DECCM = 18300; break;
                    case 14:DECCM = 26000;break;
                    case 25:DECCM = 33000; break;
                    case 35:DECCM = 39400; break;

                    }
                }
                if(alvue == 4000)
                {DLCmin = 18000;
                    DRCmin = 8000;
                    switch (T) {
                    case 0:DECCM = 19300; break;
                    case 14:DECCM = 27000;break;
                    case 25:DECCM = 34000; break;
                    case 35:DECCM = 40400; break;

                    }
                }
                if(alvue == 6000)
                {DLCmin = 21000;
                    DRCmin = 9200;
                    switch (T) {
                    case 0:DECCM = 22300; break;
                    case 14:DECCM = 30000;break;
                    case 25:DECCM = 37000; break;
                    case 35:DECCM = 43400; break;

                    }
                }
                if(alvue == 10000)
                {DLCmin = 27000;
                    DRCmin = 12000;
                    switch (T) {
                    case 0:DECCM = 28300; break;
                    case 14:DECCM = 36000;break;
                    case 25:DECCM = 43000; break;
                    case 35:DECCM = 49400; break;

                    }
                }
                if(alvue == 17000)
                {DLCmin = 36000;
                    DRCmin = 17000;
                    switch (T) {
                    case 0:DECCM = 37300; break;
                    case 14:DECCM = 45000;break;
                    case 25:DECCM = 52000; break;
                    case 35:DECCM = 58400; break;

                    }
                }
                if(alvue == 30000)
                {DLCmin = 68000;
                    DRCmin = 35000;
                    switch (T) {
                    case 0:DECCM = 69300; break;
                    case 14:DECCM = 77000;break;
                    case 25:DECCM = 82000; break;
                    case 35:DECCM = 90400; break;

                    }
                }
            }
            if(V == 950)
            {
                if(alvue <= 1000)
                {DLCmin = 37000;
                    DRCmin = 15000;
                    switch (T) {
                    case 0:DECCM = 39000; break;
                    case 14:DECCM = 50300;break;
                    case 25:DECCM = 60800; break;
                    case 35:DECCM = 70300; break;

                    }
                }
                if(alvue == 2000)
                {DLCmin = 37000;
                    DRCmin = 16000;
                    switch (T) {
                    case 0:DECCM = 39000; break;
                    case 14:DECCM = 50300;break;
                    case 25:DECCM = 60800; break;
                    case 35:DECCM = 70300; break;

                    }
                }
                if(alvue == 4000)
                {DLCmin = 38000;
                    DRCmin = 16000;
                    switch (T) {
                    case 0:DECCM = 40000; break;
                    case 14:DECCM = 51300;break;
                    case 25:DECCM = 61800; break;
                    case 35:DECCM = 71300; break;

                    }
                }
                if(alvue == 6000)
                {DLCmin = 42000;
                    DRCmin = 17000;
                    switch (T) {
                    case 0:DECCM = 44000; break;
                    case 14:DECCM = 55300;break;
                    case 25:DECCM = 65800; break;
                    case 35:DECCM = 75300; break;

                    }
                }
                if(alvue == 10000)
                {DLCmin = 43000;
                    DRCmin = 18000;
                    switch (T) {
                    case 0:DECCM = 45000; break;
                    case 14:DECCM = 56300;break;
                    case 25:DECCM = 66800; break;
                    case 35:DECCM = 76300; break;

                    }
                }
                if(alvue == 17000)
                {DLCmin = 50000;
                    DRCmin = 22000;
                    switch (T) {
                    case 0:DECCM = 52000; break;
                    case 14:DECCM = 63300;break;
                    case 25:DECCM = 73800; break;
                    case 35:DECCM = 83300; break;

                    }
                }
                if(alvue == 30000)
                {DLCmin = 68000;
                    DRCmin = 31500;
                    switch (T) {
                    case 0:DECCM = 70000; break;
                    case 14:DECCM = 81300;break;
                    case 25:DECCM = 91800; break;
                    case 35:DECCM = 101250; break;

                    }
                }
            }
        }
        else
        {
            if(V == 150)
            {
                if(alvue <= 1000)
                {
                    DLCmin = 6000;
                    DRCmin = 7618;
                    DECCM = 9000;
                }
                if(alvue == 2000)
                {
                    DLCmin = 6000;
                    DRCmin = 7618;
                    DECCM = 8900;
                }
                if(alvue == 4000)
                {
                    DLCmin = 6000;
                    DRCmin = 7618;
                    DECCM = 8900;
                }
                if(alvue == 6000)
                {
                    DLCmin = 6000;
                    DRCmin = 7678;
                    DECCM = 8900;
                }
                if(alvue == 10000)
                {
                    DLCmin = 7000;
                    DRCmin = 9148;
                    DECCM = 10300;
                }
                if(alvue == 17000)
                {
                    DLCmin = 9000;
                    DRCmin = 11762;
                    DECCM = 13300;
                }
            }
            if(V == 300)
            {
                if(alvue <= 1000)
                {
                    DLCmin = 6000;
                    DRCmin = 9753;
                    DECCM = 10600;
                }
                if(alvue == 2000)
                {
                    DLCmin = 6000;
                    DRCmin = 9753;
                    DECCM = 11100;
                }
                if(alvue == 4000)
                {
                    DLCmin = 6000;
                    DRCmin = 9753;
                    DECCM = 12100;
                }
                if(alvue == 6000)
                {
                    DLCmin = 6000;
                    DRCmin = 9953;
                    DECCM = 12600;
                }
                if(alvue == 10000)
                {
                    DLCmin = 7000;
                    DRCmin = 12272;
                    DECCM = 16600;
                }
                if(alvue == 17000)
                {
                    DLCmin = 9000;
                    DRCmin = 15779;
                    DECCM = 20600;
                }
            }
            if(V == 420)
            {
                if(alvue <= 1000)
                {
                    DLCmin = 6000;
                    DRCmin = 12572;
                    DECCM = 12900;
                }
                if(alvue == 2000)
                {
                    DLCmin = 6000;
                    DRCmin = 12572;
                    DECCM = 12900;
                }
                if(alvue == 4000)
                {
                    DLCmin = 6000;
                    DRCmin = 12572;
                    DECCM = 13400;
                }
                if(alvue == 6000)
                {
                    DLCmin = 6000;
                    DRCmin = 13043;
                    DECCM = 13900;
                }
                if(alvue == 10000)
                {
                    DLCmin = 7000;
                    DRCmin = 16886;
                    DECCM = 17900;
                }
                if(alvue == 17000)
                {
                    DLCmin = 9000;
                    DRCmin = 21710;
                    DECCM = 24900;
                }
            }
        }
    }
    if(medio == 38)
    {
        if(!alcance)
        {
            if(V == 150)
            {
                if(alvue <= 400)
                {DLCmin = 4800;
                    DRCmin = 3500;
                    switch (T) {
                    case 0:DECCM = 5100; break;
                    case 15:DECCM = 7050;break;
                    case 25:DECCM = 8550; break;
                    }
                }
                else if(alvue <= 1000)
                {DLCmin = 4800;
                    DRCmin = 3500;
                    switch (T) {
                    case 0:DECCM = 5100; break;
                    case 15:DECCM = 7050;break;
                    case 25:DECCM = 8550; break;
                    }
                }
                else if(alvue == 2000)
                {DLCmin = 5500;
                    DRCmin = 4000;
                    switch (T) {
                    case 0:DECCM = 5800; break;
                    case 15:DECCM = 7750;break;
                    case 25:DECCM = 9250; break;

                    }
                }
                else  if(alvue == 4000)
                {DLCmin = 5500;
                    DRCmin = 4000;
                    switch (T) {
                    case 0:DECCM = 5800; break;
                    case 15:DECCM = 7750;break;
                    case 25:DECCM = 9250; break;

                    }
                }
                else  if(alvue == 6000)
                {DLCmin = 7500;
                    DRCmin = 5500;
                    switch (T) {
                    case 0:DECCM = 7800; break;
                    case 15:DECCM = 9750;break;
                    case 25:DECCM = 11250; break;

                    }
                }
                else if(alvue == 10000)
                {DLCmin = 8500;
                    DRCmin = 6300;
                    switch (T) {
                    case 0:DECCM = 8800; break;
                    case 15:DECCM = 10750;break;
                    case 25:DECCM = 12250; break;

                    }
                }
                else /*17000*/
                {DLCmin = 10500;
                    DRCmin = 7900;
                    switch (T) {
                    case 0:DECCM = 10800; break;
                    case 15:DECCM = 12750;break;
                    case 25:DECCM = 14250; break;

                    }
                }
            }
            if(V == 300)
            {
                if(alvue <= 400)
                {
                    DLCmin = 6100;
                    DRCmin = 3500;
                    switch (T)
                    {
                    case 0:DECCM = 7100; break;
                    case 15:DECCM = 11000;break;
                    case 25:DECCM = 14000; break;
                    }
                }
                else if(alvue <= 1000)
                {
                    DLCmin = 6500;
                    DRCmin = 3900;
                    switch (T) {
                    case 0:DECCM = 7100; break;
                    case 15:DECCM = 11400;break;
                    case 25:DECCM = 14400; break;
                    }
                }
                else if(alvue == 2000)
                {
                    DLCmin = 7500;
                    DRCmin = 4500;
                    switch (T) {
                    case 0:DECCM = 8100; break;
                    case 15:DECCM = 12400;break;
                    case 25:DECCM = 15400; break;

                    }
                }
                else  if(alvue == 4000)
                {
                    DLCmin = 8500;
                    DRCmin = 5000;
                    switch (T) {
                    case 0:DECCM = 9100; break;
                    case 15:DECCM =13400;break;
                    case 25:DECCM = 16400; break;

                    }
                }
                else  if(alvue == 6000)
                {
                    DLCmin = 9500;
                    DRCmin = 5800;
                    switch (T) {
                    case 0:DECCM = 10100; break;
                    case 15:DECCM = 14400;break;
                    case 25:DECCM = 17400; break;

                    }
                }
                else  if(alvue == 10000)
                {
                    DLCmin = 10500;
                    DRCmin = 6500;
                    switch (T) {
                    case 0:DECCM = 11100; break;
                    case 15:DECCM = 15400;break;
                    case 25:DECCM = 18400; break;

                    }
                }
                else /*17000*/
                {
                    DLCmin = 12500;
                    DRCmin = 7900;
                    switch (T) {
                    case 0:DECCM = 13100; break;
                    case 15:DECCM = 17400;break;
                    case 25:DECCM = 20400; break;

                    }
                }
            }
            if(V == 420)
            {
                if(alvue <= 400)
                {DLCmin = 6800;
                    DRCmin = 3500;
                    switch (T) {
                    case 25:DECCM = 17300; break;
                    case 15:DECCM = 13100;break;
                    case 0:DECCM = 7640; break;
                    }
                }
                else if(alvue <= 1000)
                {DLCmin = 6800;
                    DRCmin = 3500;
                    switch (T) {
                    case 25:DECCM = 17300; break;
                    case 15:DECCM = 13100;break;
                    case 0:DECCM = 7640; break;
                    }
                }
                else if(alvue == 2000)
                {DLCmin = 7500;
                    DRCmin = 3900;
                    switch (T) {
                    case 25:DECCM = 18000; break;
                    case 15:DECCM = 13800;break;
                    case 0:DECCM = 8340; break;

                    }
                }
                else  if(alvue == 4000)
                {DLCmin = 8500;
                    DRCmin = 4400;
                    switch (T) {
                    case 0:DECCM = 9340; break;
                    case 15:DECCM = 14800;break;
                    case 25:DECCM = 19000; break;

                    }
                }
                else  if(alvue == 6000)
                {DLCmin = 9500;
                    DRCmin = 4900;
                    switch (T) {
                    case 0:DECCM = 10340; break;
                    case 15:DECCM = 15800;break;
                    case 25:DECCM = 20000; break;

                    }
                }
                else  if(alvue == 10000)
                {DLCmin = 10500;
                    DRCmin = 5500;
                    switch (T) {
                    case 0:DECCM = 11340; break;
                    case 15:DECCM = 16800;break;
                    case 25:DECCM = 21000; break;

                    }
                }
                else /*17000*/
                {DLCmin = 0;
                    DRCmin = 0;
                    switch (T) {
                    case 0:DECCM = 0; break;
                    case 15:DECCM = 0;break;
                    case 25:DECCM = 0; break;

                    }
                }
            }
            if(V == 640)
            {
                if(alvue <= 400)
                {DLCmin = 0;
                    DRCmin = 0;
                    switch (T) {
                    case 0:DECCM = 0; break;
                    case 15:DECCM = 0;break;
                    case 25:DECCM = 0; break;
                    }
                }
                else if(alvue <= 1000)
                {DLCmin = 8500;
                    DRCmin = 3600;
                    switch (T) {
                    case 0:DECCM = 9780; break;
                    case 15:DECCM = 18100;break;
                    case 25:DECCM = 24500; break;
                    }
                }
                else if(alvue == 2000)
                {DLCmin = 9500;
                    DRCmin = 4000;
                    switch (T) {
                    case 0:DECCM = 10780; break;
                    case 15:DECCM = 19100;break;
                    case 25:DECCM = 25500; break;

                    }
                }
                else  if(alvue == 4000)
                {DLCmin = 9500;
                    DRCmin = 3900;
                    switch (T) {
                    case 0:DECCM = 10780; break;
                    case 15:DECCM = 19100;break;
                    case 25:DECCM = 25500; break;

                    }
                }
                else  if(alvue == 6000)
                {DLCmin = 9500;
                    DRCmin = 3900;
                    switch (T) {
                    case 0:DECCM = 10780; break;
                    case 15:DECCM = 19100;break;
                    case 25:DECCM = 25500; break;

                    }
                }
                else  if(alvue == 10000)
                {DLCmin = 10500;
                    DRCmin = 4300;
                    switch (T) {
                    case 0:DECCM = 11780; break;
                    case 15:DECCM = 20100;break;
                    case 25:DECCM = 26500; break;

                    }
                }
                else /*17000*/
                {DLCmin = 0;
                    DRCmin = 0;
                    switch (T) {
                    case 0:DECCM = 0; break;
                    case 15:DECCM = 0;break;
                    case 25:DECCM = 0; break;

                    }
                }
            }
        }
        else
        {
            if(V == 150)
            {
                if(alvue <= 1000)
                {
                    DLCmin = 4400;
                    DRCmin = 6000;
                    DECCM = 16000;
                }
                if(alvue == 2000)
                {
                    DLCmin = 4400;
                    DRCmin = 6000;
                    DECCM = 18000;
                }
                if(alvue == 4000)
                {
                    DLCmin = 4400;
                    DRCmin = 6000;
                    DECCM = 19000;
                }
                if(alvue == 6000)
                {
                    DLCmin = 4400;
                    DRCmin = 6000;
                    DECCM = 22000;
                }
                if(alvue == 10000)
                {
                    DLCmin = 4400;
                    DRCmin = 6000;
                    DECCM = 22000;
                }

            }
            if(V == 300)
            {
                if(alvue <= 1000)
                {
                    DLCmin = 3000;
                    DRCmin = 6000;
                    DECCM = 17000;
                }
                if(alvue == 2000)
                {
                    DLCmin = 3000;
                    DRCmin = 6000;
                    DECCM = 18000;
                }
                if(alvue == 4000)
                {
                    DLCmin = 3000;
                    DRCmin = 6000;
                    DECCM = 19000;
                }
                if(alvue == 6000)
                {
                    DLCmin = 3000;
                    DRCmin = 6000;
                    DECCM = 22000;
                }
                if(alvue == 10000)
                {
                    DLCmin = 3000;
                    DRCmin = 6000;
                    DECCM = 22000;
                }

            }
        }
    }
    if(medio == 37)
    {
        if(V == 150)
        {
            if(alvue <= 400)
            {DLCmin = 4800;
                DRCmin = 3650;
                switch (T) {
                case 0:DECCM = 5100; break;
                case 24:DECCM = 9000;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 4800;
                DRCmin = 3650;
                switch (T) {
                case 0:DECCM = 5100; break;
                case 24:DECCM = 9000;break;
                }
            }
            else if(alvue == 2000)
            {DLCmin = 5800;
                DRCmin = 4400;
                switch (T) {
                case 0:DECCM = 6100; break;
                case 24:DECCM = 10000;break;
                }
            }
            else  if(alvue == 4000)
            {DLCmin = 6800;
                DRCmin = 5100;
                switch (T) {
                case 0:DECCM = 7100; break;
                case 24:DECCM = 11000;break;
                }
            }
            else  if(alvue == 6000)
            {DLCmin = 7800;
                DRCmin = 5800;
                switch (T) {
                case 0:DECCM = 8100; break;
                case 24:DECCM = 12000;break;
                }
            }
            else if(alvue == 10000)
            {DLCmin = 8800;
                DRCmin = 6600;
                switch (T) {
                case 0:DECCM = 9100; break;
                case 24:DECCM = 13000;break;
                }
            }
            else /*17000*/
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 24:DECCM = 0; break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 400)
            {
                DLCmin = 4800;
                DRCmin = 3000;
                switch (T)
                {
                case 0:DECCM = 5400; break;
                case 24:DECCM = 13200; break;
                }
            }
            else if(alvue <= 1000)
            {
                DLCmin = 4800;
                DRCmin = 3000;
                switch (T) {
                case 0:DECCM = 5400; break;
                case 24:DECCM = 13200;break;
                }
            }
            else if(alvue == 2000)
            {
                DLCmin = 5800;
                DRCmin = 3500;
                switch (T) {
                case 0:DECCM = 6400; break;
                case 24:DECCM = 14200; break;
                }
            }
            else  if(alvue == 4000)
            {
                DLCmin = 6800;
                DRCmin = 4000;
                switch (T) {
                case 0:DECCM = 7400; break;
                case 24:DECCM = 15200; break;
                }
            }
            else  if(alvue == 6000)
            {
                DLCmin = 7800;
                DRCmin = 4600;
                switch (T) {
                case 0:DECCM = 8400; break;
                case 24:DECCM = 16200; break;

                }
            }
            else  if(alvue == 10000)
            {
                DLCmin = 8800;
                DRCmin = 5300;
                switch (T) {
                case 0:DECCM = 9400; break;
                case 24:DECCM = 17200; break;
                }
            }
            else /*17000*/
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 24:DECCM = 0; break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 400)
            {DLCmin = 5800;
                DRCmin = 3100;
                switch (T) {
                case 24:DECCM = 17560; break;
                case 0:DECCM = 6640; break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 5800;
                DRCmin = 3100;
                switch (T) {
                case 24:DECCM = 17560; break;
                case 0:DECCM = 6640; break;
                }
            }
            else if(alvue == 2000)
            {DLCmin = 6800;
                DRCmin = 3600;
                switch (T) {
                case 24:DECCM = 18560; break;
                case 0:DECCM = 7640; break;
                }
            }
            else  if(alvue == 4000)
            {DLCmin = 7800;
                DRCmin = 4100;
                switch (T) {
                case 0:DECCM = 8640; break;
                case 24:DECCM = 19560; break;
                }
            }
            else  if(alvue == 6000)
            {DLCmin = 8800;
                DRCmin = 4500;
                switch (T) {
                case 0:DECCM = 9640; break;
                case 24:DECCM = 20560; break;
                }
            }
            else  if(alvue == 10000)
            {DLCmin = 9800;
                DRCmin = 5000;
                switch (T) {
                case 0:DECCM = 10640; break;
                case 24:DECCM = 21560; break;
                }
            }
            else /*17000*/
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 24:DECCM = 0; break;
                }
            }
        }
        if(V == 640)
        {
            if(alvue <= 400)
            {DLCmin = 7800;
                DRCmin = 3400;
                switch (T) {
                case 0:DECCM = 9080; break;
                case 24:DECCM = 25720;break;

                }
            }
            else if(alvue <= 1000)
            {DLCmin = 7800;
                DRCmin = 3400;
                switch (T) {
                case 0:DECCM = 9080; break;
                case 24:DECCM = 25720;break;
                }
            }
            else if(alvue == 2000)
            {DLCmin = 7800;
                DRCmin = 4100;
                switch (T) {
                case 0:DECCM = 9080; break;
                case 24:DECCM = 25720;break;

                }
            }
            else  if(alvue == 4000)
            {DLCmin = 8800;
                DRCmin = 4600;
                switch (T) {
                case 0:DECCM = 10080; break;
                case 24:DECCM = 26720;break;

                }
            }
            else  if(alvue == 6000)
            {DLCmin = 9800;
                DRCmin = 4050;
                switch (T) {
                case 0:DECCM = 11080; break;
                case 24:DECCM = 27720;break;

                }
            }
            else  if(alvue == 10000)
            {DLCmin = 10800;
                DRCmin = 4500;
                switch (T) {
                case 0:DECCM = 12080; break;
                case 24:DECCM = 28720;break;
                }
            }
            else /*17000*/
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 24:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 36)
    {
        if(V == 150)
        {
            if(alvue <= 400)
            {DLCmin = 3200;
                DRCmin = 2100;
                switch (T) {
                case 0:DECCM = 3500; break;
                case 16:DECCM = 5600;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 3200;
                DRCmin = 2100;
                switch (T) {
                case 0:DECCM = 3500; break;
                case 16:DECCM = 5600;break;
                }
            }
            else if(alvue == 2000)
            {DLCmin = 3200;
                DRCmin = 2000;
                switch (T) {
                case 0:DECCM = 3500; break;
                case 16:DECCM = 5600;break;
                }
            }
            else  if(alvue == 4000)
            {DLCmin = 3200;
                DRCmin = 2000;
                switch (T) {
                case 0:DECCM = 3500; break;
                case 16:DECCM = 5600;break;
                }
            }

            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 16:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 400)
            {
                DLCmin = 3200;
                DRCmin = 1600;
                switch (T)
                {
                case 0:DECCM = 3800; break;
                case 16:DECCM = 8000;break;
                }
            }
            else if(alvue <= 1000)
            {
                DLCmin = 3200;
                DRCmin = 1600;
                switch (T) {
                case 0:DECCM = 3800; break;
                case 16:DECCM = 8000;break;
                }
            }
            else if(alvue == 2000)
            {
                DLCmin = 4200;
                DRCmin = 2000;
                switch (T) {
                case 0:DECCM = 4800; break;
                case 16:DECCM = 9000;break;
                }
            }
            else  if(alvue == 4000)
            {
                DLCmin = 5200;
                DRCmin = 2500;
                switch (T) {
                case 0:DECCM = 5800; break;
                case 16:DECCM =10000;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 16:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 400)
            {DLCmin = 4200;
                DRCmin = 1800;
                switch (T) {
                case 16:DECCM = 10920;break;
                case 0:DECCM = 5040; break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 4200;
                DRCmin = 1800;
                switch (T) {
                case 16:DECCM = 10920;break;
                case 0:DECCM = 5040; break;
                }
            }
            else if(alvue == 2000)
            {DLCmin = 4200;
                DRCmin = 1700;
                switch (T) {
                case 16:DECCM = 10920;break;
                case 0:DECCM = 5040; break;

                }
            }
            else  if(alvue == 4000)
            {DLCmin = 5200;
                DRCmin = 2000;
                switch (T) {
                case 0:DECCM = 6040; break;
                case 16:DECCM = 11920;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 16:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 35)
    {
        if(V == 150)
        {
            if(alvue <= 1000)
            {DLCmin = 1300;
                DRCmin = 800;
                switch (T) {
                case 0:DECCM = 2300; break;
                case 5:DECCM = 3050;break;
                }
            }

            else if(alvue == 2000)
            {DLCmin = 1500;
                DRCmin = 800;
                switch (T) {
                case 0:DECCM = 2800; break;
                case 5:DECCM = 3250;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 1000)
            {
                DLCmin = 1700;
                DRCmin = 800;
                switch (T)
                {
                case 0:DECCM = 2300; break;
                case 5:DECCM = 3200;break;
                }
            }
            else if(alvue == 2000)
            {
                DLCmin = 2200;
                DRCmin = 800;
                switch (T) {
                case 0:DECCM = 2800; break;
                case 5:DECCM = 3700;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 2000)
            {DLCmin = 2200;
                DRCmin = 800;
                switch (T) {
                case 0:DECCM = 3020; break;
                case 5:DECCM = 4300;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 34)
    {
        if(V == 150)
        {
            if(alvue <= 300)
            {DLCmin = 50;
                DRCmin = 10;
                switch (T) {
                case 0:DECCM = 350; break;
                case 5:DECCM = 800;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 50;
                DRCmin = 20;
                switch (T) {
                case 0:DECCM = 350; break;
                case 5:DECCM = 800;break;
                }
            }
            else if(alvue == 2000)
            {DLCmin = 100;
                DRCmin = 40;
                switch (T) {
                case 0:DECCM = 400; break;
                case 5:DECCM = 850;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 300)
            {
                DLCmin = 50;
                DRCmin = 10;
                switch (T)
                {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1550;break;
                }
            }
            else if(alvue <= 1000)
            {
                DLCmin = 50;
                DRCmin = 18;
                switch (T) {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1550;break;
                }
            }
            else if(alvue == 2000)
            {
                DLCmin = 100;
                DRCmin = 38;
                switch (T) {
                case 0:DECCM = 700; break;
                case 5:DECCM = 1600;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 300)
            {DLCmin = 50;
                DRCmin = 10;
                switch (T) {
                case 0:DECCM = 890;break;
                case 5:DECCM = 2150; break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 50;
                DRCmin = 15;
                switch (T) {
                case 0:DECCM = 890;break;
                case 5:DECCM = 2150; break;
                }
            }
            else if(alvue == 2000)
            {DLCmin = 150;
                DRCmin = 40;
                switch (T) {
                case 5:DECCM = 2250;break;
                case 0:DECCM = 990; break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 33)
    {
        if(V == 150)
        {
            if(alvue <= 2000)
            {DLCmin = 1000;
                DRCmin = 500;
                switch (T) {
                case 0:DECCM = 1300; break;
                case 5:DECCM = 1750;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 2000)
            {
                DLCmin = 1300;
                DRCmin = 500;
                switch (T)
                {
                case 0:DECCM = 1900; break;
                case 5:DECCM = 2800;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 2000)
            {DLCmin = 1700;
                DRCmin = 500;
                switch (T) {
                case 5:DECCM = 3800;break;
                case 0:DECCM = 2540; break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 32)
    {
        if(V == 150)
        {
            if(alvue <= 2000)
            {DLCmin = 1000;
                DRCmin = 500;
                switch (T) {
                case 0:DECCM = 1300; break;
                case 5:DECCM = 1750;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 2000)
            {
                DLCmin = 1300;
                DRCmin = 500;
                switch (T)
                {
                case 0:DECCM = 1900; break;
                case 5:DECCM = 2800;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 2000)
            {DLCmin = 1600;
                DRCmin = 500;
                switch (T) {
                case 5:DECCM = 3700;break;
                case 0:DECCM = 2440; break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 31)
    {
        if(V == 150)
        {
            if(alvue <= 2000)
            {DLCmin = 1000;
                DRCmin = 500;
                switch (T) {
                case 0:DECCM = 1300; break;
                case 5:DECCM = 1750;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 2000)
            {
                DLCmin = 1200;
                DRCmin = 500;
                switch (T)
                {
                case 0:DECCM = 1800; break;
                case 5:DECCM = 2700;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 2000)
            {DLCmin = 1500;
                DRCmin = 500;
                switch (T) {
                case 0:DECCM = 2340;break;
                case 5:DECCM = 3600; break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }

    if(medio == 27)
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 10;
                switch (T) {
                case 0:DECCM = 325; break;
                case 5:DECCM = 775;break;
                }
            }

            else if(alvue <= 500)
            {DLCmin = 80;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 380; break;
                case 5:DECCM = 830;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 150;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 450; break;
                case 5:DECCM = 900;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 300;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 600; break;
                case 5:DECCM = 1050;break;
                }
            }
            else if(alvue <= 4000)
            {DLCmin = 450;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 750; break;
                case 5:DECCM = 1200;break;
                }
            }
            else if(alvue <= 6000)
            {DLCmin = 900;
                DRCmin = 680;
                switch (T) {
                case 0:DECCM = 1200; break;
                case 5:DECCM = 1650;break;
                }
            }
            else if(alvue <= 10000)
            {DLCmin = 1700;
                DRCmin = 1300;
                switch (T) {
                case 0:DECCM = 2500; break;
                case 5:DECCM = 2650;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {
                DLCmin = 25;
                DRCmin = 10;
                switch (T)
                {
                case 0:DECCM = 625; break;
                case 5:DECCM = 1525;break;
                }
            }
            else if(alvue <= 500)
            {
                DLCmin = 80;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 680; break;
                case 5:DECCM = 1580;break;
                }
            }
            else if(alvue <= 1000)
            {
                DLCmin = 150;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 750; break;
                case 5:DECCM = 1650;break;
                }
            }
            else if(alvue <= 2000)
            {
                DLCmin = 300;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 1000; break;
                case 5:DECCM = 1800;break;
                }
            }
            else if(alvue <= 4000)
            {
                DLCmin = 450;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 1050; break;
                case 5:DECCM = 1950;break;
                }
            }
            else if(alvue <= 6000)
            {
                DLCmin = 900;
                DRCmin = 680;
                switch (T) {
                case 0:DECCM = 1500; break;
                case 5:DECCM = 2400;break;
                }
            }
            else if(alvue <= 10000)
            {
                DLCmin = 1900;
                DRCmin = 1300;
                switch (T) {
                case 0:DECCM = 2500; break;
                case 5:DECCM = 3400;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 10;
                switch (T) {
                case 0:DECCM = 865; break;
                case 5:DECCM = 4225;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 80;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 920; break;
                case 5:DECCM = 4280;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin =150;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 990; break;
                case 5:DECCM = 4350;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 1140; break;
                case 5:DECCM = 4550;break;
                }
            }
            else  if(alvue <= 4000)
            {DLCmin = 700;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 1540; break;
                case 5:DECCM = 4900;break;
                }
            }
            else if(alvue <= 6000)
            {DLCmin = 1100;
                DRCmin = 680;
                switch (T) {
                case 0:DECCM = 1940; break;
                case 5:DECCM = 5300;break;
                }
            }
            else if(alvue <= 10000)
            {DLCmin = 2100;
                DRCmin = 1300;
                switch (T) {
                case 0:DECCM = 2940; break;
                case 5:DECCM = 6300;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 26 )
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 325; break;
                case 5:DECCM = 775;break;
                }
            }

            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 350; break;
                case 5:DECCM = 800;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 400; break;
                case 5:DECCM = 850;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 500; break;
                case 5:DECCM = 950;break;
                }
            }
            else if(alvue <= 4000)
            {DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1100;break;
                }
            }
            else if(alvue <= 6000)
            {DLCmin = 500;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 800; break;
                case 5:DECCM = 1250;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {
                DLCmin = 25;
                DRCmin = 5;
                switch (T)
                {
                case 0:DECCM = 625; break;
                case 5:DECCM = 1525;break;
                }
            }
            else if(alvue <= 500)
            {
                DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1550;break;
                }
            }
            else if(alvue <= 1000)
            {
                DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 700; break;
                case 5:DECCM = 1600;break;
                }
            }
            else if(alvue <= 2000)
            {
                DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 800; break;
                case 5:DECCM = 1700;break;
                }
            }
            else if(alvue <= 4000)
            {
                DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 950; break;
                case 5:DECCM = 1850;break;
                }
            }
            else if(alvue <= 6000)
            {
                DLCmin = 500;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 1100; break;
                case 5:DECCM = 2000;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 865; break;
                case 5:DECCM = 2125;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 890; break;
                case 5:DECCM = 2150;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 50;
                DRCmin = 800;
                switch (T) {
                case 0:DECCM = 940; break;
                case 5:DECCM = 2200;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 1040; break;
                case 5:DECCM = 2300;break;
                }
            }
            else if(alvue <= 4000)
            {DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 1190; break;
                case 5:DECCM = 2450;break;
                }
            }
            else if(alvue <= 6000)
            {DLCmin = 500;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 1340; break;
                case 5:DECCM = 2600;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }

    if(medio == 25)
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 325; break;
                case 5:DECCM = 775;break;
                }
            }

            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 350; break;
                case 5:DECCM = 800;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 400; break;
                case 5:DECCM = 850;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 500; break;
                case 5:DECCM = 950;break;
                }
            }
            else if(alvue <= 4000)
            {DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1100;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {
                DLCmin = 25;
                DRCmin = 5;
                switch (T)
                {
                case 0:DECCM = 625; break;
                case 5:DECCM = 1525;break;
                }
            }
            else if(alvue <= 500)
            {
                DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1550;break;
                }
            }
            else if(alvue <= 1000)
            {
                DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 700; break;
                case 5:DECCM = 1600;break;
                }
            }
            else if(alvue <= 2000)
            {
                DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 800; break;
                case 5:DECCM = 1700;break;
                }
            }
            else if(alvue <= 4000)
            {
                DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 950; break;
                case 5:DECCM = 1850;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 865; break;
                case 5:DECCM = 2125;break;
                }
            }
            if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 890; break;
                case 5:DECCM = 2150;break;
                }
            }
            if(alvue <= 1000)
            {DLCmin = 50;
                DRCmin = 800;
                switch (T) {
                case 0:DECCM = 940; break;
                case 5:DECCM = 2200;break;
                }
            }
            if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 1040; break;
                case 5:DECCM = 2300;break;
                }
            }
            if(alvue <= 4000)
            {DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 1190; break;
                case 5:DECCM = 2450;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }

    if(medio == 24)
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 325; break;
                case 5:DECCM = 775;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 350; break;
                case 5:DECCM = 800;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin =50;
                switch (T) {
                case 0:DECCM = 400; break;
                case 5:DECCM = 850;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 500; break;
                case 5:DECCM = 950;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {
                DLCmin = 25;
                DRCmin = 5;
                switch (T)
                {
                case 0:DECCM = 625; break;
                case 5:DECCM = 1525;break;
                }
            }
            else if(alvue <= 500)
            {
                DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1550;break;
                }
            }
            else if(alvue <= 1000)
            {
                DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 700; break;
                case 5:DECCM = 1600;break;
                }
            }
            else if(alvue <= 2000)
            {
                DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 800; break;
                case 5:DECCM = 1700;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 865;break;
                case 5:DECCM = 2125; break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 890;break;
                case 5:DECCM = 2150; break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 940;break;
                case 5:DECCM = 2200; break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 5:DECCM = 1040;break;
                case 0:DECCM = 2300; break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 23 )
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 325; break;
                case 5:DECCM = 775;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 350; break;
                case 5:DECCM = 800;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 400; break;
                case 5:DECCM = 850;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 500; break;
                case 5:DECCM = 950;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 625; break;
                case 5:DECCM = 1525;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1550;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 700; break;
                case 5:DECCM = 1600;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 800; break;
                case 5:DECCM = 1700;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 865; break;
                case 5:DECCM = 2125;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 890; break;
                case 5:DECCM = 2150;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 940; break;
                case 5:DECCM = 2200;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 1040; break;
                case 5:DECCM = 2300;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }

    if(medio == 22)
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 325; break;
                case 5:DECCM = 775;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 350; break;
                case 5:DECCM = 800;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 400; break;
                case 5:DECCM = 850;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 500; break;
                case 5:DECCM = 900;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 625; break;
                case 5:DECCM = 1525;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1550;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 700; break;
                case 5:DECCM = 1600;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 800; break;
                case 5:DECCM = 1700;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 865; break;
                case 5:DECCM = 2125;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 890; break;
                case 5:DECCM = 2150;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 940; break;
                case 5:DECCM = 2200;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 1040; break;
                case 5:DECCM = 2300;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 21)
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 325; break;
                case 5:DECCM = 775;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 350; break;
                case 5:DECCM = 800;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 400; break;
                case 5:DECCM = 850;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 625; break;
                case 5:DECCM = 1525;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 650; break;
                case 5:DECCM = 1550;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 700; break;
                case 5:DECCM = 1600;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 865; break;
                case 5:DECCM = 2125;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 890; break;
                case 5:DECCM = 2150;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 940; break;
                case 5:DECCM = 2200;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 5:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 44)
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 325; break;
                case 10:DECCM = 1550;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 350; break;
                case 10:DECCM = 1600;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 400; break;
                case 10:DECCM = 1700;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 500; break;
                case 10:DECCM = 1900;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 10:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 625; break;
                case 10:DECCM = 3050;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 650; break;
                case 10:DECCM = 3100;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 700; break;
                case 10:DECCM = 3200;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 800; break;
                case 10:DECCM = 3400;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 10:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 865; break;
                case 10:DECCM = 4250;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 890; break;
                case 10:DECCM = 4300;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 940; break;
                case 10:DECCM = 4400;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 1040; break;
                case 10:DECCM = 4600;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 10:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 43)
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 325; break;
                case 10:DECCM = 1550;break;
                }
            }

            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 350; break;
                case 10:DECCM = 1600;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 400; break;
                case 10:DECCM = 1700;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 500; break;
                case 10:DECCM = 1900;break;
                }
            }
            else if(alvue <= 4000)
            {DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 650; break;
                case 10:DECCM = 2200;break;
                }
            }
            else if(alvue <= 6000)
            {DLCmin = 500;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 800; break;
                case 10:DECCM = 2500;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 10:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {
                DLCmin = 25;
                DRCmin = 5;
                switch (T)
                {
                case 0:DECCM = 625; break;
                case 10:DECCM = 3050;break;
                }
            }
            else if(alvue <= 500)
            {
                DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 650; break;
                case 10:DECCM = 3100;break;
                }
            }
            else if(alvue <= 1000)
            {
                DLCmin = 100;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 700; break;
                case 10:DECCM = 3200;break;
                }
            }
            else if(alvue <= 2000)
            {
                DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 800; break;
                case 10:DECCM = 3400;break;
                }
            }
            else if(alvue <= 4000)
            {
                DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 950; break;
                case 10:DECCM = 3700;break;
                }
            }
            else if(alvue <= 6000)
            {
                DLCmin = 500;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 1100; break;
                case 10:DECCM = 4000;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 10:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 5;
                switch (T) {
                case 0:DECCM = 865; break;
                case 10:DECCM = 4250;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 50;
                DRCmin = 25;
                switch (T) {
                case 0:DECCM = 890; break;
                case 10:DECCM = 4300;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 50;
                DRCmin = 800;
                switch (T) {
                case 0:DECCM = 940; break;
                case 10:DECCM = 4400;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 200;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 1040; break;
                case 10:DECCM = 4600;break;
                }
            }
            else if(alvue <= 4000)
            {DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 1190; break;
                case 10:DECCM = 4900;break;
                }
            }
            else if(alvue <= 6000)
            {DLCmin = 500;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 1340; break;
                case 10:DECCM = 5200;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 10:DECCM = 0;break;
                }
            }
        }
    }
    if(medio == 42)
    {
        if(V == 150)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 10;
                switch (T) {
                case 0:DECCM = 325; break;
                case 10:DECCM = 1550;break;
                }
            }

            else if(alvue <= 500)
            {DLCmin = 80;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 380; break;
                case 10:DECCM = 1660;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin = 150;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 450; break;
                case 10:DECCM = 1800;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 300;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 600; break;
                case 10:DECCM = 2010;break;
                }
            }
            else if(alvue <= 4000)
            {DLCmin = 450;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 750; break;
                case 10:DECCM = 2400;break;
                }
            }
            else if(alvue <= 6000)
            {DLCmin = 900;
                DRCmin = 680;
                switch (T) {
                case 0:DECCM = 1200; break;
                case 10:DECCM = 3300;break;
                }
            }
            else if(alvue <= 10000)
            {DLCmin = 1700;
                DRCmin = 1300;
                switch (T) {
                case 0:DECCM = 2500; break;
                case 10:DECCM = 5300;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 10:DECCM = 0;break;
                }
            }
        }
        if(V == 300)
        {
            if(alvue <= 100)
            {
                DLCmin = 25;
                DRCmin = 10;
                switch (T)
                {
                case 0:DECCM = 625; break;
                case 10:DECCM = 3050;break;
                }
            }
            else if(alvue <= 500)
            {
                DLCmin = 80;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 680; break;
                case 10:DECCM = 3160;break;
                }
            }
            else if(alvue <= 1000)
            {
                DLCmin = 150;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 750; break;
                case 10:DECCM = 3300;break;
                }
            }
            else if(alvue <= 2000)
            {
                DLCmin = 300;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 1000; break;
                case 10:DECCM = 3600;break;
                }
            }
            else if(alvue <= 4000)
            {
                DLCmin = 450;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 1050; break;
                case 10:DECCM = 3900;break;
                }
            }
            else if(alvue <= 6000)
            {
                DLCmin = 900;
                DRCmin = 680;
                switch (T) {
                case 0:DECCM = 1500; break;
                case 10:DECCM = 4800;break;
                }
            }
            else if(alvue <= 10000)
            {
                DLCmin = 1900;
                DRCmin = 1300;
                switch (T) {
                case 0:DECCM = 2500; break;
                case 10:DECCM = 6800;break;
                }
            }
            else
            {
                DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 10:DECCM = 0;break;
                }
            }
        }
        if(V == 420)
        {
            if(alvue <= 100)
            {DLCmin = 25;
                DRCmin = 10;
                switch (T) {
                case 0:DECCM = 865; break;
                case 10:DECCM = 8450;break;
                }
            }
            else if(alvue <= 500)
            {DLCmin = 80;
                DRCmin = 50;
                switch (T) {
                case 0:DECCM = 920; break;
                case 10:DECCM = 8560;break;
                }
            }
            else if(alvue <= 1000)
            {DLCmin =150;
                DRCmin = 100;
                switch (T) {
                case 0:DECCM = 990; break;
                case 10:DECCM = 4700;break;
                }
            }
            else if(alvue <= 2000)
            {DLCmin = 350;
                DRCmin = 200;
                switch (T) {
                case 0:DECCM = 1140; break;
                case 10:DECCM = 9100;break;
                }
            }
            else  if(alvue <= 4000)
            {DLCmin = 700;
                DRCmin = 340;
                switch (T) {
                case 0:DECCM = 1540; break;
                case 10:DECCM = 9800;break;
                }
            }
            else if(alvue <= 6000)
            {DLCmin = 1100;
                DRCmin = 680;
                switch (T) {
                case 0:DECCM = 1940; break;
                case 10:DECCM = 10600;break;
                }
            }
            else if(alvue <= 10000)
            {DLCmin = 2100;
                DRCmin = 1300;
                switch (T) {
                case 0:DECCM = 2940; break;
                case 10:DECCM = 12600;break;
                }
            }
            else
            {DLCmin = 0;
                DRCmin = 0;
                switch (T) {
                case 0:DECCM = 0; break;
                case 10:DECCM = 0;break;
                }
            }
        }
    }


    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(10,100,120));
    pen.setStyle(Qt::DashDotLine);

    customplotf.pintaElipses (ui->widget,DLCmin/1000,"DisLanzMin","DLCmin: "+ QString::number(DLCmin)+"m",QColor(10,100,120),false,pen);
    pen.setColor(Qt::red);
    customplotf.pintaElipses (ui->widget,DRCmin/1000,"DisZonReaMin","DRCmin: "+ QString::number(DRCmin)+"m",QColor(255,0,0),true,pen);

    pen.setColor(Qt::green);
    pen.setStyle(Qt::DashDotLine);
    customplotf.pintaElipses (ui->widget,DECCM/1000,"DisDEteccionMin","DECCM: "+ QString::number(DECCM)+"m",QColor(0,255,0),true,pen);

}

void DistanciaVD::llenartabla(bool llenar, int medios)
{
    medio = medios;

    if((medios >= 40  &&  medios <= 50) || (medios >= 36 && medios<= 39))
        dialogTabla->setWindowTitle (label +"_"+ QString::number (alvue)+"_"+prob);
    else
        dialogTabla->setWindowTitle (label +"_"+ QString::number (alvue));

    if(llenar)
    {
        if(medios == 41 || medios == 48)//muis y hazestrecho
        {
            //            dialogTabla->setWindowTitle (label +"_"+ QString::number (alvue)+"_"+prob);

            tabla->setColumnCount(7);

            tabla->setHorizontalHeaderLabels(QString("Azimut(°);D.V.D.P.(Km);S.N.V.(Km);Z.M.1°(Km);S.N.V.Z.M.1°(Km);Z.M.-1°(Km);S.N.V.Z.M.-1°(Km);").split(";"));
        }
        else
        {
            if(grupo == 3)
            {
                //                dialogTabla->setWindowTitle (label +"_"+ QString::number (alvue)+"_"+prob);
                tabla->setColumnCount(3);
                tabla->setHorizontalHeaderLabels(QString("Azimut(°);Z.P.D.C.A.A.(Km);S.N.V.(Km);").split(";"));
            }
            else if(grupo == 2)
            {
                //                dialogTabla->setWindowTitle (label +"_"+ QString::number (alvue));
                tabla->setColumnCount(3);
                tabla->setHorizontalHeaderLabels(QString("Azimut(°);Z.P.F.A.A.A.(Km);S.N.V.(Km);").split(";"));
            }
            else//grupo 1 CCAA
            {
                if(medios == 39 && alvue >=100)
                {
                    //                    dialogTabla->setWindowTitle (label +"_"+ QString::number (alvue)+"_"+prob);
                    tabla->setColumnCount(7);
                    tabla->setHorizontalHeaderLabels(QString("Azimut(°);D.V.D.P.(Km);S.N.V.(Km);Z.M.-3°(Km);S.N.V.Z.M.-3°(Km);D.Z..L.(Km);D.Z.R.(Km);").split(";"));
                }
                else if(medios == 38 && alvue >=25)
                {
                    //                    dialogTabla->setWindowTitle (label +"_"+ QString::number (alvue)+"_"+prob);
                    tabla->setColumnCount(7);
                    tabla->setHorizontalHeaderLabels(QString("Azimut(°);D.V.D.P.(Km);S.N.V.(Km);Z.M.-2°(Km);S.N.V.Z.M.-2°(Km);D.Z..L.(Km);D.Z.R.(Km);").split(";"));
                }
                else if((medios == 37 || medios == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 27 || medio == 26|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21 || medio == 42|| medio == 43 || medio == 44) && alvue >= 25)
                {
                    tabla->setColumnCount(5);
                    //                    dialogTabla->setWindowTitle (label +"_"+ QString::number (alvue)+"_"+prob);
                    if( medio == 27 || medio == 26|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21 || medio == 42|| medio == 43 || medio == 44)
                        tabla->setHorizontalHeaderLabels(QString("Azimut(°);D.V.D.P.(Km);S.N.V.(Km);D.A.F.(Km);D.Z..R.(Km);").split(";"));
                    else
                        tabla->setHorizontalHeaderLabels(QString("Azimut(°);D.V.D.P.(Km);S.N.V.(Km);D.Z..L.(Km);D.Z..R.(Km);").split(";"));
                }
                else
                {
                    //                    dialogTabla->setWindowTitle (label +"_"+ QString::number (alvue)+"_"+prob);
                    tabla->setColumnCount(3);
                    tabla->setHorizontalHeaderLabels(QString("Azimut(°);D.V.D.P.(Km);S.N.V.(Km);").split(";"));
                }
            }
        }

        tabla->setRowCount(360);

        tabla->verticalHeader()->setVisible(false);
        tabla->setEditTriggers(nullptr);
        for (int i = 0; i < 360; i++)
        {
            tablaitem = new QTableWidgetItem();//azimut
            tablaitem->setText(QString::number(i));
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,0,tablaitem);

            tablaitem = new QTableWidgetItem();//DVD
            tablaitem->setText(QString::number(disVDirecta[i].at(0)/1000));
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,1,tablaitem);

            tablaitem = new QTableWidgetItem();//SNZ
            QString datostabla;
            if(disInternas[i].length() >= 1)//al menos un punto interno
            {
                datostabla = QString::number(disInternas[i].at(0)/1000) +", ";
                for (int j = 1; j < disInternas[i].length(); ++j) {
                    if(j < disInternas[i].length()-1)
                        datostabla = datostabla + QString::number(disInternas[i].at(j)/1000) +", ";
                    else
                        datostabla = datostabla + QString::number(disInternas[i].at(j)/1000 );
                }
            }
            else
                datostabla = "--";
            tablaitem->setText(datostabla);
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,2,tablaitem);
            if((medios == 39 && alvue >=100)  && grupo == 1)
            {
                /*******************************************************************/
                tablaitem = new QTableWidgetItem();//ZM-3
                datostabla = QString::number(ZonaMuertaMuis2[i].at(0)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,3,tablaitem);

                /*******************************************************************/
                tablaitem = new QTableWidgetItem();//SNVZM-3
                if(ZonaMuertaMuis2[i].length()>=2)
                {
                    datostabla = QString::number(ZonaMuertaMuis2[i].at(1)/1000) +", ";
                    for (int j = 2; j < ZonaMuertaMuis2[i].length(); j++)
                    {
                        if(j < ZonaMuertaMuis[i].length()-1)
                            datostabla = datostabla + QString::number(ZonaMuertaMuis2[i].at(j)/1000) +", ";
                        else
                            datostabla = datostabla + QString::number(ZonaMuertaMuis2[i].at(j)/1000 );
                    }
                }
                else
                    datostabla = "--";
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,4,tablaitem);

                /*******************************************************************/
                //disVDirecta[i].at (1) ------dzl
                tablaitem = new QTableWidgetItem();//dzl
                datostabla = QString::number(disVDirecta[i].at(1)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,5,tablaitem);
                //disVDirecta[i].at (2) ------dzr
                tablaitem = new QTableWidgetItem();//dzr
                datostabla = QString::number(disVDirecta[i].at(2)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,6,tablaitem);
            }
            if((medios == 38  && alvue >= 25) && grupo == 1)
            {
                /*******************************************************************/
                tablaitem = new QTableWidgetItem();//ZM-3
                datostabla = QString::number(ZonaMuertaMuis2[i].at(0)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,3,tablaitem);

                /*******************************************************************/
                tablaitem = new QTableWidgetItem();//SNVZM-3
                if(ZonaMuertaMuis2[i].length()>=2)
                {
                    datostabla = QString::number(ZonaMuertaMuis2[i].at(1)/1000) +", ";
                    for (int j = 2; j < ZonaMuertaMuis2[i].length(); j++)
                    {
                        if(j < ZonaMuertaMuis2[i].length()-1)
                            datostabla = datostabla + QString::number(ZonaMuertaMuis2[i].at(j)/1000) +", ";
                        else
                            datostabla = datostabla + QString::number(ZonaMuertaMuis2[i].at(j)/1000 );
                    }
                }
                else
                    datostabla = "--";
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,4,tablaitem);

                /*******************************************************************/
                //disVDirecta[i].at (1) ------dzl
                tablaitem = new QTableWidgetItem();//dzl
                datostabla = QString::number(disVDirecta[i].at(1)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,5,tablaitem);
                //disVDirecta[i].at (2) ------dzr
                tablaitem = new QTableWidgetItem();//dzr
                datostabla = QString::number(disVDirecta[i].at(2)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,6,tablaitem);
            }
            if(((medios == 37 || medios == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 27 || medio == 26|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21 || medio == 42 || medio == 43 || medio == 44) && alvue >= 25) && grupo == 1)
            {
                //disVDirecta[i].at (1) ------dzl
                tablaitem = new QTableWidgetItem();//dzl
                datostabla = QString::number(disVDirecta[i].at(1)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,3,tablaitem);
                //disVDirecta[i].at (2) ------dzr
                tablaitem = new QTableWidgetItem();//dzr
                datostabla = QString::number(disVDirecta[i].at(2)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,4,tablaitem);
            }
            if(medios == 41 || medios == 48)
            {
                /*******************************************************************/
                tablaitem = new QTableWidgetItem();//ZM1
                datostabla = QString::number(ZonaMuertaMuis[i].at(0)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,3,tablaitem);

                /*******************************************************************/
                tablaitem = new QTableWidgetItem();//SNVZM1
                if(ZonaMuertaMuis[i].length()>=2)
                {
                    datostabla = QString::number(ZonaMuertaMuis[i].at(1)/1000) +", ";
                    for (int j = 2; j < ZonaMuertaMuis[i].length(); j++)
                    {
                        if(j < ZonaMuertaMuis[i].length()-1)
                            datostabla = datostabla + QString::number(ZonaMuertaMuis[i].at(j)/1000) +", ";
                        else
                            datostabla = datostabla + QString::number(ZonaMuertaMuis[i].at(j)/1000 );
                    }
                }
                else
                    datostabla = "--";
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,4,tablaitem);

                /*******************************************************************/
                tablaitem = new QTableWidgetItem();//ZM-1
                datostabla = QString::number(ZonaMuertaMuis2[i].at(0)/1000);
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,5,tablaitem);

                /*******************************************************************/
                tablaitem = new QTableWidgetItem();//SNVZM-1
                if(ZonaMuertaMuis2[i].length()>=2)
                {
                    datostabla = QString::number(ZonaMuertaMuis2[i].at(1)/1000) +", ";
                    for (int j = 2; j < ZonaMuertaMuis2[i].length(); j++)
                    {
                        if(j < ZonaMuertaMuis2[i].length()-1)
                            datostabla = datostabla + QString::number(ZonaMuertaMuis2[i].at(j)/1000) +", ";
                        else
                            datostabla = datostabla + QString::number(ZonaMuertaMuis2[i].at(j)/1000 );
                    }
                }
                else
                    datostabla = "--";
                tablaitem->setText(datostabla);
                tablaitem->setTextAlignment(Qt::AlignCenter);
                tabla->setItem(i,6,tablaitem);
            }
        }

        tabla->horizontalHeader ()->resizeContentsPrecision ();
        tabla->resizeColumnsToContents ();

    }
    else
    {
        tabla->clear();
        dialogTabla->close();
    }
}

void DistanciaVD::tablaProbabilidad(QString p/*, double areaCubierta, double areaCirculo, double areaCuadrado*/, int medios)
{
    datosProbabilidad.clear ();

    if((medios >= 40  &&  medios <= 50) || (medios >= 36 && medios<= 39))
        dialogTablaP->setWindowTitle ("Comparación de áreas. " + p.remove ("_"));
    else
        dialogTablaP->setWindowTitle ("Comparación de áreas.");
    tablaP->setColumnCount(6);

    tablaP->setHorizontalHeaderLabels(QString("Zona;Area Cubierta(Km^2);Area Circunferencia(Km^2);%;Area Cuadrado(Km^2);%;").split(";"));
    tablaP->verticalHeader()->setVisible(false);
    tablaP->setEditTriggers(nullptr);

    QString datostabla;
    double d1 = 0;
    double d2 = 0;
    double d3 = 0;
    if(((medio == 39 && alvue >=100) || ((medio == 38 || medio == 37 || medio == 36  || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21 || medio == 42 || medio == 43 || medio == 44) && alvue >= 25)) && grupo == 1)
    {
        tablaP->setRowCount(3);
        ////row0
        tablaitemP = new QTableWidgetItem();//ZonaECC
        tablaitemP->setText("E.C.");
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,0,tablaitemP);
        datostabla = tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Cubierta
        d1 = ListAreas[0][0];
        tablaitemP->setText(QString::number(d1,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,1,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Circunferencia
        d2 = ListAreas[0][1];
        tablaitemP->setText(QString::number(d2,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,2,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//%1
        tablaitemP->setText(QString::number(d1/d2*100,'f',2));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,3,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Cuadrado
        d3 = ListAreas[0][2];
        tablaitemP->setText(QString::number(d3,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,4,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//%2
        tablaitemP->setText(QString::number(d1/d3*100,'f',2));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,5,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();
        datosProbabilidad.append (datostabla);
        datostabla.clear ();
        /******row1************/
        tablaitemP = new QTableWidgetItem();//ZonaDL
        if(medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21 || medio == 42 || medio == 43 || medio == 44)
            tablaitemP->setText("D.A.F.");
        else
            tablaitemP->setText("D.L.");
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(1,0,tablaitemP);
        datostabla =  tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Cubierta
        d1 = ListAreas[1][0];
        tablaitemP->setText(QString::number(d1,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(1,1,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Circunferencia
        d2 = ListAreas[1][1];
        tablaitemP->setText(QString::number(d2,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(1,2,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//%1
        tablaitemP->setText(QString::number(d1/d2*100,'f',2));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(1,3,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Cuadrado
        d3 = ListAreas[1][2];
        tablaitemP->setText(QString::number(d3,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(1,4,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//%2
        tablaitemP->setText(QString::number(d1/d3*100,'f',2));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(1,5,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();
        datosProbabilidad.append (datostabla);
        datostabla.clear ();
        /*******row2************/
        tablaitemP = new QTableWidgetItem();//ZonaDR
        tablaitemP->setText("D.R.");
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(2,0,tablaitemP);
        datostabla =  tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Cubierta
        d1 = ListAreas[2][0];
        tablaitemP->setText(QString::number(d1,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(2,1,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Circunferencia
        d2 = ListAreas[2][1];
        tablaitemP->setText(QString::number(d2,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(2,2,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//%1
        tablaitemP->setText(QString::number(d1/d2*100,'f',2));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(2,3,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Cuadrado
        d3 = ListAreas[2][2];
        tablaitemP->setText(QString::number(d3,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(2,4,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//%2
        tablaitemP->setText(QString::number(d1/d3*100,'f',2));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(2,5,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();
        datosProbabilidad.append (datostabla);
        datostabla.clear ();
        /*******************/
    }
    else
    {
        tablaP->setRowCount(1);
        QString auxF = fichero;
        if(alcance)
        {
            auxF.insert (6,"a");
        }
        QString m = fichero.left (3);
        tablaitemP = new QTableWidgetItem();//ZonaECC
        tablaitemP->setText(m);
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,0,tablaitemP);
        datostabla =  tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Cubierta
        d1 = ListAreas[0][0];
        tablaitemP->setText(QString::number(d1,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,1,tablaitemP);
        datostabla = datostabla+","+tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Circunferencia
        d2 = ListAreas[0][1];
        tablaitemP->setText(QString::number(d2,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,2,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//%1
        tablaitemP->setText(QString::number(d1/d2*100,'f',2));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,3,tablaitemP);
        datostabla = datostabla+","+tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//Area Cuadrado
        d3 = ListAreas[0][2];
        tablaitemP->setText(QString::number(d3,'f',3));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,4,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        tablaitemP = new QTableWidgetItem();//%2
        tablaitemP->setText(QString::number(d1/d3*100,'f',2));
        tablaitemP->setTextAlignment(Qt::AlignCenter);
        tablaP->setItem(0,5,tablaitemP);
        datostabla = datostabla+","+ tablaitemP->text ();

        datosProbabilidad.append (datostabla);
        datostabla.clear ();
    }

    tablaP->horizontalHeader ()->resizeContentsPrecision ();
    tablaP->resizeColumnsToContents ();

    tablaP->setMinimumSize (550,200);
    tablaP->setMaximumSize (550,200);
    dialogTablaP->setMinimumSize (660,250);
    dialogTablaP->setMaximumSize (660,250);

}

void DistanciaVD::calcularArea(int dis)
{
    Area = 0;

    D0 = 0;
    D1 = 0;
    r = 0;
    r2 = 0;
    r3 = 0;
    QList<double> auxDVD;
    areaCirculo = 0;
    areaCuadrado = 0;
    double S1 = 0;
    double S2 = 0;

    for (int a = 0; a < 360; ++a)
    {
        if(a<359)
        {
            D0 = disVDirecta[a].at (dis)/1000;
            D1 = disVDirecta[a+1].at (dis)/1000;
            for (int i = 0; i < disInternas[a].length (); ++i)
            {
                if((i)%2 == 0)
                {
                    S1 = disInternas[a].at (i)/1000;
                    S2 = disInternas[a].at (i+1)/1000;
                    D0 = D0 - (S2-S1);
                }
            }
            for (int i = 0; i < disInternas[a+1].length (); ++i)
            {
                if((i)%2 == 0)
                {
                    S1 = disInternas[a+1].at (i)/1000;
                    S2 = disInternas[a+1].at (i+1)/1000;
                    D1 = D1 - (S2-S1);
                }
            }
            r = (D0+D1)/2;
        }
        else
        {
            D0 = disVDirecta[a].at (dis)/1000;
            D1 = disVDirecta[0].at (dis)/1000;

            for (int i = 0; i < disInternas[a].length (); ++i)
            {
                if((i)%2 == 0)
                {
                    S1 = disInternas[a].at (i)/1000;
                    S2 = disInternas[a].at (i+1)/1000;
                    D0 = D0 - (S2-S1);
                }
            }
            for (int i = 0; i < disInternas[0].length (); ++i)
            {
                if((i)%2 == 0)
                {
                    S1 = disInternas[0].at (i)/1000;
                    S2 = disInternas[0].at (i+1)/1000;
                    D1 = D1 - (S2-S1);
                }
            }

            r = (D0+D1)/2;
        }
        Area = Area + 0.00873 * pow(r,2) ;

        auxDVD.append (disVDirecta[a].at (dis)/1000);
    }
    if(medio == 41 || medio == 48)
    {
        Area = Area-AreaZM-AreaZM2;
    }
    else
        Area = Area-PI*pow(dedZone,2);

    qSort(auxDVD);
    areaCirculo = PI * pow(auxDVD.last (),2);
    areaCuadrado = pow(2 * auxDVD.last (),2);
    //  tablaProbabilidad (prob,Area,areaCirculo,areaCuadrado);
}

void DistanciaVD::configInterface(int g,int m,int a)
{
    Q_UNUSED(a);
    if((m == 39  || m ==38  || m ==37  || m ==36 || m ==35 || m ==34 || m ==33 || m ==32 || m ==31 || medio == 27 || medio == 26|| m == 25 || m == 24 || m == 23 || m == 22 || m == 21 || m == 42 || m == 43 || m == 44) && g == 1 )
    {
        ui->chB_DLanzamiento->setVisible (true);
        ui->chB_DLanzMin->setVisible (true);
        ui->chB_DZDRealizanle->setVisible (true);
        ui->chB_DZDReaMin->setVisible (true);
        ui->chB_DECCMin->setVisible (true);
        if(medio == 27 || medio == 26|| m == 25 || m == 24 || m == 23 || m == 22 || m == 21 || m == 42 || m == 43 || m == 44)
        {
            ui->chB_DLanzamiento->setText ("D. Apertura Fuego");
            ui->chB_DLanzMin->setText ("D.A.F.Min");
        }
    }
}

void DistanciaVD::cerrartabla(int number)
{
    Q_UNUSED(number);
    ui->chB_Tabla->setChecked(false);
}

void DistanciaVD::cerrartablaP(int number)
{
    Q_UNUSED(number);
    ui->chB_Probabilidad->setChecked(false);
}

void DistanciaVD::mouseMove(QMouseEvent *event)
{
    double AzmObj = 0.0;

    double x = (ui->widget->xAxis->pixelToCoord(event->pos().x()));
    double y = (ui->widget->yAxis->pixelToCoord(event->pos().y()));

    DanCalc danca;
    double distancia = danca.Distancia(x,y);

    double xx = fabs(x);
    double yy = fabs(y);
    double cita = (180/M_PI)*atan2(yy,xx);
    if(x >= 0 && y < 0)//cuadrante 2
        AzmObj =(90.0 + cita);
    if(x > 0.0 && y >= 0.0)//cuadrante 1
        AzmObj =(90.0 - cita);
    if(x < 0.0 && y >= 0.0)//cuadrante 4
        AzmObj =(270.0 + cita);
    if(x < 0.0 && y <= 0.0)//cuadrante 3
        AzmObj =(270.0 - cita);

    QString punto = "Distancia:"+QString::number(distancia)+"Km; Azimut:"+QString::number(AzmObj)+"°";
    ui->widget->setToolTip(punto);
}

void DistanciaVD::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    llenartabla(false,0);
    ui->chB_Tabla->setChecked(false);
    emit cerro(this);
}

void DistanciaVD::on_pB_SalvarGrafica_clicked()
{
    filePrintPdf(true);
}

void DistanciaVD::on_pB_ZoomMas_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommas(ui->widget);
}

void DistanciaVD::on_pB_ZoomMen_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommenos(ui->widget);
}

void DistanciaVD::on_chB_ZMuerta1_toggled(bool checked)
{
    ui->widget->layer("ZMMuis1")->setVisible(checked);
    ui->widget->layer("ZMMuis1")->replot();

}

void DistanciaVD::on_chB_ZMuerta2_toggled(bool checked)
{
    ui->widget->layer("ZMMuis-1")->setVisible(checked);
    ui->widget->layer("ZMMuis-1")->replot();
}

void DistanciaVD::on_chB_Tabla_toggled(bool checked)
{
    if(checked)
    {
        dialogTabla->show();
    }
    else
        dialogTabla->close();
}

void DistanciaVD::on_chB_Probabilidad_toggled(bool checked)
{
    if(checked)
    {
        calcularArea(0);
        ListAreas[0][0] = Area;
        ListAreas[0][1] = areaCirculo;
        ListAreas[0][2] = areaCuadrado;
        if( ((medio == 39 && alvue >=100) || ((medio == 38 || medio == 37 || medio == 36  || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 27 || medio == 26|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21 || medio == 42 || medio == 43 || medio == 44) && alvue >= 25)) && grupo == 1)
        {
            calcularArea(1);
            ListAreas[1][0] = Area;
            ListAreas[1][1] = areaCirculo;
            ListAreas[1][2] = areaCuadrado;
            calcularArea(2);
            ListAreas[2][0] = Area;
            ListAreas[2][1] = areaCirculo;
            ListAreas[2][2] = areaCuadrado;
        }
        tablaProbabilidad (prob/*,Area,areaCirculo,areaCuadrado*/,medio);
        dialogTablaP->show();
    }
    else
        dialogTablaP->close();
}

void DistanciaVD::on_chB_DLanzamiento_toggled(bool checked)
{
    qDebug()<<"entro";
    if(checked)
    {
        if((medio == 39 && alvue >=100 ) || ((medio == 38 || medio == 37 || medio == 36 || medio ==35 || medio ==34 || medio ==33 || medio ==32 || medio ==31|| medio == 27 || medio == 26 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21 || medio == 42 || medio == 43 || medio == 44) && alvue >=25)  )
        {
            pintarDistancias (1);
        }
    }
    else
    {
        for (int i = 0; i < ui->widget->plottableCount (); ++i)
        {
            if(ui->widget->plottable (i)->name () == "DisLanzamiento")
            {
                ui->widget->removePlottable (i);
                i--;
            }
        }
    }
    ui->widget->layer("DisLanzamiento")->setVisible(checked);
    ui->widget->layer("DisLanzamiento")->replot();
}

void DistanciaVD::on_chB_DZDRealizanle_toggled(bool checked)
{
    if(checked)
    {
        if((medio == 39 && alvue >=100 ) || ((medio == 38 || medio == 37 || medio == 36 || medio ==35 || medio ==34 || medio ==33 || medio ==32 || medio ==31|| medio == 27 || medio == 26 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21 || medio == 42 || medio == 43 || medio == 44) && alvue >=25)  )
        {
            pintarDistancias (2);
        }
    }
    else
    {
        for (int i = 0; i < ui->widget->plottableCount (); ++i)
        {
            if(ui->widget->plottable (i)->name () == "DisZonarealizable")
            {
                ui->widget->removePlottable (i);
                i--;
            }
        }
    }

    ui->widget->layer("DisZonarealizable")->setVisible(checked);
    ui->widget->layer("DisZonarealizable")->replot();
}

void DistanciaVD::on_chB_DECCMin_toggled(bool checked)
{
    ui->widget->layer("DisDEteccionMin")->setVisible(checked);
    ui->widget->layer("DisDEteccionMin")->replot();
}

void DistanciaVD::on_chB_ZMuerta2_clicked()
{

}

void DistanciaVD::on_chB_DECCMin_clicked()
{

}

void DistanciaVD::on_chB_ZMuerta1_clicked()
{

}

void DistanciaVD::on_chB_DLanzamiento_clicked()
{

}

void DistanciaVD::on_chB_DZDRealizanle_clicked()
{

}

void DistanciaVD::on_chB_Tabla_clicked()
{

}

void DistanciaVD::on_chB_DLanzMin_toggled(bool checked)
{
    ui->widget->layer("DisLanzMin")->setVisible(checked);
    ui->widget->layer("DisLanzMin")->replot();
}


void DistanciaVD::on_chB_DZDReaMin_toggled(bool checked)
{
    ui->widget->layer("DisZonReaMin")->setVisible(checked);
    ui->widget->layer("DisZonReaMin")->replot();
}

void DistanciaVD::on_pB_Sector_clicked()
{
    bool ok;
    int a1 = QInputDialog::getInt(this, tr("Azimut inicio"),
                                  tr("A1:"), 0, 0, 360, 1, &ok);
    if (ok)
        qDebug()<<"A1: "<<a1;

    int a2 = QInputDialog::getInt(this, tr("Azimut fin"),
                                  tr("A2:"), 0, 0, 360, 1, &ok);
    if (ok)
        qDebug()<<"A2: "<<a2;

    //    if(a1 < a2)
    customplotf.pintaSector(ui->widget, a1, a2,true,QPen(QColor(Qt::red),2),m_dismax);
    //        else
    //            customplotf.pintaSector(ui->widget, a2,a1,true,QPen(QColor(Qt::red),2),m_dismax);

    ui->widget->layer("SectorPrincipal")->setVisible(true);
    ui->widget->layer("SectorPrincipal")->replot();


}
