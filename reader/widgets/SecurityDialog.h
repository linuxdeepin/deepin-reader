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
#ifndef SECURITYDIALOG_H
#define SECURITYDIALOG_H

#include <DDialog>
#include <DLabel>
DWIDGET_USE_NAMESPACE

struct NewStr {
    QStringList strList;
    QString resultStr;
    int fontHeifht = 0;
};

NewStr autoCutText(const QString &text, DLabel *pDesLbl);

class SecurityDialog : public Dtk::Widget::DDialog
{
    Q_OBJECT
public:
    explicit SecurityDialog(const QString &urlstr, QWidget *parent = nullptr);

protected:
    void changeEvent(QEvent *event) override;

private:
    /**
     * @brief autoFeed 自动换行
     */
    void autoFeed();
    /**
     * @brief setLabelColor 设置label颜色
     * @param label
     * @param alphaF 透明度
     */
    void setLabelColor(DLabel *label, qreal alphaF);

private:
    DLabel *NameLabel = nullptr;
    DLabel *ContentLabel = nullptr;
    QString m_strDesText;
    int m_iLabelOldHeight = 0;
    int m_iLabelOld1Height = 0;
    int m_iDialogOldHeight = 0;
};

#endif // SECURITYDIALOG_H
