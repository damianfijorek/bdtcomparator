
QT      += core gui

win32 {
  TARGET = bdtcomparator.exe
}
unix {
  TARGET = bdtcomparator
}

TEMPLATE = app

FORMS   += mainwindow.ui \
           aboutdialog.ui

SOURCES += main.cpp\
           mainwindow.cpp \
           datatable.cpp \
           calculator.cpp \
           resultstable.cpp \
           results.cpp \
           params.cpp \
           aboutdialog.cpp \
           permutation.cpp

HEADERS += \
           mainwindow.hpp \
           datatable.hpp \
           calculator.hpp \
           resultstable.hpp \
           results.hpp \
           params.hpp \
           aboutdialog.hpp \
           permutation.hpp

# Add path to Boost library
#INCLUDEPATH += BOOST_PATH

RESOURCES = resources.qrc
