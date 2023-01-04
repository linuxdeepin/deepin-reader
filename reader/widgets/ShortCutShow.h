/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#ifndef SHORTCUTSHOW_H
#define SHORTCUTSHOW_H

#include <QObject>
#include <QMap>

/**
 * @brief The ShortCutShow class
 * @brief   显示快捷键预览
 */

class DocSheet;
class ShortCutShow : public QObject
{
    Q_OBJECT
public:
    explicit ShortCutShow(QObject *parent = nullptr);

    /**
     * @brief setSheet
     * 设置文档对象
     * @param sheet
     */
    void setSheet(DocSheet *sheet);

public:
    /**
     * @brief show
     * 显示
     */
    void show();

private:
    /**
     * @brief initPDF
     * 初始化PDF快捷玉兰
     */
    void initPDF();

    /**
     * @brief initDJVU
     * 初始化DJVU快捷预览
     */
    void initDJVU();

private:
    /**
     * @brief The ShortCutType enum 快捷键类型
     */
    enum class ShortCutType  {
        Settings,   //设置
        File,       //文件
        Display,    //视图
        Tools,      //工具
        Edit        //编辑
    };
    /**
     * @brief The KeyDataList class 快捷键的数据结构，list<pair<快捷键、描述>>
     */
    class KeyDataList : public QList<std::pair<QString,QString>>
    {
    public:
        KeyDataList(){}
        KeyDataList(std::initializer_list<std::pair<QString,QString> > list) {
            for(auto p : list) {
                append(p);
            }
        }
        /**
         * @brief removeKey 移除快捷键
         */
        void removeKey(const QString &key){
            for(int k = 0; k < count(); k ++) {
                if(at(k).first == key) {
                    removeAt(k);
                    break;
                }
            }
        }

    };
    QMap<ShortCutType, KeyDataList> m_shortcutMap;
};

#endif // SHORTCUTSHOW_H
