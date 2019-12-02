#include "ObjectEventFilter.h"

#include <QDesktopServices>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QDebug>

#include "subjectObserver/ModuleHeader.h"
#include "subjectObserver/MsgHeader.h"

#include "controller/NotifySubject.h"
#include "controller/DataManager.h"

#include "utils/utils.h"
#include "application.h"

ObjectEventFilter::ObjectEventFilter(QObject *parent) : QObject(parent)
{
    m_pFilterList = QStringList() << KeyStr::g_esc << KeyStr::g_f1 << KeyStr::g_f11 << KeyStr::g_del
                    << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2 << KeyStr::g_ctrl_3
                    << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r
                    << KeyStr::g_pgup << KeyStr::g_pgdown << KeyStr::g_ctrl_f << KeyStr::g_ctrl_o
                    << KeyStr::g_ctrl_p << KeyStr::g_ctrl_s << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_smaller
                    << KeyStr::g_alt_1 << KeyStr::g_alt_2 << KeyStr::g_m << KeyStr::g_z
                    << KeyStr::g_ctrl_alt_f << KeyStr::g_ctrl_shift_s
                    << KeyStr::g_down << KeyStr::g_up << KeyStr::g_left << KeyStr::g_right
                    << KeyStr::g_ctrl_e << KeyStr::g_ctrl_b << KeyStr::g_ctrl_i << KeyStr::g_ctrl_l
                    << KeyStr::g_ctrl_shift_slash
                    << KeyStr::g_space << KeyStr::g_ctrl_c;
}

bool ObjectEventFilter::eventFilter(QObject *obj, QEvent *e)
{
    int nType = e->type();
    if (nType == QEvent::KeyPress) {    //  按下
        QKeyEvent *event = static_cast<QKeyEvent *>(e);
        QString key = Utils::getKeyshortcut(event);

        if (m_pFilterList.contains(key)) {
            dealWithKeyEvent(key);
            return true;
        }
    } else if (nType == QEvent::Wheel) {
        QWheelEvent *event = static_cast<QWheelEvent *>(e);
        if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
            QString sFilePath = DataManager::instance()->strOnlyFilePath();
            if (sFilePath != "") {
                if (event->delta() > 0) {
                    notifyMsg(MSG_OPERATION_LARGER);
                } else {
                    notifyMsg(MSG_OPERATION_SMALLER);
                }
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, e);
}

void ObjectEventFilter::dealWithKeyEvent(const QString &key)
{
    if (key == KeyStr::g_f1) {
        onOpenAppHelp();
    } else if (key == KeyStr::g_ctrl_alt_f) {       //  dtk 应用实现
        //onChangeWindowState();
    } else if (key == KeyStr::g_ctrl_shift_slash) {
        displayShortcuts();
    } else {
        QString sFilePath = DataManager::instance()->strOnlyFilePath();
        if (sFilePath != "") {
            notifyMsg(MSG_NOTIFY_KEY_MSG, key);
        }
    }
}

//  显示帮助文档
void ObjectEventFilter::onOpenAppHelp()
{
    QDesktopServices::openUrl(QUrl(Constant::sAcknowledgementLink));
}

void ObjectEventFilter::displayShortcuts()
{
    QRect rect = qApp->desktop()->geometry();
    QPoint pos(rect.x() + rect.width() / 2,
               rect.y() + rect.height() / 2);

    QStringList shortcutnames;
    QStringList windowKeymaps;
    windowKeymaps << KeyStr::g_f11 << KeyStr::g_esc  << KeyStr::g_f1
                  << KeyStr::g_ctrl_f << KeyStr::g_pgup << KeyStr::g_pgdown << KeyStr::g_ctrl_o << KeyStr::g_ctrl_larger
                  << KeyStr::g_ctrl_smaller << KeyStr::g_ctrl_shift_s << KeyStr::g_ctrl_e
                  << KeyStr::g_ctrl_p << KeyStr::g_ctrl_s << KeyStr::g_m << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2
                  << KeyStr::g_ctrl_3 << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r << KeyStr::g_alt_1 << KeyStr::g_alt_2
                  << KeyStr::g_ctrl_b << KeyStr::g_ctrl_i << KeyStr::g_ctrl_l << KeyStr::g_del << KeyStr::g_z
                  << KeyStr::g_ctrl_c << KeyStr::g_ctrl_x << KeyStr::g_ctrl_v << KeyStr::g_ctrl_z << KeyStr::g_ctrl_a << KeyStr::g_ctrl_shift_slash;

    shortcutnames << tr("FullScreen") << tr("Escape") << tr("Help")
                  << tr("Search") << tr("PageUp") << tr("PageDown") << tr("Open") << tr("Enlarge")
                  << tr("Narrow")  << tr("SaveAs") << tr("Export") << tr("Print")
                  << tr("Save") << tr("OpenThumbnail") << tr("AdaptePage") << tr("AdapteHeight")
                  << tr("AdapteWidth") << tr("LeftRotation") << tr("RightRotation") << tr("SelectTool")
                  << tr("HandTool") << tr("AddBookMark") << tr("AddNote") << tr("AddHighlight")
                  << tr("Delete") << tr("Magnifier") << tr("Copy") << tr("Cut") << tr("Paste")
                  << tr("Undo") << tr("Select all") << tr("ShortcutPreview");

    // windowKeymaps=m_pFilterList;
    QJsonObject shortcutObj;
    QJsonArray jsonGroups;

    QJsonObject windowJsonGroup;
    windowJsonGroup.insert("groupName", "Window");
    QJsonArray windowJsonItems;

    int index = 0;
    for (const QString &shortcutname : shortcutnames) {

        QJsonObject jsonItem;
        jsonItem.insert("name", shortcutname);
        jsonItem.insert("value", windowKeymaps.at(index));
        windowJsonItems.append(jsonItem);
        index++;
    }

    windowJsonGroup.insert("groupItems", windowJsonItems);
    jsonGroups.append(windowJsonGroup);
    shortcutObj.insert("shortcut", jsonGroups);

    QJsonDocument doc(shortcutObj);

    QStringList shortcutString;
    QString param1 = "-j=" + QString(doc.toJson().data());
    QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());

    shortcutString << param1 << param2;

    QProcess *shortcutViewProcess = new QProcess();
    shortcutViewProcess->startDetached("deepin-shortcut-viewer", shortcutString);

    connect(shortcutViewProcess, SIGNAL(finished(int)), shortcutViewProcess, SLOT(deleteLater()));
}

void ObjectEventFilter::notifyMsg(const int &msgType, const QString &msgContent)
{
    NotifySubject::getInstance()->notifyMsg(msgType, msgContent);
}
