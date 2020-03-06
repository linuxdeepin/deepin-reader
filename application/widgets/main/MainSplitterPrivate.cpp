#include "MainSplitterPrivate.h"

MainSplitterPrivate::MainSplitterPrivate(MainSplitter *parent) :
    q_ptr(parent)
{

}

void MainSplitterPrivate::qDealWithData(const int &nType, const QString &sValue)
{
    if (nType == MSG_WIDGET_THUMBNAILS_VIEW) {
        setThumbnailState(sValue);
    } else if (nType == MSG_LEFTBAR_STATE) {
        SetLeftWidgetIndex(sValue);
    } else if (nType == MSG_VIEWCHANGE_DOUBLE_SHOW) {
        OnSetViewChange(sValue);
    } else if (nType == MSG_VIEWCHANGE_ROTATE_VALUE) {
        OnSetViewRotate(sValue);
    } else if (nType == MSG_FILE_SCALE) {
        OnSetViewScale(sValue);
    } else if (nType == MSG_VIEWCHANGE_FIT) {
        OnSetViewHit(sValue);
    } else if (nType == MSG_FILE_PAGE_CHANGE) {
        OnSetCurPage(sValue);
    }
}

void MainSplitterPrivate::saveData()
{
    m_pChangeState = -1;
    for (int iLoop = Scale; iLoop < CurPage + 1; iLoop++) {
        dApp->m_pDBService->setHistroyData(m_strPath, iLoop, m_pFileDataModel.qGetData(iLoop));
    }

    dApp->m_pDBService->qSaveData(m_strPath, DB_HISTROY);
}

QString MainSplitterPrivate::qGetPath() const
{
    return m_strPath;
}

void MainSplitterPrivate::qSetPath(const QString &strPath)
{
    m_strPath = strPath;
}

void MainSplitterPrivate::setThumbnailState(const QString &sValue)
{
    m_pFileDataModel.qSetData(Thumbnail, sValue.toInt());
}

void MainSplitterPrivate::SetLeftWidgetIndex(const QString &sValue)
{
    m_pFileDataModel.qSetData(LeftIndex, sValue.toInt());
}

void MainSplitterPrivate::OnSetViewChange(const QString &sValue)
{
    m_pFileDataModel.qSetData(DoubleShow, sValue.toInt());
}

void MainSplitterPrivate::OnSetViewScale(const QString &sValue)
{
    m_pFileDataModel.qSetData(Scale, sValue.toInt());
}

void MainSplitterPrivate::OnSetViewRotate(const QString &sValue)
{
    m_pFileDataModel.qSetData(Rotate, sValue.toInt());
}

void MainSplitterPrivate::OnSetViewHit(const QString &sValue)
{
    m_pFileDataModel.qSetData(Fit, sValue.toInt());

}

void MainSplitterPrivate::OnSetCurPage(const QString &sValue)
{
    m_pFileDataModel.qSetData(CurPage, sValue.toInt());
}

FileDataModel MainSplitterPrivate::qGetFileData() const
{
    return m_pFileDataModel;
}

void MainSplitterPrivate::qSetFileData(const FileDataModel &fd)
{
    m_pFileDataModel = fd;
}

void MainSplitterPrivate::qSetFileChange(const int &nState)
{
    m_pChangeState = nState;
}

int MainSplitterPrivate::qGetFileChange()
{
    return m_pChangeState;
}
