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
#include "porque-pdf-view.h"

#include <QActionGroup>
#include <QDir>
#include <QFileDialog>
#include <QSettings>

QString porque::PORQUE_VERSION_STRING = "2025.04.01";

porque::porque(void):QMainWindow(nullptr)
{
  m_ui.setupUi(this);
  m_ui.menu_Pages->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.tab->setDocumentMode(false);
  m_ui.tab->setMovable(true);
  m_ui.tab->setTabsClosable(true);
  m_ui.tool_bar->addAction(m_ui.action_Open_PDF_Files);
  m_ui.tool_bar->addAction(m_ui.action_Settings);
  m_ui.tool_bar->setIconSize(QSize(50, 50));
  connect(m_ui.action_Open_PDF_Files,
	  &QAction::triggered,
	  this,
	  &porque::slot_open_pdf_files);
  connect(m_ui.action_Quit,
	  &QAction::triggered,
	  this,
	  &porque::slot_quit);
  connect(m_ui.action_Screen_Mode,
	  &QAction::triggered,
	  this,
	  &porque::slot_screen_mode);
  connect(m_ui.menu_Pages,
	  &QMenu::aboutToShow,
	  this,
	  &porque::slot_about_to_show_pages_menu);
  connect(m_ui.tab,
	  SIGNAL(currentChanged(int)),
	  this,
	  SLOT(slot_page_selected(int)));
  connect(m_ui.tab,
	  SIGNAL(tabCloseRequested(int)),
	  this,
	  SLOT(slot_close_tab(int)));
  connect(m_ui.tab->tab_bar(),
	  SIGNAL(tabMoved(int, int)),
	  this,
	  SLOT(slot_page_moved(int, int)),
	  Qt::QueuedConnection);
  process_terminal();
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

void porque::add_pdf_page(const QString &file_name)
{
  if(file_name.isEmpty())
    return;

  auto page = new porque_pdf_view(QUrl::fromLocalFile(file_name), this);

  m_ui.tab->setTabToolTip
    (m_ui.tab->addTab(page, QIcon(":/porque.png"), file_name), file_name);
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
  m_ui.action_Screen_Mode->setIcon
    (isFullScreen() ?
     QIcon(":/normal-screen.png") :
     QIcon(":/full-screen.png"));
  m_ui.action_Settings->setIcon(QIcon(":/settings.png"));
}

void porque::prepare_pages_menu(void)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  m_ui.menu_Pages->clear();

  auto group = m_ui.menu_Pages->findChild<QActionGroup *> ();

  if(!group)
    group = new QActionGroup(m_ui.menu_Pages);

  for(int i = 0; i < m_ui.tab->count(); i++)
    {
      auto action = new QAction(m_ui.tab->tabText(i), this);

      action->setCheckable(true);
      action->setChecked(i == m_ui.tab->currentIndex());
      action->setData(i);
      connect(action,
	      &QAction::triggered,
	      this,
	      &porque::slot_select_page);
      group->addAction(action);
      m_ui.menu_Pages->addAction(action);
    }

  if(group->actions().isEmpty())
    group->deleteLater();

  if(m_ui.menu_Pages->actions().isEmpty())
    m_ui.menu_Pages->addAction(tr("Empty"))->setEnabled(false);

  QApplication::restoreOverrideCursor();
}

void porque::process_terminal(void)
{
  QApplication::processEvents();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto const list(QApplication::arguments());

  for(int i = 1; i < list.size(); i++)
    if(list.at(i).endsWith(".pdf", Qt::CaseInsensitive))
      add_pdf_page(list.at(i));

  QApplication::restoreOverrideCursor();
}

void porque::restore(void)
{
  restoreGeometry(QSettings().value("geometry").toByteArray());
  restoreState(QSettings().value("state").toByteArray());
  isFullScreen() ?
    m_ui.action_Screen_Mode->setText(tr("Normal Screen")) :
    m_ui.action_Screen_Mode->setText(tr("Full Screen"));
  prepare_icons();
}

void porque::slot_about_to_show_pages_menu(void)
{
  prepare_pages_menu();
}

void porque::slot_close_tab(int index)
{
  m_ui.tab->widget(index) ? m_ui.tab->widget(index)->deleteLater() : (void) 0;
  m_ui.tab->removeTab(index);
}

void porque::slot_open_pdf_files(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(porque::home_path());
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setNameFilters(QStringList() << tr("PDF Files (*.pdf)"));
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Porque: Open PDF Files"));
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  QApplication::processEvents();

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      auto const list(dialog.selectedFiles());

      for(int i = 0; i < list.size(); i++)
	add_pdf_page(list.at(i));

      QApplication::restoreOverrideCursor();
    }
  else
    QApplication::processEvents();
}

void porque::slot_page_moved(int from, int to)
{
  Q_UNUSED(from);
  Q_UNUSED(to);
  prepare_pages_menu();
}

void porque::slot_page_selected(int index)
{
  if(index >= 0)
    setWindowTitle(tr("Porque (%1)").arg(m_ui.tab->tabText(index)));
  else
    setWindowTitle(tr("Porque"));
}

void porque::slot_quit(void)
{
  close();
}

void porque::slot_screen_mode(void)
{
  if(isFullScreen())
    {
      m_ui.action_Screen_Mode->setText(tr("Full Screen"));
      showNormal();
    }
  else
    {
      m_ui.action_Screen_Mode->setText(tr("Normal Screen"));
      showFullScreen();
    }

  prepare_icons();
}

void porque::slot_select_page(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action)
    m_ui.tab->setCurrentIndex(action->data().toInt());
}
