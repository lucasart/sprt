#include <algorithm>
#include "stat.h"

void Probability::set(double bayes_elo, double draw_elo)
{
	_win = 1 / (1 + std::pow(10.0, (-bayes_elo + draw_elo) / 400.0));
	_loss = 1 / (1 + std::pow(10.0, (bayes_elo + draw_elo) / 400.0));
}

double invert(double elo, double draw_elo, double eps)
{
	// Establish bounds
	double x;
	Probability p;
	for (x = elo; ; x += x) {
		p.set(x, draw_elo);
		if (std::abs(p.elo()) >= std::abs(elo))
			break;
	}

	// Dichotomy
	double lbound = std::min(0.5 * x, x);
	double ubound = std::max(0.5 * x, x);
	while (ubound - lbound > eps) {
		x = 0.5 * (lbound + ubound);
		p.set(x, draw_elo);
		if (elo < p.elo())
			ubound = x;
		else
			lbound = x;
	}
	return x;
}

namespace {

uint64_t rotate(uint64_t x, uint64_t k)
{
	return (x << k) | (x >> (64 - k));
}

}	// namespace

PRNG::PRNG(const Probability& _p)
{
	a = 0x46dd577ff603b540ULL;
	b = 0xc4077bddfacf987bULL;
	c = 0xbbf4d93b7200e858ULL;
	d = 0xd3e075cfd449bb1eULL;
	p = _p;
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
	return x < p.win() ? WIN : (x < p.win() + p.loss() ? LOSS : DRAW);
}
