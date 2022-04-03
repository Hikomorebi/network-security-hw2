#ifndef CRSAOPERATE_H
#define CRSAOPERATE_H
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cassert>
typedef unsigned long long ULONG64;


typedef struct PublicKeys
{
    ULONG64 nE;
    ULONG64 nN;
} PublicKey;
typedef struct Paraments
{
    ULONG64 d;
    ULONG64 n;
    ULONG64 e;
} Parament;
typedef struct RsaParams
{
    ULONG64 p;
    ULONG64 q;
    ULONG64 n;
    ULONG64 f;
    ULONG64 e;
    ULONG64 d;
    ULONG64 s;
} RsaParam;
//模乘运算,计算两个数的乘积然后取模
ULONG64 MulMod(ULONG64 a, ULONG64 b, ULONG64 n);

//模幂运算,计算某数的若干次幂，然后对结果进行取模运算
ULONG64 PowMod(ULONG64 base, ULONG64 pow, ULONG64 n);

//生成随机大质数
long RabinMillerKnl(ULONG64 &n);

//多次运行Rabin-Miller的检验函数，减少误判概率
long RabinMiller(ULONG64 &n, long loop);

//最终生成质数
ULONG64 RandomPrime(char bits);

//求最大公约数
ULONG64 Gcd(ULONG64 &p, ULONG64 &q);

//私钥生成
ULONG64 Euclid(ULONG64 e, ULONG64 t_n);

RsaParam RsaGetParam(void);

//产生随机的DES密钥
void GenerateDesKey(char* randomKey);

class CRSASection
{
private:
    Parament m_cParament;

public:
    //构造函数
    CRSASection();
    //加密函数，使用公钥，通过模幂运算实现计算C = (M ^ e) mod n 的加密过程
    static ULONG64 Encry(unsigned short nSorce, PublicKey &cKey)
    {
        return PowMod(nSorce, cKey.nE, cKey.nN);
    }
    //解密函数，实现M = (C ^ d) mod n的计算
    unsigned short Decry(ULONG64 nSorce);
    //公钥获取函数
    PublicKey GetPublicKey();
};

#endif