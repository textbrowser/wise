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

#include "wise-tab-bar.h"
#include "wise-tab.h"

#include <QPushButton>

wise_tab::wise_tab(QWidget *parent):QTabWidget(parent)
{
  m_tab_bar = new wise_tab_bar(this);
  m_tab_bar->setContextMenuPolicy(Qt::CustomContextMenu);
  m_tab_bar->setIconSize(QSize(24, 24));
  setStyleSheet("QTabWidget::tab-bar {"
		"alignment: left;"
		"}");
  setTabBar(m_tab_bar);
}

wise_tab::~wise_tab()
{
}

QTabBar *wise_tab::tab_bar(void) const
{
  return m_tab_bar;
}

void wise_tab::set_push_button(QPushButton *push_button, const int index)
{
  if(index < 0 || push_button == nullptr)
    return;

  auto const position = m_tab_bar->preferred_close_button_position_opposite();

  if(m_tab_bar->tabButton(index, position) == nullptr)
    m_tab_bar->setTabButton(index, position, push_button);
}
