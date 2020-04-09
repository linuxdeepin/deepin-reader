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

    void setRotateLeft();

    void setRotateRight();

    void setFileChanged(bool hasChanged);

    void setMouseDefault();

    void setMouseHand();

    bool isMouseHand();

    bool isDoubleShow();

    void setScale(double scale);

    void setFit(int fit);

    void setBookMark(int page, int state);

signals:
    void sigFileOpenResult(const QString &, const bool &);
    void sigFindOperation(const int &);
    void sigAnntationMsg(const int &, const QString &);
    void sigDeleteAnntation(const int &, const QString &);
    void sigUpdateThumbnail(const int &);

    void sigFileChanged(bool hasChanged);

    void sigRotateChanged(int rotate);

    void sigPageChanged(int page);
public:
    int dealWithData(const int &, const QString &) override;
    int qDealWithShortKey(const QString &) override;

    void OpenFilePath(const QString &);
    QString getFilePath();
    void saveData();
    void ShowFindWidget();

    bool getFileChange();

    DocummentProxy     *GetDocProxy();

    void setData(const int &, const QString &);

    FileDataModel qGetFileData();
    void qSetFileData(const FileDataModel &);

protected:
    void initWidget() override;

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *e) override;
    void leaveEvent(QEvent *)override;


private slots:
    void SlotFindOperation(const int &, const QString &strFind);

private:
    void initConnections();

private:
    FindWidget *m_pFindWidget = nullptr;

private:
    SheetBrowserPDFPrivate *const d_ptr = nullptr;
    Q_DECLARE_PRIVATE(SheetBrowserPDF)
};


#endif // FILEVIEWWIDGET_H
