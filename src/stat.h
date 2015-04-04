#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

// Bayeselo formula to get P(win) and P(Loss) as a function of elo, for a given drawelo
void proba_elo(double elo, double draw_elo, double *pwin, double *ploss);

// Pseudo Random Number Generators
uint64_t rand64();	// draw integers (uniformly) between 0 and 2^64-1
double uniform();	// draw U(0,1)

// Scores are between -1 and 1 (loss=-1, draw=0, win=1)
// So the elo<->score formulas are not the usual ones
double elo_to_score(double elo);
double score_to_elo(double score);

// Simulate a game result
enum {LOSS, DRAW, WIN};
int game_result(double pwin, double ploss);
