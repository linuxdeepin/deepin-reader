/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     leiyu <leiyu@live.com>
* Maintainer: leiyu <leiyu@deepin.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SLIDEWIDGET_H
#define SLIDEWIDGET_H

#include <DWidget>
#include <DSpinner>

DWIDGET_USE_NAMESPACE
class DocSheet;
class QTimer;
class SlidePlayWidget;
class QPropertyAnimation;
class SlideWidget : public DWidget
{
    Q_OBJECT
public:
    SlideWidget(DocSheet *docsheet);
    ~SlideWidget() override;

public slots:
    void setWidgetState(bool full);
    void handleKeyPressEvent(const QString &sKey);
    void onImageAniFinished();

private:
    void initControl();
    void initImageControl();

    void playImage();
    QPixmap drawImage(const QPixmap &srcImage);

private slots:
    void onParentDestroyed();
    void onPreBtnClicked();
    void onPlayBtnClicked();
    void onNextBtnClicked();
    void onExitBtnClicked();

    void onImagevalueChanged(const QVariant &variant);
    void onImageShowTimeOut();

    void onFetchImage(int index);
    void onUpdatePageImage(int pageIndex);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_curPageIndex = 0;
    bool m_canRestart = false;
    bool m_parentIsDestroyed = false;
    QPixmap m_lpixmap;
    QPixmap m_rpixmap;

    Qt::WindowStates m_nOldState = Qt::WindowNoState;
    DocSheet *m_docSheet;
    SlidePlayWidget *m_slidePlayWidget;
    DSpinner *m_loadSpinner;

    int m_offset = 0;
    int m_preIndex = 0;
    bool m_blefttoright = true;
    QTimer *m_imageTimer;
    QPropertyAnimation *m_imageAnimation;
};

#endif // SLIDEWIDGET_H
