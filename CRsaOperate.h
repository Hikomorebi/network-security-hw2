#ifndef CRSAOPERATE_H
#define CRSAOPERATE_H
#include <stdio.h>
#include <stdio.h>
#include <cstring>
#include <cassert>
typedef UINT64 unsigned __int64

class PublicKey
{
public:
    unsigned __int64 nE;
    unsigned __int64 nN;
};
class Paraments
{
public:
    unsigned __int64 d;
    unsigned __int64 n;
    unsigned __int64 e;
};
class RsaParam
{
public:
    unsigned __int64 p;
    unsigned __int64 q;
    unsigned __int64 n;
    unsigned __int64 f;
    unsigned __int64 e;
    unsigned __int64 d;
    unsigned __int64 s;
};
//计算两个数的乘积然后取模
unsigned __int64 MulMod(unsigned __int64 a, unsigned __int64 b, unsigned __int64 n);

//计算某数的若干次幂，然后对结果进行取模运算
unsigned __int64 PowMod(unsigned __int64 base, unsigned __int64 pow, unsigned __int64 n);

//生成随机大质数
long RabinMillerKnl(unsigned __int64 &n);

//多次运行RabinMillerKnl函数，减少误判概率
long RabinMiller(unsigned __int64 &n, long loop = 100);

//最终生成质数
unsigned __int64 RandomPrime(char bits);

//求最大公约数
unsigned __int64 Gcd(unsigned __int64 &p, unsigned __int64 &q);

//私钥生成
unsigned __int64 Euclid(unsigned __int64 e, unsigned __int64 t_n);

RsaParam RsaGetParam(void);

class CRSASection
{
public:
    static UINT64 Encry(unsigned short nSorce, PublicKey &cKey)
    {
        return PowMod(nSorce, cKey.nE, cKey.nN);
    }
    unsigned short Decry(UINT64 nSorce);
    PublicKey GetPublicKey();
};

#endif