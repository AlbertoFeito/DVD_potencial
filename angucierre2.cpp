#include "angucierre2.h"
#include "ui_angucierre2.h"

AnguCierre2::AnguCierre2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnguCierre2)
{
    ui->setupUi(this);
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );
    ui->widget->setMinimumSize(600,600);
    ui->widget->plotLayout()->insertRow(0);

    plottitle = new QCPTextElement(ui->widget);
    ui->widget->plotLayout()->addElement(0, 0,plottitle );

    connect(ui->widget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
}

AnguCierre2::~AnguCierre2()
{
    delete ui;
}

void AnguCierre2::pintar()
{   
    plottitle->setText("Ángulo de cierre Circular. Posición: "+ calculos->label);
    ui->label->setText("Ángulo de cierre Circular. Posición: "+ calculos->fichero);

    CustomPlotF customplotf;
    customplotf.initGraph(ui->widget,-600,600,-600,600,false,false);

    ui->textEdit->clear();
    ui->textEdit->setAlignment(Qt::AlignCenter);
    ui->textEdit->append("Ángulos de cierre\n Posición: "+calculos->label+"\n");
    titulo = "Ángulos de cierre\n Posición: "+calculos->label+"\n";
    ui->textEdit->setAlignment(Qt::AlignLeft);

    bool aux2 = true;
    int t = -60;

    QPen pen;

    for (int i = 40; i <= 440; i+=40)
    {
        if(i==80)
            pen.setColor(QColor(0,255,0));
        else if(i==120)
            pen.setColor(QColor(0,0,255));
        else if(i==160)
            pen.setColor(QColor(255,0,255));
        else
            pen.setColor(QColor(0,0,0));

        customplotf.pintaElipses(ui->widget,i,QString::number(t),pen.color (),aux2,pen);
        aux2 = !aux2;
        t+=30;
    }
    customplotf.pintaLineaAzimut (ui->widget);

    determDistanCiclo(calculos->grupo,calculos->arma,calculos->medio);
    determAC();

    customplotf.pintaCurva (ui->widget,"Angulos de cierre",QPen(Qt::red,2),x1,y1);
    ui->widget->replot();
}
void AnguCierre2::determAC()
{
    int min = -90;
    for (int i=0; i <= 360; i++)
    {
        double ac = 0;
        double distanciaAC =0;
        if(i < 360)
        {
            for (int j = 0; j < calculos->DatosAzimut[i].datosAngulosCierre.length(); ++j)
            {
                if(calculos->DatosAzimut[i].datosAngulosCierre.length()>=1)
                {
                    if(calculos->DatosAzimut[i].datosAngulosCierre.at(j)->distancia > distanciaArma)
                    {
                        ac = calculos->DatosAzimut[i].datosAngulosCierre.at(j-1)->angulo;
                        distanciaAC = calculos->DatosAzimut[i].datosAngulosCierre.at(j-1)->distancia;
                        break;
                    }


                        ac = calculos->DatosAzimut[i].datosAngulosCierre.last()->angulo;
                        distanciaAC = calculos->DatosAzimut[i].datosAngulosCierre.last()->distancia;

                }
                //              else
                //                {
                //                  if(calculos->DatosAzimut[i].datosAngulosCierre.last()->distancia > distanciaArma)
                //                    {
                //                      ac = -3440*calculos->AlAnt/1.5;
                //                      distanciaAC = 1.5;
                //                    }
                //                  else
                //                    {
                //                      ac = calculos->DatosAzimut[i].datosAngulosCierre.last()->angulo;
                //                      distanciaAC = calculos->DatosAzimut[i].datosAngulosCierre.last()->distancia;
                //                    }
                //                }
            }
            if(ac < min)
                ac = min;
            x1.append((120+ac*1.333)*sin(i*RADIAN));
            y1.append((120+ac*1.333)*cos(i*RADIAN));
            ui->textEdit->append("Az:"+QString::number(i)+"°"+"-->AC: "+QString::number(ac,'g',6)+"min.-->Distancia: "+QString::number(distanciaAC)+"m");

            listAngDist[i].angulocierre = QString::number(ac,'g',6);
            listAngDist[i].distanciaAngC = QString::number(distanciaAC);
        }
        else
        {/////////////////////

            for (int j = 0; j < calculos->DatosAzimut[0].datosAngulosCierre.length(); ++j)
            {
                if(calculos->DatosAzimut[0].datosAngulosCierre.length()>=1)
                {
                    if(calculos->DatosAzimut[0].datosAngulosCierre.at(j)->distancia > distanciaArma)
                    {
                        ac = calculos->DatosAzimut[0].datosAngulosCierre.at(j-1)->angulo;
//                        distanciaAC = calculos->DatosAzimut[0].datosAngulosCierre.at(j-1)->distancia;
                        break;
                    }


                        ac = calculos->DatosAzimut[0].datosAngulosCierre.last()->angulo;
//                        distanciaAC = calculos->DatosAzimut[0].datosAngulosCierre.last()->distancia;

                }
                //              else
                //                {
                //                  if(calculos->DatosAzimut[0].datosAngulosCierre.last()->distancia > distanciaArma)
                //                    {
                //                      ac = -3440*calculos->AlAnt/1.5;
                //                      distanciaAC = 1.5;
                //                    }
                //                  else
                //                    {
                //                      ac = calculos->DatosAzimut[0].datosAngulosCierre.last()->angulo;
                //                      distanciaAC = calculos->DatosAzimut[0].datosAngulosCierre.last()->distancia;
                //                    }
                //                }
            }

            if(ac < min)
                ac = min;
            x1.append((120+ac*1.333)*sin(0*RADIAN));
            y1.append((120+ac*1.333)*cos(0*RADIAN));
        }
    }
    guardarAC();
}

