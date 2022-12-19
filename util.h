#ifndef UTIL_H
#define UTIL_H

/*
 * @file util.h
 * @brief 声明一些辅助工具函数，例如字节序转换函数、CRC16校验函数和校验和函数
 */

/// CRC校验码计算
unsigned short CRC16(const unsigned char* pBuf, unsigned short len);

#endif // UTIL_H
