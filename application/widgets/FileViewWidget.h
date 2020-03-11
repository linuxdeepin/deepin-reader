/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include "CustomControl/CustomWidget.h"

class TextOperationMenu;
class DefaultOperationMenu;
class DocummentProxy;
class ProxyViewDisplay;
class FileViewWidgetPrivate;
class FindWidget;
class NoteTipWidget;
class NoteViewWidget;

/**
 * @brief The FileViewWidget class
 * @brief   文档显示区域
 */
class FileViewWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FileViewWidget)

public:
    explicit FileViewWidget(CustomWidget *parent = nullptr);
    ~FileViewWidget() override;

signals:
    void sigFileOpenOK(const QString &);
    void sigFindOperation(const int &);
    void sigAnntationMsg(const int &, const QString &);
    void sigDeleteAnntation(const int &, const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    int qDealWithShortKey(const QString &) override;

    void OpenFilePath(const QString &);
    void ShowFindWidget();

    void setFileChange(bool bchanged);
    bool getFileChange();

protected:
    void initWidget() override;

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *e) override;

private slots:
    void slotCustomContextMenuRequested(const QPoint &);

    void slotBookMarkStateChange(int, bool);
    void slotDocFilePageChanged(int);
    void SlotDocFileOpenResult(bool);

    void SlotFindOperation(const int &, const QString &strFind);

private:
    void initConnections();
    void onSetHandShape(const QString &);

private:
    void onOpenNoteWidget(const QString &msgContent);
    void onShowNoteWidget(const QString &contant);
    void __ShowPageNoteWidget(const QString &msgContent);

    void __ShowNoteTipWidget(const QString &sText);
    void __CloseFileNoteWidget();

    void __SetCursor(const QCursor &cursor);

private:
    FindWidget              *m_pFindWidget = nullptr;
    NoteViewWidget          *m_pNoteViewWidget = nullptr;
    NoteTipWidget           *m_pTipWidget = nullptr;

    TextOperationMenu       *m_operatemenu = nullptr;
    DefaultOperationMenu    *m_pDefaultMenu = nullptr;
    DocummentProxy          *m_pProxy = nullptr;

    ProxyViewDisplay            *m_pDocViewProxy = nullptr;     //  文档操作 旋转\单双页\自适应

    FileViewWidgetPrivate *d_fvptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_fvptr), FileViewWidget)
};


#endif // FILEVIEWWIDGET_H
