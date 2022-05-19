/* Copyright (c) 2013-2022 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "ScriptingView.h"

#include "GBAApp.h"
#include "ScriptingController.h"
#include "ScriptingTextBuffer.h"

using namespace QGBA;

ScriptingView::ScriptingView(ScriptingController* controller, QWidget* parent)
	: QMainWindow(parent)
	, m_controller(controller)
{
	m_ui.setupUi(this);

	m_ui.prompt->setFont(GBAApp::app()->monospaceFont());
	m_ui.log->setNewlineTerminated(true);

	connect(m_ui.prompt, &QLineEdit::returnPressed, this, &ScriptingView::submitRepl);
	connect(m_ui.runButton, &QAbstractButton::clicked, this, &ScriptingView::submitRepl);
	connect(m_controller, &ScriptingController::log, m_ui.log, &LogWidget::log);
	connect(m_controller, &ScriptingController::warn, m_ui.log, &LogWidget::warn);
	connect(m_controller, &ScriptingController::error, m_ui.log, &LogWidget::error);
	connect(m_controller, &ScriptingController::textBufferCreated, this, &ScriptingView::addTextBuffer);

	connect(m_ui.buffers, &QListWidget::currentRowChanged, this, &ScriptingView::selectBuffer);
	connect(m_ui.load, &QAction::triggered, this, &ScriptingView::load);
}

void ScriptingView::submitRepl() {
	m_ui.log->echo(m_ui.prompt->text());
	m_controller->runCode(m_ui.prompt->text());
	m_ui.prompt->clear();
}

void ScriptingView::load() {
	QString filename = GBAApp::app()->getOpenFileName(this, tr("Select script to load"), getFilters());
	if (!filename.isEmpty()) {
		m_controller->loadFile(filename);
	}
}

void ScriptingView::addTextBuffer(ScriptingTextBuffer* buffer) {
	QTextDocument* document = buffer->document();
	m_textBuffers.append(buffer);
	QListWidgetItem* item = new QListWidgetItem(document->metaInformation(QTextDocument::DocumentTitle));
	connect(buffer, &ScriptingTextBuffer::bufferNameChanged, this, [item](const QString& name) {
		item->setText(name);
	});
	connect(buffer, &QObject::destroyed, this, [this, buffer, item]() {
		m_textBuffers.removeAll(buffer);
		m_ui.buffers->removeItemWidget(item);
	});
	m_ui.buffers->addItem(item);
	m_ui.buffers->setCurrentItem(item);
}

void ScriptingView::selectBuffer(int index) {
	m_ui.buffer->setDocument(m_textBuffers[index]->document());
}

QString ScriptingView::getFilters() const {
	QStringList filters;
#ifdef USE_LUA
	filters.append(tr("Lua scripts (*.lua)"));
#endif
	filters.append(tr("All files (*.*)"));
	return filters.join(";;");
}
