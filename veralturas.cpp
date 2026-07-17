#include "veralturas.h"
#include "ui_veralturas.h"

VerAlturas::VerAlturas(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VerAlturas)
{
    ui->setupUi(this);

    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables|
                                /*QCP::iSelectItems |*/ QCP::iSelectLegend);
    //    ui->widget->setMinimumSize(600,600);
    ui->widget->plotLayout()->insertRow(0);
    plottitle = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(0, 0,plottitle );
    plottitle2 = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(2, 0,plottitle2 );
    initGraph();

    cont = 0;
    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(ui->widget, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int,QMouseEvent*)));
    ui->widget->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
    connect(ui->widget, SIGNAL(legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendItemClicked(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)));


    probabilidad = 0;

    dialogTabla = new QDialog();
    dialogTabla->setWindowFlags(Qt::WindowMinMaxButtonsHint);
    dialogTabla->setWindowFlags(dialogTabla->windowFlags() |= Qt::WindowCloseButtonHint);
    connect(dialogTabla,SIGNAL(finished(int)),this,SLOT(cerrartablaP(int)));

    salvarTabla = new QPushButton("Salvar Tabla",dialogTabla);
    salvarTabla->setMaximumSize (100,30);
    salvarTabla->setMinimumSize (100,30);
    connect(salvarTabla,SIGNAL(clicked(bool)),this,SLOT(salvaTablaP(bool)));

    headerView = new QHeaderView(Qt::Horizontal);
    tabla = new QTableWidget(dialogTabla);
    headerView->resizeSection (1,10);
    tabla->setHorizontalHeader(headerView);

    auto *QHBbotonLayout = new QHBoxLayout;
    auto *QGridlayout = new QGridLayout;
    QHBbotonLayout->insertSpacing (1,dialogTabla->width()-salvarTabla->width ());
    QHBbotonLayout->addWidget(salvarTabla);
    QHBbotonLayout->insertSpacing (-1,dialogTabla->width()-salvarTabla->width ());

    QGridlayout->addLayout(QHBbotonLayout, 0, 0);
    QGridlayout->addWidget (tabla, 1, 0);

    dialogTabla->setLayout(QGridlayout);
    dialogTabla->setMaximumSize (705,500);
    dialogTabla->setMinimumSize (705,500);

}

VerAlturas::~VerAlturas()
{
    delete ui;
}

void VerAlturas::initGraph()
{
    for (auto i : listaZM)
    {
        ui->widget->removeItem(i);
    }

    listaZM.clear ();
    ui->widget->yAxis->setRange(-600,600);//se estabelece rango de y
    ui->widget->xAxis->setRange(-600,600);
    ui->widget->xAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->yAxis->setBasePen(QPen(Qt::black,1));
    ui->widget->xAxis->setVisible(false);
    ui->widget->yAxis->setVisible(false);

    pintaLineaAzimut(ui->widget);
    pintaElipses(ui->widget,5,true,false);
    pintaElipses(ui->widget,10,true,true);
    pintaElipses(ui->widget,20,true,false);
    bool aux = true;
    bool aux2 = true;

    for (int i = 40; i <= 700; i+=20)
    {
        pintaElipses(ui->widget,i,aux,aux2);
        aux = !aux;
        if(aux)
            aux2 = !aux2;
    }

    ui->widget->replot();
}

void VerAlturas::pintaElipses(QCustomPlot *customplot, int coord,bool text,bool align)
{

    auto *elipse=new QCPItemEllipse(customplot);

    elipse->topLeft->setCoords(-coord,coord);
    elipse->bottomRight->setCoords(coord,-coord);
    if(text)
    {
        auto *distancia = new QCPItemText(customplot);
        distancia->setText(QString::number(coord)+"Km");
        distancia->position->setCoords(0,coord);
        if(align)
            distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
        else
            distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);
    }
}

