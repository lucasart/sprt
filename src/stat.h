#pragma once
#include <cstdint>
#include <cmath>

class Probability {
	double _win, _loss;
public:
	double win() const { return _win; }
	double loss() const { return _loss; }
	double draw() const { return 1.0 - _win - _loss; }
	double score() const { return win() + 0.5 * draw(); }
	double elo() const { return -400.0 * std::log10(1.0 / score() - 1.0); }
	void set(double bayes_elo, double draw_elo);
};

// Invert the BayesElo model to get: bayes_elo = f(elo, draw_elo). This cannot
// be done analytically, so we need to do it by dichotomy (eps is the precision
// required).
double invert(double elo, double draw_elo, double eps = 1e-4);

enum {LOSS, DRAW, WIN};

// Game result generator
class PRNG {
	uint64_t a, b, c, d;
	Probability p;
	uint64_t rand64();
public:
	PRNG(const Probability& p);
	int game_result();
};
