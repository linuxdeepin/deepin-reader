/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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

#ifndef DOCFILEHELPER_H
#define DOCFILEHELPER_H

#include "HelperImpl.h"

class DocFileHelper : public HelperImpl
{
    Q_OBJECT
public:
    explicit DocFileHelper(QObject *parent = nullptr);

public:
    QString qDealWithData(const int &msgType, const QString &msgContent) override;

private:
    void __PageJump(const int &pagenum);
    void __PageJumpByMsg(const int &);

//    bool onOpenFile(const QString &filePaths);

    void onSaveFile();
    void onSaveAsFile();

    void __FileCtrlCContent();
    //  复制
    void OnCopySelectContent(const QString &sCopy);

    void notifyMsg(const int &msgType, const QString &msgContent = "") ;

    void SetFileSlider(const int &nFlag);

    void CloseFile(const int &iType, const QString &sPath);
};


#endif // DOCFILEHELPER_H
