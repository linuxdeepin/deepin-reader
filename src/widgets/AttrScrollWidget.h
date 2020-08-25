/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#ifndef ATTRSCROLLWIDGET_H
#define ATTRSCROLLWIDGET_H

#include <DFrame>
#include <DWidget>

class DocSheet;
class QGridLayout;
class AttrScrollWidget : public Dtk::Widget::DFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(AttrScrollWidget)

public:
    explicit AttrScrollWidget(DocSheet *sheet, Dtk::Widget::DWidget *parent  = nullptr);

private:
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const QString &sData);
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const QDateTime &sData);
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const bool &bData);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

    QList<QWidget *> m_leftWidgetlst;
    QList<QWidget *> m_rightWidgetlst;
};

#endif // ATTRSCROLLWIDGET_H