void VerAlturas::pintaLineaAzimut(QCustomPlot *customPlot)
{
    double x;
    double y;
    for (int i = 0; i < 359; i+=30)
    {
        x = 710*sin(i*RADIAN);//lineas
        y = 710*cos(i*RADIAN);

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

void VerAlturas::pintarDistancias(const QPen& pen,int alt,int z)
{
    QVector<double> x1; QVector<double> y1;
    //    int z = 1; if(zona) z=0;
    for (int a = 0; a <= 360; a++)
    {
        if(a < 360)
        {
            x1.append(disVDirecta[a].at(z)/1000 * sin(a * RADIAN));
            y1.append(disVDirecta[a].at(z)/1000 * cos(a * RADIAN));

        }
        else//360
        {
            x1.append(disVDirecta[0].at(z)/1000*sin(0 * RADIAN));
            y1.append(disVDirecta[0].at(z)/1000*cos(0 * RADIAN));
        }
    }
    auto *curDistancias = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curDistancias->setName("Altura del objetivo: " +QString::number (alt));
    curDistancias->setObjectName (QString::number (cont));
    curDistancias->setData(x1, y1);
    curDistancias->setPen(pen);

    cont++;

    pintarZonaMuerta(zonaMuerta (fichero));

}

void VerAlturas::pintarTodasProbabilidades(const QPen& pen, int p,const QString& prob,bool zona)
{Q_UNUSED(zona);
    QVector<double> x1; QVector<double> y1;
    //    int z = 2; if(zona) z=0;

    for (int a = 0; a <= 360; a++)
    {
        if(a < 360)
        {
            x1.append(disVDirecta[a].at(p)/1000 * sin(a * RADIAN));
            y1.append(disVDirecta[a].at(p)/1000 * cos(a * RADIAN));

        }
        else//360
        {
            x1.append(disVDirecta[0].at(p)/1000*sin(0 * RADIAN));
            y1.append(disVDirecta[0].at(p)/1000*cos(0 * RADIAN));
        }
    }
    auto *curDistancias = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curDistancias->setName("Probabilidad: "+prob);
    curDistancias->setObjectName (QString::number (cont));
    curDistancias->setData(x1, y1);
    curDistancias->setPen(pen);

    cont++;
}

bool VerAlturas::leer(const QString& extencion)
{
    ui->chB_Tabla->setVisible (false);
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
        if (fichero+extencion==nombfich)
        {
            entra=true;
            break;
        }
    }
    if(entra)
    {
        QStringList todo;
        QFile file(QDir::currentPath() + "/Zonas/"+fichero+extencion);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            todo.append(line);
        }
        if( todo.length() == 360)
        {

            Av = alvuelo(QDir::currentPath() + "/Zonas/"+fichero+extencion);
            LisAv.append (Av);
            listo = true;
            for (int i = 0;i < 360; ++i)
            {
                QStringList grande=todo.at(i).split("*");
                QStringList visibilidad;
                //                double visib = 0;
                double disLanzamiento = 0;
                double disZDR = 0;
                //                QString dislan;
                if(probabilidad == 0)
                {
                    visibilidad = grande.at(1).split(",");
                    disVDirecta[i].append(visibilidad.at(0).toDouble());
                     //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                    if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                    {
                        visibilidad = grande.at (8).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (13).split(",");
                        disZDR = visibilidad.at(0).toDouble();
                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }
                else if(probabilidad == 1)
                {
                    visibilidad = grande.at(4).split(",");
                    disVDirecta[i].append(visibilidad.at(0).toDouble());
                     //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                    if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                    {
                        visibilidad = grande.at (9).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (14).split(",");
                        disZDR = visibilidad.at(0).toDouble();

                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }
                else if(probabilidad == 2)
                {
                    visibilidad = grande.at(5).split(",");
                    disVDirecta[i].append(visibilidad.at(0).toDouble());
                     //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                    if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                    {
                        visibilidad = grande.at (10).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (15).split(",");
                        disZDR = visibilidad.at(0).toDouble();

                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }
                else if(probabilidad == 3)
                {
                    visibilidad = grande.at(6).split(",");
                    disVDirecta[i].append(visibilidad.at(0).toDouble());
                     //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                    if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                    {
                        visibilidad = grande.at (11).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (16).split(",");
                        disZDR = visibilidad.at(0).toDouble();

                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }
                else
                {
                    visibilidad = grande.at(7).split(",");
                    disVDirecta[i].append(visibilidad.at(0).toDouble());
                     //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                    if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                    {
                        visibilidad = grande.at (12).split(",");
                        disLanzamiento = visibilidad.at(0).toDouble();

                        visibilidad = grande.at (17).split(",");
                        disZDR = visibilidad.at(0).toDouble();

                        disVDirecta[i].append(disLanzamiento);
                        disVDirecta[i].append(disZDR);
                    }
                }

                StrucdisVDirecta.ldvd[0].append (disVDirecta[i].at (0));
                 //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31|| medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                {
                    StrucdisVDirecta.ldvd[1].append (disVDirecta[i].at (1));
                    StrucdisVDirecta.ldvd[2].append (disVDirecta[i].at (2));
                }
            }
            LdisVDirecta.append (StrucdisVDirecta);
            StrucdisVDirecta.ldvd[0].clear ();
            StrucdisVDirecta.ldvd[1].clear ();
            StrucdisVDirecta.ldvd[2].clear ();
        }
        else
        {
            listo = false;
            file.remove();
        }
    }
    return listo;
}

