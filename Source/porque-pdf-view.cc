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

#include "porque-pdf-view.h"

#include <QPdfBookmarkModel>
#include <QPdfDocument>
#include <QPdfView>

porque_pdf_view::porque_pdf_view
(const QUrl &url, QWidget *parent):QWidget(parent)
{
  m_bookmark_model = new QPdfBookmarkModel(this);
  m_bookmark_model->setDocument(m_document = new QPdfDocument(nullptr));
  m_document->load(url.path());
  m_pdf_view = new QPdfView(this);
  m_pdf_view->setDocument(m_document);
  m_pdf_view->setPageMode(QPdfView::PageMode::MultiPage);
  m_ui.setupUi(this);
  m_ui.contents->expandAll();
  m_ui.splitter->setStretchFactor(0, 0);
  m_ui.splitter->setStretchFactor(1, 1);
  m_url = url;
  prepare();
}

porque_pdf_view::~porque_pdf_view()
{
  m_document->deleteLater();
}

void porque_pdf_view::prepare(void)
{
  m_ui.contents->setModel(m_bookmark_model);
  m_ui.frame->layout()->addWidget(m_pdf_view);
}
