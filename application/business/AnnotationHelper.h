/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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

#ifndef ANNOTATIONHELPER_H
#define ANNOTATIONHELPER_H

#include <QObject>
#include <QColor>

#include "../ModelService/IObserver.h"

/**
 * @brief The AnnotationHelper class
 *          高亮和注释 业务处理
 */

class AnnotationHelper : public QObject, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(AnnotationHelper)

public:
    explicit AnnotationHelper(QObject *parent = nullptr);

signals:
    void sigDealWithData(const int &, const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

private:
    void sendMsg(const int &, const QString &) Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &) Q_DECL_OVERRIDE;

private slots:
    void slotDealWithData(const int &, const QString &);

private:
    void __AddHighLight(const QString &msgContent);
    void __AddHighLightAnnotation(const QString &msgContent);
    void __RemoveHighLight(const QString &msgContent);
    void __ChangeAnnotationColor(const QString &msgContent);

    void __RemoveAnnotation(const QString &);
    void __UpdateAnnotationText(const QString &);

    void __AddPageIconAnnotation(const QString &);
    void __DeletePageIconAnnotation(const QString &);
    void __UpdatePageIconAnnotation(const QString &);

    void __InitConnection();

private:
    QList<int>              m_pMsgList;

};

#endif // DOCUMMENTFILEHELPER_H
