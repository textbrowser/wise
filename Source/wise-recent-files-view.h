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

#ifndef _wise_recent_files_view_h_
#define _wise_recent_files_view_h_

#include "wise.h"

#include <QFileInfo>
#include <QFuture>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>

class wise_recent_files_view_item: public QObject, public QGraphicsPixmapItem
{
  Q_OBJECT

 public:
  wise_recent_files_view_item(const QPixmap &pixmap):
    QObject(), QGraphicsPixmapItem(pixmap)
  {
    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::NoCache);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
  }

  ~wise_recent_files_view_item()
  {
  }

  QRectF boundingRect(void) const
  {
    return QRectF(0.0, 0.0, 372.0, 240.0);
  }

  QString file_name(void) const
  {
    return m_file_name;
  }

  void set_file_name(const QString &file_name)
  {
    m_file_name = file_name;
  }

 private:
  QPainterPath m_file_name_text;
  QPainterPath m_remove_button;
  QPointF m_hover_point;
  QString m_file_name;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *event)
  {
    QGraphicsPixmapItem::hoverEnterEvent(event);
    m_hover_point = event ? event->pos() : QPointF();
    update();
  }

  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
  {
    QGraphicsPixmapItem::hoverLeaveEvent(event);
    m_hover_point = QPointF();
    update();
  }

  void hoverMoveEvent(QGraphicsSceneHoverEvent *event)
  {
    QGraphicsPixmapItem::hoverMoveEvent(event);
    m_hover_point = event ? event->pos() : QPointF();
    update();
  }

  void mousePressEvent(QGraphicsSceneMouseEvent *event)
  {
    if(!event)
      {
	QGraphicsPixmapItem::mousePressEvent(event);
	return;
      }

    m_remove_button.contains
      (event->pos()) ? emit remove(m_file_name) : (void) 0;
    m_remove_button.contains(event->pos()) ? emit remove(this) : (void) 0;
  }

  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget)
  {
    if(!option || !painter)
      {
	QGraphicsPixmapItem::paint(painter, option, widget);
	return;
      }

    QPen pen;
    const qreal static radius = 7.5;

    pen.setColor(QColor(Qt::white));
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(0.0);
    painter->save();
    painter->setBrush(QBrush(pixmap()));
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawRoundedRect(boundingRect(), radius, radius); // Order.
    painter->restore();

    QIcon const static icon(":/clear.svg");

    m_remove_button.isEmpty() ?
      m_remove_button.addEllipse(-31.5 + boundingRect().topRight().x(),
				 8.5 + boundingRect().topRight().y(),
				 25.0,
				 25.0),
      m_remove_button.closeSubpath() :
      (void) 0;
    pen.setColor(QColor(222, 141, 174));
    pen.setWidthF(3.5);
    painter->save();
    painter->setPen(pen);
    painter->drawPath(m_remove_button);
    painter->fillPath
      (m_remove_button,
       m_remove_button.contains(m_hover_point) ?
       QColor(Qt::black) : QColor(Qt::white));
    icon.paint(painter, m_remove_button.boundingRect().toRect());
    painter->restore();

    auto const default_font(painter->font());
    auto const font_metrics(painter->fontMetrics());
    auto const text
      (font_metrics.elidedText(QFileInfo(m_file_name).fileName(),
			       Qt::ElideRight,
			       -50 + boundingRect().width()));
    auto const text_width = font_metrics.horizontalAdvance(text);

    m_file_name_text.isEmpty() ?
      m_file_name_text.addRoundedRect
      (10.5 + boundingRect().topLeft().x(),
       8.5 + boundingRect().topLeft().y(),
       static_cast<qreal> (5 + text_width),
       static_cast<qreal> (5 + font_metrics.height()),
       5.0,
       5.0),
      m_file_name_text.closeSubpath() :
      (void) 0;
    pen.setColor(QColor(Qt::black));
    pen.setWidthF(2.5);
    painter->save();
    painter->setPen(pen);
    painter->drawPath(m_file_name_text);
    painter->fillPath(m_file_name_text, QColor(Qt::black));
    painter->restore();
    pen.setColor(QColor(Qt::white));
    painter->save();
    painter->setPen(pen);
    painter->drawText
      (12.5 + boundingRect().topLeft().x(),
       23.5 + boundingRect().topLeft().y(),
       text);
    painter->restore();

    if(option->state & (QStyle::State_HasFocus | QStyle::State_Selected))
      {
	QPainterPath path;
	auto rect(boundingRect());
	const qreal static offset = 5.0;

	rect.setHeight(offset + rect.height());
	rect.setWidth(offset + rect.width());
	rect.setX(-offset + rect.x());
	rect.setY(-offset + rect.y());
	path.addRoundedRect(rect, radius, radius);
	painter->save();
	painter->fillPath(path, QColor(222, 141, 174, 100)); // Sassy Pink
	painter->restore();
      }
  }

 signals:
  void remove(QGraphicsItem *item);
  void remove(const QString &file_name);
};

class wise_recent_files_view: public QGraphicsView
{
  Q_OBJECT

 public:
  wise_recent_files_view(QWidget *parent);
  QAction *menu_action(void) const;

 public slots:
  void slot_gather(void);
  void slot_open(void);

 private:
  QAction *m_menu_action;
  QFuture<void> m_gather_future;
  QStringList selected_file_names(void) const;
  void enterEvent(QEnterEvent *event);
  void gather(void);
  void keyPressEvent(QKeyEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);

 private slots:
  void slot_populate(const QVectorQPairQImageQString &vector);
  void slot_remove(QGraphicsItem *item);

 signals:
  void gathered(const QVectorQPairQImageQString &vector);
  void open_file(const QString &file_name);
  void open_file(void);
  void remove(const QString &file_name);
};

#endif
