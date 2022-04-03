#include "CDesOperate.h"
#include <stdio.h>
#include <iostream>
#include <cassert>

CDesOperate::CDesOperate()
{
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            m_arrOutKey[i][j] = 0;
        }
    }
    for (int i = 0; i < 2; i++)
    {
        m_arrBufKey[i] = 0;
    }
}
INT32 CDesOperate::MakeFirstKey(ULONG32 *keyP)
{
    ULONG32 tempKey[2] = {0, 0};
    ULONG32 *pFirstKey = (ULONG32 *)m_arrBufKey;
    ULONG32 *pTempKey = (ULONG32 *)tempKey;
    memset((ULONG8 *)m_arrBufKey, 0, sizeof(m_arrBufKey));
    memcpy((ULONG8 *)tempKey, (ULONG8 *)keyP, 8);
    memset((ULONG8 *)m_arrOutKey, 0, sizeof(m_arrOutKey));
    int j = 0;
    for (j = 0; j < 28; j++)
    {
        if (keyleft[j] > 32)
        {
            if (pTempKey[1] & pc_by_bit[keyleft[j] - 1])
            {
                pFirstKey[0] |= pc_by_bit[j];
            }
        }
        else
        {
            if (pTempKey[0] & pc_by_bit[keyleft[j] - 1])
            {
                pFirstKey[0] |= pc_by_bit[j];
            }
        }
        if (keyright[j] > 32)
        {
            if (pTempKey[1] & pc_by_bit[keyright[j] - 1])
            {
                pFirstKey[1] |= pc_by_bit[j];
            }
        }
        else
        {
            if (pTempKey[0] & pc_by_bit[keyright[j] - 1])
            {
                pFirstKey[1] |= pc_by_bit[j];
            }
        }
    }

    for (j = 0; j < 16; j++)
    {
        MakeKey(&pFirstKey[0], &pFirstKey[1], j);
    }
    return SUCCESS;
}

INT32 CDesOperate::MakeData(ULONG32 *left, ULONG32 *right, ULONG32 number)
{

    ULONG32 oldRight = *right;
    ULONG8 useBySBox[8] = {0};
    ULONG32 exdesP[2] = {0};
    int j = 0;
    for (; j < 48; j++)
    {
        if (j < 24)
        {
            if (*right & pc_by_bit[des_E[j] - 1])
            {
                exdesP[0] |= pc_by_bit[j];
            }
        }
        else
        {
            if (*right & pc_by_bit[des_E[j] - 1])
            {
                exdesP[1] |= pc_by_bit[j - 24];
            }
        }
    }
    for (j = 0; j < 2; j++)
    {
        exdesP[j] ^= m_arrOutKey[number][j];
    }
    exdesP[1] >>= 8;
    useBySBox[7] = (ULONG8)(exdesP[1] & 0x0000003fL);
    exdesP[1] >>= 6;
    useBySBox[6] = (ULONG8)(exdesP[1] & 0x0000003fL);
    exdesP[1] >>= 6;
    useBySBox[5] = (ULONG8)(exdesP[1] & 0x0000003fL);
    exdesP[1] >>= 6;
    useBySBox[4] = (ULONG8)(exdesP[1] & 0x0000003fL);

    exdesP[0] >>= 8;
    useBySBox[3] = (ULONG8)(exdesP[0] & 0x0000003fL);
    exdesP[0] >>= 6;
    useBySBox[2] = (ULONG8)(exdesP[0] & 0x0000003fL);
    exdesP[0] >>= 6;
    useBySBox[1] = (ULONG8)(exdesP[0] & 0x0000003fL);
    exdesP[0] >>= 6;
    useBySBox[0] = (ULONG8)(exdesP[0] & 0x0000003fL);
    exdesP[0] = 0;
    exdesP[1] = 0;
    *right = 0;
    for (j = 0; j < 7; j++)
    {
        *right |= des_S[j][useBySBox[j]];
        *right <<= 4;
    }
    *right |= des_S[j][useBySBox[j]];
    ULONG32 tempData = 0;
    for (j = 0; j < 32; j++)
    {
        if (*right & pc_by_bit[des_P[j] - 1])
        {
            tempData |= pc_by_bit[j];
        }
    }
    *right = tempData;
    *right ^= *left;
    *left = oldRight;

    return SUCCESS;
}
INT32 CDesOperate::MakeKey(ULONG32 *keyleft, ULONG32 *keyright, ULONG32 number)
{
    ULONG32 tmpkey[2] = {0, 0};
    ULONG32 *Ptmpkey = (ULONG32 *)tmpkey;
    ULONG32 *Poutkey = (ULONG32 *)&m_arrOutKey[number];
    ULONG32 leftandtab[3] = {0x0, 0x80000000, 0xC0000000};
    INT32 j;
    memset((ULONG8 *)tmpkey, 0, sizeof(tmpkey));
    Ptmpkey[0] = *keyleft & leftandtab[lefttable[number]];
    Ptmpkey[1] = *keyright & leftandtab[lefttable[number]];
    if (lefttable[number] == 1)
    {
        Ptmpkey[0] >>= 27;
        Ptmpkey[1] >>= 27;
    }
    else
    {
        Ptmpkey[0] >>= 26;
        Ptmpkey[1] >>= 26;
    }
    Ptmpkey[0] &= 0xfffffff0;
    Ptmpkey[1] &= 0xfffffff0;
    *keyleft <<= lefttable[number];
    *keyright <<= lefttable[number];
    *keyleft |= Ptmpkey[0];
    *keyright |= Ptmpkey[1];
    Ptmpkey[0] = 0;
    Ptmpkey[1] = 0;
    for (j = 0; j < 48; j++)
    {
        if (j < 24)
        {
            if (*keyleft & pc_by_bit[keychoose[j] - 1])
            {
                Poutkey[0] |= pc_by_bit[j];
            }
        }
        else
        {
            if (*keyright & pc_by_bit[(keychoose[j] - 29)])
            {
                Poutkey[1] |= pc_by_bit[j - 24];
            }
        }
    }
    return SUCCESS;
}

