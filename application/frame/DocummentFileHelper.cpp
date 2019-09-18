#include "DocummentFileHelper.h"
#include <DApplication>
#include <QClipboard>
#include "subjectObserver/MsgHeader.h"
#include "controller/DataManager.h"
#include <DFileDialog>
#include "utils/utils.h"

DocummentFileHelper::DocummentFileHelper(QObject *parent) : QObject(parent)
{
    m_pDocummentProxy = DocummentProxy::instance();
    setObserverName("DocummentFileHelper");

    initConnections();

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->addObserver(this);
    }

    m_pNotifySubject = NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

//  保存
void DocummentFileHelper::onSaveFile()
{
    m_pDocummentProxy->save(m_szFilePath, false);
}

//  另存为
void DocummentFileHelper::onSaveAsFile()
{
    DFileDialog dialog;
    QString fileName = dialog.getSaveFileName(nullptr, tr("Save File"),
                                              "/home/",
                                              Utils::getSuffixList());
    if (fileName != "") {
        QString filePath = fileName + ".pdf";
        m_pDocummentProxy->save(filePath, true);
    }
}

//  打开　文件路径
void DocummentFileHelper::slotOpenFile(const QString &filePaths)
{
    if (nullptr != m_pDocummentProxy) {

        QStringList fileList = filePaths.split("@#&wzx",  QString::SkipEmptyParts);
        int nSize = fileList.size();
        if (nSize > 0) {
            QString sPath = fileList.at(0);
            bool rl = m_pDocummentProxy->openFile(DocType_PDF, sPath);
            if (rl) {
                m_szFilePath = sPath;
                DataManager::instance()->setStrOnlyFilePath(sPath);

                m_pDocummentProxy->scaleRotateAndShow(2, RotateType_Normal);

                //  通知 其他窗口， 打开文件成功了！！！
                NotifySubject::getInstance()->sendMsg(MSG_OPERATION_OPEN_FILE_OK);

            } else {
                sendMsg(MSG_OPERATION_OPEN_FILE_FAIL);
            }
        }
    }
}

//  复制
void DocummentFileHelper::onCopySelectContent()
{
    QString sss = "";
    bool rl = m_pDocummentProxy->getSelectTextString(sss);
    if (rl) {
        QClipboard *clipboard = DApplication::clipboard();   //获取系统剪贴板指针
        clipboard->setText(sss);
    }
}

//  放映
void DocummentFileHelper::onFileSlider()
{
    bool bSlideModel = m_pDocummentProxy->showSlideModel();    //  开启幻灯片
    if (bSlideModel) {
        DataManager::instance()->setCurShowState(FILE_SLIDE);
    }
}

void DocummentFileHelper::initConnections()
{
    connect(this, SIGNAL(sigOpenFile(const QString &)), this, SLOT(slotOpenFile(const QString &)));
}

int DocummentFileHelper::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPEN_FILE_PATH:                //  打开文件
        emit sigOpenFile(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SAVE_FILE :          //  保存文件
        onSaveFile();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SAVE_AS_FILE:        //  另存为文件
        onSaveAsFile();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_COPY:           //  复制
        onCopySelectContent();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SLIDE:               //  放映
        onFileSlider();
        return ConstantMsg::g_effective_res;
    case MSG_NOTIFY_KEY_MSG :
        if ("Ctrl+S" == msgContent) {
            onSaveFile();
            return ConstantMsg::g_effective_res;
        }
        if ("Esc" == msgContent) {
            if (DataManager::instance()->CurShowState() == FILE_SLIDE) {    //  幻灯片模式，　需要退出
                m_pDocummentProxy->exitSlideModel();
                DataManager::instance()->setCurShowState(FILE_NORMAL);
            }
        }
    }
    return 0;
}

void DocummentFileHelper::sendMsg(const int &msgType, const QString &msgContent)
{
    m_pMsgSubject->sendMsg(this, msgType, msgContent);
}

void DocummentFileHelper::setObserverName(const QString &name)
{
    m_strObserverName = name;
}
