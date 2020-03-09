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

#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <QObject>

/**
 * @brief The AnnotationHelper class
 *          高亮和注释 业务处理
 */

class DocummentProxy;

class Annotation : public QObject
{
    Q_OBJECT
public:
    explicit Annotation(QObject *parent = nullptr);

public:
    void setProxy(DocummentProxy *);

    void AddHighLight(const QString &);
    QString AddHighLightAnnotation(const QString &);
    QString RemoveHighLight(const QString &);
    void ChangeAnnotationColor(const QString &);

    QString RemoveAnnotation(const QString &);
    void UpdateAnnotationText(const QString &);

    void AddPageIconAnnotation(const QString &);
    QString DeletePageIconAnnotation(const QString &);
    QString UpdatePageIconAnnotation(const QString &);

private:
    DocummentProxy      *m_pProxy = nullptr;
};

#endif // ANNOTATIONHELPER_H
