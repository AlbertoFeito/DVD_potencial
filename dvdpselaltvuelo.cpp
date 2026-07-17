#include "dvdpselaltvuelo.h"
#include "ui_dvdpselaltvuelo.h"
#include <QDebug>
#include <QApplication>
dvdpSelAltVuelo::dvdpSelAltVuelo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::dvdpSelAltVuelo)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    alcance = false;
    dm00 = false;
    dm25 = false;
    dm50 = false;
    d100 = false;
    d200 = false;
    d300 = false;
    d400 = false;
    d500 = false;
    d1000 = false;
    d2000 = false;
    d4000 = false;
    d6000 = false;
    d10000 = false;
    d17000 = false;
    d30000 = false;
    dObjetLocale = false;
    alvue = 0;
    todoCalculado = false;


    connect(&dista,SIGNAL(progreso(int,QString)),this,SLOT(progreso(int,QString)));

    connect(&dista,SIGNAL(findehilo(bool)),this,SLOT(findhilo(bool)));

    barra = new QStatusBar(this);
    barra->showMessage("Señale la Altura de Vuelo");
    ui->layautBarra->addWidget(barra);

    EscalaColor[14] = QPen(QColor(0,250,0,150),2);
    EscalaColor[13] = QPen(QColor(10,250,10,150),2);
    EscalaColor[12] = QPen(QColor(20,250,20,150),3);
    EscalaColor[11] = QPen(QColor(30,250,30,150),4);
    EscalaColor[10] = QPen(QColor(40,250,40,150),5);
    EscalaColor[9] = QPen(QColor(50,250,50,150),3);
    EscalaColor[8] = QPen(QColor(60,250,60,150),3);
    EscalaColor[7] = QPen(QColor(70,250,70,150),3);
    EscalaColor[6] = QPen(QColor(80,250,80,150),3);
    EscalaColor[5] = QPen(QColor(90,250,90,150),3);
    EscalaColor[4] = QPen(QColor(100,250,100,150),3);
    EscalaColor[3] = QPen(QColor(110,250,110,150),3);
    EscalaColor[2] = QPen(QColor(120,250,120,150),3);
    EscalaColor[1] = QPen(QColor(130,250,130,150),3);
    EscalaColor[0] = QPen(QColor(250,0,0,150),2);
    confile = 0;

    velocidad = ui->cB_Velocidad->currentText ().toInt ();
    tiempo = ui->cB_Tiempo->currentText ().toInt ();
    //  velocidad = 300;
    //  tiempo = 25;
    parametro = 0;
    probabilidad = 0;
    ui->pB_TProbabilidades->setEnabled (false);

    // === NUEVO: Configurar combos Sigma y Banda ===
    configurarCombosSigmaYBanda();

    // Conectar señales de los combos
    connect(ui->cB_Sigma, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &dvdpSelAltVuelo::on_cB_Sigma_currentIndexChanged);
    connect(ui->cB_Banda, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &dvdpSelAltVuelo::on_cB_Banda_currentIndexChanged);
}

dvdpSelAltVuelo::~dvdpSelAltVuelo()
{
    delete ui;
}

void dvdpSelAltVuelo::closeEvent(QCloseEvent *event)
{
    Q_UNUSED (event);
    for (auto & var : lisCustomPlot)
    {
        var->close();
        delete var;
    }
    lisCustomPlot.clear();
    for (auto & lisverAltura : lisverAlturas)
    {
        lisverAltura->close();
        delete lisverAltura;
    }
    lisverAlturas.clear();

    if (dista.isRunning())
        dista.stop();
    dista.quit();
    dista.wait();

    emit cerro();

}

void dvdpSelAltVuelo::on_pB_Aceptar_clicked()
{
    ui->pB_Aceptar->setEnabled(false);
    velocidad = ui->cB_Velocidad->currentText ().toInt ();
    tiempo = ui->cB_Tiempo->currentText ().toInt ();
    calcular();
}

