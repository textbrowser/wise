macx {
dmg.commands = make install && hdiutil create Wise.d.dmg -srcfolder Wise.d
QMAKE_EXTRA_TARGETS += dmg
}

unix {
purge.commands = find . -name \'*~\' -exec rm {} \;
QMAKE_EXTRA_TARGETS += purge
}

CONFIG += qt release warn_on
DEFINES +=
LANGUAGE = C++
QMAKE_CLEAN += Wise
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
                          -Wno-c++20-attribute-extensions \
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
                          -std=c++20
} else:freebsd-* {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Werror \
                          -Wextra \
                          -Wformat=2 \
                          -Wno-c++20-attribute-extensions \
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
                          -std=c++20
} else:macx {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wenum-compare \
                          -Wextra \
                          -Wformat=2 \
                          -Wno-c++20-attribute-extensions \
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
                          -std=c++20
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
                          -std=c++20
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
                          -std=c++20
contains(QMAKE_HOST.arch, ppc) {
QMAKE_CXXFLAGS_RELEASE -= -Wformat-overflow=2 \
                          -Wstringop-overflow=4
}
}

greaterThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS_RELEASE += -std=c++20
QMAKE_CXXFLAGS_RELEASE -= -std=c++11
}

macx {
QMAKE_DISTCLEAN += -r Wise.d
}

win32 {
QMAKE_DISTCLEAN += rm -fr .qmake* Temporary debug release
} else {
QMAKE_DISTCLEAN += -f .qmake* -fr Temporary
}

QMAKE_STRIP = echo

macx {
ICON = Icons/wise.icns
}

win32 {
RC_FILE = Icons/wise.rc
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

FORMS += UI/wise.ui \
         UI/wise-pdf-view.ui \
         UI/wise-settings.ui
HEADERS += Source/wise.h \
           Source/wise-pdf-view.h \
           Source/wise-recent-files-view.h \
           Source/wise-settings.h \
           Source/wise-tab.h \
           Source/wise-tab-bar.h
SOURCES += Source/wise.cc \
           Source/wise-main.cc \
           Source/wise-pdf-view.cc \
           Source/wise-recent-files-view.cc \
           Source/wise-settings.cc \
           Source/wise-tab.cc \
           Source/wise-tab-bar.cc
TRANSLATIONS =

PROJECTNAME = Wise
TARGET = Wise
TEMPLATE = app

android {
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android
DISTFILES += Android/AndroidManifest.xml \
             Android/build.gradle \
             Android/res/values/libs.xml
android: include(/home/saturn/Android/Sdk/android_openssl/openssl.pri)
}

# Installation Procedures

macx {
macdeployqt.extra = $$[QT_INSTALL_BINS]/macdeployqt Wise.d/Wise.app \
                    -executable=Wise.d/Wise.app/Contents/MacOS/Wise
macdeployqt.path  = Wise.app
preinstall.extra  = rm -fr Wise.d/Wise.app
preinstall.path   = Wise.d
wise.extra        = cp -r ./Wise.app Wise.d/.
wise.path         = Wise.d

INSTALLS = preinstall \
           wise \
           macdeployqt
}

win32 {
plugins.files = $$[QT_INSTALL_PLUGINS]\\*
plugins.path = release\\plugins\\.
qt.files = Qt\\qt.conf
qt.path = release\\.
qtlibraries.files = $$[QT_INSTALL_BINS]\\Qt*Core.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Gui.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Network.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Pdf.dll \
                    $$[QT_INSTALL_BINS]\\Qt*PrintSupport.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Sql.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Widgets.dll \
                    $$[QT_INSTALL_BINS]\\libgcc_s_seh-*.dll \
                    $$[QT_INSTALL_BINS]\\libstdc++-*.dll \
                    $$[QT_INSTALL_BINS]\\libwinpthread-*.dll
qtlibraries.path = release\\.

INSTALLS = plugins \
           qt \
           qtlibraries
}

android {
DISTFILES += Android/AndroidManifest.xml \
             Android/build.gradle \
             Android/gradle.properties \
             Android/gradle/wrapper/gradle-wrapper.jar \
             Android/gradle/wrapper/gradle-wrapper.properties \
             Android/gradlew \
             Android/gradlew.bat \
             Android/res/values/libs.xml \
             Android/res/xml/qtprovider_paths.xml
}
