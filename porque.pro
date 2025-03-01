macx {
dmg.commands = make install && hdiutil create Porque.d.dmg -srcfolder Porque.d
QMAKE_EXTRA_TARGETS += dmg
}

unix {
purge.commands = find . -name \'*~\' -exec rm {} \;
QMAKE_EXTRA_TARGETS += purge
}

CONFIG += qt release warn_on
DEFINES +=
LANGUAGE = C++
QMAKE_CLEAN += Porque
QT += concurrent gui network printsupport sql widgets

contains(QMAKE_HOST.arch, armv7l) {
QMAKE_CXXFLAGS_RELEASE += -march=armv7
}

qtHaveModule(pdf) {
qtHaveModule(pdfwidgets) {
QT += pdf pdfwidgets
} else {
error("The QtPdfWidgets module was not found.")
}
} else {
error("The QtPdf module was not found.")
}

QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE -= -O2

android {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-qual \
                          -Wenum-compare \
                          -Wextra \
                          -Wfloat-equal \
                          -Wformat=2 \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIC \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++17
} else:freebsd-* {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Werror \
                          -Wextra \
                          -Wformat=2 \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++17
} else:macx {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wenum-compare \
                          -Wextra \
                          -Wformat=2 \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++17
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
} else:win32 {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wenum-compare \
                          -Wextra \
                          -Wformat=2 \
                          -Wl,-z,relro \
                          -Wno-class-memaccess \
                          -Wno-deprecated-copy \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIE \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++17
} else {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wenum-compare \
                          -Wextra \
                          -Wfloat-equal \
                          -Wformat=2 \
                          -Wformat-overflow=2 \
                          -Wl,-z,relro \
                          -Wlogical-op \
                          -Wno-class-memaccess \
                          -Wno-deprecated-copy \
                          -Wold-style-cast \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wstringop-overflow=4 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++17
contains(QMAKE_HOST.arch, ppc) {
QMAKE_CXXFLAGS_RELEASE -= -Wformat-overflow=2 \
                          -Wstringop-overflow=4
}
}

greaterThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS_RELEASE += -std=c++17
QMAKE_CXXFLAGS_RELEASE -= -std=c++11
}

macx {
QMAKE_DISTCLEAN += -r Porque.d
}

win32 {
QMAKE_DISTCLEAN += del /F /Q .qmake* \
                   rmdir /Q /S Temporary \
                   rmdir /Q /S debug \
                   rmdir /Q /S release
} else {
QMAKE_DISTCLEAN += -f .qmake* \
                   -fr Temporary
}

QMAKE_STRIP = echo

macx {
ICON = Icons/Logo/porque-logo.icns
}

win32 {
RC_FILE = Icons/porque.rc
}

INCLUDEPATH += Source

macx {
LIBS += -framework AppKit -framework Cocoa
OBJECTIVE_HEADERS += Source/CocoaInitializer.h
OBJECTIVE_SOURCES += Source/CocoaInitializer.mm
} else {
LIBS +=
}

MOC_DIR = Temporary/moc
OBJECTS_DIR = Temporary/obj
RCC_DIR = Temporary/rcc
RESOURCES = Documentation/documentation.qrc Icons/icons.qrc
UI_DIR = Temporary/ui

FORMS += UI/porque.ui \
         UI/porque-pdf-view.ui
HEADERS += Source/porque.h \
           Source/porque-pdf-view.h \
           Source/porque-tab.h \
           Source/porque-tab-bar.h
SOURCES += Source/porque.cc \
           Source/porque-main.cc \
           Source/porque-pdf-view.cc \
           Source/porque-tab.cc \
           Source/porque-tab-bar.cc
TRANSLATIONS =

PROJECTNAME = Porque
TARGET = Porque
TEMPLATE = app

android {
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android
DISTFILES += Android/AndroidManifest.xml \
             Android/build.gradle \
             Android/res/values/libs.xml
android: include(/home/saturn/Android/Sdk/android_openssl/openssl.pri)
}

# Installation Procedures

android {
examples.depends += FORCE

INSTALLS += examples
}

macx {
macdeployqt.extra = $$[QT_INSTALL_BINS]/macdeployqt Porque.d/Porque.app \
                    -executable=Porque.d/Porque.app/Contents/MacOS/Porque
macdeployqt.path = Porque.app
preinstall.extra = rm -fr Porque.d/Porque.app
preinstall.path = Porque.d

INSTALLS = preinstall \
           macdeployqt
}

win32 {
plugins.files = $$[QT_INSTALL_PLUGINS]\\*
plugins.path = release\\plugins\\.
qt.files = Qt\\qt.conf
qt.path = release\\.
qtlibraries.files = $$[QT_INSTALL_BINS]\\Qt6Core.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Gui.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Network.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Pdf.dll \
                    $$[QT_INSTALL_BINS]\\Qt6PrintSupport.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Sql.dll \
                    $$[QT_INSTALL_BINS]\\Qt6Widgets.dll \
                    $$[QT_INSTALL_BINS]\\libgcc_s_seh-1.dll \
                    $$[QT_INSTALL_BINS]\\libstdc++-6.dll \
                    $$[QT_INSTALL_BINS]\\libwinpthread-1.dll
qtlibraries.path = release\\.

INSTALLS = plugins \
           qt \
           qtlibraries
}