void dvdpSelAltVuelo::calcular()
{
    dista.fichero = calculos->fichero;
    dista.nombreAux = calculos->label;
    dista.calculos = calculos;
    // === NUEVO: Configurar Sigma ===
    double valorSigma = ui->cB_Sigma->currentData().toDouble();
    dista.setSigmaSeleccionado(valorSigma);

    // === NUEVO: Configurar Tipo de Banda (solo para medio 40) ===
    if (ui->cB_Banda->isVisible()) {
        QString tipoBanda = ui->cB_Banda->currentData().toString();
        dista.setTipoOnda(tipoBanda);
    } else {
        dista.setTipoOnda("");  // Vacío para otros medios
    }

    // Guardar texto del sigma para uso interno (si lo necesitas)
    dista.strSigma = ui->cB_Sigma->currentText();

    todoCalculado = false;

    ui->pBar_Progreso->setMaximum(360);
    ui->pBar_Progreso->setVisible(true);
    dista.velocidad = velocidad;
    dista.tiempo = tiempo;

    dista.alcance = alcance;

    dista.strSigma = ui->cB_Sigma->currentText();

    if( (!dObjetLocale)&&(ui->rB_ObjetosLocales))
    {
        this->barra->showMessage("Mostrando gráfica para 0m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 0;
    }
    else if(ui->rB_ObjetosLocales->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled(false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 0;
        dObjetLocale = false;
        dista.ObjLoc = true;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.determDistanCiclo(/*grupo,arma,medio*/);
        //        dista.distancia = 100000;
        //        dista.distanciamax = 100000;
        dista.angulositio = angulositio;

        dista.start();
    }


    if( (!dm00)&&(ui->rB_0m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 0m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 0;
    }
    else if(ui->rB_0m->isChecked())
    {
        ui->rB_25m->setEnabled(false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 0;
        dm00 = false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.determDistanCiclo(/*grupo,arma,medio*/);
        dista.angulositio = angulositio;
        dista.start();
    }

    if( (!dm25)&&(ui->rB_25m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 25m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 25;
    }
    else if(ui->rB_25m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_50m->setEnabled (false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 25;
        dm25 = false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }

    if( (!dm50)&&(ui->rB_50m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 50m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 50;
    }
    else if(ui->rB_50m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 50;
        dm50 = false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }

    if( (!d100)&&(ui->rB_100m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 100m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 100;
    }
    else if(ui->rB_100m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue=100;
        d100=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;

        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }
    /***********************/
    if( (!d200)&&(ui->rB_200m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 200m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 200;
    }
    else if(ui->rB_200m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue=200;
        d200=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }

    if( (!d300)&&(ui->rB_300m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 300m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 300;
    }
    else if(ui->rB_300m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue=300;
        d300=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }
    if( (!d400)&&(ui->rB_400m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 400m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 400;
    }
    else if(ui->rB_400m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue=400;
        d400=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }
    /***********************/
    if ((!d500)&&(ui->rB_500m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 500m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 500;
    }
    else if(ui->rB_500m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 500;
        d500=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }
    if((!d1000)&&(ui->rB_1000m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 1000m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 1000;
    }
    else if(ui->rB_1000m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 1000;
        d1000=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }

    if((!d2000)&&(ui->rB_2000m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 2000m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 2000;
    }
    else if(ui->rB_2000m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 2000;
        d2000=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }
    if((!d4000)&&(ui->rB_4000m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 4000m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 4000;
    }
    else if(ui->rB_4000m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 4000;
        d4000=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }

    if((!d6000)&&(ui->rB_6000m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 6000m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 6000;
    }
    else if(ui->rB_6000m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 6000;
        d6000=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }

    if((!d10000)&&(ui->rB_10000m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 10000m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 10000;
    }
    else if(ui->rB_10000m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 10000;
        d10000=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }
    if((!d17000)&&(ui->rB_17000m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 17000m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 17000;
    }
    else if(ui->rB_17000m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_30000m->setEnabled(false);
        alvue = 17000;
        d17000=false;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }

    if((!d30000)&&(ui->rB_30000m->isChecked()))
    {
        this->barra->showMessage("Mostrando gráfica para 30000m. Esta operación puede tardar unos minutos");
        todoCalculado = true;
        alvue = 30000;
    }
    else if(ui->rB_30000m->isChecked())
    {
        ui->rB_0m->setEnabled(false);
        ui->rB_25m->setEnabled (false);
        ui->rB_50m->setEnabled(false);
        ui->rB_100m->setEnabled(false);
        ui->rB_200m->setEnabled(false);
        ui->rB_300m->setEnabled(false);
        ui->rB_400m->setEnabled(false);
        ui->rB_500m->setEnabled(false);
        ui->rB_1000m->setEnabled(false);
        ui->rB_2000m->setEnabled(false);
        ui->rB_4000m->setEnabled(false);
        ui->rB_6000m->setEnabled(false);
        ui->rB_10000m->setEnabled(false);
        ui->rB_17000m->setEnabled(false);
        alvue = 30000;
        dista.ObjLoc = false;
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();
    }
    if(todoCalculado)
    {
        dista.Vuelo = alvue;
        dista.calculos = calculos;
        dista.angulositio = angulositio;
        dista.determDistanCiclo(/*grupo,arma,medio*/);

        dista.start();

        ui->pB_Aceptar->setEnabled(true);
        ui->pB_Cancelar->setEnabled(true);
        ui->pBar_Progreso->reset();
        ui->pBar_Progreso->setVisible(false);
        this->barra->showMessage("Señale la Altura de Objetivo");
    }
}

void dvdpSelAltVuelo::parar()
{
    if (dista.isRunning())
        dista.stop();
    dista.quit();
    dista.wait();
}

void dvdpSelAltVuelo::radioButons(int g, int m)
{
    medio = m;
    if(g == 1)
    {
        if(m == 0)//pov
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);

            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if (m == 11 )//pop camara 20
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);
            ui->rB_17000m->setEnabled(true);

            ui->rB_30000m->setEnabled(false);
        }
        if (m == 12 || m == 13)//pop camara 30 y 40
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);
            ui->rB_17000m->setEnabled(true);
            //esto es otra modificaci'on
            ui->rB_30000m->setEnabled( m == 13 );
        }
        if(m == 21 || m == 22 || m == 23 || m == 24 || m == 25 ||  m == 26 || m == 27 || m == 31 || m == 32 || m == 33 || m == 34 || m == 35 )//zvd
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            //modicaci'on p q el 21 no llega a 2000m
            ui->rB_2000m->setEnabled(m != 21);
            //modifico p q no sale la zona de 4000 para 25
            ui->rB_4000m->setEnabled( m == 25 ||  m == 26 || m == 27);
            ui->rB_6000m->setEnabled( m == 26 || m == 27);
            ui->rB_10000m->setEnabled( m == 27);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if( m == 36)//zvd
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);

            ui->rB_6000m->setEnabled(false);
            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }

        if(m == 37)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);

            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if(m == 38)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);
            ui->rB_17000m->setEnabled(true);
            ui->rB_30000m->setEnabled(false);
        }
        if(m == 39)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);
            ui->rB_17000m->setEnabled(true);
            ui->rB_30000m->setEnabled(true);
        }
        if( m == 40 || m == 50)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);

            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);
            ui->rB_17000m->setEnabled(true);
            ui->rB_30000m->setEnabled(true);
        }
        if( m == 41 )
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);

            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if( m == 42)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);

            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if( m == 43)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);

            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if( m == 44)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);

            ui->rB_4000m->setEnabled(false);
            ui->rB_6000m->setEnabled(false);
            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }

        if( m == 45)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);

            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);
            ui->rB_17000m->setEnabled(true);
            ui->rB_30000m->setEnabled(true);
        }

        if( m == 46)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);

            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);
            ui->rB_17000m->setEnabled(true);
            ui->rB_30000m->setEnabled(false);
        }

        if( m == 47)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);

            ui->rB_4000m->setEnabled(false);
            ui->rB_6000m->setEnabled(false);
            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }


        if(m == 48)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);

            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if( m == 49 )
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (true);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);

            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
    }
    if(g == 2)//zpf
    {
        if(m == 21)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);

            ui->rB_2000m->setEnabled(false);
            ui->rB_4000m->setEnabled(false);
            ui->rB_6000m->setEnabled(false);
            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if(m == 22 || m == 23 || m == 24 || m == 25 || m == 44)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            //modifico p q no sale habilitada 4000 con 25
            ui->rB_4000m->setEnabled(m == 25);
            ui->rB_6000m->setEnabled(false);
            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if(m == 26 || m == 43)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);

            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if(m == 27 || m == 42)
        {
            ui->rB_0m->setEnabled(true);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);


            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
    }
    if(g == 3)
    {
        if(m == 31 || m == 32 || m == 33 || m == 34 || m == 35)
        {
            ui->rB_0m->setEnabled(false);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled(false);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);

            ui->rB_4000m->setEnabled(false);
            ui->rB_6000m->setEnabled(false);
            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }

        if( m == 36)//zpd
        {
            ui->rB_0m->setEnabled(false);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);

            ui->rB_6000m->setEnabled(false);
            ui->rB_10000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }

        if(m == 37)
        {
            ui->rB_0m->setEnabled(false);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled(false);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);

            ui->rB_6000m->setEnabled(false);
            ui->rB_17000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if(m == 38)
        {
            ui->rB_0m->setEnabled(false);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled(true);
            ui->rB_100m->setChecked(true);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_10000m->setEnabled(true);
            ui->rB_17000m->setEnabled(true);

            ui->rB_6000m->setEnabled(false);
            ui->rB_30000m->setEnabled(false);
        }
        if(m == 39)
        {
            ui->rB_0m->setEnabled(false);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled(false);
            ui->rB_100m->setEnabled(true);
            ui->rB_200m->setEnabled(true);
            ui->rB_300m->setEnabled(true);
            ui->rB_400m->setEnabled(true);
            ui->rB_500m->setEnabled(true);
            ui->rB_500m->setChecked(true);
            ui->rB_1000m->setEnabled(true);
            ui->rB_2000m->setEnabled(true);
            ui->rB_4000m->setEnabled(true);
            ui->rB_6000m->setEnabled(false);
            ui->rB_10000m->setEnabled(true);
            ui->rB_17000m->setEnabled(true);
            ui->rB_30000m->setEnabled(true);
        }
    }
}

