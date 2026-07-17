#ifndef DEFCOMUNES_H
#define DEFCOMUNES_H



//---------------------------REPRESENTACION -----------------------------------------//
//Color de los elementos informativos

#define Color_Cuba         QColor(0,100,0)//RGB(0,127,0);
#define Color_America      QColor(0,127,0)//RGB(0,127,0);
#define Color_Provincia    QColor(168,0,0)
#define Color_Geografica   QColor(125,126,127)
#define Color_Daafar       QColor(154,133,153)
#define Color_CuadDaafar   QColor(66,67,68)
#define Color_CDaafar      QColor(255,231,127)
#define Color_Aguas        QColor(200,0,200)//RGB(127,0,127);
#define Color_Fir          QColor(0,150,210)//RGB(0,89,145);
#define Color_Corredores   QColor(0,150,210)//RGB(0,89,145);
#define Color_Marcas       QColor(66,67,68)//RGB(66,67,68);
#define Color_Mur          QColor(160,63,15)
#define Color_Ejercitos    QColor(127,127,127)
#define Color_Ruta         QColor(128,128,255)

//----Parametro de corrimiento vertical para el sistema de coordenadas polares ------//
#define DVERTICAL 4.0	//ROIG 27 09 CORRECCION AL MAPA

//---------------------------DATOS DE ORIGRN SISTEMA --------------------------------//
#define LONGCENTRAL         100.00
#define LATCENTRAL          10.00

//---------------------------DATOS DE ORIGRN CNC --------------------------------//
#define LONGCNC             87.00
#define LATCNC              18.00

//---------------------------PRIORIDAD HILOS --------------------------------//
#define     HILORX          0
#define     HILOCOM         1
#define     HILOPROC        2
#define     HILOREPRE       3
#define     HILOTX          5
#define     HILOSRX         6
#define     HILOCOPIA       7

//---------------------------CONSTANTES MATEMATICAS --------------------------------//
#define RADIAN              M_PI/180.0
#define GRADOS              57.2957795130823208767981548141052
#define UN_DIA              86400000

//---------------------------CONSTANTES DEL SISTEMA--------------------------------//
#define     CODIND          170     //código para la validación de la Indicación de Blancos con inf. del nivel
#define     CODINDSUP		171     //código para la validación de la Indicación de Blancos con inf. del Mando Superior
#define     CODASIGN		85      //código para la validación de la Indicación de Blancos con inf. del nivel
#define     CODASIGNSUP		86      //código para la validación de la Indicación de Blancos con inf. del Mando Superior
#define     CODNUM          150     //código para la validación del cambio de número de la Trayectoria
#define     CANTIND         10      //cantidad máxima de blancos a realizar indicación de blanco en un puesto de trabajo
#define     CANTDATRX		70      //cantidad de byte posibles a recibir en el hilo de recepción.
#define     CANTPERDIDA		5       //cantidad de periodos que se espera para dar perdida a un objetivo
#define     CANTPERDMANUAL  12      //cantidad de periodos que se espera para dar perdida a un objetivo manual
#define     CANTPTOINTERF	10      //máxima cantidad de puntos de la zona de interferencia (General)
#define     MAXPTOZMET		10      //máxima cantidad de puntos para la zona de interferencia Meteorológica
#define     MAXPTOZPAS		10      //máxima cantidad de puntos para la zona de interferencia Pasiva
#define     MAXPTOPORTADOR	2       //máxima cantidad de puntos para el Portador de interferencia
#define     MAXPTOSECTOR	3       //máxima cantidad de puntos para el sector de interferencia
#define     MODTRAB_RA		1
#define     MODTRAB_OC		2

//---------------------------PARAMETROS DE LAS FUENTES--------------------------------//
#define     MAXFUENTES		18      //la fuente 18 (17)es el puerto del PGM 25/04/08
#define     MAXFTEVIRT		8		//máxima cantidad de fuentes virtuales a mostrar por el sistema
#define     NO_PFUENTE      88      //fuente no valida está conectada al sistema pero
                                    //no es parte de las 16 fuentes
#define     PUERTOPGM       17      //lugar en el arreglo de las fuentes asignado a
                                    //los datos del PGM o Mando superior
#define     MAXOBJETIVOS	251     //máxima cantidad de objetivos en el sistema (250)
#define     MAXCANTNIVEL    50      //máxima cantidad de niveles de procesamiento
#define     MAXCANTTIPOINF  255     //máxima cantidad de tipos de información
#define     INVAL_OBJ       255     //número de objetivo no valido para el nivel, a
                                    //los efecto no hay dato del objetivo(NULL)
