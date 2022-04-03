#ifndef CDESOPERATE_H
#define CDESOPERATE_H
#define SUCCESS 1
#define FAIL 0
#define DECRYPT 1
#define ENCRYPT 0
#include <stdio.h>
#include <cstring>
#include <cassert>
typedef int INT32;
typedef char INT8;
typedef unsigned char ULONG8;
typedef unsigned short ULONG16;
typedef unsigned long ULONG32;
static ULONG8 pc_first[64] = {/*初始置换IP*/
                              58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
                              62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
                              57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
                              61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};
static ULONG8 pc_last[64] = {/*逆初始置换IP-1*/
                             40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
                             38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
                             36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
                             34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25};
static ULONG32 pc_by_bit[64] = {
    0x80000000L,
    0x40000000L,
    0x20000000L,
    0x10000000L,
    0x8000000L,
    0x4000000L,
    0x2000000L,
    0x1000000L,
    0x800000L,
    0x400000L,
    0x200000L,
    0x100000L,
    0x80000L,
    0x40000L,
    0x20000L,
    0x10000L,
    0x8000L,
    0x4000L,
    0x2000L,
    0x1000L,
    0x800L,
    0x400L,
    0x200L,
    0x100L,
    0x80L,
    0x40L,
    0x20L,
    0x10L,
    0x8L,
    0x4L,
    0x2L,
    0x1L,
    0x80000000L,
    0x40000000L,
    0x20000000L,
    0x10000000L,
    0x8000000L,
    0x4000000L,
    0x2000000L,
    0x1000000L,
    0x800000L,
    0x400000L,
    0x200000L,
    0x100000L,
    0x80000L,
    0x40000L,
    0x20000L,
    0x10000L,
    0x8000L,
    0x4000L,
    0x2000L,
    0x1000L,
    0x800L,
    0x400L,
    0x200L,
    0x100L,
    0x80L,
    0x40L,
    0x20L,
    0x10L,
    0x8L,
    0x4L,
    0x2L,
    0x1L,
};
static ULONG8 des_P[32] = {
    16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26,
    5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9,
    19, 13, 30, 6, 22, 11, 4, 25};

static ULONG8 des_E[48] = {
    32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1};
