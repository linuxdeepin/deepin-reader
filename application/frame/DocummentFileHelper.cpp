#include "DocummentFileHelper.h"
#include <DApplication>
#include <QClipboard>
#include "subjectObserver/MsgHeader.h"
#include "controller/DataManager.h"
#include <DFileDialog>
#include "utils/utils.h"
#include <QDesktopServices>
#include "translator/Frame.h"
#include <DMessageBox>
#include "subjectObserver/ModuleHeader.h"

DocummentFileHelper::DocummentFileHelper(QObject *parent)
    : QObject(parent)
{
    m_pDocummentProxy = DocummentProxy::instance();
    setObserverName();

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

DocummentFileHelper::~DocummentFileHelper()
{
    if (m_pDocummentProxy && m_szFilePath != "") {
        m_pDocummentProxy->closeFile();
    }
}

//  ctrl s  保存文件
void DocummentFileHelper::slotSaveFileCtrlS()
{
    m_pDocummentProxy->save(m_szFilePath, true);
}

//  保存
void DocummentFileHelper::slotSaveFile()
{
    m_pDocummentProxy->save(m_szFilePath, true);
}

//  另存为
void DocummentFileHelper::slotSaveAsFile()
{
    QString sFilter = getFileFilter();

    if (sFilter != "") {
        DFileDialog dialog;
        dialog.selectFile(m_szFilePath);
        QString filePath = dialog.getSaveFileName(nullptr, Frame::sSaveFile, m_szFilePath, sFilter);
        if (filePath != "") {
            QString sFilePath = getFilePath(filePath);

            m_pDocummentProxy->save(sFilePath, true);

            m_szFilePath = sFilePath;

            QFileInfo info(m_szFilePath);
            setAppShowTitle(info.baseName());
        }
    }
}

QString DocummentFileHelper::getFileFilter()
{
    QString sFilter = "";
    if (m_nCurDocType == DocType_PDF) {
        sFilter = Constant::sPdf_Filter;
    } else if (m_nCurDocType == DocType_TIFF) {
        sFilter = Constant::sTiff_Filter;
    } else if (m_nCurDocType == DocType_PS) {
        sFilter = Constant::sPs_Filter;
    } else if (m_nCurDocType == DocType_XPS) {
        sFilter = Constant::sXps_Filter;
    } else if (m_nCurDocType == DocType_DJVU) {
        sFilter = Constant::sDjvu_Filter;
    }
    return sFilter;
}

QString DocummentFileHelper::getFilePath(const QString &inputPath)
{
    QString filePath = inputPath;
    if (m_nCurDocType == DocType_PDF) {
        if (!filePath.endsWith(".pdf")) {
            filePath += ".pdf";
        }
    } else if (m_nCurDocType == DocType_TIFF) {
        if (!filePath.endsWith(".tiff")) {
            filePath += ".tiff";
        }
    } else if (m_nCurDocType == DocType_PS) {
        if (!filePath.endsWith(".ps")) {
            filePath += ".ps";
        }
    } else if (m_nCurDocType == DocType_XPS) {
        if (!filePath.endsWith(".xps")) {
            filePath += ".xps";
        }
    } else if (m_nCurDocType == DocType_DJVU) {
        if (!filePath.endsWith(".djvu")) {
            filePath += ".djvu";
        }
    }
    return filePath;
}

void DocummentFileHelper::setCurDocuType(const QString &sCompleteSuffix)
{
    if (sCompleteSuffix == "pdf") {
        m_nCurDocType = DocType_PDF;
    } else if (sCompleteSuffix == "tiff") {
        m_nCurDocType = DocType_TIFF;
    } else if (sCompleteSuffix == "ps") {
        m_nCurDocType = DocType_PS;
    } else if (sCompleteSuffix == "xps") {
        m_nCurDocType = DocType_XPS;
    } else if (sCompleteSuffix == "djvu") {
        m_nCurDocType = DocType_DJVU;
    }
}

//  打开　文件路径
void DocummentFileHelper::slotOpenFile(const QString &filePaths)
{
    //  已经打开了文件，　询问是否需要保存当前打开的文件
    if (m_szFilePath != "") {
        //  是否有操作
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            if (QMessageBox::Yes == DMessageBox::question(nullptr, Frame::sSaveFile, Frame::sSaveFileTitle)) {
                m_pDocummentProxy->save(m_szFilePath, true);
            }
        }
        sendMsg(MSG_CLOSE_FILE);
        m_pDocummentProxy->closeFile();
    }

    QStringList fileList = filePaths.split(Constant::sQStringSep,  QString::SkipEmptyParts);
    int nSize = fileList.size();
    if (nSize > 0) {
        QString sPath = fileList.at(0);

        QFileInfo info(sPath);

        QString sCompleteSuffix = info.completeSuffix();
        setCurDocuType(sCompleteSuffix);

        bool rl = m_pDocummentProxy->openFile(m_nCurDocType, sPath);
        if (rl) {
            m_szFilePath = sPath;
            DataManager::instance()->setStrOnlyFilePath(sPath);
            //  通知 其他窗口， 打开文件成功了！！！
            NotifySubject::getInstance()->sendMsg(MSG_OPERATION_OPEN_FILE_OK);

            setAppShowTitle(info.baseName());
        } else {
            sendMsg(MSG_OPERATION_OPEN_FILE_FAIL);
        }
    }
}

