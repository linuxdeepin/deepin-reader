// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/fx_basic.h"
#include "fxcrt_qt.h"

#if _FXM_PLATFORM_ == _FXM_PLATFORM_QT_
IFXCRT_FileAccess* FXCRT_FileAccess_Create()
{
    return new CFXCRT_FileAccess_Qt;
}
void FXCRT_Qt_GetFileMode(quint32 dwModes, QIODevice::OpenMode &nFlags)
{
    if (dwModes & FX_FILEMODE_ReadOnly) {
        nFlags |= QIODevice::ReadOnly;
    } else {
        nFlags |= QIODevice::ReadWrite;
        if (dwModes & FX_FILEMODE_Truncate) {
            nFlags |= QIODevice::Truncate;
        }
    }
}

IFXCRT_FileAccess* IFXCRT_FileAccess::Create() {
  return new CFXCRT_FileAccess_Qt;
}

CFXCRT_FileAccess_Qt::CFXCRT_FileAccess_Qt()
{
}
CFXCRT_FileAccess_Qt::~CFXCRT_FileAccess_Qt()
{
    Close();
}
bool CFXCRT_FileAccess_Qt::Open(const CFX_ByteStringC& fileName, quint32 dwMode)
{
    QString filename = QString::fromUtf8(fileName.c_str(), fileName.GetLength());
    m_file.setFileName(filename);
    QIODevice::OpenMode nFlags;
    FXCRT_Qt_GetFileMode(dwMode, nFlags);
    return m_file.open(nFlags);
}
bool CFXCRT_FileAccess_Qt::Open(const CFX_WideStringC& fileName, quint32 dwMode)
{
    return Open(FX_UTF8Encode(fileName).AsStringC(), dwMode);
}
void CFXCRT_FileAccess_Qt::Close()
{
    m_file.close();
}
void CFXCRT_FileAccess_Qt::Release()
{
    delete this;
}
FX_FILESIZE CFXCRT_FileAccess_Qt::GetSize() const
{
    return m_file.size();
}
FX_FILESIZE CFXCRT_FileAccess_Qt::GetPosition() const
{
    return m_file.pos();
}
FX_FILESIZE CFXCRT_FileAccess_Qt::SetPosition(FX_FILESIZE pos)
{
    ;
    if (!m_file.seek(pos)) {
        return (FX_FILESIZE) - 1;
    }
    return m_file.pos();
}
size_t CFXCRT_FileAccess_Qt::Read(void* pBuffer, size_t szBuffer)
{
    return m_file.read((char*) pBuffer,szBuffer);
}
size_t CFXCRT_FileAccess_Qt::Write(const void* pBuffer, size_t szBuffer)
{
    return m_file.write((char*) pBuffer, szBuffer);
}
size_t CFXCRT_FileAccess_Qt::ReadPos(void* pBuffer, size_t szBuffer, FX_FILESIZE pos)
{
    if (pos >= GetSize()) {
        return 0;
    }
    if (SetPosition(pos) == (FX_FILESIZE) - 1) {
        return 0;
    }
    return Read(pBuffer, szBuffer);
}
size_t CFXCRT_FileAccess_Qt::WritePos(const void* pBuffer, size_t szBuffer, FX_FILESIZE pos)
{
    if (SetPosition(pos) == (FX_FILESIZE) - 1) {
        return 0;
    }
    return Write(pBuffer, szBuffer);
}
bool CFXCRT_FileAccess_Qt::Flush()
{
    return m_file.flush();
}
bool CFXCRT_FileAccess_Qt::Truncate(FX_FILESIZE szFile)
{
    return m_file.resize(szFile);
}
bool FX_File_Exist(const CFX_ByteStringC& fileName)
{
    return QFile::exists(QString::fromUtf8(fileName.c_str(), fileName.GetLength()));
}
bool FX_File_Exist(const CFX_WideStringC& fileName)
{
    return FX_File_Exist(FX_UTF8Encode(fileName).AsStringC());
}
bool FX_File_Delete(const CFX_ByteStringC& fileName)
{
    return remove(fileName.c_str()) > -1;
}
bool FX_File_Delete(const CFX_WideStringC& fileName)
{
    return FX_File_Delete(FX_UTF8Encode(fileName).AsStringC());
}
bool FX_File_Copy(const CFX_ByteStringC& fileNameSrc, const CFX_ByteStringC& fileNameDst)
{
    CFXCRT_FileAccess_Qt src, dst;
    if (!src.Open(fileNameSrc, FX_FILEMODE_ReadOnly)) {
        return false;
    }
    FX_FILESIZE size = src.GetSize();
    if (!size) {
        return false;
    }
    if (!dst.Open(fileNameDst, FX_FILEMODE_Truncate)) {
        return false;
    }
    size_t num = 0;
    uint8_t* pBuffer = FX_Alloc(uint8_t, 32768);
    num = src.Read(pBuffer, 32768);
    while (num) {
        if (dst.Write(pBuffer, num) != num) {
            break;
        }
        num = src.Read(pBuffer, 32768);
    }
    FX_Free(pBuffer);
    return true;
}
bool FX_File_Copy(const CFX_WideStringC& fileNameSrc, const CFX_WideStringC& fileNameDst)
{
    return FX_File_Copy(FX_UTF8Encode(fileNameSrc).AsStringC(), FX_UTF8Encode(fileNameDst).AsStringC());
}
bool FX_File_Move(const CFX_ByteStringC& fileNameSrc, const CFX_ByteStringC& fileNameDst)
{
    return rename(fileNameSrc.c_str(), fileNameDst.c_str());
}
bool FX_File_Move(const CFX_WideStringC& fileNameSrc, const CFX_WideStringC& fileNameDst)
{
    return FX_File_Move(FX_UTF8Encode(fileNameSrc).AsStringC(), FX_UTF8Encode(fileNameDst).AsStringC());
}
#endif
