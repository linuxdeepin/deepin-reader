/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chendu <chendu@uniontech.com>
*
* Maintainer: chendu <chendu@uniontech.com>
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
#include "SecurityDialog.h"

#include <DFontSizeManager>
#include <DLabel>
#include <DPlatformWindowHandle>
#include <DGuiApplicationHelper>

#include <QTextOption>
#include <QPainter>

NewStr autoCutText(const QString &text, DLabel *pDesLbl)
{

    if (text.isEmpty() || nullptr == pDesLbl) {
        return NewStr();
    }

    QFont font; // 应用使用字体对象
    QFontMetrics font_label(font);
    QString strText = text;
    int titlewidth = font_label.width(strText);
    QString str;
    NewStr newstr;
    int width = pDesLbl->width();
    if (titlewidth < width) {
        newstr.strList.append(strText);
        newstr.resultStr += strText;
    } else {
        for (int i = 0; i < strText.count(); i++) {
            str += strText.at(i);

            if (font_label.width(str) > width) { //根据label宽度调整每行字符数
                str.remove(str.count() - 1, 1);
                newstr.strList.append(str);
                newstr.resultStr += str + "\n";
                str.clear();
                --i;
            }
        }
        newstr.strList.append(str);
        newstr.resultStr += str;
    }
    newstr.fontHeifht = font_label.height();
    return newstr;
}

SecurityDialog::SecurityDialog(const QString &urlstr, QWidget *parent)
    : DDialog(parent)
{
    setFixedWidth(380);
    setIcon(QIcon::fromTheme("deepin-reader"));
    QString str1(tr("This document is trying to connect to:"));
    QString str2(tr("If you trust the website, click Allow, otherwise click Block."));

    addButton(tr("Block", "button"));
    addButton(tr("Allow", "button"), true, ButtonRecommend);


    DLabel *strlabel = new DLabel;
    strlabel->setObjectName("NameLabel1");
    strlabel->setFixedWidth(340);
    strlabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    strlabel->setText(str1);
    strlabel->setWordWrap(true);
    DFontSizeManager::instance()->bind(strlabel, DFontSizeManager::T6, QFont::Normal);
    strlabel->setForegroundRole(DPalette::ToolTipText);

    m_strDesText = urlstr;
    DLabel *strlabel2 = new DLabel;
    strlabel2->setObjectName("ContentLabel");
    strlabel2->setFixedWidth(340);
    strlabel2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    strlabel2->setText(urlstr);
    DFontSizeManager::instance()->bind(strlabel2, DFontSizeManager::T6, QFont::Medium);

    DLabel *strlabel3 = new DLabel;
    strlabel3->setObjectName("NameLabel3");
    strlabel3->setFixedWidth(340);
    strlabel3->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    strlabel3->setText(str2);
    strlabel3->setWordWrap(true);
    DFontSizeManager::instance()->bind(strlabel3, DFontSizeManager::T6, QFont::Normal);
    strlabel3->setForegroundRole(DPalette::ToolTipText);

    addContent(strlabel, Qt::AlignHCenter);
    addContent(strlabel2, Qt::AlignHCenter);
    addContent(strlabel3, Qt::AlignHCenter);

    autoFeed(strlabel, strlabel2, strlabel3);
}

void SecurityDialog::autoFeed(DLabel *label1, DLabel *label2, DLabel *label3)
{
    NewStr newstr = autoCutText(m_strDesText, label2);
    label2->setText(newstr.resultStr);
    int height_lable = newstr.strList.size() * newstr.fontHeifht;
    label2->setFixedHeight(height_lable);

    if (0 == m_iLabelOldHeight) { // 第一次exec自动调整
        adjustSize();
    } else {
        setFixedHeight(m_iDialogOldHeight - m_iLabelOldHeight - m_iLabelOld1Height - m_iLabel3Old1Height
                       + height_lable + label1->height() + label3->height()); //字号变化后自适应调整
    }
    m_iLabelOldHeight = height_lable;
    m_iLabelOld1Height = /*newstr.fontHeifht*/label1->height();
    m_iLabel3Old1Height = /*newstr.fontHeifht*/label3->height();
    qInfo() << m_iDialogOldHeight << m_iLabelOldHeight << m_iLabelOld1Height << m_iLabel3Old1Height;
    m_iDialogOldHeight = height();
}

void SecurityDialog::changeEvent(QEvent *event)
{
    if (QEvent::FontChange == event->type()) {
        qInfo() << children();
        Dtk::Widget::DLabel *p = findChild<Dtk::Widget::DLabel *>("ContentLabel");
        if (nullptr != p) {
            Dtk::Widget::DLabel *pNameLabel = findChild<Dtk::Widget::DLabel *>("NameLabel1");
            if (nullptr != pNameLabel) {
                Dtk::Widget::DLabel *pNameLabel3 = findChild<Dtk::Widget::DLabel *>("NameLabel3");
                if (nullptr != pNameLabel3) {
                    autoFeed(pNameLabel, p, pNameLabel3);
                }
            }
        }
    }

    DDialog::changeEvent(event);
}