bool VerAlturas::leerProbabilidad(const QString& extencion)
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
        if (fichero+extencion==nombfich)
        {
            entra=true;
            break;
        }
    }
    if(entra)
    {
        QStringList todo;
        QFile file(QDir::currentPath() + "/Zonas/"+fichero+extencion);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            todo.append(line);
        }
        if( todo.length() == 360)
        {

            Av = alvuelo(QDir::currentPath() + "/Zonas/"+fichero+extencion);
            LisAv.append (Av);
            listo = true;
            for (int i = 0;i < 360; ++i)
            {
                QStringList grande=todo.at(i).split("*");
                QStringList visibilidad;
                double disLanzamiento = 0;
                double disZDR = 0;
                visibilidad = grande.at (1).split(",");//0.5
                disVDirecta[i].append(visibilidad.at(0).toDouble());//0.5

                visibilidad = grande.at(4).split(",");
                disVDirecta[i].append(visibilidad.at(0).toDouble());//0.6

                visibilidad = grande.at(5).split(",");
                disVDirecta[i].append(visibilidad.at(0).toDouble());//0.7

                visibilidad = grande.at(6).split(",");
                disVDirecta[i].append(visibilidad.at(0).toDouble());//0.8

                visibilidad = grande.at(7).split(",");
                disVDirecta[i].append(visibilidad.at(0).toDouble());//0.9
                 //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                {
                    visibilidad = grande.at (8).split(",");
                    disLanzamiento = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disLanzamiento);
                    visibilidad = grande.at (9).split(",");
                    disLanzamiento = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disLanzamiento);
                    visibilidad = grande.at (10).split(",");
                    disLanzamiento = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disLanzamiento);
                    visibilidad = grande.at (11).split(",");
                    disLanzamiento = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disLanzamiento);
                    visibilidad = grande.at (12).split(",");
                    disLanzamiento = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disLanzamiento);

                    visibilidad = grande.at (13).split(",");
                    disZDR = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disZDR);

                    visibilidad = grande.at (14).split(",");
                    disZDR = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disZDR);

                    visibilidad = grande.at (15).split(",");
                    disZDR = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disZDR);

                    visibilidad = grande.at (16).split(",");
                    disZDR = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disZDR);

                    visibilidad = grande.at (17).split(",");
                    disZDR = visibilidad.at(0).toDouble();
                    disVDirecta[i].append(disZDR);
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

void VerAlturas::on_pB_ZoomMas_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommas(ui->widget);
}

void VerAlturas::on_pB_ZoomMenos_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommenos(ui->widget);
}

void VerAlturas::on_pB_SalvarGrafica_clicked()
{
    salvarGrafica(cual);
}

