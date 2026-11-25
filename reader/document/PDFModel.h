// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 -2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PDFMODEL_H
#define PDFMODEL_H

#include "Model.h"

#include <QCoreApplication>
#include <QMutex>
#include <QScopedPointer>
#include <QPointF>
#include <QRectF>

class DPdfDoc;
class DPdfPage;
class DPdfAnnot;

namespace deepin_reader {
class PDFAnnotation : public Annotation
{
    Q_OBJECT
    Q_DISABLE_COPY(PDFAnnotation)

    friend class PDFPage;

public:
    ~PDFAnnotation() override;

    QList<QRectF> boundary() const override;

    QString contents() const override;

    int type() override;

    DPdfAnnot *ownAnnotation() override;

private:
    explicit PDFAnnotation(DPdfAnnot *dannotation);

    DPdfAnnot *m_dannotation;

};

class PDFPage : public Page
{
    Q_DISABLE_COPY(PDFPage)

    friend class PDFDocument;
public:
    ~PDFPage() override;

    /**
     * @brief 获取大小 会根据当前设备的像素密度保持每一页在不同设备上大小相同.
     * @return
     */
    QSizeF sizeF() const override;

    QImage render(int width, int height, const QRect &slice = QRect()) const override;

    Link getLinkAtPoint(const QPointF &point) override;

    bool hasWidgetAnnots() const override;

    QString text(const QRectF &rect) const override;

    QList<Word> words() override;

    QVector<PageSection> search(const QString &text, bool matchCase, bool wholeWords) const override;

    QList< Annotation * > annotations() const override;

    Annotation *addHighlightAnnotation(const QList<QRectF> &boundaries, const QString &text, const QColor &color) override;

    bool removeAnnotation(Annotation *annotation) override;

    bool updateAnnotation(Annotation *annotation, const QString &, const QColor &) override;

    Annotation *addIconAnnotation(const QRectF &ponit, const QString &text) override;

    Annotation *moveIconAnnotation(Annotation *annot, const QRectF &rect) override;

private:
    explicit PDFPage(QMutex *mutex, DPdfPage *page);

    QMutex *m_docMutex = nullptr;

    DPdfPage *m_page = nullptr;

    bool m_wordLoaded = false;

    QList<Word> m_words;
};

class PDFDocument : public Document
{
    Q_DISABLE_COPY(PDFDocument)
public:
    explicit PDFDocument(DPdfDoc *document, const QString &password = QString());

    virtual ~PDFDocument() override;

    int pageCount() const override;

    Page *page(int index) const override;

    QStringList saveFilter() const override;

    QString label(int index) const override;

    bool save() const override;

    bool saveAs(const QString &filePath) const override;

    Outline outline() const override;

    Properties properties() const override;

    QString fileIdentifier() const override;

    /**
     * @brief imageObjectRects 探测页面上图片对象的包围盒
     * 通过 libdeepin-pdfium 导出的 PDFium C API 枚举页面对象实现，
     * 结果与 image(index,width,height) 整页渲染输出像素一一对齐；
     * 探测失败（加密/损坏等）返回空，夜间模式回退整页反色
     */
    QVector<QRectF> imageObjectRects(int index, int width, int height) const;

    static PDFDocument *loadDocument(const QString &filePath, const QString &password, deepin_reader::Document::Error &error);

private:
    DPdfDoc *m_document = nullptr;

    QMutex *m_docMutex = nullptr;

    mutable Properties m_fileProperties;

    mutable Outline m_outline;

    qreal m_xRes = 72;

    qreal m_yRes = 72;

    QString m_password;             // 文档密码（探测句柄加载加密文档时使用）

    mutable void *m_probeDoc = nullptr;     // PDFium FPDF_DOCUMENT，图片对象探测句柄（懒加载）

    mutable bool m_probeFailed = false;     // 探测句柄加载失败标记，避免反复尝试
};
}


#endif // PDFMODEL_H
