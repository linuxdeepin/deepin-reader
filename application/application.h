/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <DApplication>
#include "DBService.h"
#include "ModelService.h"

class AppInfo;

#if defined(dApp)
#undef dApp
#endif
#define dApp (static_cast<Application *>(QCoreApplication::instance()))

DWIDGET_USE_NAMESPACE

class Application : public DApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    void setSreenRect(const QRect &rect);
    void adaptScreenView(int &w, int &h);
    inline void setScale(const double &scale)
    {
        m_dScal = scale;
    }
    double scale() const
    {
        return m_dScal;
    }
    void setFlush(const bool &flush)
    {
        m_bFlush = flush;
    }
    bool bFlush()
    {
        return m_bFlush;
    }

protected:
    void handleQuitAction() override;

public:
    DBService           *m_pDBService = nullptr;
    ModelService        *m_pModelService = nullptr;
    AppInfo             *m_pAppInfo = nullptr;
    double m_dScal{1.0};//左侧列表缩放比例
    bool m_bFlush{true};//是否自适应宽

private:
    void initCfgPath();
    void initChildren();
    void initI18n();
};

#endif  // APPLICATION_H_
