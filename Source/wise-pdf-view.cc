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
#include "wise-settings.h"

#include <QKeyEvent>
#include <QPainter>
#include <QPdfBookmarkModel>
#include <QPdfPageNavigator>
#include <QPdfPageRenderer>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QScrollBar>
#include <QShortcut>

qreal static maximum_zoom_factor = 10.0;
qreal static minimum_zoom_factor = 0.25;
qreal static zoom_constant = 2.0;

static bool numeric_sort(const QString &string_1, const QString &string_2)
{
  return QString(string_1).remove('%').toInt() <
    QString(string_2).remove('%').toInt();
}

wise_pdf_view_view::wise_pdf_view_view(QWidget *parent):QPdfView(parent)
{
}

void wise_pdf_view_view::keyPressEvent(QKeyEvent *event)
{
  QPdfView::keyPressEvent(event);

  if(event)
    {
      auto keyboard_modifiers(QGuiApplication::keyboardModifiers());

      if(keyboard_modifiers == Qt::ControlModifier)
	{
	  if(event->key() == Qt::Key_End)
	    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
	  else if(event->key() == Qt::Key_Home)
	    verticalScrollBar()->setValue(0);
	}
    }
}

wise_pdf_view::wise_pdf_view
(const QUrl &url, QWidget *parent):QWidget(parent)
{
  m_bookmark_model = new QPdfBookmarkModel(this);
  m_bookmark_model->setDocument(m_document = new QPdfDocument(this));
  m_page_renderer = new QPdfPageRenderer(this);
  m_page_renderer->setDocument(m_document);
  m_page_renderer->setRenderMode(wise_settings::render_mode());
  m_pdf_view = new wise_pdf_view_view(this);
  m_pdf_view->setDocument(m_document);
  m_pdf_view->setPageMode(QPdfView::PageMode::MultiPage);
  m_pdf_view->setZoomMode(QPdfView::ZoomMode::FitInView);
  m_ui.setupUi(this);
  connect(m_document,
	  SIGNAL(statusChanged(QPdfDocument::Status)),
	  this,
	  SLOT(slot_document_status_changed(QPdfDocument::Status)));
  connect(m_pdf_view->verticalScrollBar(),
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slot_scrolled(int)));
  connect(m_ui.contents,
	  SIGNAL(activated(const QModelIndex &)),
	  this,
	  SLOT(slot_contents_selected(const QModelIndex &)));
  connect(m_ui.first_page,
	  &QToolButton::clicked,
	  this,
	  &wise_pdf_view::slot_first_page);
  connect(m_ui.last_page,
	  &QToolButton::clicked,
	  this,
	  &wise_pdf_view::slot_last_page);
  connect(m_ui.next_page,
	  &QToolButton::clicked,
	  m_ui.page,
	  &QSpinBox::stepUp);
  connect(m_ui.page,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slot_select_page(int)));
  connect(m_ui.password,
	  &QLineEdit::returnPressed,
	  this,
	  &wise_pdf_view::slot_password_changed);
  connect(m_ui.password_accept,
	  &QToolButton::clicked,
	  this,
	  &wise_pdf_view::slot_password_changed);
  connect(m_ui.previous_page,
	  &QToolButton::clicked,
	  m_ui.page,
	  &QSpinBox::stepDown);
  connect(m_ui.print,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slot_print(void)));
  connect(m_ui.view_size,
	  SIGNAL(activated(int)),
	  this,
	  SLOT(slot_view_size_activated(int)));
  connect(m_ui.zoom_in,
	  &QToolButton::clicked,
	  this,
	  &wise_pdf_view::slot_zoom_in);
  connect(m_ui.zoom_out,
	  &QToolButton::clicked,
	  this,
	  &wise_pdf_view::slot_zoom_out);
  m_ui.contents->setModel(m_bookmark_model);
  m_ui.contents_meta_splitter->setStretchFactor(0, 1);
  m_ui.contents_meta_splitter->setStretchFactor(1, 0);
  m_ui.frame->layout()->addWidget(m_pdf_view);
  m_ui.password_frame->setVisible(false);
  m_ui.pdf_view_splitter->setStretchFactor(0, 0);
  m_ui.pdf_view_splitter->setStretchFactor(1, 1);
  m_url = url;
  new QShortcut(tr("Ctrl+0"), this, SLOT(slot_zoom_reset(void)));
  prepare();
  prepare_widget_states();
  QTimer::singleShot(10, this, SLOT(slot_load_document(void)));
}

wise_pdf_view::~wise_pdf_view()
{
}

void wise_pdf_view::find(void)
{
  m_ui.find->selectAll();
  m_ui.find->setFocus();
}

void wise_pdf_view::prepare(void)
{
  m_ui.meta->setRowCount(0);

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
      tool_button->setIconSize(QSize(25, 25));
#ifdef Q_OS_MACOS
      tool_button->setStyleSheet
	("QToolButton {border: none;}"
	 "QToolButton::menu-button {border: none;}"
	 "QToolButton::menu-indicator {image: none;}");
#endif
    }

  m_ui.contents->expandAll();
  m_ui.contents->setVisible(m_bookmark_model->rowCount() > 0);
  m_ui.meta->resizeColumnToContents(0);
  m_ui.meta->resizeColumnToContents(1);
  m_ui.page->setMaximum(m_document->pageCount());
  m_ui.page->setMinimum(1); // The document's page count may be zero.
  m_ui.page->setToolTip
    (QString("[%1, %2]").arg(m_ui.page->minimum()).arg(m_ui.page->maximum()));
}

