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

#include "wise-settings.h"

#include <QSettings>

wise_settings::wise_settings(QWidget *parent):QWidget(parent)
{
  m_ui.setupUi(this);
  m_ui.proxy_group_box->setVisible(false);
  restore();

  foreach(auto check_box, findChildren<QCheckBox *> ())
    connect(check_box,
	    SIGNAL(stateChanged(int)),
	    this,
	    SLOT(slot_check_box_state_changed(int)));

  foreach(auto combination_box, findChildren<QComboBox *> ())
    connect(combination_box,
	    SIGNAL(currentIndexChanged(int)),
	    this,
	    SLOT(slot_combination_box_current_index_changed(int)));
}

wise_settings::~wise_settings()
{
}

QPdfDocumentRenderOptions wise_settings::render_options(void)
{
  QPdfDocumentRenderOptions options;
  QPdfDocumentRenderOptions::RenderFlags render_flags =
    QPdfDocumentRenderOptions::RenderFlag::None;
  wise_settings settings(nullptr);

  if(settings.m_ui.annotations->isChecked())
    render_flags |= QPdfDocumentRenderOptions::RenderFlag::Annotations;

  if(settings.m_ui.grayscale->isChecked())
    render_flags |= QPdfDocumentRenderOptions::RenderFlag::Grayscale;

  if(settings.m_ui.halftone->isChecked())
    render_flags |= QPdfDocumentRenderOptions::RenderFlag::ForceHalftone;

  if(settings.m_ui.image_aliased->isChecked())
    render_flags |= QPdfDocumentRenderOptions::RenderFlag::ImageAliased;

  if(settings.m_ui.lcd_optimized->isChecked())
    render_flags |= QPdfDocumentRenderOptions::RenderFlag::OptimizedForLcd;

  if(settings.m_ui.path_aliased->isChecked())
    render_flags |= QPdfDocumentRenderOptions::RenderFlag::PathAliased;

  if(settings.m_ui.text_aliased->isChecked())
    render_flags |= QPdfDocumentRenderOptions::RenderFlag::TextAliased;

  options.setRenderFlags(render_flags);
  return options;
}

QPdfPageRenderer::RenderMode wise_settings::render_mode(void)
{
  wise_settings settings(nullptr);

  if(settings.m_ui.render_mode->currentText() == tr("Single Threads"))
    return QPdfPageRenderer::RenderMode::SingleThreaded;
  else
    return QPdfPageRenderer::RenderMode::MultiThreaded;
}

QPdfView::PageMode wise_settings::page_mode(void)
{
  wise_settings settings(nullptr);

  if(settings.m_ui.page_mode->currentText() == tr("Multiple"))
    return QPdfView::PageMode::MultiPage;
  else
    return QPdfView::PageMode::SinglePage;
}

bool wise_settings::left_panel_visible(void)
{
#ifdef Q_OS_ANDROID
  return QSettings().value("settings/left-panel-checked", false).toBool();
#else
  return QSettings().value("settings/left-panel-checked", true).toBool();
#endif
}

void wise_settings::restore(void)
{
  foreach(auto check_box, findChildren<QCheckBox *> ())
    {
      auto const key(QString("settings/%1").arg(check_box->objectName()));

      check_box->setChecked(QSettings().value(key).toInt());
    }

  foreach(auto combination_box, findChildren<QComboBox *> ())
    {
      auto const key
	(QString("settings/%1").arg(combination_box->objectName()));

      combination_box->setCurrentIndex
	(qBound(0,
		QSettings().value(key).toInt(),
		combination_box->count() - 1));
    }
}

void wise_settings::slot_check_box_state_changed(int state)
{
  auto check_box = qobject_cast<QCheckBox *> (sender());

  if(!check_box)
    return;

  QSettings().setValue
    (QString("settings/%1").arg(check_box->objectName()), state);
}

void wise_settings::slot_combination_box_current_index_changed(int index)
{
  auto combination_box = qobject_cast<QComboBox *> (sender());

  if(!combination_box)
    return;

  QSettings().setValue
    (QString("settings/%1").arg(combination_box->objectName()), index);
}
