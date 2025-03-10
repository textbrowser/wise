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

#ifndef wise_pdf_view_h
#define wise_pdf_view_h

#include "ui_wise-pdf-view.h"

#include <QPdfDocument>
#include <QPdfView>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QUrl>

class QPdfBookmarkModel;
class QPdfPageRenderer;
class QPdfSearchModel;
class QPrinter;

class wise_pdf_view_search_view_item_delegate: public QStyledItemDelegate
{
  Q_OBJECT

 public:
  wise_pdf_view_search_view_item_delegate(QWidget *parent);

 private:
  void paint(QPainter *painter,
	     const QStyleOptionViewItem &option,
	     const QModelIndex &index) const;
};

class wise_pdf_view_view: public QPdfView
{
  Q_OBJECT

 public:
  wise_pdf_view_view(QWidget *parent);

  ~wise_pdf_view_view()
  {
  }

 private:
  void keyPressEvent(QKeyEvent *event);
};

class wise_pdf_view: public QWidget
{
  Q_OBJECT

 public:
  wise_pdf_view(const QUrl &url, QWidget *parent);
  ~wise_pdf_view();
  void find(void);
  void print(void);
  void set_page_mode(const QPdfView::PageMode page_mode);

 private:
  QPdfBookmarkModel *m_bookmark_model;
  QPdfDocument *m_document;
  QPdfPageRenderer *m_page_renderer;
  QPdfSearchModel *m_search_model;
  QUrl m_url;
  Ui_wise_pdf_view m_ui;
  wise_pdf_view_view *m_pdf_view;
  void prepare(void);
  void prepare_view_size(void);
  void prepare_widget_states(void);

 private slots:
  void slot_contents_selected(const QModelIndex &index);
  void slot_document_status_changed(QPdfDocument::Status status);
  void slot_first_page(void);
  void slot_last_page(void);
  void slot_load_document(void);
  void slot_password_changed(void);
  void slot_print(QPrinter *printer);
  void slot_print(void);
  void slot_scrolled(int value);
  void slot_search(void);
  void slot_search_count_changed(void);
  void slot_search_view_selected
    (const QModelIndex &current, const QModelIndex &previous);
  void slot_select_page(int value);
  void slot_view_size_activated(int index);
  void slot_zoom_in(void);
  void slot_zoom_out(void);
  void slot_zoom_reset(void);
};

#endif