#define     INVAL_OBJ_SEC	5000    //número de objetivo no valido para el Secundario,
                                    //a los efecto no hay dato del objetivo(NULL)
#define     CANTMARCAS		4000    //máxima cantidad de marcas de objetivos que puede
                                    //recibir el sistema en un período
#define     CON_PUERTO      1       //establecida comunicación por modem
#define     CON_SOCKET      2       //establecida comunicación por red
#define     CON_SIST        3       //establecida comunicación para la fuente cero
#define     MAXPLANV        1000    //máxima cantidad de planes de vuelo a recibir
#define     MAXCANTESTRB	25      //máxima cantidad de marcas en el estrobo

//---------------------------PARAMETROS DE LOS SOCKETS--------------------------------//
#define     MAXABONADOS		101
#define     MAXBUFFER_RX    18000   //máximo tamaño del buffer de recepción
#define PUERTOSERVER		6341	//puerto de red fundamental del sistema
#define PUERTOSERVER_PM		0000	//definido para futuras prestaciones
#define PUERTOSERVER_SEC	3120	//definido para recibir de Radares Secundarios y PTR
#define PUERTOSERVER_RA		3122	//definido para recibir de Radioaltímetros
#define PUERTOSERVER_PV		4002	//definido para recibir el plan de vuelo
#define     PM_SUPERIOR		1       //destinacion que debe tener la conexión
                                    //(para recibir del mando superior)
#define     SERVIDOR		2       //destinacion que debe tener la
                                    //conexión (socket servidor)
#define     CLIENTE         3       //destinacion que debe tener la
                                    //conexión (socket cliente)
#define     RESPSERVER      4       //socket que se crea en escucha (Lisent) en la
                                    //variante socket servidor
#define     TAMANO_BUF      32      //tamaño del buffer de conexión de los socket

#define     TIMECOMP		7       //tiempo de comprobación por encuesta de los socket
#define     DEMORA   		16      //demora para cerrar un socket

//---------------------------valores de la variable bSocket--------------------------------//
#define     NOVALIDO        0
#define     CREADO          1
#define     ACEPTADO        2
#define     RECIBE          3
#define     TRANSMITE       4
#define     CONECTADO       5
#define     CERRADO         6
#define     SINDATO         7
#define     DESCONECTADO    8
#define     CONFLICTO       9
#define     CONECTANDO      10

//---------------CODIGO DE LOS NIVELES DE PROCESAMIENTO		(CGS) (SAC)--------------------//
#define     ERL             0       //NIVEL Para las ERL Primarias
#define     BONRT           1       //NIVEL Para los Batallones Radiotícnicos
#define     INTEL           2       //NIVEL Para la información de
#define     MEDIO_INTEL		3       //NIVEL Para los medios de exploracion pasiva
#define     SEC_DATO		4       //NIVEL Para la información de los datos del IACC
#define     CNC_PV          5       //NIVEL Para la información de Plan de Vuelo de los datos del IACC
#define     BRIG_ELAB		6       //NIVEL Para las Brigadas DAAFAR elaboración
#define     CNC_ELAB		7       //NIVEL Para el CNC-PV para la representación de la información
#define     EXP_VISUAL		8       //NIVEL Para la información de la exploración visual
#define     POP_SASA		9       //NIVEL Para los medios de exploración visual
//#define   Reservado              10
#define     EJTO_ELAB		11      //NIVEL Para los Ejercitos elaboración
#define     MGR			    12      //NIVEL Para la información de La Marina de Guerra
#define     MEDIO_MGR		13      //NIVEL Para los medios de la Marina de Guerra
#define     PGM_JEF         14      //NIVEL Para la información del PGM jefatura
#define     PGM_ELAB		15      //NIVEL Para la información del PGM elaboración
//#define	Reservado           16
#define     REGION_ELAB		17      //NIVEL Para la información de la Región Militar de elaboración
#define     SECTOR          18      //NIVEL Para la información del Sector Militar
#define     REGION_JEF		19      //NIVEL Para la información de la Región Militar de jefatura
#define     EXPERTO         20      //NIVEL Para la información del sistema experto
#define     EMB_ME          21      //NIVEL MAQUINA DE EXPLORACION DE LA EMBOSCADA INTEGRAL
#define     EMB_MEI         22      //NIVEL MAQUINA DEL JEFE DE LA EMBOSCADA INTEGRAL
#define     EMB_DAA         23      //NIVEL MAQUINA DEL JEFE DE LA DAA DE LA EMBOSCADA INTEGRAL
#define     BRIG_JEF		24      //NIVEL Para las Brigadas DAAFAR jefatura
#define     EMB_CCAA		25      //NIVEL MAQUINA DE LOS CCAA
#define     EMB_DF          26      //NIVEL MAQUINA DE DIRECTORES DE FUEGO (CORTO ALCANCE) DE LA EMBOSCADA INTEGRAL
#define     EJTO_JEF		27      //NIVEL Para los Ejercitos jefatura
#define     TGF             28      //NIVEL Para las tropas guarda fronteras
#define     MEDIO_TGF		29      //NIVEL Para los medios de las tropas guarda fronteras
#define     INVITADO1		30      //NIVEL Para los abonados que participan en el sistema como invitados,
                                        //solo representa la inf del nivel del cual es invitado.
