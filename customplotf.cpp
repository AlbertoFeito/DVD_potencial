#include "customplotf.h"

CustomPlotF::CustomPlotF(QWidget *parent) : QCustomPlot(parent)
{
    puntos = new AZIDIS*[360];
    for (int a = 0; a < 360; a++)
    {
        puntos[a] = new AZIDIS[360];
        for (int d = 0; d < 360; d++)
        {
            QUtiles::PolarToReal(a, (d + 1), PM_0_90, &puntos[a][d].X, &puntos[a][d].Y);
        }
    }

    //    curvaSector = new QCPCurve(customplot->xAxis, customplot->yAxis);
    //    curvaSector->setLayer (LayerName);
    //    curvaSector->setName(name);
    //    curvaSector->setData(x,y);
    //    curvaSector->setPen(pen);
    //    curvaSector->setBrush (QBrush(QColor(0,0,0),Qt::Dense7Pattern));
    //    curvaSector->setScatterStyle (scStyle);
}

void CustomPlotF::initGraph(QCustomPlot *customPlot,double lowerY,double upperY,double lowerX,double upperX,bool visibleX,bool visibleY)
{
    customPlot->yAxis->setRange(lowerY,upperY);//se estabelece rango de y
    customPlot->xAxis->setRange(lowerX,upperX);
    customPlot->xAxis->setBasePen(QPen(Qt::black,1));
    customPlot->yAxis->setBasePen(QPen(Qt::black,1));
    customPlot->xAxis->setVisible(visibleX);
    customPlot->yAxis->setVisible(visibleY);
    this->addLayer("AzimutDist",nullptr,QCustomPlot::limAbove);
    this->addLayer("Sector",nullptr,QCustomPlot::limAbove);
    customPlot->replot ();
}
void CustomPlotF::pintaElipses(QCustomPlot *customplot, double coord, const QString& text,const QColor& textColor, bool align, const QPen& pen)
{
    auto *elipse=new QCPItemEllipse(customplot);

    elipse->topLeft->setCoords(-coord,coord);
    elipse->bottomRight->setCoords(coord,-coord);
    elipse->setPen(pen);
    auto *distancia = new QCPItemText(customplot);
    distancia->setText(text);
    distancia->setColor(textColor);
    distancia->position->setCoords(0,coord);
    if(align)
        distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
    else
        distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);

}
void CustomPlotF::pintaElipses(QCustomPlot *customplot, double coord, const QString& text,const QColor& textColor, bool align, const QPen& pen,QList<QCPItemEllipse *> *listaElipse)
{
    auto *elipse=new QCPItemEllipse(customplot);

    elipse->topLeft->setCoords(-coord,coord);
    elipse->bottomRight->setCoords(coord,-coord);
    elipse->setPen(pen);
    listaElipse->append (elipse);
    auto *distancia = new QCPItemText(customplot);
    distancia->setText(text);
    distancia->setColor(textColor);
    distancia->position->setCoords(0,coord);
    if(align)
        distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
    else
        distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);

}
void CustomPlotF::pintaElipses(QCustomPlot *customplot, double coord, const QString& LayerName, const QString& text, const QColor& textColor, bool align, const QPen& pen)
{
    auto *elipse=new QCPItemEllipse(customplot);
    elipse->setLayer (LayerName);
    elipse->topLeft->setCoords(-coord,coord);
    elipse->bottomRight->setCoords(coord,-coord);
    elipse->setPen(pen);

    auto *distancia = new QCPItemText(customplot);
    distancia->setLayer (LayerName);
    distancia->setText(text);
    distancia->setColor(textColor);
    distancia->position->setCoords(0,coord);
    if(align)
        distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
    else
        distancia->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);

}
void CustomPlotF::pintaElipses(QCustomPlot *customplot, int coord,bool text,bool align)
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
void CustomPlotF::pintaElipses(QCustomPlot *customplot,double lat, double lon,QList<QCPItemEllipse *> *listaElipse,QList<QCPItemText *> *listaText)
{
    double toplefx; double toplefy; double botrighX; double botrighy;

    for (int p = 0;  p <= 320; p+=10)
    {
        auto *elipse=new QCPItemEllipse(customplot);
        elipse->setLayer("AzimutDist");
        listaElipse->append (elipse);
        toplefy = dancal.Latitud_2 (puntos[0][p-1].Y,lat);
        toplefx = dancal.Longitud_2 (puntos[270][p-1].X,lon,lat);

        botrighy = dancal.Latitud_2 (puntos[180][p-1].Y,lat);
        botrighX = dancal.Longitud_2 (puntos[90][p-1].X,lon,lat);

        elipse->topLeft->setCoords(toplefx,toplefy);
        elipse->bottomRight->setCoords(botrighX,botrighy);


        if(p%20==0 && p > 0)
        {
            auto *distancia = new QCPItemText(customplot);
            listaText->append (distancia);
            distancia->setLayer("AzimutDist");
            distancia->setText(QString::number(p)+"Km");
            distancia->position->setCoords(lon,toplefy);
        }
    }
    toplefy = dancal.Latitud_2 (puntos[0][80].Y,lat);
    toplefx = dancal.Longitud_2 (puntos[270][100].X,lon,lat);

    botrighy = dancal.Latitud_2 (puntos[180][80].Y,lat);
    botrighX = dancal.Longitud_2 (puntos[90][100].X,lon,lat);

    customplot->yAxis->setRange(toplefy,botrighy);//se estabelece rango de y
    customplot->xAxis->setRange(toplefx,botrighX);
}