void VerAlturas::salvarGrafica(const QString& altura)
{
    QString nom;
    nom = (QDir::currentPath()+"/GraficasPDF/"+fichero+"_"+altura);

    QString fileName = QFileDialog::getSaveFileName(this, "Exportar a PDF",nom,"*.pdf",nullptr,nullptr);

    if (!fileName.isEmpty())
    {
        ui->widget->savePdf(fileName,700,700);
    }

    QString fileName2 = QFileDialog::getSaveFileName(this, "Exportar a JPG",nom,"*.jpg",nullptr,nullptr);
    if (!fileName2.isEmpty())
    {
        ui->widget->saveJpg(fileName2,700,700);
    }
}

void VerAlturas::on_checkBox_toggled(bool checked)
{
    if(checked)
    {
        //make legend wrap text in two columns
        ui->widget->legend->setWrap(2);
        ui->widget->legend->setRowSpacing(1);
        ui->widget->legend->setColumnSpacing(2);
        ui->widget->legend->setFillOrder(QCPLayoutGrid::FillOrder::foColumnsFirst,true);
        ui->widget->legend->setVisible(true);
    }
    else
    {
        ui->widget->legend->setVisible(false);
    }
    ui->widget->replot();
}

void VerAlturas::legendItemClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *event)
{
    Q_UNUSED(legend);
    Q_UNUSED(event);
    for (int i=0; i < ui->widget->plottableCount (); ++i)//seleccionar grafica
    {
        auto *curve = dynamic_cast<QCPCurve*>(ui->widget->plottable (i));
        if(legendItem == ui->widget->legend->itemWithPlottable(curve))
        {
            curve->setSelection (QCPDataSelection(curve->data()->dataRange()));
            break;
        }
    }
}

void VerAlturas::graphClicked(QCPAbstractPlottable *plottable, int dataIndex,QMouseEvent* event)
{
    Q_UNUSED(event);
    Q_UNUSED(dataIndex);
    QString message = plottable->name ();

    plottitle2->setText (message);

    QCPPlottableLegendItem *item = ui->widget->legend->itemWithPlottable(plottable);
    item->setSelected(true);
}

void VerAlturas::cerrartablaP(int number)
{
    Q_UNUSED(number);
    ui->chB_Tabla->setChecked(false);
}

void VerAlturas::salvaTablaP(bool b)
{
    Q_UNUSED (b);
    QString nom;
    QString aux;
    aux = fichero;

    aux.replace(":", "" );
    aux.replace(".", "_" );

    if(aux.at (4) =='0')//pov
        aux.insert (4,"0");
    QString heathers;

    aux.insert (24,"TP_");
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
            arr << "<td align=center colspan=6><b> Todas las Probabilidades: "+aux+"</b></td>";
            arr << "</tr>";

            heathers = "Azimut(°)";
            arr << "<tr>" <<
                   "<td align=center><b>" + heathers + "</b></td>";

            heathers = "D.V.D.P P=0.5 (Km)";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "D.V.D.P P=0.6 (Km)";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "D.V.D.P P=0.7 (Km)";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "D.V.D.P P=0.8 (Km)";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            heathers = "D.V.D.P P=0.9 (Km)";
            arr << "<td align=center><b>" + heathers + "</b></td>";

            arr << "</tr>" ;
            for (int i = 0; i < 360; ++i)
            {
                //                datostabla.clear();
                //                datostablalist.clear ();
                //                datostabla = datosProbabilidad.at (i);
                //                datostablalist.append (datostabla.split (','));
                arr << "<tr>" <<
                       "<td align=center>" +QString::number(i) + "</td>" <<
                       "<td align=center>" +QString::number(disVDirecta[i].at(0)/1000) + "</td>" <<
                       "<td align=center>" +QString::number(disVDirecta[i].at(1)/1000) + "</td>" <<
                       "<td align=center>" +QString::number(disVDirecta[i].at(2)/1000) + "</td>" <<
                       "<td align=center>" +QString::number(disVDirecta[i].at(3)/1000) + "</td>" <<
                       "<td align=center>" +QString::number(disVDirecta[i].at(4)/1000) + "</td>" ;
                arr << "</tr>" ;
            }
            arr <<  "</table>" <<
                    "</body>" <<
                    "</html>";
            f.close();
        }
    }
}

