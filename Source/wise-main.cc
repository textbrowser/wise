/*
** Copyright (c) Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from Wise without specific prior written permission.
**
** WISE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** WISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "wise.h"

#include <QApplication>
#include <QDir>
#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif
#include <QSettings>

#ifdef Q_OS_MACOS
#include "CocoaInitializer.h"
#endif

int main(int argc, char *argv[])
{
  for(int i = 0; i < argc; i++)
    if(!argv || !argv[i])
      continue;

#if defined(Q_OS_ANDROID)
#else
  QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);
#endif

  QApplication application(argc, argv);

  application.setWindowIcon(QIcon(":/wise.png"));

  QDir dir;

  dir.mkdir(wise::home_path());

#ifdef Q_OS_MACOS
  /*
  ** Eliminate pool errors on MacOS.
  */

  CocoaInitializer ci;
#endif
  QCoreApplication::setApplicationName("Wise");
  QCoreApplication::setApplicationVersion(WISE_VERSION_STRING);
  QCoreApplication::setOrganizationName("Wise");
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath
    (QSettings::IniFormat, QSettings::UserScope, wise::home_path());

  int rc = 0;

  {
    wise wise;

#ifndef Q_OS_ANDROID
    wise.show();
#else
    wise.showMaximized();
#endif
    rc = static_cast<int> (application.exec());
  }

#ifdef Q_OS_ANDROID
  auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());

  activity.callMethod<void> ("finishAndRemoveTask");
#endif
  return rc;
}