void CustomPlotF::pintaCurva(QCustomPlot *customplot,const QString& name,const QPen& pen, const QVector<double>& x1, const QVector<double>& y1)
{
    auto *curva = new QCPCurve(customplot->xAxis, customplot->yAxis);
    curva->setName(name);
    curva->setData(x1,y1);
    curva->setPen(pen);

}
void CustomPlotF::pintaCurva(QCustomPlot *customplot,QString LayerName,QString name,QPen pen, QVector<double> x, QVector<double> y,QCPScatterStyle scStyle)
{

    if(curvaSector ==nullptr)
        curvaSector = new QCPCurve(customplot->xAxis, customplot->yAxis);
    curvaSector->setLayer (LayerName);
    curvaSector->setName(name);
    curvaSector->setData(x,y);
    curvaSector->setPen(pen);
    curvaSector->setBrush (QBrush(QColor(0,0,0),Qt::Dense7Pattern));
    curvaSector->setScatterStyle (scStyle);

}
void CustomPlotF::pintaCurva(QCustomPlot *customplot,const QString& LayerName,const QString& name,const QPen& pen,const QBrush& brush, const QVector<double>& x1, const QVector<double>& y1)
{
    auto *curva = new QCPCurve(customplot->xAxis, customplot->yAxis);
    curva->setLayer (LayerName);
    curva->setName(name);
    curva->setData(x1,y1);
    curva->setPen(pen);
    curva->setBrush (brush);
}

