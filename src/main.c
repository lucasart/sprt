#include <stdio.h>
#include <stdlib.h>
#include "stat.h"

/* Use this value to cap the number of games (standard SPRT has no cap) */
#define T	0xffffffff

/* BayesElo model. See proba_elo() function. To estimate draw_elo, use:
 * draw_elo = 200.log10[(1-w)/w.(1-l)/l]
 * where (w,l) are the win and loss ratio */
double draw_elo;
double elo0, elo1;

bool SPRT_one(double pwin, double ploss, const double llr_inc[3], unsigned *t)
{
	// LLR (Log Likelyhood Ratio)
	const double alpha = 0.05;	// alpha = max type I error (reached on elo = elo0)
	const double beta = 0.05;	// beta = max type II error for elo >= elo2 (reached on elo = elo2)
	const double lower_bound = log(beta / (1-alpha));
	const double upper_bound = log((1-beta) / alpha);
	double LLR = 0;

	for (*t = 0; *t < T; ++*t) {
		const int X = game_result(pwin, ploss);
		LLR += llr_inc[X];

		if (LLR < lower_bound)
			return false;
		else if (LLR > upper_bound)
			return true;
	}

	// We hit the cap: accept if LLR is closest to upper_bound
	return (upper_bound - LLR) < (LLR - lower_bound);
}

void SPRT_average(double pwin, double ploss, unsigned nb_simu, double *accept, unsigned *avg_stop)
{
	// Calculate the probability laws under H0 and H1
	double pwin0, ploss0, pdraw0;
	double pwin1, ploss1, pdraw1;
	proba_elo(elo0, draw_elo, &pwin0, &ploss0); pdraw0 = 1-pwin0-ploss0;
	proba_elo(elo1, draw_elo, &pwin1, &ploss1); pdraw1 = 1-pwin1-ploss1;

	// Calculate the log-likelyhood ratio increment for each game result X(t)
	const double llr_inc[3] = {
		log(ploss1 / ploss0),
		log(pdraw1 / pdraw0),
		log(pwin1 / pwin0)
	};

	// Collect the risk and reward statistics along the way
	unsigned accept_cnt = 0;	// Counter for H1 accepted
	uint64_t sum_stop = 0;		// sum of stopping times (to compute average)

	for (unsigned simu = 0; simu < nb_simu; ++simu) {
		unsigned t;
		accept_cnt += SPRT_one(pwin, ploss, llr_inc, &t);
		sum_stop += t;
	}

	*accept = (double)accept_cnt / nb_simu;
	*avg_stop = sum_stop / nb_simu;
}

int main(int argc, char **argv)
{
	if (argc != 8) {
		puts("7 parameters required: elo_min elo_max elo_step nb_simu draw_elo elo0 elo1\n");
		return EXIT_FAILURE;
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

		SPRT_average(pwin, ploss, nb_simu, &accept, &avg_stop);
		printf("%.2f,%.2f,%.4f,%u\n", elo, ELO, accept, avg_stop);
	}

	return EXIT_SUCCESS;
}