#define     INVITADO2		31      //NIVEL Para los abonados que participan en el sistema como invitados
                                        //representa la inf del mando superior.
#define     MAQ_PARTE		32      //NIVEL Para las máquinas del parte de los escalones de mando
#define     CNC_JEF         33      //NIVEL Para el CNC-PV Jefatura para la representación de la información
#define     CNC_CONTROL		34      //NIVEL Para el CNC-PV Controlador para la representación de la información
//#define	R_RESP_JEF           35     //NIVEL Para el PM de Zona de Responsabilidad (Elaboración)
//#define	R_RESP_ELAB          36     //NIVEL Para el PM de Zona de Responsabilidad (Jefatura)
#define     BRIG_INF_JEF    37      //NIVEL Brigada de infanteria de Zona de Responsabilidad (jefatura)
#define     BRIG_INF_ELAB   38      //NIVEL Brigada de infanteria de Zona de Responsabilidad (Elaboración)
#define     GPO_ART         39      //NIVEL Grupo Artilleria
#define     BAT_ART         40      //NIVEL Bateria Artilleria
#define     DIV_TANQ_JEF    41      //NIVEL División de Tanque (Jetatura)
#define     DIV_TANQ_ELAB   42      //NIVEL División de Tanque (Elaboración)
#define     BRIG_TANQ_JEF   43      //NIVEL Brigada de Tanque (Jetatura)
#define     BRIG_TANQ_ELAB  44      //NIVEL División de Tanque (Elaboración)
#define     GPO_TANQUE		45      //NIVEL Batallón de Tanque
#define     BAT_TANQUE		46      //NIVEL Pelotón de Tanque
#define     OPER_UNO		47      //NIVEL para el opreador uno de elaboración
#define     OPER_DOS		48      //NIVEL para el opreador dos de elaboración
#define     OPER_TRES		49      //NIVEL para el opreador tres de elaboración
#define     OPER_CUATRO		50      //NIVEL para el opreador cuatro de elaboración

//---------------CODIGO DE COMIENZO DE LOS PAQUETES--------------------//
#define     COMIENZO        165

//---------------Definiciones para el trabajo con los Radioaltímetros--------------------//
#define     RGORAMIN		177     //Rango mánimo de CES para Radioaltímetro
#define     RGORAMAX		216     //Rango máximo de CES para Radioaltímetro
#define     ID_PARA_RA		16      //Tipo paquete para la Identificacion con los Radioaltímetros
#define     UBIC_RA         17      //Tipo paquete para la ubicación de los Radioaltímetros
#define     DATO_RA         18      //tipo paquete para solicitud y entrega de la altura por Radioaltímetro

//---------------Dimencion de los paquetes de datos de objetivos--------------------//
#define     PAQINIC         70      //cant de byte en el comienzo del fichero del control de objetivos

#define		PAQMoS_C		21		//cantidad de bytes que conforman el paquete CORTO de información Monosensor
#define		PAQMoS_M		23		//cantidad de bytes que conforman el paquete MEDIO de información Monosensor
#define		PAQMoS_L		26		//cantidad de bytes que conforman el paquete LARGO de información Monosensor
#define		PAQMoS_E		31		//cantidad de bytes que conforman el paquete EXTRA LARGO de información Monosensor

#define		PAQMuS_C		23		//cantidad de bytes que conforman el paquete CORTO de información Multisensor
#define		PAQMuS_M		27		//cantidad de bytes que conforman el paquete MEDIO de información Multisensor
#define		PAQMuS_L		32		//cantidad de bytes que conforman el paquete LARGO de información Multisensor
#define		PAQMuS_E		38		//cantidad de bytes que conforman el paquete EXTRA LARGO de información Multisensor
#define		PAQMuS_S		50		//cantidad de bytes que conforman el paquete SUPER LARGO de información Multisensor
#define		PAQMuS_X		54		//cantidad de bytes que conforman el paquete MAS LARGO de información Multisensor

