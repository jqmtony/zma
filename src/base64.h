#ifndef BASE_64_H
#define BASE_64_H
/**
 * Base64 ����/����
 * @author liruixing
 */
#include <iostream>
#include <string>
#include <cstring>

class Base64{
private:
    std::string _base64_table;
    static const char base64_pad = '=';
public:
    Base64()
    {
        _base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; /*����Base64����ʹ�õı�׼�ֵ�*/
    }
    
    // ���������unsigned���ͣ�����������ĵ�ʱ�����
    std::string encode(const unsigned char * str,int bytes);
    std::string decode(const char *str,int bytes);;
};
#endif