TEMPLATE = app
TARGET = waterBeam
DESTDIR = ../local/bin
DEPENDPATH += .
INCLUDEPATH += . ..

mac {
  CONFIG -= app_bundle
}

# Build config
CONFIG += qt debug c++17 link_pkgconfig

# Input sources (be explicit so BetheBlochFormula.cpp is definitely compiled)
SOURCES += \
    waterBeam.cpp \
    BetheBlochFormula.cpp

QMAKE_DEL_FILE = rm -rf
QMAKE_DISTCLEAN += ../local

mac {
  PKG_CONFIG_PATH += $$[QT_HOST_PREFIX]/opt/qt/libexec/lib/pkgconfig:$$[QT_INSTALL_LIBS]/pkgconfig
  PKG_CONFIG = PKG_CONFIG_PATH=$$PKG_CONFIG_PATH pkg-config
}

# Qat libraries needed:
# - QatPlotWidgets: PlotView, etc.
# - QatPlotting: PlotFunction1D, PlotStream, InterpolatingFunction helper bits
# - QatGenericFunctions: AbsFunction base, BetheBloch, etc.
PKGCONFIG += QatPlotWidgets QatPlotting QatGenericFunctions

