#pragma once
#include <cstdint>
#include <cassert>
#include <cmath>

struct Probability {
	double win, loss;
	double draw() const { return 1.0 - win - loss; }
	void set(double bayes_elo, double draw_elo);
};

/* Probability -> (bayes_elo, draw_elo) cannot be done analytically
 * Instead use scale formula (first order approximation valid for small elo
 * values) */
double scale(double draw_elo);	// elo = scale * bayeselo (approx for small abs(elo))

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