void AnguCierre2::guardarAC()
{
    QString fi = calculos->fichero;
    QString aux = "ZAC" + fi.remove(0,3);
    QString filenameAC = QString(QDir::currentPath() + "/AngulosCierre/" + aux + ".aci");//confecciono el nombre del fichero

    QFile fAC(filenameAC);
    QTextStream out(&fAC);
    if(!fAC.exists())
    {
        if(fAC.open(QFile::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            for (auto & i:listAngDist)/*(int i = 0;i < 360; ++i)*/
            {
                out << i.angulocierre << "," << i.distanciaAngC << endl;
            }
        }
    }
}
void AnguCierre2::determDistanCiclo(int g, int a, int m)
{
    if(g == 1)//dvd
    {
        if(a == 0) //POV
        {
            distanciaArma = 12060;
        }

        if(a == 1)//POP_C
        {
            if(m == 11)
                distanciaArma = 20070;
            if(m == 12)
                distanciaArma = 30060;
            if(m == 13)
                distanciaArma = 40050;
        }

        if(a ==  2)//AAA_
        {
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

        if(a == 3)//CAA_
        {
//            if(m == 46)
//                distanciaArma = 30060;
//            if(m == 47)
//                distanciaArma = 60030;
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
                distanciaArma = 700020;
        }
    }

    if(g ==  2) //zpf
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

    if(g == 3) //zpd
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

void AnguCierre2::filePrintPdf(bool cual)
{
    QString nom;
    QString aux;
    aux = calculos->fichero;
    aux.replace(":", "" );
    QString heathers;
    if(cual)
    {
        nom=(QDir::currentPath()+"/TablasPDF/"+aux+"_TAC");

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
                       "<td align=center colspan=3><b>" + titulo + "</b></td>" <<
                       "</tr>";
                heathers = "Azimut";
                arr << "<tr>" <<
                       "<td align=center><b>" + heathers + "</b></td>";

                heathers = "Ángulo de cierre(minutos)";
                arr << "<td align=center><b>" + heathers;

                heathers = "Distancia del ángulo de cierre(metros)";
                arr << "<td align=center><b>" + heathers + "</b></td>" <<
                       "</tr>" ;

                for (int i = 0; i < 360; ++i)
                {
                    arr << "<tr>" <<
                           "<td align=center>" +QString::number(i) + "</td>" <<
                           "<td align=center>" + listAngDist[i].angulocierre + "</td>" <<
                           "<td align=center>" + listAngDist[i].distanciaAngC + "</td>" <<
                           "</tr>" ;
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
        nom=(QDir::currentPath()+"/GraficasPDF/"+aux+"_ac2");
        CustomPlotF customplotf;
        customplotf.filePrintPdf (ui->widget,nom);
    }
}

void AnguCierre2::mouseMove(QMouseEvent *event)
{
    double AzmObj=0.0;
    double ac = 0.0;
    double distanciaAC = 0.0;
    int min = -90;

    double x=(ui->widget->xAxis->pixelToCoord(event->pos().x()));
    double y=(ui->widget->yAxis->pixelToCoord(event->pos().y()));

    double xx=fabs(x);
    double yy=fabs(y);
    double cita = (180/M_PI)*atan2(yy,xx);
    if(x>=0 && y<0)//cuadrante 2
        AzmObj=(90.0+cita);
    if(x>0.0 && y>=0.0)//cuadrante 1
        AzmObj=(90.0-cita);
    if(x<0.0 && y>=0.0)//cuadrante 4
        AzmObj=(270.0+cita);
    if(x<0.0 && y<=0.0)//cuadrante 3
        AzmObj=(270.0-cita);

    if((int)AzmObj == 360)
        AzmObj = 0;

    if(calculos->arma == 4)
    {
        //********** arma == 4 ERL **************/
        ac = calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.last()->angulo;
        distanciaAC = calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.last()->distancia;
        //******************ERL***************//
    }
    else
    {
        for (int j = 0; j < calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.length(); ++j)
        {
            if(calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.length()>1)
            {
                if(calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.at(j)->distancia > distanciaArma)
                {
                    ac = calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.at(j-1)->angulo;
                    distanciaAC = calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.at(j-1)->distancia;
                    break;
                }


                    ac = calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.last()->angulo;
                    distanciaAC = calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.last()->distancia;

            }
            else
            {
                if(calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.last()->distancia > distanciaArma)
                {

                    double anC = 3440*calculos->AlAnt/1.5;
                    double DisAngulocierre = anC/3440 *1.5 + calculos->datoterrenoMiPos.altura+calculos->AlAnt;
                    ac = DisAngulocierre;
                    distanciaAC = 1.5;
                }
                else
                {
                    ac = calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.last()->angulo;
                    distanciaAC = calculos->DatosAzimut[(int)AzmObj].datosAngulosCierre.last()->distancia;
                }
            }
        }
    }

    if(ac < min)
        ac = min;
    QString punto="Azimut: "+QString::number((int)AzmObj)+"°, AngCierre: "+QString::number(ac,'g',6)+"min. , Distancia: "+QString::number(distanciaAC)+" m";
    ui->widget->setToolTip(punto);
}

void AnguCierre2::on_pushButton_4_clicked()
{
    filePrintPdf(false);
}

void AnguCierre2::on_pushButton_3_clicked()
{
    filePrintPdf(true);
}

void AnguCierre2::on_pB_Zoommas_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommas(ui->widget);
}

void AnguCierre2::on_pB_Zoommen_clicked()
{
    CustomPlotF customplotf;
    customplotf.zoommenos(ui->widget);
}
