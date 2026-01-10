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

#ifndef wise_h
#define wise_h

#include "ui_wise.h"

#include <QTimer>

#define WISE_VERSION_STRING "2026.01.10"

class wise_settings;
typedef QVector<QPair<QImage, QString> > QVectorQPairQImageQString;

class wise: public QMainWindow
{
  Q_OBJECT

 public:
  wise(void);
  ~wise();
  QString static home_path(void);

 private:
  QTimer m_timer;
  Ui_wise m_ui;
  wise_settings *m_settings;
  void add_pdf_page(const QString &file_name);
  void closeEvent(QCloseEvent *event);
  void prepare_icons(void);
  void prepare_pages_menu(void);
  void process_terminal(void);
  void restore(void);

 private slots:
  void slot_about(void);
  void slot_about_to_show_pages_menu(void);
  void slot_close_tab(int index);
  void slot_close_tab(void);
  void slot_find(void);
  void slot_forget_recent_file(const QString &file_name);
  void slot_open_pdf_files(const QString &file_name);
  void slot_open_pdf_files(void);
  void slot_page_moved(int from, int to);
  void slot_page_selected(int index);
  void slot_print(void);
  void slot_quit(void);
  void slot_process_events(void);
  void slot_recent_files(void);
  void slot_release_notes(void);
  void slot_save_recent_file(const QImage &image, const QUrl &url);
  void slot_screen_mode(void);
  void slot_select_page(void);
  void slot_settings(void);
  void slot_view_tab_bar(void);
  void slot_view_tool_bar(void);

 signals:
  void recent_file_saved(void);
};

#endif
