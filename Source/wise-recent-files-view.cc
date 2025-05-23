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

#include <QDir>
#include <QFileInfo>
#include <QKeyEvent>
#include <QScrollBar>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtConcurrent>

#include "wise-recent-files-view.h"

wise_recent_files_view::wise_recent_files_view(QWidget *parent):
  QGraphicsView(parent)
{
  connect(this,
	  SIGNAL(gathered(const QVectorQPairQImageQString &)),
	  this,
	  SLOT(slot_populate(const QVectorQPairQImageQString &)));
  m_menu_action = new QAction(QIcon(":/recent.svg"), tr("Recent Files"), this);
  setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  setCacheMode(QGraphicsView::CacheNone);
  setDragMode(QGraphicsView::NoDrag);
  setRenderHints(QPainter::Antialiasing |
		 QPainter::LosslessImageRendering |
		 QPainter::SmoothPixmapTransform |
		 QPainter::TextAntialiasing);
  setRubberBandSelectionMode(Qt::IntersectsItemShape);
  setScene(new QGraphicsScene(this));
  setStyleSheet("QGraphicsView {border: none;}");
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#ifdef Q_OS_MACOS
  // qt.pointer.dispatch: skipping QEventPoint()

  viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
#endif
  slot_gather();
}

QAction *wise_recent_files_view::menu_action(void) const
{
  return m_menu_action;
}

void wise_recent_files_view::enterEvent(QEnterEvent *event)
{
  QGraphicsView::enterEvent(event);
  setFocus();
}

void wise_recent_files_view::gather(void)
{
  QString connection_name("gather");
  QVectorQPairQImageQString vector;

  {
    auto db(QSqlDatabase::addDatabase("QSQLITE", connection_name));

    db.setDatabaseName
      (wise::home_path() + QDir::separator() + "wise-recent-files.db");

    if(db.open())
      {
	QSqlQuery query(db);

	query.setForwardOnly(true);

	if(query.exec("SELECT file_name, image FROM wise_recent_files "
		      "ORDER BY 1"))
	  while(m_gather_future.isCanceled() == false && query.next())
	    {
	      QFileInfo const file_info(query.value(0).toString());
	      QImage image;

	      if(image.loadFromData(QByteArray::
				    fromBase64(query.value(1).toByteArray()),
				    "PNG"))
		vector << QPair<QImage, QString>
		  (image, file_info.absoluteFilePath());
	    }
      }

    db.close();
  }

  QSqlDatabase::removeDatabase(connection_name);
  emit gathered(vector);
}

void wise_recent_files_view::keyPressEvent(QKeyEvent *event)
{
  QGraphicsView::keyPressEvent(event);

  if(event)
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
      emit open_file();
}

void wise_recent_files_view::slot_gather(void)
{
  if(m_gather_future.isFinished())
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    m_gather_future = QtConcurrent::run(this, &wise_recent_files_view::gather);
#else
    m_gather_future = QtConcurrent::run(&wise_recent_files_view::gather, this);
#endif
}

void wise_recent_files_view::slot_populate
(const QVectorQPairQImageQString &vector)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  auto const h_value = horizontalScrollBar()->value();
  auto const v_value = verticalScrollBar()->value();

  resetTransform();
  scene()->clear();
  setSceneRect(0.0, 0.0, 1.0, 1.0);

  QPixmap static missing(":/missing-file.png", "PNG");
  const int columns = 3;
  const qreal offset_h = 15.0;
  const qreal offset_w = 15.0;
  int column_index = 0;
  int row_index = 0;

  if(missing.size() != QSize(372, 240))
    missing = missing.scaled
      (372, 240, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

  for(int i = 0; i < vector.size(); i++)
    {
      auto effect = new QGraphicsDropShadowEffect();
      auto item = new wise_recent_files_view_item
	(QPixmap::fromImage(vector.at(i).first));

      connect(item,
	      SIGNAL(open(const QString &)),
	      this,
	      SIGNAL(open_file(const QString &)));
      connect(item,
	      SIGNAL(remove(QGraphicsItem *)),
	      this,
	      SLOT(slot_remove(QGraphicsItem *)));
      connect(item,
	      SIGNAL(remove(const QString &)),
	      this,
	      SIGNAL(remove(const QString &)));
      effect->setBlurRadius(0.0);
      effect->setColor(QColor(59, 59, 59));
      effect->setOffset(0.0, 0.0);

      auto const height = 25.0 + item->boundingRect().size().height();
      auto const width = 25.0 + item->boundingRect().size().width();

      if(row_index == 0)
	item->setPos(column_index * width + offset_w, offset_h);
      else
	item->setPos
	  (column_index * width + offset_w, height * row_index + offset_h);

      column_index += 1;
      item->setData(Qt::UserRole, vector.at(i).second);
      item->set_file_name(vector.at(i).second);
      item->setGraphicsEffect(effect);
      item->setPixmap
	(QFileInfo(vector.at(i).second).isReadable() ?
	 item->pixmap() : missing);
      scene()->addItem(item);

      if(column_index >= columns)
	{
	  column_index = 0;
	  row_index += 1;
	}
    }

  auto rect(scene()->itemsBoundingRect());

  rect.setHeight(offset_h + rect.height());
  rect.setX(0.0);
  rect.setY(0.0);
  horizontalScrollBar()->setValue(h_value);
  setSceneRect(rect);
  verticalScrollBar()->setValue(v_value);
  QApplication::restoreOverrideCursor();
}

void wise_recent_files_view::slot_remove(QGraphicsItem *item)
{
  if(!item)
    return;

  scene()->removeItem(item);
  delete item;
  QTimer::singleShot(50, this, SLOT(slot_gather(void)));
}
