// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_SRC_FXCRT_FXCRT_QT_H_
#define CORE_SRC_FXCRT_FXCRT_QT_H_

#include "extension.h"

#if _FXM_PLATFORM_ == _FXM_PLATFORM_QT_

#include <QFile>

class CFXCRT_FileAccess_Qt : public IFXCRT_FileAccess
{
public:
    CFXCRT_FileAccess_Qt();
    virtual ~CFXCRT_FileAccess_Qt();
    virtual bool		Open(const CFX_ByteStringC& fileName, quint32 dwMode);
    virtual bool		Open(const CFX_WideStringC& fileName, quint32 dwMode);
    virtual void		Close();
    virtual void		Release();
    virtual FX_FILESIZE	GetSize() const;
    virtual FX_FILESIZE	GetPosition() const;
    virtual FX_FILESIZE	SetPosition(FX_FILESIZE pos);
    virtual size_t		Read(void* pBuffer, size_t szBuffer);
    virtual size_t		Write(const void* pBuffer, size_t szBuffer);
    virtual size_t		ReadPos(void* pBuffer, size_t szBuffer, FX_FILESIZE pos);
    virtual size_t		WritePos(const void* pBuffer, size_t szBuffer, FX_FILESIZE pos);
    virtual bool		Flush();
    virtual bool		Truncate(FX_FILESIZE szFile);
protected:
    QFile	m_file;
};
#endif

#endif  // CORE_SRC_FXCRT_FXCRT_QT_H_
