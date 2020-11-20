/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     duanxiaohui<duanxiaohui@uniontech.com>
*
* Maintainer: duanxiaohui<duanxiaohui@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef NOTEVIEWWIDGET_H
#define NOTEVIEWWIDGET_H

#include "widgets/CustomWidget.h"

#include <QTimer>

namespace  deepin_reader {
class Annotation;
}
class SheetBrowser;
class TransparentTextEdit;

/**
 * @brief The NoteViewWidget class
 * 注释编辑框
 */
class NoteViewWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NoteViewWidget)
    friend class NoteShadowViewWidget;

public:
    explicit NoteViewWidget(DWidget *parent = nullptr);
    ~NoteViewWidget() override;

signals:
    /**
     * @brief sigNeedShowTips
     * 窗口提示框显示信号
     * @param tips
     * @param index
     */
    void sigNeedShowTips(const QString &tips, int index);

    /**
     * @brief sigRemoveAnnotation
     * 删除所有注释信号
     * @param annotation
     * @param tips
     */
    void sigRemoveAnnotation(deepin_reader::Annotation *annotation, bool tips);

    /**
     * @brief sigUpdateAnnotation
     * 更新节点注释内容信号
     * @param annotation
     * @param text
     */
    void sigUpdateAnnotation(deepin_reader::Annotation *annotation, const QString &text);

    /**
     * @brief sigHide
     * 窗口隐藏信号
     */
    void sigHide();

public:
    /**
     * @brief setEditText
     * 设置注释编辑框编辑内容
     * @param note
     */
    void setEditText(const QString &note);

    /**
     * @brief setAnnotation
     * 设置注释
     * @param annotation
     */
    void setAnnotation(deepin_reader::Annotation *annotation);

public slots:
    /**
     * @brief onBlurWindowChanged
     * 系统特效状态变更
     */
    void onBlurWindowChanged();

private slots:
    void onShowMenu();

protected:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget() override;

    /**
     * @brief hideEvent
     * 隐藏事件
     * @param event
     */
    void hideEvent(QHideEvent *event) override;

    /**
     * @brief paintEvent
     * 绘制事件
     * @param event
     */
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_strNote = "";
    TransparentTextEdit *m_pTextEdit = nullptr;
    deepin_reader::Annotation *m_annotation = nullptr;
    SheetBrowser *m_brower;
};

class NoteShadowViewWidget : public DWidget
{
public:
    explicit NoteShadowViewWidget(QWidget *parent);

    NoteViewWidget *getNoteViewWidget();

    /**
     * @brief showWidget
     * 显示注释编辑框窗口
     */
    void showWidget(const QPoint &);

private:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget();

private:
    NoteViewWidget *m_noteViewWidget = nullptr;
};

#endif  // NOTEVIEWWIDGET_H
