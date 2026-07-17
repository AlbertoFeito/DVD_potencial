#-------------------------------------------------
#
# Project created by QtCreator 2017-08-25T11:49:43
#
#-------------------------------------------------

QT       += core gui positioning sql network
QT += core
qtHaveModule(printsupport): QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ZVisibilidad
TEMPLATE = app
RC_FILE = icon.rc
QMAKE_LFLAGS += -fopenmp
QMAKE_CXXFLAGS+= -fopenmp
LIBS += -fopenmp
SOURCES += main.cpp\
#    basedatos/cbasedatos.cpp \
    c_objetoslocales.cpp \
        qinicial.cpp \
    dlgselecpo.cpp \
    qventanainicialerlypov.cpp \
    dvdpentrada.cpp \
    qcustomplot.cpp \
    ccalculos.cpp \
    dancalc.cpp \
    cdatoangulocierre.cpp \
    cdatoazimut.cpp \
    cdatoterreno.cpp \
    socket/gmsgeocoord.cpp \
    socket/socket.cpp \
    trabsasa.cpp \
    utiles.cpp \
    hilo.cpp \
    perfil.cpp \
    graficaperfil.cpp \
    angcierre1.cpp \
    angucierre2.cpp \
    angulocierre.cpp \
    distanciavd.cpp \
    distanthread.cpp \
    dvdpselaltvuelo.cpp \
    comondirecta.cpp \
    cpuntoapunto.cpp \
    qeditenteros.cpp \
    zpfaaa.cpp \
    zpfaaaentrada.cpp \
    zdcaaentrada.cpp \
    zpdcaaa.cpp \
    c_grafplazoleta.cpp \
    c_perfizc.cpp \
    c_plazoletazc.cpp \
    hilografplazoleta.cpp \
    dlgseleczpfozvd.cpp \
    qficheroini.cpp \
    dlgselectzpdozvd.cpp \
    dlgselcterl.cpp \
    veralturas.cpp \
    dlgbiestatico.cpp \
    dlgbiestaticoentrada.cpp \
    dlgsectorbiestatico.cpp \
    campos.cpp \
    qfindfiles.cpp \
    cdvdbiestaticos.cpp \
    customplotf.cpp \
    dlgobjetivop.cpp

HEADERS  += qinicial.h \
#    basedatos/cbasedatos.h \
    c_objetoslocales.h \
    dlgselecpo.h \
    qventanainicialerlypov.h \
    dvdpentrada.h \
    qcustomplot.h \
    ccalculos.h \
    defcomunes.h \
    dancalc.h \
    cdatoterreno.h \
    cdatoangulocierre.h \
    cdatoazimut.h \
    socket/gmsgeocoord.h \
    socket/socket.h \
    trabsasa.h \
    utiles.h \
    hilo.h \
    perfil.h \
    graficaperfil.h \
    angcierre1.h \
    angucierre2.h \
    angulocierre.h \
    distanciavd.h \
    distanthread.h \
    dvdpselaltvuelo.h \
    comondirecta.h \
    cpuntoapunto.h \
    qeditenteros.h \
    zpfaaa.h \
    zpfaaaentrada.h \
    zdcaaentrada.h \
    zpdcaaa.h \
    hilografplazoleta.h \
    c_grafplazoleta.h \
    c_perfizc.h \
    c_plazoletazc.h \
    dlgseleczpfozvd.h \
    qficheroini.h \
    dlgselectzpdozvd.h \
    dlgselcterl.h \
    veralturas.h \
    dlgbiestatico.h \
    dlgbiestaticoentrada.h \
    dlgsectorbiestatico.h \
    campos.h \
    qfindfiles.h \
    direntlinux.h \
    direntwin32.h \
    cdvdbiestaticos.h \
    customplotf.h \
    dlgobjetivop.h

FORMS    += qinicial.ui \
    dlgselecpo.ui \
    qventanainicialerlypov.ui \
    dvdpentrada.ui \
    perfil.ui \
    graficaperfil.ui \
    angulocierre.ui \
    angcierre1.ui \
    angucierre2.ui \
    dvdpselaltvuelo.ui \
    distanciavd.ui \
    cpuntoapunto.ui \
    comondirecta.ui \
    zpfaaa.ui \
    zpfaaaentrada.ui \
    zdcaaentrada.ui \
    zpdcaaa.ui \
    c_grafplazoleta.ui \
    c_perfizc.ui \
    c_plazoletazc.ui \
    dlgseleczpfozvd.ui \
    dlgselectzpdozvd.ui \
    dlgselcterl.ui \
    veralturas.ui \
    dlgbiestatico.ui \
    cdistanciabiestatico.ui \
    dlgbiestaticoentrada.ui \
    dlgsectorbiestatico.ui \
    campos.ui \
    cdvdbiestaticos.ui \
    dlgobjetivop.ui

RESOURCES += \
    style.qrc

DISTFILES += \
    icon.rc \
    m21g1.txt \
    m22g1.txt \
    m23g1.txt \
    m24g1.txt \
    m25g1.txt \
    m26g1.txt \
    m27g1.txt \
    m31g1.txt \
    m32g1.txt \
    m33g1.txt \
    m34g1.txt \
    m35g1.txt \
    m36g1.txt \
    m37g1.txt \
    m38a.txt \
    m38g1.txt \
    m39a.txt \
    m39g1.txt \
    m42g1.txt \
    m43g1.txt \
    m44g1.txt
#DEFINES += QCUSTOMPLOT_USE_OPENGL