void dvdpSelAltVuelo::progreso(int c, const QString& mensaje)
{
    // qDebug()<<c <<"c";
    ui->pBar_Progreso->setValue(c);
    this->barra->showMessage(mensaje);
}

void dvdpSelAltVuelo::findhilo(bool final)
{
    if(final)
    {
        this->barra->showMessage("Mostrando gráfica para "+QString::number(alvue)+"m. Esta operación puede tardar unos minutos");

        customp = new DistanciaVD(calculos->dismax);
        customp->prob =  "_P:"+ui->cB_Probabildad->currentText ();
        customp->p =  ui->cB_Probabildad->currentIndex ();
        customp->arma = calculos->arma;
        customp->initGraph();
        customp->alvue = alvue;
        customp->label = calculos->label;
        customp->fichero = dista.fichero;
        customp->label = dista.nombreAux;
        customp->medio = calculos->medio;
        customp->grupo = calculos->grupo;
        customp->alcance = alcance;
        for (int a = 0; a < 360; a++)
        {
            customp->disInternas[a].append(dista.disInternas[a]);
            customp->disVDirecta[a].append(dista.disVDirecta[a]);
            if(calculos->medio == 41 || calculos->medio == 48 || ((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36 || calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31|| calculos->medio == 27 || calculos->medio == 26 || calculos->medio == 25 || calculos->medio == 24 || calculos->medio == 23 || calculos->medio == 22 || calculos->medio == 21 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)  && calculos->grupo == 1))
            {
                if(dista.ZonaMuertaMuis[a].isEmpty())
                    dista.ZonaMuertaMuis[a].append(0);

                customp->ZonaMuertaMuis[a].append(dista.ZonaMuertaMuis[a]);
                if(dista.ZonaMuertaMuis2[a].isEmpty())
                {
                    dista.ZonaMuertaMuis2[a].append(0);
                }
                customp->ZonaMuertaMuis2[a].append(dista.ZonaMuertaMuis2[a]);
            }
        }
        customp->angulositio = angulositio;

        if(calculos->medio == 41 || calculos->medio == 48 || ((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36|| calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 27 || calculos->medio == 26|| calculos->medio == 25 || calculos->medio == 24 || calculos->medio == 23 || calculos->medio == 22 || calculos->medio == 21 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44) && calculos->grupo == 1))
            customp->pintarZonaMuertaMuis();
        if(calculos->grupo == 1)
        {
            if((calculos->medio == 39  && alvue>=100) || ((calculos->medio == 38  || calculos->medio == 37 || calculos->medio == 36 || calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 27 || calculos->medio == 26|| calculos->medio == 25 || calculos->medio == 24 || calculos->medio == 23 || calculos->medio == 22 || calculos->medio == 21 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44) && alvue>=25))
            {
                customp->pintarZonaMuerta(dista.zonamuerta);
                customp->T = dista.tiempo;
                customp->V = dista.velocidad;
                customp->pintarZonaMuertaCCAA ();
            }
        }


        if( ((calculos->arma == 2 || calculos->arma == 3) && !calculos->fichero.contains ("ZVD")) || calculos->medio == 40 || calculos->medio == 50 )
            customp->pintarZonaMuerta(dista.zonamuerta);

        if(dista.ObjLoc)
            customp->pintarObjetoLocal(0);
        else
            customp->pintarDistancias(0);
        this->barra->showMessage("pintarDistancias");

        customp->llenartabla(true,calculos->medio);

        //      customp->disVDirecta[a].append ();
        customp->configInterface (calculos->grupo,calculos->medio,calculos->arma);
        this->barra->showMessage("configInterface");
        customp->showMaximized();
        this->barra->showMessage("mostrar dvd");

        lisCustomPlot.append(customp);

        radioButons(calculos->grupo,calculos->medio);
        ui->pB_Aceptar->setEnabled(true);
        ui->pB_Cancelar->setEnabled(true);
        ui->pBar_Progreso->reset();
        ui->pBar_Progreso->setVisible(false);
        //        if(calculos->medio == 49 || calculos->medio == 41)
        //        this->barra->showMessage("Señale la Altura de Objetivo");
    }
    else
    {
        ui->pB_Aceptar->setEnabled(true);
        ui->pB_Cancelar->setEnabled(true);
        ui->pBar_Progreso->reset();
        ui->pBar_Progreso->setVisible(false);
        this->barra->showMessage("Señale la Altura de Objetivo");
    }
    ui->chB_Alcance->setChecked (false);
}

void dvdpSelAltVuelo::on_pB_Cerrar_clicked()
{
    this->close();
}

void dvdpSelAltVuelo::on_pB_Cancelar_clicked()
{
    this->close();
}

