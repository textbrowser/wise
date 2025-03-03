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
#include "wise-pdf-view.h"
#include "wise-settings.h"

#include <QActionGroup>
#include <QDir>
#include <QFileDialog>
#include <QSettings>

QString wise::WISE_VERSION_STRING = "2025.04.01";

wise::wise(void):QMainWindow(nullptr)
{
  m_settings = new wise_settings(this);
  m_settings->setVisible(false);
  m_ui.setupUi(this);
  m_ui.action_Close_Page->setEnabled(false);
  m_ui.menu_Pages->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.tab->setMovable(true);
  m_ui.tab->setTabsClosable(true);
  m_ui.tool_bar->addAction(m_ui.action_Open_PDF_Files);
  m_ui.tool_bar->addAction(m_ui.action_Settings);
  m_ui.tool_bar->setIconSize(QSize(50, 50));
  connect(m_ui.action_Close_Page,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slot_close_tab(void)));
  connect(m_ui.action_Open_PDF_Files,
	  &QAction::triggered,
	  this,
	  &wise::slot_open_pdf_files);
  connect(m_ui.action_Print,
	  &QAction::triggered,
	  this,
	  &wise::slot_print);
  connect(m_ui.action_Quit,
	  &QAction::triggered,
	  this,
	  &wise::slot_quit);
  connect(m_ui.action_Settings,
	  &QAction::triggered,
	  this,
	  &wise::slot_settings);
  connect(m_ui.action_Screen_Mode,
	  &QAction::triggered,
	  this,
	  &wise::slot_screen_mode);
  connect(m_ui.menu_Pages,
	  &QMenu::aboutToShow,
	  this,
	  &wise::slot_about_to_show_pages_menu);
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

wise::~wise()
{
}

QString wise::home_path(void)
{
#ifdef Q_OS_WINDOWS
  return QDir::currentPath() + QDir::separator() + ".wise";
#else
  return QDir::homePath() + QDir::separator() + ".wise";
#endif
}

void wise::add_pdf_page(const QString &file_name)
{
  if(file_name.isEmpty())
    return;

  m_ui.action_Close_Page->setEnabled(true);

  auto page = new wise_pdf_view(QUrl::fromLocalFile(file_name), this);

  m_ui.tab->setTabToolTip
    (m_ui.tab->addTab(page, QIcon(":/wise.png"), file_name), file_name);
  m_ui.tab->setCurrentIndex(m_ui.tab->indexOf(page));
  page->set_page_mode(m_settings->page_mode());
}

void wise::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
  QSettings().setValue("geometry", saveGeometry());
  QSettings().setValue("state", saveState());
}

void wise::prepare_icons(void)
{
  m_ui.action_Open_PDF_Files->setIcon(QIcon(":/open-file.png"));
  m_ui.action_Print->setIcon(QIcon(":/print.png"));
  m_ui.action_Quit->setIcon(QIcon(":/quit.png"));
  m_ui.action_Screen_Mode->setIcon
    (isFullScreen() ?
     QIcon(":/normal-screen.png") :
     QIcon(":/full-screen.png"));
  m_ui.action_Settings->setIcon(QIcon(":/settings.png"));
}

void wise::prepare_pages_menu(void)
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
	      &wise::slot_select_page);
      group->addAction(action);
      m_ui.menu_Pages->addAction(action);
    }

  if(group->actions().isEmpty())
    group->deleteLater();

  if(m_ui.menu_Pages->actions().isEmpty())
    m_ui.menu_Pages->addAction(tr("Empty"))->setEnabled(false);

  QApplication::restoreOverrideCursor();
}

void wise::process_terminal(void)
{
  QApplication::processEvents();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto const list(QApplication::arguments());

  for(int i = 1; i < list.size(); i++)
    if(list.at(i).endsWith(".pdf", Qt::CaseInsensitive))
      add_pdf_page(list.at(i));

  QApplication::restoreOverrideCursor();
}

void wise::restore(void)
{
  restoreGeometry(QSettings().value("geometry").toByteArray());
  restoreState(QSettings().value("state").toByteArray());
  isFullScreen() ?
    m_ui.action_Screen_Mode->setText(tr("Normal Screen")) :
    m_ui.action_Screen_Mode->setText(tr("Full Screen"));
  prepare_icons();
}

void wise::slot_about_to_show_pages_menu(void)
{
  prepare_pages_menu();
}

void wise::slot_close_tab(int index)
{
  m_ui.action_Close_Page->setEnabled(m_ui.tab->count() > 1);

  if(m_settings != m_ui.tab->widget(index))
    m_ui.tab->widget(index) ?
      m_ui.tab->widget(index)->deleteLater() : (void) 0;

  m_ui.tab->removeTab(index);
}

void wise::slot_close_tab(void)
{
  slot_close_tab(m_ui.tab->currentIndex());
}

void wise::slot_open_pdf_files(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(wise::home_path());
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setNameFilters(QStringList() << tr("PDF Files (*.pdf)"));
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Wise: Open PDF Files"));
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

void wise::slot_page_moved(int from, int to)
{
  Q_UNUSED(from);
  Q_UNUSED(to);
  prepare_pages_menu();
}

void wise::slot_page_selected(int index)
{
  if(index >= 0)
    setWindowTitle(tr("Wise (%1)").arg(m_ui.tab->tabText(index)));
  else
    setWindowTitle(tr("Wise"));
}

void wise::slot_print(void)
{
  auto page = qobject_cast<wise_pdf_view *> (m_ui.tab->currentWidget());

  if(page)
    page->print();
}

void wise::slot_quit(void)
{
  close();
}

void wise::slot_screen_mode(void)
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

void wise::slot_select_page(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action)
    m_ui.tab->setCurrentIndex(action->data().toInt());
}

void wise::slot_settings(void)
{
  if(m_ui.tab->indexOf(m_settings) >= 0)
    {
      m_ui.tab->setCurrentIndex(m_ui.tab->indexOf(m_settings));
      return;
    }

  m_ui.action_Close_Page->setEnabled(true);
  m_ui.tab->addTab(m_settings, QIcon(":/settings.png"), tr("Wise Settings"));
  m_ui.tab->setCurrentIndex(m_ui.tab->indexOf(m_settings));
  m_ui.tab->setTabToolTip
    (m_ui.tab->indexOf(m_settings), tr("Wise Settings"));
}