int VerAlturas::alvuelo(const QString& strFile)
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

double VerAlturas::zonaMuerta(const QString& strFile)
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
        //      if(m >= 36)
        //        {
        //          ui->cB_Probabilidad->setEnabled (true);
        //        }
        //      else
        //        {
        //          ui->cB_Probabilidad->setEnabled (false);
        //        }
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

void VerAlturas::pintarZonaMuerta(double zonamuerta)
{
    double dedZone = zonamuerta/1000;
    if(dedZone>0)
    {
        CustomPlotF customplotf;
        customplotf.pintaElipses (ui->widget,dedZone,"",QColor(0,0,255),false,QPen(Qt::blue,2),&listaZM);
    }
}

void VerAlturas::llenartabla(bool llenar, int medios)
{
    medio = medios;
    if(llenar)
    {//medio 0 11 12 13
        //medio y grupo 2 3
        if(medios != 0 && medios != 11 && medios != 12  && medios != 13  && grupo != 2 && grupo !=3)
        {
            dialogTabla->setWindowTitle ("Tabla de probabilidades");

            tabla->setColumnCount(6);

            tabla->setHorizontalHeaderLabels(QString("Azimut(°);D.V.D.P. P = 0.5(Km);D.V.D.P. P = 0.6(Km);D.V.D.P. P = 0.7(Km);D.V.D.P. P = 0.8(Km);D.V.D.P. P = 0.9(Km);").split(";"));
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

            tablaitem = new QTableWidgetItem();//DVD0.5
            tablaitem->setText(QString::number(disVDirecta[i].at(0)/1000));
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,1,tablaitem);

            tablaitem = new QTableWidgetItem();//DVD0.6
            tablaitem->setText(QString::number(disVDirecta[i].at(1)/1000));
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,2,tablaitem);

            tablaitem = new QTableWidgetItem();//DVD0.7
            tablaitem->setText(QString::number(disVDirecta[i].at(2)/1000));
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,3,tablaitem);

            tablaitem = new QTableWidgetItem();//DVD0.8
            tablaitem->setText(QString::number(disVDirecta[i].at(3)/1000));
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,4,tablaitem);

            tablaitem = new QTableWidgetItem();//DVD0.9
            tablaitem->setText(QString::number(disVDirecta[i].at(4)/1000));
            tablaitem->setTextAlignment(Qt::AlignCenter);
            tabla->setItem(i,5,tablaitem);
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

void VerAlturas::configInterface()
{
    //tuvimos que hacer visible los ckb para los medios medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42
    if((medio == 44 || medio == 43 || medio == 42 || medio == 39 || medio == 38 || medio == 37 || medio == 36  || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 27 || medio == 26 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && grupo == 1 )
    {
        ui->rB_DVDP->setVisible (true);
        ui->rB_ZDR->setVisible (true);
        ui->rB_DZL->setVisible (true);
    }
    else
    {
        ui->rB_DVDP->setVisible (false);
        ui->rB_ZDR->setVisible (false);
        ui->rB_DZL->setVisible (false);
    }
}

void VerAlturas::on_chB_Tabla_toggled(bool checked)
{
    if(checked)
    {
        //      dialogTabla->setFixedSize(width(),height());
        dialogTabla->show();
    }
    else
        dialogTabla->close();
}

void VerAlturas::on_rB_DVDP_toggled(bool checked)
{
    ui->widget->clearPlottables ();
    cont=0;
    if(checked)
    {
        if(probOno)
        {
            for (int i = 0; i < LdisVDirecta.length (); ++i)
            {
                Av = LisAv.at (i);

                for (int j = 0; j < 360; ++j)
                {
                    disVDirecta[j].clear();

                    disVDirecta[j].append (LdisVDirecta[i].ldvd[0].at (j));
                     //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                    if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                    {
                        disVDirecta[j].append (LdisVDirecta[i].ldvd[1].at (j));
                        disVDirecta[j].append (LdisVDirecta[i].ldvd[2].at (j));
                    }
                }

                pintarDistancias (QPen(QColor(0,255,0,150),2),LisAv.at (i),0);

            }
        }
        else
        {
            pintarTodasProbabilidades (QPen(Qt::green,2),4,"0.9",true);
            pintarTodasProbabilidades (QPen(Qt::green,2),3,"0.8",true);
            pintarTodasProbabilidades (QPen(Qt::green,2),2,"0.7",true);
            pintarTodasProbabilidades (QPen(Qt::green,2),1,"0.6",true);
            pintarTodasProbabilidades (QPen(Qt::green,2),0,"0.5",true);
        }
    }
    else
    {

    }
    ui->widget->replot ();
}

