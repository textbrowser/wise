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

#include "porque-tab.h"
#include "porque-tab-bar.h"

#include <QStyle>
#include <QToolButton>
#include <QtMath>

porque_tab_bar::porque_tab_bar(QWidget *parent):QTabBar(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  setDocumentMode(true);
  setElideMode(Qt::ElideRight);
  setExpanding(true);
  setStyleSheet("QTabBar::tear {border: none; image: none; width: 0px;}");
  setUsesScrollButtons(true);
  connect(this,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slot_custom_context_menu_requested(const QPoint &)));

  foreach(auto tool_button, findChildren <QToolButton *> ())
    if(tool_button)
      tool_button->setStyleSheet
	(QString("QToolButton {background-color: %1;"
		 "border: none;"
		 "margin-bottom: 0px;"
		 "margin-top: 0px;"
		 "}"
		 "QToolButton::menu-button {border: none;}").
	 arg(QWidget::palette().color(QWidget::backgroundRole()).name()));
}

porque_tab_bar::~porque_tab_bar()
{
}

QSize porque_tab_bar::tabSizeHint(int index) const
{
  auto const tab_position = qobject_cast<porque_tab *> (parentWidget()) ?
    qobject_cast<porque_tab *> (parentWidget())->tabPosition() :
    QTabWidget::North;
  auto size(QTabBar::tabSizeHint(index));

  if(tab_position == QTabWidget::East || tab_position == QTabWidget::West)
    {
      int preferred_tab_height = 175;

      preferred_tab_height = qBound
	(125,
	 qMax(rect().height() / qMax(1, count()), size.height()),
	 175);
      preferred_tab_height = 5 * qCeil(preferred_tab_height / 5.0),
      size.setHeight(preferred_tab_height);
    }
  else
    {
#ifdef Q_OS_MACOS
      int preferred = 250;
#else
      int preferred = 250;
#endif
      int preferred_tab_width = 0;

      preferred_tab_width = qBound
	(125,
	 qMax(rect().width() / qMax(1, count()), size.width()),
	 preferred);
      preferred_tab_width = 5 * qCeil(preferred_tab_width / 5.0),
      size.setHeight(10 + size.height());
      size.setWidth(preferred_tab_width);
    }

  return size;
}

QTabBar::ButtonPosition porque_tab_bar::
preferred_close_button_position_opposite(void) const
{
#ifdef Q_OS_MACOS
  if(!style())
    return QTabBar::RightSide;
#else
  if(!style())
    return QTabBar::RightSide;
#endif

  auto const button_position = static_cast<QTabBar::ButtonPosition>
    (style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this));

#ifdef Q_OS_MACOS
  return button_position == QTabBar::LeftSide ?
    QTabBar::RightSide : QTabBar::LeftSide;
#else
  return button_position == QTabBar::LeftSide ?
    QTabBar::RightSide : QTabBar::LeftSide;
#endif
}

void porque_tab_bar::slot_custom_context_menu_requested(const QPoint &point)
{
  Q_UNUSED(point);
}
