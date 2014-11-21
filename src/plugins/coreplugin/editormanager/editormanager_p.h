/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef EDITORMANAGER_P_H
#define EDITORMANAGER_P_H

#include "documentmodel.h"
#include "editorarea.h"
#include "editormanager.h"
#include "editorview.h"
#include "ieditor.h"

#include <coreplugin/idocument.h>

#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QAction;
class QTimer;
QT_END_NAMESPACE

namespace Core {

class EditorManager;

namespace Internal {

class EditorClosingCoreListener;
class MainWindow;
class OpenEditorsViewFactory;
class OpenEditorsWindow;

class EditorManagerPrivate : public QObject
{
    Q_OBJECT

    friend class Core::EditorManager;

public:
    static EditorArea *mainEditorArea();
    static EditorView *currentEditorView();
    static void setCurrentEditor(IEditor *editor, bool ignoreNavigationHistory = false);
    static IEditor *openEditor(EditorView *view,
                               const QString &fileName,
                               Id editorId = Id(),
                               EditorManager::OpenEditorFlags flags = EditorManager::NoFlags,
                               bool *newEditor = 0);
    static IEditor *openEditorAt(EditorView *view,
                                 const QString &fileName,
                                 int line,
                                 int column = 0,
                                 Id editorId = Id(),
                                 EditorManager::OpenEditorFlags flags = EditorManager::NoFlags,
                                 bool *newEditor = 0);
    static IEditor *duplicateEditor(IEditor *editor);
    static IEditor *activateEditor(EditorView *view, IEditor *editor,
                                   EditorManager::OpenEditorFlags flags = EditorManager::NoFlags);
    static IEditor *activateEditorForDocument(EditorView *view, IDocument *document,
                                              EditorManager::OpenEditorFlags flags = 0);
    static void activateEditorForEntry(EditorView *view, DocumentModel::Entry *entry,
                                       EditorManager::OpenEditorFlags flags = EditorManager::NoFlags);
    /* closes the document if there is no other editor on the document visible */
    static void closeEditorOrDocument(IEditor *editor);

    static EditorView *viewForEditor(IEditor *editor);
    static void setCurrentView(EditorView *view);

    static MakeWritableResult makeFileWritable(IDocument *document);
    static void doEscapeKeyFocusMoveMagic();

    static Id getOpenWithEditorId(const QString &fileName, bool *isExternalEditor = 0);

    static void saveSettings();
    static void readSettings();
    static void setReloadSetting(IDocument::ReloadSetting behavior);
    static IDocument::ReloadSetting reloadSetting();
    static void setAutoSaveEnabled(bool enabled);
    static bool autoSaveEnabled();
    static void setAutoSaveInterval(int interval);
    static int autoSaveInterval();

    static void splitNewWindow(Internal::EditorView *view);
    static void closeView(Internal::EditorView *view);
    static void emptyView(Internal::EditorView *view);

    static void updateActions();

    static void updateWindowTitleForDocument(IDocument *document, QWidget *window);

    static void vcsOpenCurrentEditor();
    static void makeCurrentEditorWritable();

public slots:
    static bool saveDocument(Core::IDocument *document);
    static bool saveDocumentAs(Core::IDocument *document);

    static void split(Qt::Orientation orientation);
    static void removeAllSplits();
    static void gotoNextSplit();

    void handleDocumentStateChange();
    static void editorAreaDestroyed(QObject *area);

private slots:
    static void gotoNextDocHistory();
    static void gotoPreviousDocHistory();

    static void autoSave();
    static void handleContextChange(const QList<Core::IContext *> &context);

    static void copyFilePathFromContextMenu();
    static void copyFileNameFromContextMenu();
    static void saveDocumentFromContextMenu();
    static void saveDocumentAsFromContextMenu();
    static void revertToSavedFromContextMenu();
    static void closeEditorFromContextMenu();
    static void closeOtherDocumentsFromContextMenu();

    static void closeAllEditorsExceptVisible();
    static void revertToSaved(IDocument *document);

    static void showInGraphicalShell();
    static void openTerminal();
    static void findInDirectory();

    static void split();
    static void splitNewWindow();
    static void removeCurrentSplit();

    static void setCurrentEditorFromContextChange();

private:
    static OpenEditorsWindow *windowPopup();
    static void showPopupOrSelectDocument();

    static IEditor *createEditor(Id editorId = Id(), const QString &fileName = QString());
    static void addEditor(IEditor *editor);
    static void removeEditor(IEditor *editor);
    static IEditor *placeEditor(EditorView *view, IEditor *editor);
    static void activateView(EditorView *view);
    static void restoreEditorState(IEditor *editor);
    static int visibleDocumentsCount();
    static EditorArea *findEditorArea(const EditorView *view, int *areaIndex = 0);
    static IEditor *pickUnusedEditor(Internal::EditorView **foundView = 0);
    static void addDocumentToRecentFiles(IDocument *document);
    static void updateAutoSave();
    static void updateMakeWritableWarning();
    static void setupSaveActions(IDocument *document, QAction *saveAction,
                                 QAction *saveAsAction, QAction *revertToSavedAction);
    static void updateWindowTitle();

private:
    explicit EditorManagerPrivate(QObject *parent);
    ~EditorManagerPrivate();
    void init();

    QList<EditLocation> m_globalHistory;
    QList<EditorArea *> m_editorAreas;
    QPointer<IEditor> m_currentEditor;
    QPointer<IEditor> m_scheduledCurrentEditor;
    QPointer<EditorView> m_currentView;
    QTimer *m_autoSaveTimer;

    // actions
    QAction *m_revertToSavedAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_closeCurrentEditorAction;
    QAction *m_closeAllEditorsAction;
    QAction *m_closeOtherDocumentsAction;
    QAction *m_closeAllEditorsExceptVisibleAction;
    QAction *m_gotoNextDocHistoryAction;
    QAction *m_gotoPreviousDocHistoryAction;
    QAction *m_goBackAction;
    QAction *m_goForwardAction;
    QAction *m_splitAction;
    QAction *m_splitSideBySideAction;
    QAction *m_splitNewWindowAction;
    QAction *m_removeCurrentSplitAction;
    QAction *m_removeAllSplitsAction;
    QAction *m_gotoNextSplitAction;

    QAction *m_copyFilePathContextAction;
    QAction *m_copyFileNameContextAction;
    QAction *m_saveCurrentEditorContextAction;
    QAction *m_saveAsCurrentEditorContextAction;
    QAction *m_revertToSavedCurrentEditorContextAction;

    QAction *m_closeCurrentEditorContextAction;
    QAction *m_closeAllEditorsContextAction;
    QAction *m_closeOtherDocumentsContextAction;
    QAction *m_closeAllEditorsExceptVisibleContextAction;
    QAction *m_openGraphicalShellAction;
    QAction *m_openTerminalAction;
    QAction *m_findInDirectoryAction;
    DocumentModel::Entry *m_contextMenuEntry;
    IEditor *m_contextMenuEditor;

    OpenEditorsWindow *m_windowPopup;
    EditorClosingCoreListener *m_coreListener;

    QMap<QString, QVariant> m_editorStates;
    OpenEditorsViewFactory *m_openEditorsFactory;

    IDocument::ReloadSetting m_reloadSetting;

    EditorManager::WindowTitleHandler m_titleAdditionHandler;
    EditorManager::WindowTitleHandler m_titleVcsTopicHandler;

    bool m_autoSaveEnabled;
    int m_autoSaveInterval;
};

} // Internal
} // Core

#endif // EDITORMANAGER_P_H