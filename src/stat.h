#pragma once
#include <cstdint>
#include <cassert>
#include <cmath>

// (bayes_elo, draw_elo) -> (P(win), P(loss))
void proba_elo(double bayes_elo, double draw_elo, double& pwin, double& ploss);

// (P(win), P(loss)) -> (bayes_elo, draw_elo) cannot be done analytically
// Instead use scale formula (first order approximation valid for small elo values)
double scale(double draw_elo);	// elo = scale * bayeselo (approx for small abs(elo))

enum {LOSS, DRAW, WIN};

// Game result generator
class PRNG {
	uint64_t a, b, c, d;
	double pwin, ploss;

	uint64_t rand64();
	double uniform();
public:
	PRNG(double _pwin, double _ploss);
	int game_result();
};
