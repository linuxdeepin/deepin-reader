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
#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <DListWidget>

DWIDGET_USE_NAMESPACE

/**
 * @brief The CustomListWidget class
 * 自定义缩略图ListWidget
 */
class CustomListWidget : public DListWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CustomListWidget)

public:
    explicit CustomListWidget(DWidget *parent = nullptr);

signals:
    void sigSelectItem(QListWidgetItem *);
    void sigValueChanged(int);

public:
    QListWidgetItem *insertWidgetItem(const int &);
//    inline void setRotateAngele(const int &angle) {m_nRotate = angle;}

private slots:
//    void slot_loadImage(const int &row, const QImage &image);
    void slotShowSelectItem(QListWidgetItem *);

//private:
//    int m_nRotate = 0;           // 缩略图旋转角度

    // QWidget interface
//protected:
//    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
};

#endif // CUSTOMLISTWIDGET_H
