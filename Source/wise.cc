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
#include "wise-recent-files-view.h"
#include "wise-settings.h"

#include <QActionGroup>
#include <QBuffer>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextBrowser>

QString wise::WISE_VERSION_STRING = "2025.04.01";

wise::wise(void):QMainWindow(nullptr)
{
  m_settings = new wise_settings(this);
  m_settings->setVisible(false);
#ifdef Q_OS_ANDROID
  m_timer.start(50);
#endif
  m_ui.setupUi(this);
  m_ui.action_Close_Page->setEnabled(false);
  m_ui.action_Print->setEnabled(false);
#ifdef Q_OS_ANDROID
  m_ui.action_Print->setVisible(false);
#endif
  m_ui.menu_Pages->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.tab->setMovable(true);
  m_ui.tab->setTabsClosable(true);
  m_ui.tool_bar->addAction(m_ui.action_Open_PDF_Files);
  m_ui.tool_bar->addAction(m_ui.action_Recent_Files);
  m_ui.tool_bar->addAction(m_ui.action_Release_Notes);
  m_ui.tool_bar->addAction(m_ui.action_Settings);
  m_ui.tool_bar->setIconSize(QSize(25, 25));
  new QShortcut(tr("Ctrl+F"), this, SLOT(slot_find(void)));
  connect(&m_timer,
	  &QTimer::timeout,
	  this,
	  &wise::slot_process_events);
  connect(m_ui.action_About,
	  &QAction::triggered,
	  this,
	  &wise::slot_about);
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
  connect(m_ui.action_Recent_Files,
	  &QAction::triggered,
	  this,
	  &wise::slot_recent_files);
  connect(m_ui.action_Release_Notes,
	  &QAction::triggered,
	  this,
	  &wise::slot_release_notes);
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

  foreach(auto tool_button, findChildren<QToolButton *> ())
    {
      tool_button->setArrowType(Qt::NoArrow);
      tool_button->setAutoRaise(true);
      tool_button->setIconSize(QSize(25, 25));
#if defined(Q_OS_ANDROID) || defined(Q_OS_MACOS)
      tool_button->setStyleSheet
	("QToolButton {border: none;}"
	 "QToolButton::menu-button {border: none;}"
	 "QToolButton::menu-indicator {image: none;}");
#endif
    }

#ifdef Q_OS_ANDROID
  m_ui.action_Screen_Mode->setEnabled(false);
#endif
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

  QString const connection_name("add_pdf_page");

  {
    auto db(QSqlDatabase::addDatabase("QSQLITE", connection_name));

    db.setDatabaseName
      (home_path() + QDir::separator() + "wise-recent-files.db");

    if(db.open())
      {
	QSqlQuery query(db);

	query.exec("CREATE TABLE IF NOT EXISTS wise_recent_files "
		   "(file_name TEXT NOT NULL PRIMARY KEY, image TEXT)");
	query.prepare
	  ("INSERT INTO wise_recent_files (file_name) VALUES (?)");
	query.addBindValue(QFileInfo(file_name).absoluteFilePath());
	query.exec();
      }

    db.close();
  }

  QSqlDatabase::removeDatabase(connection_name);
  m_ui.action_Close_Page->setEnabled(true);

  auto page = new wise_pdf_view(QUrl::fromLocalFile(file_name), this);

  connect(page,
	  SIGNAL(save_recent_file(const QImage &, const QUrl &)),
	  this,
	  SLOT(slot_save_recent_file(const QImage &, const QUrl &)));
  m_ui.tab->setTabToolTip
    (m_ui.tab->addTab(page,
		      QIcon(":/wise.png"),
		      QFileInfo(file_name).fileName()), file_name);
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
  m_ui.action_Open_PDF_Files->setIcon(QIcon(":/open-file.svg"));
  m_ui.action_Print->setIcon(QIcon(":/print.svg"));
  m_ui.action_Quit->setIcon(QIcon(":/quit.png"));
  m_ui.action_Release_Notes->setIcon(QIcon(":/release-notes.png"));
  m_ui.action_Screen_Mode->setIcon
    (isFullScreen() ?
     QIcon(":/normal-screen.svg") :
     QIcon(":/full-screen.svg"));
  m_ui.action_Settings->setIcon(QIcon(":/settings.svg"));
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
    m_ui.action_Screen_Mode->setText(tr("&Normal Screen")) :
    m_ui.action_Screen_Mode->setText(tr("&Full Screen"));
  prepare_icons();
}

void wise::slot_about(void)
{
  auto message_box = findChild<QMessageBox *> ("about");

  if(!message_box)
    {
      message_box = new QMessageBox(this);
      message_box->setFont(QApplication::font());
      message_box->setIconPixmap
	(QPixmap(":/wise.png").scaled(QSize(128, 128),
				      Qt::KeepAspectRatio,
				      Qt::SmoothTransformation));
      message_box->setModal(false);
      message_box->setObjectName("about");
      message_box->setStandardButtons(QMessageBox::Close);
      message_box->setText
	(tr("<html><b>Wise</b> Version %1<br>"
	    "Architecture %2.<br>"
	    "Compiled on %3, %4.<br>"
	    "Made with love by textbrower.<br>"
	    "<b>Wonderfully-interactive and simple education program "
	    "for reading portable documents.</b><br>"
	    "Please visit "
	    "<a href=\"https://textbrowser.github.io/wise\">"
	    "https://textbrowser.github.io/wise</a> "
	    "for project information.</html>").
	 arg(WISE_VERSION_STRING).
	 arg(QSysInfo::currentCpuArchitecture()).
	 arg(__DATE__).
	 arg(__TIME__));
      message_box->setTextFormat(Qt::RichText);
      message_box->setWindowIcon(windowIcon());
      message_box->setWindowModality(Qt::NonModal);
      message_box->setWindowTitle(tr("Wise: About"));
      message_box->button(QMessageBox::Close)->setShortcut(tr("Ctrl+W"));
    }

  message_box->showNormal();
  message_box->activateWindow();
  message_box->raise();
}

