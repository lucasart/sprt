#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <thread>
#include "stat.h"

/* Use this value to cap the number of games (standard SPRT has no cap) */
#define T	0xffffffff

struct Result {
	double elo, bayes_elo;
	double pwin, ploss;
	double accept, stop;
};

bool SPRT_one(const double llr_inc[3], PRNG& prng, unsigned& t)
{
	// LLR (Log Likelyhood Ratio)
	const double alpha = 0.05;	// alpha = max type I error (reached on elo = elo0)
	const double beta = 0.05;	// beta = max type II error for elo >= elo1 (reached on elo = elo1)
	const double lower_bound = std::log(beta / (1-alpha));
	const double upper_bound = std::log((1-beta) / alpha);
	double LLR = 0;

	for (t = 0; t < T; ++t) {
		LLR += llr_inc[prng.game_result()];

		if (LLR < lower_bound)
			return false;
		else if (LLR > upper_bound)
			return true;
	}

	// We hit the cap: accept if LLR is closest to upper_bound
	return upper_bound - LLR < LLR - lower_bound;
}

void SPRT_average(unsigned nb_simu, const double llr_inc[3], Result& r)
{
	PRNG prng(r.pwin, r.ploss);

	unsigned accept_cnt = 0;
	uint64_t sum_stop = 0;

	for (unsigned simu = 0; simu < nb_simu; ++simu) {
		unsigned t;
		accept_cnt += SPRT_one(llr_inc, prng, t);
		sum_stop += t;
	}

	r.accept = (double)accept_cnt / nb_simu;
	r.stop = (double)sum_stop / nb_simu;
}

int main(int argc, char **argv)
{
	// Parse parameters
	if (argc != 8) {
		std::cout << "7 parameters required: elo_min elo_max elo_step nb_simu draw_elo bayes_elo0 bayes_elo1" << std::endl;
		return EXIT_FAILURE;
	}
	const double elo_min = atof(argv[1]), elo_max = atof(argv[2]), elo_step = atof(argv[3]);
	const unsigned nb_simu = atoll(argv[4]);
	const double draw_elo = atof(argv[5]);
	const double bayes_elo0 = atof(argv[6]);
	const double bayes_elo1 = atof(argv[7]);

	// Calculate probability laws under H0 and H1
	double pwin0, ploss0, pdraw0;
	double pwin1, ploss1, pdraw1;
	proba_elo(bayes_elo0, draw_elo, pwin0, ploss0), pdraw0 = 1 - pwin0 - ploss0;
	proba_elo(bayes_elo1, draw_elo, pwin1, ploss1), pdraw1 = 1 - pwin1 - ploss1;

	// Pre-calculate LLR increment for each game result
	const double llr_inc[3] = {
		std::log(ploss1 / ploss0),
		std::log(pdraw1 / pdraw0),
		std::log(pwin1 / pwin0)
	};

	// Prepare vector of results
	std::vector<Result> res;
	for (double elo = elo_min; elo <= elo_max; elo += elo_step) {
		Result r;
		r.elo = elo;
		r.bayes_elo = r.elo / scale(draw_elo);
		proba_elo(r.bayes_elo, draw_elo, r.pwin, r.ploss);
		res.push_back(r);
	}

	// Run iterations concurrently
	std::vector<std::thread> threads;
	auto it = res.begin();
	for (double elo = elo_min; elo <= elo_max; elo += elo_step)
		threads.push_back(std::thread(SPRT_average, nb_simu, llr_inc, std::ref(*it++)));
	for (auto& thread : threads)
		thread.join();

	// Display results
	std::cout << std::setw(10) << "ELO" << std::setw(10) << "BayesElo"
		<< std::setw(10) << "P(win)" << std::setw(10) << "P(loss)"
		<< std::setw(10) << "P(pass)" << std::setw(10) << "avg(stop)"
		<< std::endl;
	for (auto& r : res)
		std::cout << std::fixed << std::setprecision(2)
			<< std::setw(10) << r.elo << std::setw(10) << r.bayes_elo
			<< std::setprecision(4) << std::setw(10) << r.pwin << std::setw(10) << r.ploss << std::setw(10) << r.accept
			<< std::setprecision(0) << std::setw(10) << r.stop
			<< std::endl;

	return EXIT_SUCCESS;
}
