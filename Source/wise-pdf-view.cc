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

#include "wise-pdf-view.h"

#include <QPainter>
#include <QPdfBookmarkModel>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QShortcut>

wise_pdf_view::wise_pdf_view
(const QUrl &url, QWidget *parent):QWidget(parent)
{
  m_bookmark_model = new QPdfBookmarkModel(this);
  m_bookmark_model->setDocument(m_document = new QPdfDocument(this));
  m_document->load(url.path());
  m_pdf_view = new QPdfView(this);
  m_pdf_view->setDocument(m_document);
  m_pdf_view->setPageMode(QPdfView::PageMode::MultiPage);
  m_ui.setupUi(this);
  connect(m_ui.contents,
	  SIGNAL(activated(const QModelIndex &)),
	  this,
	  SLOT(slot_contents_selected(const QModelIndex &)));
  connect(m_ui.print,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slot_print(void)));
  m_ui.contents->setModel(m_bookmark_model);
  m_ui.contents_meta_splitter->setStretchFactor(0, 1);
  m_ui.contents_meta_splitter->setStretchFactor(1, 0);
  m_ui.pdf_view_splitter->setStretchFactor(0, 0);
  m_ui.pdf_view_splitter->setStretchFactor(1, 1);
  m_url = url;
  prepare();
}

wise_pdf_view::~wise_pdf_view()
{
}

void wise_pdf_view::prepare(void)
{
  QMap<QString, QPdfDocument::MetaDataField> meta;

  meta[tr("Author")] = QPdfDocument::MetaDataField::Author;
  meta[tr("Creation Date")] = QPdfDocument::MetaDataField::CreationDate;
  meta[tr("Creator")] = QPdfDocument::MetaDataField::Creator;
  meta[tr("Keywords")] = QPdfDocument::MetaDataField::Keywords;
  meta[tr("Modification Date")] =
    QPdfDocument::MetaDataField::ModificationDate;
  meta[tr("Producer")] = QPdfDocument::MetaDataField::Producer;
  meta[tr("Subject")] = QPdfDocument::MetaDataField::Subject;
  meta[tr("Title")] = QPdfDocument::MetaDataField::Title;
  m_ui.meta->setRowCount(meta.size());

  QMap<QString, QPdfDocument::MetaDataField>::const_iterator it;

  for(it = meta.constBegin(); it != meta.constEnd(); ++it)
    {
      auto const row = static_cast<int> (std::distance(meta.constBegin(), it));
      auto item1 = new QTableWidgetItem(it.key());
      auto item2 = new QTableWidgetItem
	(m_document->metaData(it.value()).toString());

      item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      item2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.meta->setItem(row, 0, item1);
      m_ui.meta->setItem(row, 1, item2);
    }

  foreach(auto tool_button, findChildren<QToolButton *> ())
    {
      tool_button->setArrowType(Qt::NoArrow);
      tool_button->setAutoRaise(true);
#ifdef Q_OS_MACOS
      tool_button->setStyleSheet
	("QToolButton {border: none;}"
	 "QToolButton::menu-button {border: none;}"
	 "QToolButton::menu-indicator {image: none;}");
#endif
    }

  m_ui.contents->expandAll();
  m_ui.contents->setVisible(m_bookmark_model->rowCount() > 0);
  m_ui.frame->layout()->addWidget(m_pdf_view);
  m_ui.meta->resizeColumnToContents(0);
  m_ui.meta->resizeColumnToContents(1);
}

void wise_pdf_view::print(void)
{
  slot_print();
}

void wise_pdf_view::set_page_mode(const QPdfView::PageMode page_mode)
{
  m_pdf_view->setPageMode(page_mode);
}

void wise_pdf_view::slot_contents_selected(const QModelIndex &index)
{
  if(!index.isValid())
    return;

  auto const page = index.data
    (static_cast<int> (QPdfBookmarkModel::Role::Page)).toInt();
  auto const zoom_level = index.data
    (static_cast<int> (QPdfBookmarkModel::Role::Level)).toReal();

  m_pdf_view->pageNavigator()->jump(page, QPointF(), zoom_level);
}

void wise_pdf_view::slot_print(QPrinter *printer)
{
  if(!printer)
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  QPainter painter(printer);

  painter.setRenderHints
    (QPainter::Antialiasing |
     QPainter::LosslessImageRendering |
     QPainter::SmoothPixmapTransform |
     QPainter::TextAntialiasing);

  QList<int> pages;
  auto const dpi_x = printer->physicalDpiX();
  auto const dpi_y = printer->physicalDpiY();
  auto const page_ranges(printer->pageRanges());
  auto const page_rect(printer->pageRect(QPrinter::Unit::Inch));
  auto const rect
    (QRectF(dpi_x * page_rect.x(),
	    dpi_y * page_rect.y(),
	    dpi_x * page_rect.width(),
	    dpi_y * page_rect.height()));

  if(page_ranges.isEmpty() == false)
    {
      for(int i = 1; i <= m_document->pageCount(); i++)
	if(page_ranges.contains(i))
	  pages.append(i - 1);
    }
  else
    for(int i = 0; i < m_document->pageCount(); i++)
      pages.append(i);

  foreach(auto const i, pages)
    {
      if(i != pages.at(0))
	printer->newPage();

      auto const size(2 * m_document->pagePointSize(i).toSize());

      painter.drawImage(rect, m_document->render(i, size));
    }

  QApplication::restoreOverrideCursor();
}

void wise_pdf_view::slot_print(void)
{
  QPrinter printer(QPrinter::HighResolution);

  printer.setColorMode(QPrinter::Color);
  printer.setPageMargins(QMarginsF(0.25, 0.25, 0.25, 0.25));
  printer.setPageSize(QPageSize(QPageSize::PageSizeId::Letter));

  QScopedPointer<QPrintPreviewDialog> dialog
    (new QPrintPreviewDialog(&printer, this));

  connect(dialog.data(),
	  SIGNAL(paintRequested(QPrinter *)),
	  this,
	  SLOT(slot_print(QPrinter *)));
  dialog->setWindowTitle("Wise: Print Preview");
#ifdef Q_OS_ANDROID
  dialog->showMaximized();
#endif
  new QShortcut(tr("Ctrl+W"), dialog.data(), SLOT(close(void)));
  new QShortcut(tr("Esc"), dialog.data(), SLOT(close(void)));
  QApplication::processEvents();
  dialog->exec();
  QApplication::processEvents();
}