void CustomPlotF::pintaCurva(QCustomPlot *customplot, const QString& LayerName, const QString& name, const QPen& pen, const QBrush& brush, const QVector<double>& x1, const QVector<double>& y1, QList<QCPCurve *> *listaCurva)
{
    auto *curva = new QCPCurve(customplot->xAxis, customplot->yAxis);
    curva->setLayer (LayerName);
    curva->setName(name);
    curva->setData(x1,y1);
    curva->setPen(pen);
    curva->setBrush (brush);
    listaCurva->append (curva);
}
void CustomPlotF::pintaCurva(QCustomPlot *customplot,const QString& LayerName,const QString& name,const QPen& pen, double x, double y,const QCPScatterStyle& scStyle)
{
    auto *curva = new QCPCurve(customplot->xAxis, customplot->yAxis);
    curva->setLayer (LayerName);
    curva->setName(name);
    curva->addData(x,y);
    curva->setPen(pen);
    curva->setScatterStyle (scStyle);
}
void CustomPlotF::pintaCurva(QCustomPlot *customplot,const QString& LayerName,const QString& name,const QPen& pen, double x, double y,const QCPScatterStyle& scStyle, QList<QCPCurve *> *listaCurva)
{
    auto *curva = new QCPCurve(customplot->xAxis, customplot->yAxis);
    curva->setLayer (LayerName);
    curva->setName(name);
    curva->addData(x,y);
    curva->setPen(pen);
    curva->setScatterStyle (scStyle);
    listaCurva->append (curva);
}
void CustomPlotF::pintaLinea(QCustomPlot *customPlot,const QString& layer, const QPen& pen, double xIni,double yIni,double xFin,double yFin)
{
    QCPLineEnding head;
    head.setStyle( QCPLineEnding::esBar);
    head.setWidth(3);
    head.setLength(3);

    auto  * linein = new QCPItemLine(customPlot);

    linein->setLayer(layer);
    linein->setPen(pen);
    linein->setHead(head);
    linein->setTail(head);

    linein->start->setCoords(xIni,yIni);
    linein->end->setCoords(xFin,yFin);

}
void CustomPlotF::pintaLineaAzimut(QCustomPlot *customPlot)
{
    double x;
    double y;
    for (int i = 0; i < 359; i+=30)
    {
        x = 510*sin(i*RADIAN);//lineas
        y = 510*cos(i*RADIAN);

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
void CustomPlotF::pintaSector(QCustomPlot *customPlot, int a1, int a2, bool dentro, QPen pen, int dismax)
{
    if(curvaSector ==nullptr)
        curvaSector = new QCPCurve(customPlot->xAxis, customPlot->yAxis);

    xV.clear ();
    yV.clear ();

    int i1 = a1 % 360;
    int i2 = a2 % 360;
    if (i1 < 0) i1 += 360;
    if (i2 < 0) i2 += 360;
    if (i1 <= i2) {
        for (int i = i1; i <= i2; i++) {
            if (i < 360) {
                xV.append (puntos[i][dismax].X);
                yV.append (puntos[i][dismax].Y);
            }
        }
    } else {
        for (int i = i1; i < 360 ; i++) {
            xV.append (puntos[i][dismax].X);
            yV.append (puntos[i][dismax].Y);
        }
        for (int i = 0; i <= i2; i++) {
            xV.append (puntos[i][dismax].X);
            yV.append (puntos[i][dismax].Y);
        }
    }

    xV.prepend (0.0);
    yV.prepend (0.0);
    xV.append (0.0);
    yV.append (0.0);
    pintaCurva (customPlot,"SectorPrincipal","SectorP",pen,xV,yV,QCPScatterStyle::ssDot);
}

QCPCurve *CustomPlotF::getCurvaSector() const
{
    return curvaSector;
}

void CustomPlotF::setCurvaSector(QCPCurve *value)
{
    curvaSector = value;
}
void CustomPlotF::pintaLineaAzimut(QCustomPlot *customPlot,double lat, double lon,QList<QCPItemLine *> *listaLine,QList<QCPItemText *> *listaText)
{
    double x;
    double y;

    for (int i = 0; i < 359; i+=30)
    {
        y = dancal.Latitud_2 (puntos[i][335].Y,lat);
        x = dancal.Longitud_2 (puntos[i][335].X,lon,lat);

        auto  *line = new QCPItemLine(customPlot);
        auto *text = new QCPItemText(customPlot);
        line->setLayer ("AzimutDist");
        text->setLayer ("AzimutDist");
        listaText->append (text);
        listaLine->append (line);

        line->start->setCoords(lon,lat);
        line->end->setCoords(x,y);
        if(i!=0)
            text->setText(QString::number(abs(360-i)));
        else
            text->setText(QString::number(i));
        text->position->setCoords(x,y);
        switch (i) {
        case 0:
            text->setPositionAlignment(Qt::AlignBottom|Qt::AlignHCenter);
            break;
        case 30: case 60: case 90:
            text->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);
            break;
        case 120: case 150:
            text->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
            break;
        case 180: case 210: case 240:
            text->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
            break;
        case 270: case 300: case 330:
            text->setPositionAlignment(Qt::AlignBottom|Qt::AlignLeft);
            break;
        default:
            break;
        }
        text->setFont(QFont(font().family(), 10));
    }
}
void CustomPlotF::zoommas(QCustomPlot *customPlot)
{
    QCPRange range;
    range = customPlot->xAxis->range();
    customPlot->xAxis->scaleRange(0.85, (range.lower + range.upper) / 2);

    range = customPlot->yAxis->range();
    customPlot->yAxis->scaleRange(0.85, (range.lower + range.upper) / 2);
    customPlot->replot();
}

void CustomPlotF::zoommenos(QCustomPlot *customPlot)
{
    QCPRange range;

    range = customPlot->xAxis->range();
    customPlot->xAxis->scaleRange(1.17647058823529, (range.lower + range.upper) / 2);

    range = customPlot->yAxis->range();
    customPlot->yAxis->scaleRange(1.17647058823529, (range.lower + range.upper) / 2);
    customPlot->replot();
}

void CustomPlotF::filePrintPdf(QCustomPlot *customPlot,const QString& file)
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exportar a PDF",file,"*.pdf",nullptr,nullptr);

    if (!fileName.isEmpty())
    {
        customPlot->savePdf(fileName,1024,768);
    }

    QString fileName2 = QFileDialog::getSaveFileName(this, "Exportar a JPG",file,"*.jpeg",nullptr,nullptr);
    if (!fileName2.isEmpty())
    {
        customPlot->saveJpg(fileName2,1024,768);
    }
}