void wise_pdf_view::prepare_view_size(void)
{
  auto const percent = static_cast<int> (100.0 * m_pdf_view->zoomFactor());
  auto const index = m_ui.view_size->findText(QString("%1%").arg(percent));

  if(index < 0)
    {
      QStringList list;

      for(int i = 0; i < m_ui.view_size->count(); i++)
	list << m_ui.view_size->itemText(i);

      list << QString("%1%").arg(percent);
      std::sort(list.begin(), list.end(), numeric_sort);
      m_ui.view_size->blockSignals(true);
      m_ui.view_size->clear();
      m_ui.view_size->addItems(list);
      m_ui.view_size->blockSignals(false);
      m_ui.view_size->setCurrentIndex
	(m_ui.view_size->findText(QString("%1%").arg(percent)));
    }
  else
    m_ui.view_size->setCurrentIndex(index);
}

void wise_pdf_view::prepare_widget_states(void)
{
  m_ui.first_page->setEnabled(1 < m_ui.page->value());
  m_ui.last_page->setEnabled(m_ui.page->maximum() > m_ui.page->value());
  m_ui.next_page->setEnabled(m_ui.page->maximum() > m_ui.page->value());
  m_ui.previous_page->setEnabled(1 < m_ui.page->value());
  m_ui.zoom_in->setEnabled
    (m_pdf_view->zoomFactor() < 1 ||
     m_pdf_view->zoomFactor() < maximum_zoom_factor);
  m_ui.zoom_out->setEnabled
    (m_pdf_view->zoomFactor() > 1 ||
     m_pdf_view->zoomFactor() > minimum_zoom_factor);
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
  m_ui.page->blockSignals(true);
  m_ui.page->setValue(page + 1);
  m_ui.page->blockSignals(false);
}

void wise_pdf_view::slot_document_status_changed(QPdfDocument::Status status)
{
  if(status == QPdfDocument::Status::Ready)
    {
      prepare();
      prepare_widget_states();
    }
}

void wise_pdf_view::slot_first_page(void)
{
  m_ui.page->setValue(m_ui.page->minimum());
}

void wise_pdf_view::slot_last_page(void)
{
  m_ui.page->setValue(m_ui.page->maximum());
}

void wise_pdf_view::slot_load_document(void)
{
  m_ui.password_frame->setVisible
    (m_document->load(m_url.path()) == QPdfDocument::Error::IncorrectPassword);
}

void wise_pdf_view::slot_password_changed(void)
{
  m_document->setPassword(m_ui.password->text());
  m_ui.password_frame->setVisible
    (m_document->load(m_url.path()) == QPdfDocument::Error::IncorrectPassword);
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

      painter.drawImage
	(rect, m_document->render(i, size, wise_settings::render_options()));
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

void wise_pdf_view::slot_scrolled(int value)
{
  Q_UNUSED(value);
  m_ui.page->setValue(m_pdf_view->pageNavigator()->currentPage() + 1);
  prepare_widget_states();
}

void wise_pdf_view::slot_select_page(int value)
{
  m_pdf_view->pageNavigator()->jump(value - 1, QPointF());
  prepare_widget_states();
}

void wise_pdf_view::slot_view_size_activated(int index)
{
  auto const zoom_factor = m_ui.view_size->itemText(index).remove('%').toInt();

  if(zoom_factor != 0)
    {
      m_pdf_view->setZoomFactor(zoom_factor / 100.0);
      m_pdf_view->setZoomMode(QPdfView::ZoomMode::Custom);
    }
  else
    {
      m_pdf_view->setZoomFactor(1.0);

      if(m_ui.view_size->currentText() == tr("Width-Fit"))
	m_pdf_view->setZoomMode(QPdfView::ZoomMode::FitToWidth);
      else
	m_pdf_view->setZoomMode(QPdfView::ZoomMode::FitInView);
    }

  prepare_widget_states();
}

void wise_pdf_view::slot_zoom_in(void)
{
  m_pdf_view->setZoomFactor(m_pdf_view->zoomFactor() * zoom_constant);
  m_pdf_view->setZoomMode(QPdfView::ZoomMode::Custom);
  m_ui.zoom_out->setEnabled(true);
  prepare_view_size();
  prepare_widget_states();
}

void wise_pdf_view::slot_zoom_out(void)
{
  m_pdf_view->setZoomFactor(m_pdf_view->zoomFactor() / zoom_constant);
  m_pdf_view->setZoomMode(QPdfView::ZoomMode::Custom);
  m_ui.zoom_in->setEnabled(true);
  prepare_view_size();
  prepare_widget_states();
}

void wise_pdf_view::slot_zoom_reset(void)
{
  m_pdf_view->setZoomFactor(1.0);
  m_pdf_view->setZoomMode(QPdfView::ZoomMode::Custom);
  m_ui.zoom_in->setEnabled(true);
  m_ui.zoom_out->setEnabled(true);
  prepare_view_size();
  prepare_widget_states();
}
