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
#ifndef BOOKMARKBUTTON_H
#define BOOKMARKBUTTON_H

#include <DPushButton>
#include <DWidget>

class BookMarkButton : public Dtk::Widget::DPushButton
{
    Q_OBJECT
public:
    BookMarkButton(Dtk::Widget::DWidget *parent = nullptr);

    bool clickState();

    void setClickState(bool state);

signals:
    void sigClicked(bool);

protected:

    virtual void paintEvent(QPaintEvent *e) override;

    virtual void  enterEvent(QEvent *e) override;

    virtual void  leaveEvent(QEvent *e) override;

    virtual void mousePressEvent(QMouseEvent *e) override;

    virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
    bool  ishovered;
    bool  ispressed;
    bool  isclicked;
};

#endif // BOOKMARKBUTTON_H