//  设置  应用显示名称
void DocummentFileHelper::setAppShowTitle(const QString &fileName)
{
    QString sTitle = "";
    m_pDocummentProxy->title(sTitle);
    if (sTitle == "") {
        sTitle = fileName;
    }
    sendMsg(MSG_OPERATION_OPEN_FILE_TITLE, sTitle);
}

//  复制
void DocummentFileHelper::slotCopySelectContent()
{
    QString sCopy = "";
    bool rl = m_pDocummentProxy->getSelectTextString(sCopy);
    if (rl && sCopy != "") {
        QClipboard *clipboard = DApplication::clipboard();   //获取系统剪贴板指针
        clipboard->setText(sCopy);
    }
}

/**
 * @brief DocummentFileHelper::slotFileSlider
 * @param nFlag 1 开启放映, 0 退出放映
 */
void DocummentFileHelper::slotFileSlider(const int &nFlag)
{
    if (nFlag == 1) {
        QThread::msleep(100);
        bool bSlideModel = m_pDocummentProxy->showSlideModel();    //  开启幻灯片
        if (bSlideModel) {
            DataManager::instance()->setCurShowState(FILE_SLIDE);
        }
    } else {
        if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
            bool rl = m_pDocummentProxy->exitSlideModel();
            if (rl) {
                DataManager::instance()->setCurShowState(FILE_NORMAL);
            }
        }
    }
}

//  文档　跳转页码　．　打开浏览器
void DocummentFileHelper::onClickPageLink(Page::Link *pLink)
{
    Page::LinkType_EM linkType = pLink->type;
    if (linkType == Page::LinkType_NULL) {

    } else if (linkType == Page::LinkType_Goto) {
        int page = pLink->page;
        m_pDocummentProxy->pageJump(page);
    } else if (linkType == Page::LinkType_GotoOtherFile) {

    } else if (linkType == Page::LinkType_Browse) {
        QString surlOrFileName = pLink->urlOrFileName;
        QDesktopServices::openUrl(QUrl(surlOrFileName, QUrl::TolerantMode));
    } else if (linkType == Page::LinkType_Execute) {

    }
}


void DocummentFileHelper::initConnections()
{
    connect(this, SIGNAL(sigOpenFile(const QString &)), this, SLOT(slotOpenFile(const QString &)));
    connect(this, SIGNAL(sigSaveFile()), this, SLOT(slotSaveFile()));
    connect(this, SIGNAL(sigSaveAsFile()), this, SLOT(slotSaveAsFile()));
    connect(this, SIGNAL(sigFileSlider(const int &)), this, SLOT(slotFileSlider(const int &)));
    connect(this, SIGNAL(sigCopySelectContent()), this, SLOT(slotCopySelectContent()));
    connect(this, SIGNAL(sigSaveFileCtrlS()), this, SLOT(slotSaveFileCtrlS()));
}

int DocummentFileHelper::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPEN_FILE_PATH:                //  打开文件
        emit sigOpenFile(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SAVE_FILE :          //  保存文件
        emit sigSaveFile();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SAVE_AS_FILE:        //  另存为文件
        emit sigSaveAsFile();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_COPY:           //  复制
        emit sigCopySelectContent();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SLIDE:               //  放映
        emit sigFileSlider(1);
        return ConstantMsg::g_effective_res;
    case MSG_NOTIFY_KEY_MSG :
        if ("Ctrl+S" == msgContent) {
            emit sigSaveFileCtrlS();
            return ConstantMsg::g_effective_res;
        }
        if ("Esc" == msgContent) {
            emit sigFileSlider(0);
        }
    }
    return 0;
}

void DocummentFileHelper::sendMsg(const int &msgType, const QString &msgContent)
{
    m_pMsgSubject->sendMsg(this, msgType, msgContent);
}

void DocummentFileHelper::setObserverName()
{
    m_strObserverName = "DocummentFileHelper";
}
