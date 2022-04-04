#include "CRSAOperate.h"
#include <stdlib.h>
#include <time.h>

ULONG64 MulMod(ULONG64 a, ULONG64 b, ULONG64 n)
{
    return (a % n) * (b % n) % n;
}

ULONG64 PowMod(ULONG64 base, ULONG64 pow, ULONG64 n)
{
    ULONG64 a = base, b = pow, c = 1;
    while (b)
    {
        while (!(b & 1))
        {
            b >>= 1;
            a = MulMod(a, a, n);
        }
        b--;
        c = MulMod(a, c, n);
    }
    return c;
}

long RabinMillerKnl(ULONG64 &n)
{
    ULONG64 a, q, k, v;
    q = n - 1;
    k = 0;
    while (!(q & 1))
    {
        ++k;
        q >>= 1;
    }
    a = 2 + rand() % (n - 3);
    v = PowMod(a, q, n);
    if (v == 1)
    {
        return 1;
    }
    for (int j = 0; j < k; j++)
    {
        unsigned int z = 1;
        for (int w = 0; w < j; w++)
        {
            z *= 2;
        }
        if (PowMod(a, z * q, n) == n - 1)
            return 1;
    }
    return 0;
}

long RabinMiller(ULONG64 &n, long loop = 100)
{
    for (long i = 0; i < loop; i++)
    {
        if (!RabinMillerKnl(n))
        {
            return 0;
        }
    }
    return 1;
}
ULONG64 RandomPrime(char bits)
{
    ULONG64 base;
    do
    {
        base = (unsigned long)1 << (bits - 1); //保证最高位是 1
        base += rand() % base;                 //再加上一个随机数
        base |= 1;                             //保证最低位是 1,即保证是奇数
    } while (!RabinMiller(base, 30));          //进行拉宾－米勒测试 30 次
    return base;                               //全部通过认为是质数
}

ULONG64 Gcd(ULONG64 &p, ULONG64 &q)
{
    ULONG64 a = p > q ? p : q;
    ULONG64 b = p < q ? p : q;
    ULONG64 t;
    if (p == q)
    {
        return p; //两数相等,最大公约数就是本身
    }
    else
    {
        while (b) //辗转相除法,gcd(a,b)=gcd(b,a-qb)
        {
            a = a % b;
            t = a;
            a = b;
            b = t;
        }
        return a;
    }
}

ULONG64 Euclid(ULONG64 e, ULONG64 t_n)
{
    ULONG64 Max = 0xffffffffffffffff - t_n;
    ULONG64 i = 1;
    while (1)
    {
        if (((i * t_n) + 1) % e == 0)
        {
            return ((i * t_n) + 1) / e;
        }
        i++;
        ULONG64 Tmp = (i + 1) * t_n;
        if (Tmp > Max)
        {
            return 0;
        }
    }
    return 0;
}
RsaParam RsaGetParam()
{
    RsaParam Rsa = {0};
    ULONG64 t;
    Rsa.p = RandomPrime(16); //随机生成两个素数
    Rsa.q = RandomPrime(16);
    Rsa.n = Rsa.p * Rsa.q;
    Rsa.f = (Rsa.p - 1) * (Rsa.q - 1);
    do
    {
        Rsa.e = rand() % 65535;
        Rsa.e |= 1;
    } while (Gcd(Rsa.e, Rsa.f) != 1);
    Rsa.d = Euclid(Rsa.e, Rsa.f);
    Rsa.s = 0;
    t = Rsa.n >> 1;
    while (t)
    {
        Rsa.s++;
        t >>= 1;
    }
    return Rsa;
}

unsigned short CRSASection::Decry(ULONG64 nSorce)
{
    ULONG64 nRes = PowMod(nSorce, m_cParament.d, m_cParament.n);
    unsigned short *pRes = (unsigned short *)&(nRes);
    if (pRes[1] != 0 || pRes[3] != 0 || pRes[2] != 0)
    { // error
        return 0;
    }
    else
    {
        return pRes[0];
    }
}
PublicKey CRSASection::GetPublicKey()
{
    PublicKey cTmp;
    cTmp.nE = this->m_cParament.e;
    cTmp.nN = this->m_cParament.n;
    return cTmp;
}

void GenerateDesKey(char *randomKey)
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < 8; i++)
    {
        randomKey[i] = rand() % 93 + 33;
    }
    return;
}

CRSASection::CRSASection()
{
    RsaParam rp = RsaGetParam();
    m_cParament.d = rp.d;
    m_cParament.e = rp.e;
    m_cParament.n = rp.n;
    
}