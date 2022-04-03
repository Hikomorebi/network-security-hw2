#include "CRSAOperate.h"

unsigned __int64 MulMod(unsigned __int64 a, unsigned __int64 b, unsigned __int64 n)
{
    return (a % n) * (b % n) % n;
}

unsigned __int64 PowMod(unsigned __int64 base, unsigned __int64 pow, unsigned __int64 n)
{
    unsigned __int64 a = base, b = pow, c = 1;
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

long RabinMillerKnl(unsigned __int64 &n)
{
    unsigned __int64 a, q, k, v;
    q = n - 1;
    k = 0;
    while (!(q & 1))
    {
        ++k;
        q >>= 1;
    }
    a = 2 + cRadom.Random(n - 3);
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

long RabinMiller(unsigned __int64 &n, long loop = 100)
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
unsigned __int64 RandomPrime(char bits)
{
    unsigned __int64 base;
    do
    {
        base = (unsigned long)1 << (bits - 1); //保证最高位是 1
        base += cRadom.Random(base);           //再加上一个随机数
        base |= 1;                             //保证最低位是 1,即保证是奇数
    } while (!RabinMiller(base, 30));          //进行拉宾－米勒测试 30 次
    return base;                               //全部通过认为是质数
}

unsigned __int64 Gcd(unsigned __int64 &p, unsigned __int64 &q)
{
    unsigned __int64 a = p > q ? p : q;
    unsigned __int64 b = p < q ? p : q;
    unsigned __int64 t;
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

unsigned __int64 Euclid(unsigned __int64 e, unsigned __int64 t_n)
{
    unsigned __int64 Max = 0xffffffffffffffff - t_n;
    unsigned __int64 i = 1;
    while (1)
    {
        if (((i * t_n) + 1) % e == 0)
        {
            return ((i * t_n) + 1) / e;
        }
        i++;
        unsigned __int64 Tmp = (i + 1) * t_n;
        if (Tmp > Max)
        {
            return 0;
        }
    }
    return 0;
}
RsaParam RsaGetParam(void)
{
    RsaParam Rsa = {0};
    UINT64 t;
    Rsa.p = RandomPrime(16); //随机生成两个素数
    Rsa.q = RandomPrime(16);
    Rsa.n = Rsa.p * Rsa.q;
    Rsa.f = (Rsa.p - 1) * (Rsa.q - 1);
    do
    {
        Rsa.e = m_cRadom.Random(65536);
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

unsigned short CRSASection::Decry(UINT64 nSorce)
{
    UINT64 nRes = PowMod(nSorce, m_cParament.d, m_cParament.n);
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