void VerAlturas::on_rB_DZL_toggled(bool checked)
{
    ui->widget->clearPlottables ();
    cont=0;
    if(checked)
    {
        if(probOno)
        {
            for (int i = 0; i < LdisVDirecta.length (); ++i)
            {
                Av = LisAv.at (i);
                for (int j = 0; j < 360; ++j)
                {
                    disVDirecta[j].clear();
                    disVDirecta[j].append (LdisVDirecta[i].ldvd[0].at (j));
                     //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                    if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                    {
                        disVDirecta[j].append (LdisVDirecta[i].ldvd[1].at (j));
                        disVDirecta[j].append (LdisVDirecta[i].ldvd[2].at (j));
                    }
                }
                 //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                {
                    pintarDistancias (QPen(QColor(10,100,120),2),LisAv.at (i),1);
                }
            }
        }
        else
        {
            pintarTodasProbabilidades (QPen(QColor(10,100,120),2),9,"0.9",true);
            pintarTodasProbabilidades (QPen(QColor(10,100,120),2),8,"0.8",true);
            pintarTodasProbabilidades (QPen(QColor(10,100,120),2),7,"0.7",true);
            pintarTodasProbabilidades (QPen(QColor(10,100,120),2),6,"0.6",true);
            pintarTodasProbabilidades (QPen(QColor(10,100,120),2),5,"0.5",true);
        }
    }
    ui->widget->replot ();
}

void VerAlturas::on_rB_ZDR_toggled(bool checked)
{
    ui->widget->clearPlottables ();
    cont=0;
    if(checked)
    {
        if(probOno)
        {
            for (int i = 0; i < LdisVDirecta.length (); ++i)
            {
                Av = LisAv.at (i);
                for (int j = 0; j < 360; ++j)
                {
                    disVDirecta[j].clear();
                    disVDirecta[j].append (LdisVDirecta[i].ldvd[0].at (j));
                     //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                    if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                    {
                        disVDirecta[j].append (LdisVDirecta[i].ldvd[1].at (j));
                        disVDirecta[j].append (LdisVDirecta[i].ldvd[2].at (j));
                    }
                }
                   //se agreg'o 26 27 42 43 44 para poder ver su reprecentaci'on
                if(((medio == 39 && Av >= 100) || ((medio == 26|| medio == 27|| medio == 44|| medio == 43|| medio == 42||medio == 38 || medio == 37 || medio == 36 || medio == 35 || medio == 34 || medio == 33 || medio == 32 || medio == 31 || medio == 25 || medio == 24 || medio == 23 || medio == 22 || medio == 21) && Av >= 25)) && grupo == 1)
                {
                    pintarDistancias (QPen(QColor(255,0,0,150),2),LisAv.at (i),2);
                }
            }
        }
        else
        {
            pintarTodasProbabilidades (QPen(Qt::red,2),14,"0.9",true);
            pintarTodasProbabilidades (QPen(Qt::red,2),13,"0.8",true);
            pintarTodasProbabilidades (QPen(Qt::red,2),12,"0.7",true);
            pintarTodasProbabilidades (QPen(Qt::red,2),11,"0.6",true);
            pintarTodasProbabilidades (QPen(Qt::red,2),10,"0.5",true);
        }
    }
    ui->widget->replot ();
}
