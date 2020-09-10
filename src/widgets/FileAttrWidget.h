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

/**
 * @brief The FileAttrWidget class
 * 文件信息窗口
 */
class FileAttrWidget : public DAbstractDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(FileAttrWidget)

public:
    explicit FileAttrWidget(DWidget *parent = nullptr);

    ~FileAttrWidget() override;

    /**
     * @brief setFileAttr
     * 设置文档对象
     * @param sheet
     */
    void setFileAttr(DocSheet *sheet);

    /**
     * @brief showScreenCenter
     * 居中显示
     */
    void showScreenCenter();

private:
    /**
     * @brief initWidget
     * 初始化控件
     */
    void initWidget();

    /**
     * @brief initCloseBtn
     * 初始化关闭按钮
     */
    void initCloseBtn();

    /**
     * @brief initImageLabel
     * 初始化图片显示控件
     */
    void initImageLabel();

    /**
     * @brief addTitleFrame
     * 添加标题控件
     */
    void addTitleFrame(const QString &);

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;
    ImageWidget *frameImage = nullptr;
};

#endif  // FILEATTRWIDGET_H
