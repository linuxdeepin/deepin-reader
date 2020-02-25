/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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

#ifndef DOCUMMENTFILEHELPER_H
#define DOCUMMENTFILEHELPER_H

#include <QObject>

#include "application.h"

#include "docview/commonstruct.h"

/**
 * @brief The DocummentFileHelper class
 * 封装 DocView 函数调用
 */

class DocummentFileHelper : public QObject, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(DocummentFileHelper)

public:
    explicit DocummentFileHelper(QObject *parent = nullptr);

signals:
    void sigDealWithData(const int &, const QString &);
    void sigDealWithKeyMsg(const QString &);
    void sigFileSlider(const int &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

public:
    bool save(const QString &filepath, bool withChanges);

    void qRemoveTabFile(const int &iType,  const QString &sPath);
    void qAppExitFile(const int &iType, const QString &sPath);

private:
    void __PageJump(const int &pagenum);
    void __PageJumpByMsg(const int &);
    void __FileCtrlCContent();

    void initConnections();
    void onOpenFile(const QString &filePaths);
//    void onOpenFiles(const QString &filePaths);

    void onSaveFile();
    void onSaveAsFile();

    // IObserver interface
    void sendMsg(const int &msgType, const QString &msgContent = "") Q_DECL_OVERRIDE;
    void notifyMsg(const int &msgType, const QString &msgContent = "") Q_DECL_OVERRIDE;

private slots:
    void slotDealWithData(const int &, const QString &);
    void slotDealWithKeyMsg(const QString &);

    void slotCopySelectContent(const QString &);
    void slotFileSlider(const int &);

private:
    QList<int>              m_pMsgList;
    QList<QString>          m_pKeyMsgList;
//    QString                 m_szFilePath = "";
//    DocType_EM              m_nCurDocType = DocType_NULL;
};

#endif // DOCUMMENTFILEHELPER_H