static ULONG8 des_S[8][64] = {
    {0xe, 0x0, 0x4, 0xf, 0xd, 0x7, 0x1, 0x4, 0x2, 0xe, 0xf, 0x2, 0xb,
     0xd, 0x8, 0x1, 0x3, 0xa, 0xa, 0x6, 0x6, 0xc, 0xc, 0xb, 0x5, 0x9,
     0x9, 0x5, 0x0, 0x3, 0x7, 0x8, 0x4, 0xf, 0x1, 0xc, 0xe, 0x8, 0x8,
     0x2, 0xd, 0x4, 0x6, 0x9, 0x2, 0x1, 0xb, 0x7, 0xf, 0x5, 0xc, 0xb,
     0x9, 0x3, 0x7, 0xe, 0x3, 0xa, 0xa, 0x0, 0x5, 0x6, 0x0, 0xd},
    {0xf, 0x3, 0x1, 0xd, 0x8, 0x4, 0xe, 0x7, 0x6, 0xf, 0xb, 0x2, 0x3,
     0x8, 0x4, 0xf, 0x9, 0xc, 0x7, 0x0, 0x2, 0x1, 0xd, 0xa, 0xc, 0x6,
     0x0, 0x9, 0x5, 0xb, 0xa, 0x5, 0x0, 0xd, 0xe, 0x8, 0x7, 0xa, 0xb,
     0x1, 0xa, 0x3, 0x4, 0xf, 0xd, 0x4, 0x1, 0x2, 0x5, 0xb, 0x8, 0x6,
     0xc, 0x7, 0x6, 0xc, 0x9, 0x0, 0x3, 0x5, 0x2, 0xe, 0xf, 0x9},
    {0xa, 0xd, 0x0, 0x7, 0x9, 0x0, 0xe, 0x9, 0x6, 0x3, 0x3, 0x4, 0xf,
     0x6, 0x5, 0xa, 0x1, 0x2, 0xd, 0x8, 0xc, 0x5, 0x7, 0xe, 0xb, 0xc,
     0x4, 0xb, 0x2, 0xf, 0x8, 0x1, 0xd, 0x1, 0x6, 0xa, 0x4, 0xd, 0x9,
     0x0, 0x8, 0x6, 0xf, 0x9, 0x3, 0x8, 0x0, 0x7, 0xb, 0x4, 0x1, 0xf,
     0x2, 0xe, 0xc, 0x3, 0x5, 0xb, 0xa, 0x5, 0xe, 0x2, 0x7, 0xc},
    {0x7, 0xd, 0xd, 0x8, 0xe, 0xb, 0x3, 0x5, 0x0, 0x6, 0x6, 0xf, 0x9,
     0x0, 0xa, 0x3, 0x1, 0x4, 0x2, 0x7, 0x8, 0x2, 0x5, 0xc, 0xb, 0x1,
     0xc, 0xa, 0x4, 0xe, 0xf, 0x9, 0xa, 0x3, 0x6, 0xf, 0x9, 0x0, 0x0,
     0x6, 0xc, 0xa, 0xb, 0xa, 0x7, 0xd, 0xd, 0x8, 0xf, 0x9, 0x1, 0x4,
     0x3, 0x5, 0xe, 0xb, 0x5, 0xc, 0x2, 0x7, 0x8, 0x2, 0x4, 0xe},
    {0x2, 0xe, 0xc, 0xb, 0x4, 0x2, 0x1, 0xc, 0x7, 0x4, 0xa, 0x7, 0xb,
     0xd, 0x6, 0x1, 0x8, 0x5, 0x5, 0x0, 0x3, 0xf, 0xf, 0xa, 0xd, 0x3,
     0x0, 0x9, 0xe, 0x8, 0x9, 0x6, 0x4, 0xb, 0x2, 0x8, 0x1, 0xc, 0xb,
     0x7, 0xa, 0x1, 0xd, 0xe, 0x7, 0x2, 0x8, 0xd, 0xf, 0x6, 0x9, 0xf,
     0xc, 0x0, 0x5, 0x9, 0x6, 0xa, 0x3, 0x4, 0x0, 0x5, 0xe, 0x3},
    {0xc, 0xa, 0x1, 0xf, 0xa, 0x4, 0xf, 0x2, 0x9, 0x7, 0x2, 0xc, 0x6,
     0x9, 0x8, 0x5, 0x0, 0x6, 0xd, 0x1, 0x3, 0xd, 0x4, 0xe, 0xe, 0x0,
     0x7, 0xb, 0x5, 0x3, 0xb, 0x8, 0x9, 0x4, 0xe, 0x3, 0xf, 0x2, 0x5,
     0xc, 0x2, 0x9, 0x8, 0x5, 0xc, 0xf, 0x3, 0xa, 0x7, 0xb, 0x0, 0xe,
     0x4, 0x1, 0xa, 0x7, 0x1, 0x6, 0xd, 0x0, 0xb, 0x8, 0x6, 0xd},
    {0x4, 0xd, 0xb, 0x0, 0x2, 0xb, 0xe, 0x7, 0xf, 0x4, 0x0, 0x9, 0x8,
     0x1, 0xd, 0xa, 0x3, 0xe, 0xc, 0x3, 0x9, 0x5, 0x7, 0xc, 0x5, 0x2,
     0xa, 0xf, 0x6, 0x8, 0x1, 0x6, 0x1, 0x6, 0x4, 0xb, 0xb, 0xd, 0xd,
     0x8, 0xc, 0x1, 0x3, 0x4, 0x7, 0xa, 0xe, 0x7, 0xa, 0x9, 0xf, 0x5,
     0x6, 0x0, 0x8, 0xf, 0x0, 0xe, 0x5, 0x2, 0x9, 0x3, 0x2, 0xc},
    {0xd, 0x1, 0x2, 0xf, 0x8, 0xd, 0x4, 0x8, 0x6, 0xa, 0xf, 0x3, 0xb,
     0x7, 0x1, 0x4, 0xa, 0xc, 0x9, 0x5, 0x3, 0x6, 0xe, 0xb, 0x5, 0x0,
     0x0, 0xe, 0xc, 0x9, 0x7, 0x2, 0x7, 0x2, 0xb, 0x1, 0x4, 0xe, 0x1,
     0x7, 0x9, 0x4, 0xc, 0xa, 0xe, 0x8, 0x2, 0xd, 0x0, 0xf, 0x6, 0xc,
     0xa, 0x9, 0xd, 0x0, 0xf, 0x3, 0x3, 0x5, 0x5, 0x6, 0x8, 0xb}};
static ULONG8 keyleft[28] =
    {
        57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
        10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36};
static ULONG8 keyright[28] =
    {
        63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
        14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4};
static ULONG8 lefttable[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
static ULONG8 keychoose[48] = {
    14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};
class CDesOperate
{
private:
    ULONG32 m_arrOutKey[16][2];
    ULONG32 m_arrBufKey[2];
    INT32 HandleData(ULONG32 *left, ULONG8 choice);
    INT32 MakeData(ULONG32 *left, ULONG32 *right, ULONG32 number);
    INT32 MakeKey(ULONG32 *keyleft, ULONG32 *keyright, ULONG32 number);
    INT32 MakeFirstKey(ULONG32 *keyP);

public:
    CDesOperate();
    //~CDesOperate();
    INT32 Encry(char *pPlaintext, int nPlaintextLength, char *pCipherBuffer, int &nCipherBufferLength, char *pKey, int nKeyLength);
    INT32 Decry(char *pCipher, int nCipherBufferLength, char *pPlaintextBuffer, int &nPlaintextBufferLength, char *pKey, int nKeyLength);
};
#endif