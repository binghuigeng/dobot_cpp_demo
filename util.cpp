#include "util.h"

unsigned short CRC16(unsigned char *pBuf, unsigned short len)
{
    unsigned short uCRC = 0xFFFF;
    int i, j;
    for (i = 0; i < len; i++)
    {
        uCRC ^= (*(pBuf + i));
        for (j = 0; j < 8; j++)
        {
            if ((uCRC & 0x0001) == 0x0001)
            {
                uCRC = (uCRC >> 1);
                uCRC ^= 0xA001;
            }
            else
                uCRC = (uCRC >> 1);
        }
    }
    return uCRC;
}
