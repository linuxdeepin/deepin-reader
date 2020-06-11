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

class SheetBrowserPDFPrivate;

class Annotation : public QObject
{
    Q_OBJECT
public:
    explicit Annotation(QObject *parent = nullptr);

public:
    void AddHighLight(const QString &);
    void AddHighLightAnnotation(const QString &);
    void RemoveHighLight(const QString &);
    void ChangeAnnotationColor(const QString &);

    void RemoveAnnotation(const QString &);
    void UpdateAnnotationText(const QString &);

    void AddPageIconAnnotation(const QString &);
    void DeletePageIconAnnotation(const QString &);
    void UpdatePageIconAnnotation(const QString &);
    void handleNote(const int &, const QString &);

private:
    SheetBrowserPDFPrivate      *fvmPrivate = nullptr;
    friend class SheetBrowserPDFPrivate;
};

#endif // ANNOTATIONHELPER_H
