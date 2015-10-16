#ifndef __STRINGCODE_H__
#define __STRINGCODE_H__

#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

typedef vector<int> IntVector;

/** \brief 获取utf8字符串的unicode码
 *
 * \param s char* 字符串
 * \param bytes int* 当前unicode占用的字节数量
 * \return int unicode值
 *
 */
int utf8ToUnicode(const char *s, int *bytes);


/** \brief 获取unicode码的数组
 *
 * \param s char* 字符串指针
 * \param sz size_t 字符串的长度
 * \param v IntVector 获取的unicode序列
 * \return int
 *
 */
int converToUnicode(const char* s, size_t sz, IntVector* v);

#endif // __STRINGCODE_H__
