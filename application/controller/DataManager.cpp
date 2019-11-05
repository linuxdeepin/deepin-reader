#include "DataManager.h"
#include "application.h"

#include <DWidget>
#include <DGuiApplicationHelper>
DWIDGET_USE_NAMESPACE

DataManager::DataManager(QObject *parent) :
    QObject (parent)
{
    m_strOnlyFilePath = "";
    m_listColor.append(QColor("#FFA503"));
    m_listColor.append(QColor("#FF1C49"));
    m_listColor.append(QColor("#9023FC"));
    m_listColor.append(QColor("#3468FF"));
    m_listColor.append(QColor("#00C7E1"));
    m_listColor.append(QColor("#05EA6B"));
    m_listColor.append(QColor("#FEF144"));
    m_listColor.append(QColor("#D5D5D1"));
}

QString DataManager::strOnlyFilePath() const
{
    return m_strOnlyFilePath;
}

void DataManager::setStrOnlyFilePath(const QString &strOnlyFilePath)
{
    if (strOnlyFilePath == "") {
        m_bIsUpdate = true;
    } else {
        m_bIsUpdate = false;
    }
    dApp->dbM->setStrFilePath(strOnlyFilePath);

    m_strOnlyFilePath = strOnlyFilePath;
}

int DataManager::CurShowState() const
{
    return m_nCurShowState;
}

void DataManager::setCurShowState(int nCurShowState)
{
    m_nCurShowState = nCurShowState;
}

bool DataManager::bIsUpdate() const
{
    return m_bIsUpdate;
}

void DataManager::setBIsUpdate(bool bIsUpdate)
{
    m_bIsUpdate = bIsUpdate;
}

QList<QColor> DataManager::getLightColorList()
{
    return m_listColor;
}

void DataManager::setMousePressLocal(const bool &highLight, const QPoint &point)
{
    m_bIsHighLight = highLight;

    QPoint t_point;
    int t_w = point.x();
    int t_h = point.y();

    int screenW =  m_screenRect.width();
    int screenH =  m_screenRect.height();

    int noteWidgetW = m_smallNoteSize.width();
    int noteWidgetH = m_smallNoteSize.height();

    if(t_h + noteWidgetH > screenH){
        t_h = screenH - noteWidgetH;
    }

    if(t_w+noteWidgetW > screenW){
        t_w -= noteWidgetW;
    }

    t_point.setX(t_w);
    t_point.setY(t_h);

    m_point = t_point;
}

QString DataManager::gettrCurrentTheme() const
{
    return m_strCurrentTheme;
}

void DataManager::settrCurrentTheme(const QString &strCurrentTheme)
{
    m_strCurrentTheme = strCurrentTheme;
}