#define		PAQPGM_C		23		//cantidad de bytes que conforman el paquete CORTO de información del PGM
#define		PAQPGM_M		27		//cantidad de bytes que conforman el paquete MEDIO de información del PGM
#define		PAQPGM_L		32		//cantidad de bytes que conforman el paquete LARGO de información del PGM
#define		PAQPGM_E		38		//cantidad de bytes que conforman el paquete EXTRA LARGO de información del PGM
#define		PAQPGM_S		50		//cantidad de bytes que conforman el paquete SUPER LARGO de información del PGM
#define		PAQPGM_X		54		//cantidad de bytes que conforman el paquete MAS LARGO de información del PGM

#define		PAQInd_B		19		//cantidad de bytes que conforman el paquete de indicación de blancos
#define		PAQASIGN		19		//cantidad de bytes que conforman el paquete de asignación de objetivo
#define		PAQCam_N		21		//cantidad de bytes que conforman el paquete de cambio de número del objetivo
#define		PAQMANDO		13		//cantidad de bytes que conforman el paquete de Mando
#define		PAQUBIC			14		//cantidad de bytes que conforman el paquete de Ubicacion
#define		PAQUBIC_RA		18		//cantidad de bytes que conforman el paquete de Ubicacion del RA
#define		PAQAUTENTIC		18		//cantidad de bytes que conforman el paquete de Autenticacion
#define		PAQESTSENSOR	11		//cantidad de bytes que conforman el paquete de Estatus Sensor

#define		PaqIdParaRA		10		//cantidad de bytes que conforman el paquete de Identificacion con el RA
#define		PaqDatoRA		17		//cantidad de bytes que conforman el paquete de datos del Radioaltímetro

#define		PAQMETEOR		44		//cantidad de bytes que conforman el paquete de Meteoro

#define		PAQINTERF_C		21		//cantidad de bytes que conforman el paquete de Interferencia con 2 puntos
#define		PAQINTERF_M		25		//cantidad de bytes que conforman el paquete de Interferencia con 3 puntos
#define		PAQINTERF_L		29		//cantidad de bytes que conforman el paquete de Interferencia con 4 puntos
#define		PAQINTERF_E		53		//cantidad de bytes que conforman el paquete de Interferencia con 10 puntos
#define		PAQPV_C			56		//cantidad de bytes que conforman el paquete CORTO de información del Plan de Vuelo
#define		PAQPV_M			62		//cantidad de bytes que conforman el paquete MEDIO de información del Plan de Vuelo

//-----------------------------constantes de control---------------------------------//
#define		CALIDAD_1		3
#define		CALIDAD_2		30
#define		CALIDAD_3		9
#define		CALIDAD_4		90

//----------------------------------------PUERTO---------------------------------//
#define     ASCII_XON       0x11
#define     ASCII_XOFF      0x13
#define     MOD_MALO        0
#define     MOD_COMPROB     1
#define     MOD_COMANDO     2
#define     MOD_CONECT      3
#define     MOD_DATO        4
#define     MAXPORT         8
#define     T_RS232         0
#define     T_MODNORMAL     1
#define     T_MODINTEL      2

//---------------------------DEFINICION DE LOS ESTROBOS-----------------------------//
//#define EstroboDetec        13.0      //24		no se usa
//#define EstroboConfirma		10.0       //19		no se usa
//#define EstroboSeguimiento	5.5       //11		nose usa
#define EstroboAsoc			8.0       //4		se usa para asociar marcas con la trayectoria
#define EstroboAltura		10        //4		se usa para asociar marcas con la trayectoria
#define EstroboAlturaCaza	50        //4		se usa para asociar marcas con la trayectoria
#define EstroboCambioNum	1.0       //2		no se usa
#define EstroboDistMaxRA	360		  //Dista Max a transmitir obj a los RadioAltimetros

//---CONSTANTES DEFINIDAS PARA LA PROGRAMACION DEL ROUTER (RAIDA)----//
#define UNDEFINE			   -1
#define PRESS_ENTER_TO_START	1
#define SECOND_QUIDWAY			2
#define RESET_SA				3
#define DIS_CURR				4
#define WAIT				    5
#define REBOOT					6
#define PROG					7
//---------------------------------------//
#define INIT_PACK				10
#define END_PACK				13
//---------------------------------------//

//-----------CONSTANTES PARA EL TRABAJO CON EL IPDA-----------------//
#define CANTMODELOS				3


#endif // DEFCOMUNES_H
