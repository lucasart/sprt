#include <stdio.h>
#include <stdlib.h>
#include "stat.h"

/* Use this value to cap the number of games (standard SPRT has no cap) */
#define T	0xffffffff

/* DRAW_ELO controls the proportion of draws. See proba_elo() function. To estimate this value, use
 * draw_elo = 200.log10[(1-w)/w.(1-l)/l]
 * where (w,l) are the win and loss ratio */
double draw_elo;
double elo0, elo1;	// expressed in BayesElo

/* Parametrization of the SPRT is here */
const double alpha = 0.05;		// alpha = max type I error (reached on elo = elo0)
const double beta = 0.05;		// beta = max type II error for elo >= elo2 (reached on elo = elo2)

void SPRT_stop(double pwin, double ploss, unsigned nb_simu, double *accept, unsigned *avg_stop)
{
	// LLR bounds
	const double lower_bound = log(beta / (1-alpha));
	const double upper_bound = log((1-beta) / alpha);

	// Calculate the probability laws under H0 and H1
	double pwin0, ploss0, pdraw0;
	double pwin1, ploss1, pdraw1;
	proba_elo(elo0, draw_elo, &pwin0, &ploss0); pdraw0 = 1-pwin0-ploss0;
	proba_elo(elo1, draw_elo, &pwin1, &ploss1); pdraw1 = 1-pwin1-ploss1;

	// Calculate the log-likelyhood ratio increment for each game result Xt
	const double llr_inc[3] = {
		log(ploss1 / ploss0),
		log(pdraw1 / pdraw0),
		log(pwin1 / pwin0)
	};

	// Calculate the true values of E(Xt) and V(Xt), and elo
	const double mu = pwin-ploss, v = pwin+ploss - mu*mu;
	const double elo = 200*log10(pwin/ploss*(1-ploss)/(1-pwin));

	// Collect the risk and reward statistics along the way
	unsigned accept_cnt = 0;	// Counter for H1 accepted
	uint64_t sum_stop = 0;		// sum of stopping times (to compute average)

	for (unsigned simu = 0; simu < nb_simu; ++simu) {
		/* Simulate one trajectory of T games
		 * - Calculate the LLR random walk along the way
		 * - early stop when LLR crosses a bound */
		bool accepted;					// patch acceptation
		double LLR = 0;					// log-likelyhood ratio
		unsigned count[3] = {0,0,0};	// counts the number of: LOSS, DRAW, WIN (in this order)

		unsigned t;
		for (t = 0; t < T; ++t) {
			const int X = game_result(pwin, ploss);
			LLR += llr_inc[X+1];
			++count[X+1];

			if (LLR < lower_bound) {
				// patch rejected by early stopping
				accepted = false;
				sum_stop += t;
				break;
			} else if (LLR > upper_bound) {
				// patch accepted by early stopping
				accepted = true;
				sum_stop += t;
				break;
			}
		}

		if (t == T) {
			// patch was not early stopped: accept H1 if LLR is closest to upper_bound
			accepted = (upper_bound - LLR) < (LLR - lower_bound);
			sum_stop += T;
		}

		accept_cnt += accepted;
	}

	*accept = (double)accept_cnt / nb_simu;
	*avg_stop = sum_stop / nb_simu;
}

int main(int argc, char **argv)
{
	if (argc != 8) {
		puts("7 parameters requires: elo_min elo_max elo_step nb_simu draw_elo elo0 elo1\n");
		exit(EXIT_FAILURE);
	}

	const double elo_min = atof(argv[1]), elo_max = atof(argv[2]), elo_step = atof(argv[3]);
	const unsigned nb_simu = atoll(argv[4]);
	draw_elo = atof(argv[5]);
	elo0 = atof(argv[6]);
	elo1 = atof(argv[7]);

	// Print header
	puts("BayesELO,ELO,P(pass),avg(stop)");

	for (double elo = elo_min; elo <= elo_max; elo += elo_step) {
		double pwin, ploss, accept;
		unsigned avg_stop;

		proba_elo(elo, draw_elo, &pwin, &ploss);
		double score = 0.5 + (pwin - ploss) / 2;
		double ELO = -400 * log10(1/score - 1);

		SPRT_stop(pwin, ploss, nb_simu, &accept, &avg_stop);
		printf("%.2f,%.2f,%.4f,%u\n", elo, ELO, accept, avg_stop);
	}
}
