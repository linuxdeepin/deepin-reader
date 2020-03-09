#include "FileViewWidgetPrivate.h"
#include "docview/docummentproxy.h"
#include "FileViewWidget.h"
#include "application.h"
#include "business/AppInfo.h"
FileViewWidgetPrivate::FileViewWidgetPrivate(FileViewWidget *parent):
    q_ptr(parent)
{
    m_filechanged = false;
}

void FileViewWidgetPrivate::AddHighLight(const QString &msgContent)
{
    Q_Q(FileViewWidget);
    DocummentProxy *_proxy = q->getDocumentProxy();
    if (_proxy) {
        QString nSx = "", nSy = "", nEx = "", nEy = "";
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 4) {
            nSx = sList.at(0);
            nSy = sList.at(1);
            nEx = sList.at(2);
            nEy = sList.at(3);
        } else if (sList.size() == 5) {
            nSx = sList.at(0);
            nSy = sList.at(1);
            nEx = sList.at(2);
            nEy = sList.at(3);
            int iIndex = sList.at(4).toInt();

            QColor color = dApp->m_pAppInfo->getLightColorList().at(iIndex);
            dApp->m_pAppInfo->setSelectColor(color);
        }

        QPoint pStartPoint(nSx.toInt(), nSy.toInt());
        QPoint pEndPoint(nEx.toInt(), nEy.toInt());
        QColor color = dApp->m_pAppInfo->selectColor();

        if (!_proxy->addAnnotation(pStartPoint, pEndPoint, color).isEmpty())
            m_filechanged = true;
    }
}

void FileViewWidgetPrivate::AddHighLightNote(const QString &msgContent)
{
    Q_Q(FileViewWidget);
    QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    if (sList.size() == 6) {
        QString nSx = sList.at(0);
        QString nSy = sList.at(1);
        QString nEx = sList.at(2);
        QString nEy = sList.at(3);
        QString sNote = sList.at(4);
        QString sPage = sList.at(5);
        DocummentProxy *_proxy = q->getDocumentProxy();
        if (_proxy) {
            QPoint pStartPoint(nSx.toInt(), nSy.toInt());
            QPoint pEndPoint(nEx.toInt(), nEy.toInt());
            QColor color = dApp->m_pAppInfo->selectColor();

            QString strUuid = _proxy->addAnnotation(pStartPoint, pEndPoint, color);
            if (strUuid != "") {
                _proxy->setAnnotationText(sPage.toInt(), strUuid, sNote);
                m_filechanged = true;

                QJsonObject notifyObj;
                notifyObj.insert("content", strUuid);
                notifyObj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET + Constant::sQStringSep + NOTE_WIDGET);
                QJsonDocument notifyDoc(notifyObj);
                notifytoframe(MSG_NOTE_ADD_ITEM, notifyDoc.toJson(QJsonDocument::Compact));
            }
        }
    }
}

void FileViewWidgetPrivate::RemoveHighLight(const QString &msgContent)
{
    Q_Q(FileViewWidget);
    DocummentProxy *_proxy = q->getDocumentProxy();
    if (_proxy) {
        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (contentList.size() == 2) {
            QString sX = contentList.at(0);
            QString sY = contentList.at(1);

            QPoint tempPoint(sX.toInt(), sY.toInt());

            QString sUuid = _proxy->removeAnnotation(tempPoint);
            if (sUuid != "") {
                m_filechanged = true;
                QJsonObject notifyObj;
                notifyObj.insert("content", sUuid);
                notifyObj.insert("to", MAIN_TAB_WIDGET + Constant::sQStringSep + LEFT_SLIDERBAR_WIDGET + Constant::sQStringSep + NOTE_WIDGET);

                QJsonDocument notifyDoc(notifyObj);
                notifytoframe(MSG_NOTE_DELETE_ITEM, notifyDoc.toJson(QJsonDocument::Compact));
            }
        }
    }
}

void FileViewWidgetPrivate::ChangeAnnotationColor(const QString &msgContent)
{
    Q_Q(FileViewWidget);
    DocummentProxy *_proxy = q->getDocumentProxy();
    if (_proxy) {
        QStringList contentList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (contentList.size() == 3) {
            QString sIndex = contentList.at(0);
            QString sUuid = contentList.at(1);
            QString sPage = contentList.at(2);

            int iIndex = sIndex.toInt();
            QColor color = dApp->m_pAppInfo->getLightColorList().at(iIndex);
            _proxy->changeAnnotationColor(sPage.toInt(), sUuid, color);     //  更新高亮顏色,  是对文档进行了操作
            m_filechanged = true;
        }
    }

}

void FileViewWidgetPrivate::RemoveAnnotation(const QString &msgContent)
{
    Q_Q(FileViewWidget);
    DocummentProxy *_proxy = q->getDocumentProxy();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 2) {
            QString sUuid = sList.at(0);
            QString sPage = sList.at(1);
            _proxy->removeAnnotation(sUuid, sPage.toInt());
            m_filechanged = true;
        }
    }

}

void FileViewWidgetPrivate::UpdateAnnotationText(const QString &msgContent)
{
    Q_Q(FileViewWidget);
    DocummentProxy *_proxy = q->getDocumentProxy();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString sText = sList.at(1);
            QString sPage = sList.at(2);

            _proxy->setAnnotationText(sPage.toInt(), sUuid, sText);
            m_filechanged = true;
        }
    }

}

void FileViewWidgetPrivate::AddPageIconAnnotation(const QString &msgContent)
{
    Q_Q(FileViewWidget);
    DocummentProxy *_proxy = q->getDocumentProxy();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString  sNote = sList.at(1);
            QString sPage = sList.at(2);
            _proxy->setAnnotationText(sPage.toInt(), sUuid, sNote);
            QString t_str = sUuid.trimmed() + Constant::sQStringSep + sNote.trimmed() + Constant::sQStringSep + sPage;
            m_filechanged = true;
        }
    }
}

void FileViewWidgetPrivate::DeletePageIconAnnotation(const QString &msgContent)
{
    Q_Q(FileViewWidget);

    DocummentProxy *_proxy = q->getDocumentProxy();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 2) {
            QString sUuid = sList.at(0);
            QString sPage = sList.at(1);
            _proxy->removeAnnotation(sUuid, sPage.toInt());
            m_filechanged = true;
        }
    }
}

void FileViewWidgetPrivate::UpdatePageIconAnnotation(const QString &msgContent)
{
    Q_Q(FileViewWidget);
    DocummentProxy *_proxy = q->getDocumentProxy();
    if (_proxy) {
        QStringList sList = msgContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
        if (sList.size() == 3) {
            QString sUuid = sList.at(0);
            QString sNote = sList.at(1);
            QString sPage = sList.at(2);
            _proxy->setAnnotationText(sPage.toInt(), sUuid, sNote);
            m_filechanged = true;
        }
    }
}

void FileViewWidgetPrivate::notifytoframe(const int &type, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(type, msgContent);
}
