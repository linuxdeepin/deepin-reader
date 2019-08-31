#ifndef MAINSHOWSPLITTER_H
#define MAINSHOWSPLITTER_H

#include <DSplitter>
#include <QResizeEvent>
#include "LeftSidebarWidget.h"
#include "FileViewWidget.h"
#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainShowDataWidget class
 * @brief   主要用于显示文档 和 操作界面, 分割器
 */

class MainShowSplitter : public DSplitter, public IThemeObserver
{
    Q_OBJECT
public:
    MainShowSplitter(DWidget *parent = nullptr);
    ~MainShowSplitter() override;

signals:
    void sigFileMagnifyingState(const bool&) const;

private:
    void initWidgets();

private slots:
    void setSidebarVisible(const bool&) const;
    void setFileHandShapeState(const bool&) const;
//    void setFileMagnifyingState(const bool&) const;

private:
    LeftSidebarWidget   *m_pLeftShowWidget = nullptr;       //  侧边栏
    FileViewWidget      *m_pFileViewWidget = nullptr;       //  显示文档区域

    ThemeSubject        *m_pThemeSubject = nullptr;

    // IObserver interface
public:
    int update(const QString &) override;
};

#endif // MAINSHOWDATAWIDGET_H
