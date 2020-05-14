/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
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
#ifndef FILEVIEWWIDGET_H
#define FILEVIEWWIDGET_H

#include "CustomControl/CustomWidget.h"
#include "pdfControl/docview/commonstruct.h"

class DocummentProxy;
class ProxyFileDataModel;
class FindWidget;
/**
 * @brief The SheetBrowserPDF class
 * @brief   文档显示区域
 */
class DocSheet;
class SheetBrowserPDFPrivate;
class SheetBrowserPDF : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SheetBrowserPDF)
public:
    explicit SheetBrowserPDF(DocSheet *sheet, DWidget *parent = nullptr);

    ~SheetBrowserPDF() override;

    void setDoubleShow(bool isShow);

    void rotateLeft();

    void rotateRight();

    void setFileChanged(bool hasChanged);

    void setMouseDefault();

    void setMouseHand();

    bool isMouseHand();

    bool isDoubleShow();

    void setScale(double scale);

    void setFit(int fit);

    void setBookMark(int page, int state);

    void showNoteWidget(int page, const QString &uuid, const int &type = NOTE_HIGHLIGHT);

    void handleFindOperation(const int &, const QString &strFind);

    void setFindWidget(FindWidget *findWidget);

    void copySelectedText();

    void highlightSelectedText();

    void addSelectedTextHightlightAnnotation();

signals:
    void sigFileOpenResult(const QString &, const bool &);

    void sigAnntationMsg(const int &, const QString &);

    void sigDeleteAnntation(const int &, const QString &);

    void sigUpdateThumbnail(const int &);

    void sigFileChanged();

    void sigRotateChanged(int rotate);

    void sigPageChanged(int page);

    void sigFindContantComming(const stSearchRes &);    //搜索到新的词

    void sigFindFinished();                             //搜索完成

public:
    void OpenFilePath(const QString &);

    QString getFilePath();

    void saveOper();

    bool saveData();

    bool saveAsData(QString targetFilePath);

    bool getFileChange();

    DocummentProxy *GetDocProxy();

    void setOper(const int &, const QVariant &);

    QVariant getOper(int type);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void wheelEvent(QWheelEvent *e) override;

    void leaveEvent(QEvent *)override;

private:
    void initConnections();

private:
    SheetBrowserPDFPrivate *const d_ptr = nullptr;
    Q_DECLARE_PRIVATE(SheetBrowserPDF)
};


#endif // FILEVIEWWIDGET_H
