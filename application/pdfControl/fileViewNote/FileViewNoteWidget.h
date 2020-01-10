/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#ifndef FILEVIEWNOTEWIDGET_H
#define FILEVIEWNOTEWIDGET_H

#include <DTextEdit>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QTextEdit>
#include <QVBoxLayout>
#include "CustomControl/CustomClickLabel.h"
#include "CustomControl/CustomWidget.h"

class CustemTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit CustemTextEdit(DWidget *parent = nullptr);

signals:
    void sigShowTips();

protected:
    // void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    void init();
    int calcTextSize(const QString &);
    QString getMaxLenStr(QString);

private slots:
    void slotTextEditMaxContantNum();

private:
    int m_nMaxContantLen = 1500;  // 允许输入文本最大长度
};

/**
 *@brief The FileViewNoteWidget class
 *@brief 添加注释子界面
 */
class FileViewNoteWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FileViewNoteWidget)

public:
    explicit FileViewNoteWidget(CustomWidget *parent = nullptr);
    ~FileViewNoteWidget() Q_DECL_OVERRIDE;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

public:
    void setEditText(const QString &note);
    void closeWidget();
    void showWidget(const int &);
    void showWidget(const QPoint &);

    void setPointAndPage(const QString &);

    void setNoteUuid(const QString &pNoteUuid);
    void setNotePage(const QString &pNotePage);

protected:
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void initWidget() Q_DECL_OVERRIDE;

private:
    void initConnections();

private slots:
    void slotUpdateTheme();
    void slotClosed();
    void slotTextEditMaxContantNum();
    void slotShowTips();

private:
    /*CustemTextEdit DTextEdit*/
    QString m_pHighLightPointAndPage = "";
    QString m_pNoteUuid = "";
    QString m_pNotePage = "";

    CustemTextEdit *m_pTextEdit = nullptr;    // 注释
    CustomClickLabel *m_pCloseLab = nullptr;  // 关闭
    QString m_strNote = "";                   // 注释内容
};

#endif  // FILEVIEWNOTEWIDGET_H