void wise::slot_about_to_show_pages_menu(void)
{
  prepare_pages_menu();
}

void wise::slot_close_tab(int index)
{
  m_ui.action_Close_Page->setEnabled(m_ui.tab->count() > 1);

  if(qobject_cast<wise_pdf_view *> (m_ui.tab->widget(index)))
    m_ui.tab->widget(index) ?
      m_ui.tab->widget(index)->deleteLater() : (void) 0;

  m_ui.tab->removeTab(index);
}

void wise::slot_close_tab(void)
{
  slot_close_tab(m_ui.tab->currentIndex());
}

void wise::slot_find(void)
{
  auto page = qobject_cast<wise_pdf_view *> (m_ui.tab->currentWidget());

  if(page)
    page->find();
}

void wise::slot_open_pdf_files(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(wise::home_path());
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setLabelText(QFileDialog::Accept, tr("Select"));
  dialog.setNameFilters(QStringList() << tr("PDF Files (*.pdf)"));
#ifndef Q_OS_ANDROID
  dialog.setOption(QFileDialog::DontUseNativeDialog);
#endif
  dialog.setWindowIcon(windowIcon());
  dialog.setWindowTitle(tr("Wise: Open PDF Files"));
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
  m_ui.action_Print->setEnabled
    (qobject_cast<wise_pdf_view *> (m_ui.tab->widget(index)));

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

void wise::slot_process_events(void)
{
  repaint();
  QApplication::processEvents();
}

void wise::slot_quit(void)
{
  close();
}

void wise::slot_recent_files(void)
{
  auto view = findChild<wise_recent_files_view *> ();

  if(m_ui.tab->indexOf(view) >= 0)
    {
      m_ui.tab->setCurrentIndex(m_ui.tab->indexOf(view));
      return;
    }

  if(view == nullptr)
    {
      view = new wise_recent_files_view(this);
      connect(this,
	      &wise::recent_file_saved,
	      view,
	      &wise_recent_files_view::slot_gather);
    }

  m_ui.action_Close_Page->setEnabled(true);
  m_ui.tab->addTab(view, QIcon(":/recent.svg"), tr("Wise Recent Files"));
  m_ui.tab->setCurrentIndex(m_ui.tab->indexOf(view));
  m_ui.tab->setTabToolTip(m_ui.tab->indexOf(view), tr("Wise Recent Files"));
}

void wise::slot_release_notes(void)
{
  auto frame = findChild<QFrame *> ("release-notes");

  if(m_ui.tab->indexOf(frame) >= 0)
    {
      m_ui.tab->setCurrentIndex(m_ui.tab->indexOf(frame));
      return;
    }

  if(frame == nullptr)
    {
      frame = new QFrame(this);
      frame->setLayout(new QHBoxLayout());
      frame->setObjectName("release-notes");

      auto text_browser = new QTextBrowser(this);

      text_browser->setSource(QUrl::fromUserInput("qrc:/ReleaseNotes.html"));
      frame->layout()->addWidget(text_browser);
    }

  m_ui.action_Close_Page->setEnabled(true);
  m_ui.tab->addTab
    (frame, QIcon(":/release-notes.png"), tr("Wise Release Notes"));
  m_ui.tab->setCurrentIndex(m_ui.tab->indexOf(frame));
  m_ui.tab->setTabToolTip(m_ui.tab->indexOf(frame), tr("Wise Release Notes"));
}

void wise::slot_save_recent_file(const QImage &image, const QUrl &url)
{
  QString const connection_name("slot_save_recent_file");

  {
    auto db(QSqlDatabase::addDatabase("QSQLITE", connection_name));

    db.setDatabaseName
      (home_path() + QDir::separator() + "wise-recent-files.db");

    if(db.open())
      {
	QBuffer buffer;
	QByteArray bytes;
	QSqlQuery query(db);

	buffer.setBuffer(&bytes);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "PNG");
	query.prepare
	  ("INSERT OR REPLACE INTO wise_recent_files "
	   "(file_name, image) VALUES (?, ?)");
	query.addBindValue(QFileInfo(url.path()).absoluteFilePath());
	query.addBindValue(bytes.toBase64());
	query.exec();
      }

    db.close();
  }

  QSqlDatabase::removeDatabase(connection_name);
  emit recent_file_saved();
}

void wise::slot_screen_mode(void)
{
  if(isFullScreen())
    {
      m_ui.action_Screen_Mode->setText(tr("&Full Screen"));
      showNormal();
    }
  else
    {
      m_ui.action_Screen_Mode->setText(tr("&Normal Screen"));
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
