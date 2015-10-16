#ifndef __STRINGCODE_H__
#define __STRINGCODE_H__

#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

typedef vector<int> IntVector;

/** \brief ��ȡutf8�ַ�����unicode��
 *
 * \param s char* �ַ���
 * \param bytes int* ��ǰunicodeռ�õ��ֽ�����
 * \return int unicodeֵ
 *
 */
int utf8ToUnicode(const char *s, int *bytes);


/** \brief ��ȡunicode�������
 *
 * \param s char* �ַ���ָ��
 * \param sz size_t �ַ����ĳ���
 * \param v IntVector ��ȡ��unicode����
 * \return int
 *
 */
int converToUnicode(const char* s, size_t sz, IntVector* v);

#endif // __STRINGCODE_H__
