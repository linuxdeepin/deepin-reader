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

#include "widgets/CustomWidget.h"
#include "pdfControl/docview/CommonStruct.h"

class DocummentProxy;
class ProxyFileDataModel;
class FindWidget;
/**
 * @brief The SheetBrowserPDF class
 * @brief   文档显示区域
 */

class DocSheetPDF;
class SheetBrowserPDFPrivate;
class SheetBrowserPDF : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SheetBrowserPDF)
public:
    explicit SheetBrowserPDF(DocSheetPDF *sheet, DWidget *parent = nullptr);

    ~SheetBrowserPDF() override;

    int rotateLeft();

    int rotateRight();

    void setFileChanged(bool hasChanged);

    void setMouseDefault();

    void setMouseHand();

    bool isDoubleShow();

    void setBookMark(int page, int state);

    void showNoteWidget(int page, const QString &uuid, const int &type = NOTE_HIGHLIGHT);

    void handleFindOperation(const int &, const QString &strFind);

    void setFindWidget(FindWidget *findWidget);

    void copySelectedText();

    void highlightSelectedText();

    void addSelectedTextHightlightAnnotation();

    void defaultFocus();

    void deleteAnntation(const int &, const QString &);

signals:
    void sigFileOpenResult(const QString &, const bool &);

    void sigAnntationMsg(const int &, const QString &);

    void sigUpdateThumbnail(const int &);

    void sigDataChanged();

    void sigPageChanged(int index);

    void sigFindContantComming(const stSearchRes &);    //搜索到新的词

    void sigFindFinished();                             //搜索完成

    void sigSizeChanged(double scaleFactor);

public:
    void OpenFilePath(const QString &);

    QString getFilePath();

    void saveOper();

    bool saveData();

    bool saveAsData(QString targetFilePath);

    bool getFileChange();

    DocummentProxy *GetDocProxy();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void wheelEvent(QWheelEvent *e) override;

    void leaveEvent(QEvent *)override;

private slots:
    void onHandleResize();

private:
    void initConnections();
    double setFit(int fit);
    void setScale(double scale);
    void setDoubleShow(bool isShow);

private:
    friend class DocSheetPDF;
    SheetBrowserPDFPrivate *const d_ptr = nullptr;
    Q_DECLARE_PRIVATE(SheetBrowserPDF)
    QTimer *m_timer = nullptr;
};


#endif // FILEVIEWWIDGET_H
