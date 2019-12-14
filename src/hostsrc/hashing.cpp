#include"hashing.h"

int do_hashing(uint16_t key)
{
    std::string res=MD5(std::to_string(key)).toStr();
    res=res.substr(res.length()-8);
    uint32_t numvalue=trans_str_int_16(res);
    return numvalue>>22;
}

uint32_t trans_str_int_16(std::string str)
{
    uint32_t res=0;
    for(int i=0;i<str.length();i++)
    {
        res<<=4;
        int value=0;
        if(str[i]>='0'&&str[i]<='9')
            value=str[i]-48;
        else
            value=str[i]-87;
        res+=value;
            
    }
    return res;
}