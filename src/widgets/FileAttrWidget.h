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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FILEATTRWIDGET_H
#define FILEATTRWIDGET_H

#include <DAbstractDialog>
#include <DWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE
class DocSheet;
class QVBoxLayout;
class ImageWidget;
class FileAttrWidget : public DAbstractDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(FileAttrWidget)

public:
    explicit FileAttrWidget(DWidget *parent = nullptr);

    ~FileAttrWidget() override;

    void setFileAttr(DocSheet *sheet);

    void showScreenCenter();

private:
    void initWidget();

    void initCloseBtn();

    void initImageLabel();

    void addTitleFrame(const QString &);

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;
    ImageWidget *frameImage = nullptr;
};

#endif  // FILEATTRWIDGET_H