INT32 CDesOperate::HandleData(ULONG32 *left, ULONG8 choice)
{
    int number = 0, j = 0;
    ULONG32 *right = &left[1];
    ULONG32 tmp = 0;
    ULONG32 tmpbuf[2] = {0, 0};
    for (j = 0; j < 64; j++)
    {
        if (j < 32)
        {
            if (pc_first[j] > 32)
            {
                if (*right & pc_by_bit[pc_first[j] - 1])
                {
                    tmpbuf[0] |= pc_by_bit[j];
                }
            }
            else
            {
                if (*left & pc_by_bit[pc_first[j] - 1])
                {
                    tmpbuf[0] |= pc_by_bit[j];
                }
            }
        }
        else
        {
            if (pc_first[j] > 32)
            {
                if (*right & pc_by_bit[pc_first[j] - 1])
                {
                    tmpbuf[1] |= pc_by_bit[j];
                }
            }
            else
            {
                if (*left & pc_by_bit[pc_first[j] - 1])
                {
                    tmpbuf[1] |= pc_by_bit[j];
                }
            }
        }
    }

    *left = tmpbuf[0];
    *right = tmpbuf[1];
    tmpbuf[0] = 0;
    tmpbuf[1] = 0;
    if (choice == 0)
    {
        for (number = 0; number < 16; ++number)
            MakeData(left, right, (ULONG32)number);
    }
    else if (choice == 1)
    {
        for (number = 15; number >= 0; --number)
            MakeData(left, right, (ULONG32)number);
    }

    ULONG32 temp;
    temp = *left;
    *left = *right;
    *right = temp;
    for (j = 0; j < 64; j++)
    {
        if (j < 32)
        {
            if (pc_last[j] > 32)
            {
                if (*right & pc_by_bit[pc_last[j] - 1])
                {
                    tmpbuf[0] |= pc_by_bit[j];
                }
            }
            else
            {
                if (*left & pc_by_bit[pc_last[j] - 1])
                {
                    tmpbuf[0] |= pc_by_bit[j];
                }
            }
        }
        else
        {
            if (pc_last[j] > 32)
            {
                if (*right & pc_by_bit[pc_last[j] - 1])
                {
                    tmpbuf[1] |= pc_by_bit[j];
                }
            }
            else
            {
                if (*left & pc_by_bit[pc_last[j] - 1])
                {
                    tmpbuf[1] |= pc_by_bit[j];
                }
            }
        }
    }

    *left = tmpbuf[0];
    *right = tmpbuf[1];
    return SUCCESS;
}

INT32 CDesOperate::Encry(char *pPlaintext, int nPlaintextLength, char *pCipherBuffer,
                         int &nCipherBufferLength, char *pKey, int nKeyLength)
{
    if (nKeyLength != 8)
    {
        return FAIL;
    }
    MakeFirstKey((ULONG32 *)pKey);
    int nLenthofLong = ((nPlaintextLength + 7) / 8) * 2;
    if (nCipherBufferLength < nLenthofLong * 4)
    {
        nCipherBufferLength = nLenthofLong * 4;
        assert(0);
        return FAIL;
    }
    memset(pCipherBuffer, 0, nCipherBufferLength);
    ULONG32 *pOutPutSpace = (ULONG32 *)pCipherBuffer;
    ULONG32 *pSource;
    if (nPlaintextLength != sizeof(ULONG32) * nLenthofLong)
    {
        pSource = new ULONG32[nLenthofLong];
        memset(pSource, 0, sizeof(ULONG32) * nLenthofLong);
        memcpy(pSource, pPlaintext, nPlaintextLength);
    }
    else
    {
        pSource = (ULONG32 *)pPlaintext;
    }
    ULONG32 gp_msg[2] = {0, 0};
    for (int i = 0; i < (nLenthofLong / 2); i++)
    {
        gp_msg[0] = pSource[2 * i];
        gp_msg[1] = pSource[2 * i + 1];
        HandleData(gp_msg, ENCRYPT);
        pOutPutSpace[2 * i] = gp_msg[0];
        pOutPutSpace[2 * i + 1] = gp_msg[1];
    }
    if (pPlaintext != (char *)pSource)
    {
        delete[] pSource;
    }

    return SUCCESS;
}

INT32 CDesOperate::Decry(char *pCipher, int nCipherBufferLength, char *pPlaintextBuffer, int &nPlaintextBufferLength, char *pKey, int nKeyLength)
{
    if (nCipherBufferLength % 8 != 0)
    {
        return FAIL;
    }
    if (nPlaintextBufferLength < nCipherBufferLength)
    {
        assert(0);
        return FAIL;
    }
    if (nKeyLength != 8)
    {
        return FAIL;
    }
    MakeFirstKey((ULONG32 *)pKey);
    memset(pPlaintextBuffer, 0, nPlaintextBufferLength);
    ULONG32 *pSouce = (ULONG32 *)pCipher;
    ULONG32 *pDest = (ULONG32 *)pPlaintextBuffer;
    ULONG32 gp_msg[2] = {0, 0};
    for (int i = 0; i < (nCipherBufferLength / 8); i++)
    {
        gp_msg[0] = pSouce[2 * i];
        gp_msg[1] = pSouce[2 * i + 1];
        HandleData(gp_msg, DECRYPT);
        pDest[2 * i] = gp_msg[0];
        pDest[2 * i + 1] = gp_msg[1];
    }
    return SUCCESS;
}