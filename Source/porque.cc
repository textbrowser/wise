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
**    derived from Porque without specific prior written permission.
**
** PORQUE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** PORQUE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "porque.h"

#include <QDir>
#include <QSettings>

QString porque::PORQUE_VERSION_STRING = "2025.04.01";

porque::porque(void):QMainWindow(nullptr)
{
  m_ui.setupUi(this);
  m_ui.tool_bar->addAction(m_ui.action_Open_PDF_Files);
  m_ui.tool_bar->setIconSize(QSize(50, 50));
  connect(m_ui.action_Quit,
	  &QAction::triggered,
	  this,
	  &porque::slot_quit);
  prepare_icons();
  restore();
}

porque::~porque()
{
}

QString porque::home_path(void)
{
#ifdef Q_OS_WINDOWS
  return QDir::currentPath() + QDir::separator() + ".porque";
#else
  return QDir::homePath() + QDir::separator() + ".porque";
#endif
}

void porque::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
  QSettings().setValue("geometry", saveGeometry());
  QSettings().setValue("state", saveState());
}

void porque::prepare_icons(void)
{
  m_ui.action_Open_PDF_Files->setIcon(QIcon(":/open-file.png"));
  m_ui.action_Quit->setIcon(QIcon(":/quit.png"));
}

void porque::restore(void)
{
  restoreGeometry(QSettings().value("geometry").toByteArray());
  restoreState(QSettings().value("state").toByteArray());
}

void porque::slot_quit(void)
{
  close();
}
