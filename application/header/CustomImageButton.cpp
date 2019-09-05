#include "CustomImageButton.h"
#include <QDebug>
#include <QPainter>
#include "utils.h"

CustomImageButton::CustomImageButton(DWidget *parent)
    : DPushButton (parent)
{
    this->setFixedSize(QSize(36, 36));
}

bool CustomImageButton::event(QEvent *e)
{
    int nType = e->type();
//    if (!this->isChecked()) {
    if (nType == QEvent::HoverEnter) {
        m_nBtnState = NORMAL_HOVER;
    } else if (nType == QEvent::HoverLeave) {
        m_nBtnState = NORMAL_STATE;
//        }
    } else if ( nType == QEvent::MouseButtonPress) {
        m_nBtnState = NORMAL_PRESS;
    } else if (nType == QEvent::MouseButtonRelease ) {
        m_nBtnState = NORMAL_STATE;
    }
    return DPushButton::event(e);
}

void CustomImageButton::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    if (this->isChecked()) {
        QPixmap _pixmap;
        if (QFile::exists(m_strCheckPic)) {
            _pixmap = Utils::renderSVG(m_strCheckPic, this->size());

//            painter.fillRect(this->geometry(), QColor(255, 0, 0));
            painter.drawPixmap(0, 0, _pixmap);
        } else {
            _pixmap = Utils::renderSVG(m_strNormalPic, QSize(36, 36));
            painter.fillRect(this->geometry(), QColor("#0081FF"));
            painter.drawPixmap(0, 0, _pixmap);
        }
    } else {
        if (m_nBtnState == NORMAL_STATE) {
            QPixmap _pixmap = Utils::renderSVG(m_strNormalPic, QSize(36, 36));
            painter.drawPixmap(0, 0, _pixmap);
        } else if (m_nBtnState == NORMAL_HOVER) {
            QPixmap _pixmap;
            if (QFile::exists(m_strHoverPic)) {
                _pixmap = Utils::renderSVG(m_strHoverPic, QSize(36, 36));
                painter.drawPixmap(0, 0, _pixmap);
            } else {
                _pixmap = Utils::renderSVG(m_strNormalPic, QSize(36, 36));
                painter.fillRect(this->geometry(), QColor(0, 0, 0, 5));
                painter.drawPixmap(0, 0, _pixmap);
            }
        } else if (m_nBtnState == NORMAL_PRESS) {
            QPixmap _pixmap;
            if (QFile::exists(m_strPressPic)) {
                _pixmap = Utils::renderSVG(m_strPressPic, QSize(36, 36));
                painter.drawPixmap(0, 0, _pixmap);
            } else {
                _pixmap = Utils::renderSVG(m_strNormalPic, QSize(36, 36));
                painter.fillRect(this->geometry(), QColor(1, 1, 1, 40));
                painter.drawPixmap(0, 0, _pixmap);
            }
        }
    }
}


void CustomImageButton::setStrCheckPic(const QString &strCheckPic)
{
    m_strCheckPic = strCheckPic;
}

void CustomImageButton::setStrPressPic(const QString &strPressPic)
{
    m_strPressPic = strPressPic;
}

void CustomImageButton::setStrHoverPic(const QString &strHoverPic)
{
    m_strHoverPic = strHoverPic;
}

void CustomImageButton::setStrNormalPic(const QString &strNormalPic)
{
    m_strNormalPic = strNormalPic;
}
