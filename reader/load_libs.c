/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* Maintainer: shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
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
#include "load_libs.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <dlfcn.h>
void PrintError(){
    char *error;
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
}
static LoadLibNames g_ldnames;
static LoadLibs *pLibs = NULL;
static LoadLibs *newClass(void)
{
    fprintf(stderr, "INFO: Allocating LoadLibs structure\n");
    pLibs = (LoadLibs *)malloc(sizeof(LoadLibs));
    if (!pLibs) {
        fprintf(stderr, "ERROR: Failed to allocate LoadLibs\n");
        return NULL;
    }
    pLibs->m_document_clip_copy = NULL;
    pLibs->m_document_close = NULL;
//    RTLD_NOW：在dlopen返回前，解析出全部没有定义的符号，解析不出来返回NULL。
//    RTLD_LAZY：暂缓决定，等有需要时再解出符号
    void *handle = NULL;
    if (g_ldnames.chDocumentPr != NULL) {
        fprintf(stderr, "INFO: Loading library: %s\n", g_ldnames.chDocumentPr);
        handle = dlopen(g_ldnames.chDocumentPr, RTLD_LAZY);
        if (handle == NULL) {
            fprintf(stderr, "ERROR: Failed to load library\n");
            PrintError();
        }
    } else {
        fprintf(stderr, "ERROR: Library path is NULL\n");
    }
    if (handle == NULL) {
        return pLibs;
    }

    fprintf(stderr, "INFO: Resolving symbol: document_clip_copy\n");
    pLibs->m_document_clip_copy = (uos_document_clip_copy)dlsym(handle, "document_clip_copy");
    PrintError();
    fprintf(stderr, "INFO: Resolving symbol: document_close\n");
    pLibs->m_document_close = (uos_document_close)dlsym(handle, "document_close");
    PrintError();

    assert(pLibs != NULL);
    return pLibs;
}

/**
 * 饿汉式
 * 支持延迟加载，但是为了多线程安全，性能有所降低
 * 注意：方法内部要加锁，防止多线程多次创建
 * */
LoadLibs *getLoadLibsInstance()
{
    static pthread_mutex_t mutex;
    static int mutex_initialized = 0;
    
    if (!mutex_initialized) {
        pthread_mutex_init(&mutex, NULL);
        mutex_initialized = 1;
        fprintf(stderr, "INFO: Initialized mutex for thread safety\n");
    }
    
    //双检锁
    if (pLibs == NULL) {
        fprintf(stderr, "INFO: Acquiring mutex for LoadLibs initialization\n");
        pthread_mutex_lock(&mutex);
        if (pLibs == NULL) {
            fprintf(stderr, "INFO: Creating new LoadLibs instance\n");
            pLibs = newClass();
        }
        fprintf(stderr, "INFO: Releasing mutex after LoadLibs initialization\n");
        pthread_mutex_unlock(&mutex);
    }

    return pLibs;
}

void setLibNames(LoadLibNames tmp)
{
    fprintf(stderr, "INFO: Setting library names\n");
    if(tmp.chDocumentPr == NULL) {
        fprintf(stderr, "WARNING: Received NULL document printer library path\n");
        g_ldnames.chDocumentPr = NULL;
    } else {
        fprintf(stderr, "INFO: Copying library path: %s\n", tmp.chDocumentPr);
        g_ldnames.chDocumentPr = (char*)malloc(strlen(tmp.chDocumentPr)+1);
        if (!g_ldnames.chDocumentPr) {
            fprintf(stderr, "ERROR: Failed to allocate memory for library path\n");
            return;
        }
        strcpy(g_ldnames.chDocumentPr,tmp.chDocumentPr);
    }
}

