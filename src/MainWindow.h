/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
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
#ifndef MainWindow_H
#define MainWindow_H

#include <DMainWindow>

class Central;
class DocSheet;
class MainWindow : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

public:
    explicit MainWindow(DMainWindow *parent = nullptr);

    ~MainWindow() override;

    void addSheet(DocSheet *sheet);

    bool hasSheet(DocSheet *sheet);

    void activateSheet(DocSheet *sheet);  //将该sheet显示到最前面

    void closeWithoutSave();

    void setDocTabBarWidget(QWidget *widget);

public:
    void openfiles(const QStringList &filepaths);

    void doOpenFile(const QString &filePath);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void initUI();

    void showDefaultSize();

    void initShortCut();

private slots:
    void onShortCut(const QString &);
    void onUpdateTitleLabelRect();

private:
    QWidget *m_docTabbarWidget = nullptr;
    Central *m_central = nullptr;
    bool m_needSave = true;

public:
    static MainWindow *windowContainSheet(DocSheet *sheet);
    static bool allowCreateWindow();
    static MainWindow *createWindow();
    static QList<MainWindow *> m_list;
};

#endif // MainWindow_H