void dvdpSelAltVuelo::on_pB_BAltura_clicked()
{
    velocidad = ui->cB_Velocidad->currentText ().toInt ();
    tiempo = ui->cB_Tiempo->currentText ().toInt ();
    //  velocidad = 300;
    //  tiempo = 25;
    parametro = 0;
    bool existe = false;
    auto *verbajas = new VerAlturas();
    lisverAlturas.append (verbajas);
    verbajas->plottitle->setText ("Bajas Alturas Posición: " + calculos->label+"_P:"+ui->cB_Probabildad->currentText ());
    verbajas->medio = calculos->medio;
    verbajas->grupo = calculos->grupo;
    verbajas->velocidad = velocidad;
    verbajas->tiempo = tiempo;
    verbajas->fichero = calculos->fichero;
    if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36  || calculos->medio == 35  || calculos->medio == 34  || calculos->medio == 33  || calculos->medio == 32  || calculos->medio == 31|| calculos->medio == 27 || calculos->medio == 26 || calculos->medio == 25  || calculos->medio == 24  || calculos->medio == 23  || calculos->medio == 22  || calculos->medio == 21 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)  && calculos->grupo == 1)
    {
        verbajas->fichero.append ("_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));

    }
    // === NUEVO: Añadir sufijo Sigma si corresponde ===
    double sigma = ui->cB_Sigma->currentData().toDouble();
    if (sigma != 1.0 && ui->cB_Sigma->isVisible()) {
        if (sigma == 5.0)       verbajas->fichero.append("_S5");
        else if (sigma == 0.5)  verbajas->fichero.append("_S05");
        else if (sigma == 0.1)  verbajas->fichero.append("_S01");

        // Añadir sufijo Banda si es medio 40
        if (calculos->medio == 40 && ui->cB_Banda->isVisible()) {
            verbajas->fichero.append("_" + ui->cB_Banda->currentData().toString());
        }
    }


    verbajas->probOno = true;
    verbajas->configInterface();
    //    if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36  || calculos->medio == 35  || calculos->medio == 34  || calculos->medio == 33  || calculos->medio == 32  || calculos->medio == 31 || calculos->medio == 27 || calculos->medio == 26|| calculos->medio == 25  || calculos->medio == 24  || calculos->medio == 23  || calculos->medio == 22  || calculos->medio == 21)  && calculos->grupo == 1)
    //    {
    //        if (calculos->medio == 21 || calculos->medio == 22 ||calculos->medio == 26 ||calculos->medio == 27 || calculos->medio == 32 || calculos->medio == 33 || calculos->medio == 34 || calculos->medio == 42 || calculos->medio == 43)
    //        {
    //            verbajas->fichero.insert (24,"_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    //            //            nombreAux.insert (24,"_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    //            //        calculos->label.insert (24,"_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    //        }
    //        else
    //        {
    //            verbajas-> fichero.insert (23,"_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    //            //            nombreAux.insert (23,"_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    //            //        calculos->label.insert (23,"_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    //        }
    //    }
    //    if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36 || calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 25 || calculos->medio == 24 || calculos->medio == 23 || calculos->medio == 22 || calculos->medio == 21)  && calculos->grupo == 1)
    //        verbajas->fichero.insert (23,"_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    verbajas->cual = "BAlt";
    verbajas->cont = 0;
    verbajas->probabilidad = probabilidad;
    QPen pen;
    if(calculos->grupo != 2 && calculos->grupo != 3)
    {
        pen = EscalaColor[14];
    }
    else
        pen = EscalaColor[0];

    if(verbajas->leer (".m00"))
    {
        verbajas->pintarDistancias(pen,0,0);
        existe = true;
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 0m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(verbajas->leer (".m25"))
    {
        existe = true;
        verbajas->pintarDistancias(pen, 25,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 25m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(verbajas->leer (".m50"))
    {
        existe = true;
        verbajas->pintarDistancias(pen, 50,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 50m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(verbajas->leer (".001"))
    {
        existe = true;
        verbajas->pintarDistancias(pen, 100,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 100m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(verbajas->leer (".002"))
    {
        existe = true;
        verbajas->pintarDistancias(pen, 200,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 200m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(verbajas->leer (".003"))
    {
        existe = true;
        verbajas->pintarDistancias(pen, 300,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 300m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(verbajas->leer (".004"))
    {
        existe = true;
        verbajas->pintarDistancias(pen, 400,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 400m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }


    if(existe)
    {
        verbajas->setWindowTitle ("Bajas Alturas Posición: " + calculos->fichero );
        verbajas->show ();
    }
    else
    {
        QMessageBox::information (nullptr,"Información","Debe calcular la visibilidad directa\npara las diferentes alturas del objetivo primero.",QMessageBox::Ok);
    }
}

void dvdpSelAltVuelo::on_pB_MAltura_clicked()
{
    velocidad = ui->cB_Velocidad->currentText ().toInt ();
    tiempo = ui->cB_Tiempo->currentText ().toInt ();
    //  velocidad = 300;
    //  tiempo = 25;
    parametro = 0;

    bool existe = false;
    auto *vermedianas = new VerAlturas();
    lisverAlturas.append (vermedianas);
    vermedianas->plottitle->setText ("Medianas Alturas Posición: " + calculos->label+"_P:"+ui->cB_Probabildad->currentText ());
    vermedianas->medio = calculos->medio;
    vermedianas->grupo = calculos->grupo;
    vermedianas->velocidad = velocidad;
    vermedianas->tiempo = tiempo;
    vermedianas->fichero = calculos->fichero;
    if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36  || calculos->medio == 35  || calculos->medio == 34  || calculos->medio == 33  || calculos->medio == 32  || calculos->medio == 31|| calculos->medio == 27 || calculos->medio == 26 || calculos->medio == 25  || calculos->medio == 24  || calculos->medio == 23  || calculos->medio == 22  || calculos->medio == 21 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)  && calculos->grupo == 1)
    {
        vermedianas->fichero.append ("_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    }

    // === NUEVO: Añadir sufijo Sigma si corresponde ===
    double sigma = ui->cB_Sigma->currentData().toDouble();
    if (sigma != 1.0 && ui->cB_Sigma->isVisible()) {
        if (sigma == 5.0)       vermedianas->fichero.append("_S5");
        else if (sigma == 0.5)  vermedianas->fichero.append("_S05");
        else if (sigma == 0.1)  vermedianas->fichero.append("_S01");

        if (calculos->medio == 40 && ui->cB_Banda->isVisible()) {
            vermedianas->fichero.append("_" + ui->cB_Banda->currentData().toString());
        }
    }

    vermedianas->probOno = true;
    vermedianas->configInterface();
    vermedianas->cual = "MAlt";
    vermedianas->cont = 0;
    vermedianas->probabilidad = probabilidad;
    QPen pen;
    if(calculos->grupo != 2 && calculos->grupo != 3)
    {
        pen = EscalaColor[14];
    }
    else
        pen = EscalaColor[0];
    if(vermedianas->leer (".005"))
    {
        existe = true;
        vermedianas->pintarDistancias(pen, 500,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 500m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(vermedianas->leer (".010"))
    {
        existe = true;
        vermedianas->pintarDistancias(pen,1000,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 1000m de altura no existe o el medio\n seleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(vermedianas->leer (".020"))
    {
        existe = true;
        vermedianas->pintarDistancias(pen, 2000,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 2000m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(vermedianas->leer (".040"))
    {
        existe = true;
        vermedianas->pintarDistancias(pen, 4000,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 4000m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    //  if(vermedianas->leer (".060"))
    //    vermedianas->pintarDistancias(EscalaColor[8], QBrush(QColor(0,100,250,100)));

    if(existe)
    {
        vermedianas->setWindowTitle ("Medianas Alturas Posición: " + calculos->fichero );
        //      vermedianas->llenartabla (true,medio);
        vermedianas->show ();
    }
    else
    {
        QMessageBox::information (nullptr,"Información","Debe calcular la visibilidad directa\npara las diferentes alturas del objetivo primero.",QMessageBox::Ok);
    }

}

void dvdpSelAltVuelo::on_pB_GAltura_clicked()
{
    velocidad = ui->cB_Velocidad->currentText ().toInt ();
    tiempo = ui->cB_Tiempo->currentText ().toInt ();
    //  velocidad = 300;
    //  tiempo = 25;
    parametro = 0;
    bool existe = false;
    auto *veraltas = new VerAlturas();
    lisverAlturas.append (veraltas);
    veraltas->plottitle->setText ("Grandes Alturas Posición: " + calculos->label+"_P:"+ui->cB_Probabildad->currentText ());
    veraltas->medio = calculos->medio;
    veraltas->grupo = calculos->grupo;
    veraltas->velocidad = velocidad;
    veraltas->tiempo = tiempo;
    veraltas->fichero = calculos->fichero;
    if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36  || calculos->medio == 35  || calculos->medio == 34  || calculos->medio == 33  || calculos->medio == 32  || calculos->medio == 31|| calculos->medio == 27 || calculos->medio == 26 || calculos->medio == 25  || calculos->medio == 24  || calculos->medio == 23  || calculos->medio == 22  || calculos->medio == 21 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)  && calculos->grupo == 1)
    {
        veraltas->fichero.append ("_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    }

    // === NUEVO: Añadir sufijo Sigma si corresponde ===
    double sigma = ui->cB_Sigma->currentData().toDouble();
    if (sigma != 1.0 && ui->cB_Sigma->isVisible()) {
        if (sigma == 5.0)       veraltas->fichero.append("_S5");
        else if (sigma == 0.5)  veraltas->fichero.append("_S05");
        else if (sigma == 0.1)  veraltas->fichero.append("_S01");

        if (calculos->medio == 40 && ui->cB_Banda->isVisible()) {
            veraltas->fichero.append("_" + ui->cB_Banda->currentData().toString());
        }
    }


    veraltas->probOno = true;
    veraltas->configInterface();
    veraltas->cual = "GAlt";
    veraltas->probabilidad = probabilidad;
    QPen pen;
    veraltas->cont = 0;
    if(calculos->grupo != 2 && calculos->grupo != 3)
        pen = EscalaColor[14];
    else
        pen = EscalaColor[0];

    if(veraltas->leer (".060"))
    {
        existe = true;
        veraltas->pintarDistancias(pen, 6000,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 6000m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(veraltas->leer (".100"))
    {
        existe = true;
        veraltas->pintarDistancias(pen, 10000,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 10000m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(veraltas->leer (".170"))
    {
        existe = true;
        veraltas->pintarDistancias(pen, 17000,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 17000m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }
    if(veraltas->leer (".300"))
    {
        existe = true;
        veraltas->pintarDistancias(pen, 30000,0);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de 30000m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }

    if(existe)
    {
        veraltas->setWindowTitle ("Grandes Alturas Posición: " + calculos->fichero );
        //      veraltas->llenartabla (true,medio);
        veraltas->show ();
    }
    else
    {
        QMessageBox::information (nullptr,"Información","Debe calcular la visibilidad directa\npara las diferentes alturas del objetivo primero.",QMessageBox::Ok);
    }
}

void dvdpSelAltVuelo::guardar()
{
    QFile f(QDir::currentPath() + "/Zonas/"+dista.fichero);
    qDebug()<<"guardar selAlvue"<<dista.fichero;
    QTextStream out(&f);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        qFatal("No puedo abrir el fichero para escritura.");

    double coox = 0;
    double cooy = 0;
    double disaux = 0;
    //  if(calculos->medio != 50)
    //    {
    //      disaux = 320;
    //    }
    //  else
    //    disaux = 600;
    for (int a = 0; a < 360; a++)
    {
        if(customp->disVDirecta[a].at (0)/1000 <= 320)
        {
            cooy = calculos->DatosAzimut[a].datosterreno.at((int)customp->disVDirecta[a].at (0)/90)->LatOR/*Ymap*/;
            coox = calculos->DatosAzimut[a].datosterreno.at((int)customp->disVDirecta[a].at (0)/90)->LonOR*-1/*Xmap*/;
        }
        else
        {
            DanCalc danca;
            double Ykm = customp->disVDirecta[a].at(0)*cos(a*RADIAN)/1000;
            double Xkm = customp->disVDirecta[a].at(0)*sin(a*RADIAN)/1000;

            double LatOR = danca.Latitud_2(Ykm,calculos->datoterrenoMiPos.LatOR);//determinar la latitud del objetivo con respecto al radar en grados decimales
            double LonOR = danca.Longitud_2(Xkm,calculos->datoterrenoMiPos.LonOR,LatOR);//determinar la longitud del objetivo con respecto al radar en grados decimales

            cooy = /*calculos->DatosAzimut[a].datosterreno.last()->*/LatOR/*Ymap*/;
            coox = /*calculos->DatosAzimut[a].datosterreno.last()->*/LonOR*-1/*Xmap*/;
        }
        out << coox << "," << cooy << ",*" << customp->disVDirecta[a].at(0);

        for (double i : customp->disInternas[a])
        {
            out << "," << i;
        }

        out << "*";
        if(customp->ZonaMuertaMuis[a].length()>=1)
        {
            out<< customp->ZonaMuertaMuis[a].at(0);
            if(customp->ZonaMuertaMuis[a].length()>1)
            {
                for (int i = 1; i < customp->ZonaMuertaMuis[a].length(); ++i)
                {
                    out << "," << customp->ZonaMuertaMuis[a].at(i);
                }
            }
        }
        else
        {
            customp->ZonaMuertaMuis[a].append(0);
            out << 0;
        }
        out << "*";

        if(customp->ZonaMuertaMuis2[a].length()>=1)
        {
            out <<customp->ZonaMuertaMuis2[a].at(0);
            if(customp->ZonaMuertaMuis2[a].length()>1)
            {
                for (int i = 1; i < customp->ZonaMuertaMuis2[a].length(); ++i)
                {
                    out << "," << customp->ZonaMuertaMuis2[a].at(i);
                }
            }
        }
        else
        {
            customp->ZonaMuertaMuis2[a].append(0);
            out << 0;
        }
        ///////probabilidad
        if(calculos->fichero.contains ("ZVD") && (calculos->medio >= 21))
        {
            out << "*";
            disaux = customp->disVDirecta[a].at (0);
            if(disaux > dista.disMax.at (1))//06
                out << dista.disMax.at (1)<<"*";
            else
            {
                out << disaux<<"*";
                dista.disMax.replace (1,disaux);
            }

            if(disaux > dista.disMax.at (2))//07
                out << dista.disMax.at (2)<<"*";
            else
            {
                out << disaux<<"*";
                dista.disMax.replace (2,disaux);
            }

            if(disaux > dista.disMax.at (3))//08
                out << dista.disMax.at (3)<<"*";
            else
            {
                out << disaux<<"*";
                dista.disMax.replace (3,disaux);
            }

            if(disaux > dista.disMax.at (4))//09
                out << dista.disMax.at (4)<<"*";
            else
            {
                out << disaux<<"*";
                dista.disMax.replace (4,disaux);
            }

            ////VOLGA/////
            if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36 || calculos->medio == 35 || calculos->medio == 34 || calculos->medio == 33 || calculos->medio == 32 || calculos->medio == 31 || calculos->medio == 25 || calculos->medio == 24 || calculos->medio == 23 || calculos->medio == 22 || calculos->medio == 21)  && calculos->grupo == 1)
            {
                double disLanza;
                disLanza = customp->disVDirecta[a].at (0)- tiempo * velocidad;//0.5
                out <<disLanza<<"*";

                disLanza = dista.disMax.at (1) - tiempo * velocidad;//0.6
                out <<disLanza<<"*";

                disLanza = dista.disMax.at (2) - tiempo * velocidad; //0.7
                out <<disLanza<<"*";

                disLanza = dista.disMax.at (3) - tiempo * velocidad;//0.8
                out <<disLanza<<"*";

                disLanza = dista.disMax.at (4) - tiempo * velocidad;//0.9
                out <<disLanza<<"*";

                //              customp->disVDirecta[a].append (dista.disMax.at (probabilidad) - tiempo * velocidad);
            }
        }
        out << endl;
    }
    f.close();//cierro el fichero
}

void dvdpSelAltVuelo::configInterface()
{
    ui->chB_Alcance->setEnabled (false);
    // === NUEVO: Actualizar visibilidad de Sigma y Banda según medio ===
    actualizarVisibilidadSigma();

    if(calculos->fichero.contains ("ZVD") && (calculos->medio >= 21))
    {
        // se increment'o a 37 p q 36 no cumple la condici'on
        if(calculos->medio >= 37 )
        {
            setWindowTitle ("Selecione altura del objetivo y probabilidad de detección");
            ui->groupBox->setTitle ("Selecione altura del objetivo y probabilidad de detección");
            ui->cB_Probabildad->setEnabled (true);
            ui->pB_TProbabilidades->setEnabled (true);
            // se modifica p q 44 debe estar deshabilitado
            if((calculos->medio == 38) || (calculos->medio == 39))
                ui->chB_Alcance->setEnabled (true);
            if(calculos->medio == 44 )    ui->pB_GAltura->setEnabled (false );
        }
        else
        {
            setWindowTitle ("Selecione altura del objetivo");
            ui->groupBox->setTitle ("Selecione altura del objetivo");
            ui->cB_Probabildad->setEnabled (false);
            dista.probabilidad = 0;
            ui->pB_TProbabilidades->setEnabled (false);
            //se modifica p q el 26 y 27 tiene que estar habilitados
            ui->pB_GAltura->setEnabled ( calculos->medio == 26 || calculos->medio == 27);
        }

        if((calculos->medio < 21 || calculos->medio > 39) && (calculos->medio < 42 || calculos->medio > 44))
        {
            ui->cB_Tiempo->setEnabled (false);
            ui->cB_Velocidad->setEnabled (false);
            dista.probabilidad = ui->cB_Probabildad->currentIndex ();
            ui->pB_TProbabilidades->setEnabled (true);
        }
        else
        {  //llernar combo de velocidad y tiempo segun medio
            ui->cB_Tiempo->clear ();
            ui->cB_Velocidad->clear ();

            if(calculos->medio >= 21 && calculos->medio <36)
            {
                ui->cB_Tiempo->addItem ("0");
                ui->cB_Tiempo->addItem ("5");

                ui->cB_Velocidad->addItem ("150");
                ui->cB_Velocidad->addItem ("300");
                ui->cB_Velocidad->addItem ("420");
                ui->cB_Velocidad->setCurrentIndex (1);
            }
            else if(calculos->medio == 36)
            {
                ui->cB_Tiempo->addItem ("0");
                ui->cB_Tiempo->addItem ("16");

                ui->cB_Velocidad->addItem ("150");
                ui->cB_Velocidad->addItem ("300");
                ui->cB_Velocidad->addItem ("420");
                ui->cB_Velocidad->setCurrentIndex (1);
            }
            else if(calculos->medio == 37)
            {
                ui->cB_Tiempo->addItem ("0");
                ui->cB_Tiempo->addItem ("24");

                ui->cB_Velocidad->addItem ("150");
                ui->cB_Velocidad->addItem ("300");
                ui->cB_Velocidad->addItem ("420");
                ui->cB_Velocidad->addItem ("640");
                ui->cB_Velocidad->setCurrentIndex (1);

                dista.probabilidad = ui->cB_Probabildad->currentIndex ();
            }
            else if(calculos->medio == 38)
            {
                ui->cB_Tiempo->addItem ("0");
                ui->cB_Tiempo->addItem ("15");
                ui->cB_Tiempo->addItem ("25");
                ui->cB_Tiempo->setCurrentIndex (2);

                ui->cB_Velocidad->addItem ("150");
                ui->cB_Velocidad->addItem ("300");
                ui->cB_Velocidad->addItem ("420");
                ui->cB_Velocidad->addItem ("640");
                ui->cB_Velocidad->setCurrentIndex (1);

                dista.probabilidad = ui->cB_Probabildad->currentIndex ();
            }
            else  if(calculos->medio >=42 && calculos->medio<=44)
            {
                ui->cB_Tiempo->addItem ("0");
                ui->cB_Tiempo->addItem ("10");
                ui->cB_Tiempo->setCurrentIndex (1);

                ui->cB_Velocidad->addItem ("150");
                ui->cB_Velocidad->addItem ("300");
                ui->cB_Velocidad->addItem ("420");
                ui->cB_Velocidad->setCurrentIndex (1);

                dista.probabilidad = ui->cB_Probabildad->currentIndex ();
            }
            else
            {
                ui->cB_Tiempo->addItem ("0");
                ui->cB_Tiempo->addItem ("14");
                ui->cB_Tiempo->addItem ("25");
                ui->cB_Tiempo->addItem ("35");
                ui->cB_Tiempo->setCurrentIndex (2);

                ui->cB_Velocidad->addItem ("150");
                ui->cB_Velocidad->addItem ("300");
                ui->cB_Velocidad->addItem ("420");
                ui->cB_Velocidad->addItem ("640");
                ui->cB_Velocidad->addItem ("950");
                ui->cB_Velocidad->setCurrentIndex (1);

                dista.probabilidad = ui->cB_Probabildad->currentIndex ();
            }
        }
    }
    else
    {
        setWindowTitle ("Selecione altura del objetivo");
        ui->groupBox->setTitle ("Selecione altura del objetivo");
        ui->cB_Probabildad->setEnabled (false);
        dista.probabilidad = 0;
        ui->pB_TProbabilidades->setEnabled (false);
        ui->cB_Tiempo->setEnabled (false);
        ui->cB_Velocidad->setEnabled (false);
    }
}

void dvdpSelAltVuelo::on_cB_Probabildad_currentIndexChanged(int index)
{
    dista.probabilidad = index;
    probabilidad = index;
    //   customp->prob =  "_P:"+ui->cB_Probabildad->currentText ();
}

void dvdpSelAltVuelo::on_pB_TProbabilidades_clicked()
{
    velocidad = ui->cB_Velocidad->currentText ().toInt ();
    tiempo = ui->cB_Tiempo->currentText ().toInt ();

    parametro = 0;
    auto *verTodasProb = new VerAlturas();
    lisverAlturas.append (verTodasProb);
    verTodasProb->plottitle->setText ("Todas las Probabilidades: " + calculos->label);
    verTodasProb->medio = calculos->medio;
    verTodasProb->grupo = calculos->grupo;
    verTodasProb->velocidad = velocidad;
    verTodasProb->tiempo = tiempo;
    verTodasProb->fichero = calculos->fichero;
    if((calculos->medio == 39 || calculos->medio == 38 || calculos->medio == 37 || calculos->medio == 36  || calculos->medio == 35  || calculos->medio == 34  || calculos->medio == 33  || calculos->medio == 32  || calculos->medio == 31|| calculos->medio == 27 || calculos->medio == 26 || calculos->medio == 25  || calculos->medio == 24  || calculos->medio == 23  || calculos->medio == 22  || calculos->medio == 21 || calculos->medio == 42 || calculos->medio == 43 || calculos->medio == 44)  && calculos->grupo == 1)
    {
        verTodasProb->fichero.append ("_"+QString::number (velocidad)+"_"+QString::number (tiempo)+"_"+QString::number (parametro));
    }

    // === NUEVO: Añadir sufijo Sigma si corresponde ===
    double sigma = ui->cB_Sigma->currentData().toDouble();
    if (sigma != 1.0 && ui->cB_Sigma->isVisible()) {
        if (sigma == 5.0)       verTodasProb->fichero.append("_S5");
        else if (sigma == 0.5)  verTodasProb->fichero.append("_S05");
        else if (sigma == 0.1)  verTodasProb->fichero.append("_S01");

        if (calculos->medio == 40 && ui->cB_Banda->isVisible()) {
            verTodasProb->fichero.append("_" + ui->cB_Banda->currentData().toString());
        }
    }

    verTodasProb->probOno = false;
    verTodasProb->configInterface();
    bool existe = false;

    if(ui->rB_0m->isChecked ())     alvue = 0;
    if(ui->rB_25m->isChecked ())    alvue = 25;
    if(ui->rB_50m->isChecked ())    alvue = 50;
    if(ui->rB_100m->isChecked ())   alvue = 100;
    if(ui->rB_200m->isChecked ())   alvue = 200;
    if(ui->rB_300m->isChecked ())   alvue = 300;
    if(ui->rB_400m->isChecked ())   alvue = 400;
    if(ui->rB_500m->isChecked ())   alvue = 500;
    if(ui->rB_1000m->isChecked ())  alvue = 1000;
    if(ui->rB_2000m->isChecked ())  alvue = 2000;
    if(ui->rB_4000m->isChecked ())  alvue = 4000;
    if(ui->rB_6000m->isChecked ())  alvue = 6000;
    if(ui->rB_10000m->isChecked ()) alvue = 10000;
    if(ui->rB_17000m->isChecked ()) alvue = 17000;
    if(ui->rB_30000m->isChecked ()) alvue = 30000;

    if(verTodasProb->leerProbabilidad (extension(alvue)))
    {
        existe = true;
        verTodasProb->pintarTodasProbabilidades (QPen(Qt::green,2),4,"0.9",true);
        verTodasProb->pintarTodasProbabilidades (QPen(Qt::green,2),3,"0.8",true);
        verTodasProb->pintarTodasProbabilidades (QPen(Qt::green,2),2,"0.7",true);
        verTodasProb->pintarTodasProbabilidades (QPen(Qt::green,2),1,"0.6",true);
        verTodasProb->pintarTodasProbabilidades (QPen(Qt::green,2),0,"0.5",true);
    }
    else
    {
        QMessageBox::information (nullptr,"Información","El fichero de "+ QString::number (alvue) +"m de altura no existe o el medio\nseleccionado no llega a esa altura.",QMessageBox::Ok);
    }

    if(existe)
    {
        verTodasProb->setWindowTitle ("Todas las Probabilidades: " + calculos->fichero );
        verTodasProb->llenartabla (true,medio);
        verTodasProb->show ();
    }
    else
    {
        QMessageBox::information (nullptr,"Información","Debe calcular la visibilidad directa\npara las diferentes alturas del objetivo primero.",QMessageBox::Ok);
    }
}

QString dvdpSelAltVuelo::extension(int h)
{
    QString strFile;
    if(h == 0   ) strFile = ".m00";
    if(h == 25  ) strFile = ".m25";
    if(h == 50  ) strFile = ".m50";
    if(h == 100 ) strFile = ".001";
    if(h == 200 ) strFile = ".002";
    if(h == 300 ) strFile = ".003";
    if(h == 400 ) strFile = ".004";
    if(h == 500 ) strFile = ".005";
    if(h == 1000) strFile = ".010";
    if(h == 2000) strFile = ".020";
    if(h == 4000) strFile = ".040";
    if(h == 6000) strFile = ".060";
    if(h == 10000)strFile = ".100";
    if(h == 17000)strFile = ".170";
    if(h == 30000)strFile = ".300";

    return strFile;
}

void dvdpSelAltVuelo::on_cB_Tiempo_currentTextChanged(const QString &arg1)
{
    tiempo = arg1.toInt ();
}

void dvdpSelAltVuelo::on_cB_Velocidad_currentTextChanged(const QString &arg1)
{
    velocidad = arg1.toInt ();
}

void dvdpSelAltVuelo::on_pB_Aceptar_toggled(bool checked)
{

}

void dvdpSelAltVuelo::on_pB_ObjetosLocales_clicked()
{
    dista.fichero = calculos->fichero;
    dista.nombreAux = calculos->label;
    dista.calculos = calculos;

    todoCalculado = false;

    ui->pBar_Progreso->setMaximum(360);
    ui->pBar_Progreso->setVisible(true);
    dista.velocidad = velocidad;
    dista.tiempo = tiempo;

    alvue = 0;
    dm00 = false;
    dista.Vuelo = alvue;
    dista.calculos = calculos;
    dista.determDistanCiclo(/*grupo,arma,medio*/);
    dista.angulositio = angulositio;
    dista.start();
}

void dvdpSelAltVuelo::on_chB_Alcance_toggled(bool checked)
{
    alcance = checked;
    if(checked)
    {
        if (medio == 38)
        {
            ui->rB_0m->setEnabled (false);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled (false);
            ui->rB_100m->setEnabled (false);
            ui->rB_200m->setEnabled (false);
            ui->rB_300m->setEnabled (false);
            ui->rB_400m->setEnabled (false);
            ui->rB_17000m->setEnabled (false);
        }
        if(medio == 39)
        {
            ui->rB_0m->setEnabled (false);
            ui->rB_25m->setEnabled (false);
            ui->rB_50m->setEnabled (false);
            ui->rB_30000m->setEnabled (false);

        }
    }
    else
    {
        ui->rB_0m->setEnabled (true);
        ui->rB_25m->setEnabled (true);
        ui->rB_50m->setEnabled (true);
        ui->rB_100m->setEnabled (true);
        ui->rB_200m->setEnabled (true);
        ui->rB_300m->setEnabled (true);
        ui->rB_400m->setEnabled (true);
        ui->rB_17000m->setEnabled (true);
        ui->rB_30000m->setEnabled (true);
    }

}

//----------------------------------------------------------------------------
// NUEVA FUNCIÓN: Configurar combos Sigma y Banda
//----------------------------------------------------------------------------
void dvdpSelAltVuelo::configurarCombosSigmaYBanda()
{
    // === COMBO SIGMA ===
    ui->cB_Sigma->clear();
    ui->cB_Sigma->addItem("1", 1.0);      // Normal
    ui->cB_Sigma->addItem("5", 5.0);      // Sigma 5
    ui->cB_Sigma->addItem("0.5", 0.5);    // Sigma 0.5
    ui->cB_Sigma->addItem("0.1", 0.1);    // Sigma 0.1

    // === COMBO BANDA (solo para medio 40) ===
    ui->cB_Banda->clear();
    ui->cB_Banda->addItem("Métrica (M)", "M");
    ui->cB_Banda->addItem("Decimétrica (DM)", "DM");
    ui->cB_Banda->addItem("Centimétrica (CM)", "CM");

    // Estado inicial: ocultar ambos (se mostrarán según el medio)
    ui->lB_Sigma->setVisible(false);
    ui->cB_Sigma->setVisible(false);
    ui->lB_Banda->setVisible(false);
    ui->cB_Banda->setVisible(false);
}


//----------------------------------------------------------------------------
// NUEVA FUNCIÓN: Actualizar visibilidad del combo Sigma
//----------------------------------------------------------------------------
void dvdpSelAltVuelo::actualizarVisibilidadSigma()
{
    bool usaSigma = false;
    if(calculos != nullptr)
        // Medios que usan Sigma: 36, 38, 39, 40, 42, 43, 44
        usaSigma = (calculos->medio == 36 || calculos->medio == 37 || calculos->medio == 38 ||
                    calculos->medio == 39 || calculos->medio == 40 ||
                    calculos->medio == 42 || calculos->medio == 43 ||
                    calculos->medio == 44);

    ui->lB_Sigma->setVisible(usaSigma);
    ui->cB_Sigma->setVisible(usaSigma);

    if (!usaSigma) {
        ui->cB_Sigma->setCurrentIndex(0);  // Resetear a "1" (normal)
    }

    actualizarVisibilidadBanda();
}


//----------------------------------------------------------------------------
// NUEVA FUNCIÓN: Actualizar visibilidad del combo Banda
//----------------------------------------------------------------------------
void dvdpSelAltVuelo::actualizarVisibilidadBanda()
{
    // Mostrar combo Banda SOLO si es medio 40 Y sigma != 1
    double sigma = ui->cB_Sigma->currentData().toDouble();
    bool muestraBanda = false;
    if(calculos != nullptr)
        muestraBanda = (calculos->medio == 40 && sigma != 1.0);

    ui->lB_Banda->setVisible(muestraBanda);
    ui->cB_Banda->setVisible(muestraBanda);

    if (!muestraBanda) {
        ui->cB_Banda->setCurrentIndex(0);  // Resetear a "M"
    }
}


//----------------------------------------------------------------------------
// NUEVOS SLOTS
//----------------------------------------------------------------------------
void dvdpSelAltVuelo::on_cB_Sigma_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    actualizarVisibilidadBanda();
}

void dvdpSelAltVuelo::on_cB_Banda_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    // No se necesita acción adicional, el valor se lee al calcular
}
