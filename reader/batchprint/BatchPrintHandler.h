// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BATCH_PRINT_HANDLER_H
#define BATCH_PRINT_HANDLER_H

#include <QObject>

/**
 * @brief The BatchPrintHandler class
 * 批量打印处理器
 * 从文件管理器右键菜单触发，不显示 UI
 * 通过 CUPS lp 命令将打印任务推入系统队列
 */
class BatchPrintHandler : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(BatchPrintHandler)

public:
    /**
     * @brief runBatchPrint 批量打印入口（不创建 UI）
     * @param filePaths 文件路径列表
     * @return 退出码（0=成功，非0=部分或全部失败）
     */
    static int runBatchPrint(const QStringList &filePaths);

private:
    /**
     * @brief isSupportedFormat 检查文件格式是否支持批量打印
     * @param filePath 文件路径
     * @return 是否支持
     */
    static bool isSupportedFormat(const QString &filePath);

    /**
     * @brief printDocument 打印单个文档
     * @param filePath 文件路径
     * @return 是否成功
     */
    static bool printDocument(const QString &filePath);

    /**
     * @brief printWithLp 使用 lp 命令推送打印任务到 CUPS
     * @param filePath 文件路径
     * @return 是否成功
     */
    static bool printWithLp(const QString &filePath);
};

#endif // BATCH_PRINT_HANDLER_H
