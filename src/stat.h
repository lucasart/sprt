#pragma once
#include <cstdint>
#include <cassert>
#include <cmath>

struct Probability {
	double win, loss;
	double draw() const { return 1.0 - win - loss; }
	double elo() const;
	void set(double bayes_elo, double draw_elo);
};

/* Invert the BayesElo model to get: bayes_elo = f(elo, draw_elo). This cannot
 * be done analytically, so we need to do it by dichotomy (eps is the
 * precision required). */
double invert(double elo, double draw_elo, double eps = 1e-4);

enum {LOSS, DRAW, WIN};

// Game result generator
class PRNG {
	uint64_t a, b, c, d;
	Probability p;

	uint64_t rand64();
	double uniform();
public:
	PRNG(const Probability& p);
	int game_result();
};
