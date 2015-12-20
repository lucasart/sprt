#include <algorithm>
#include "stat.h"

void Probability::set(double bayesElo, double drawElo)
{
    win  = 1 / (1 + std::pow(10.0, (drawElo - bayesElo) / 400));
    loss = 1 / (1 + std::pow(10.0, (drawElo + bayesElo) / 400));
}

namespace {

uint64_t rotate(uint64_t x, uint64_t k)
{
    return (x << k) | (x >> (64 - k));
}

}    // namespace

PRNG::PRNG(const Probability& _p) : p(_p)
{
    a = 0x46dd577ff603b540ULL;
    b = 0xc4077bddfacf987bULL;
    c = 0xbbf4d93b7200e858ULL;
    d = 0xd3e075cfd449bb1eULL;
}

uint64_t PRNG::rand64()
{
    const uint64_t e = a - rotate(b,  7);
    a = b ^ rotate(c, 13);
    b = c + rotate(d, 37);
    c = d + e;
    return d = e + a;
}

int PRNG::game_result()
{
    double x = (double)rand64() / 0xffffffffffffffffULL;
    return x < p.win ? WIN : (x < p.win + p.loss ? LOSS : DRAW);
}
