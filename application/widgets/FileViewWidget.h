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

class FindWidget;
/**
 * @brief The FileViewWidget class
 * @brief   文档显示区域
 */
class FileViewWidgetPrivate;
class FileViewWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FileViewWidget)

public:
    explicit FileViewWidget(DWidget *parent = nullptr);
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
    void SlotFindOperation(const int &, const QString &strFind);

private:
    void initConnections();

private:
    FindWidget              *m_pFindWidget = nullptr;

private:
    FileViewWidgetPrivate *const d_ptr = nullptr;
    Q_DECLARE_PRIVATE(FileViewWidget)
};


#endif // FILEVIEWWIDGET_H
