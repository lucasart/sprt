#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include "stat.h"

struct Result {
	double elo, bayes_elo;
	Probability p;
	double pass, stop;

	void print() const {
		std::cout << std::fixed << std::setprecision(2)
			<< std::setw(8) << elo << std::setw(10) << bayes_elo
			<< std::setprecision(4) << std::setw(10) << p.win << std::setw(10)
			<< p.loss << std::setw(10) << p.draw() << std::setw(10) << pass
			<< std::setprecision(0) << std::setw(10) << stop << std::endl;
	}
};

bool SPRT_one(const std::vector<double>& llr_inc, PRNG& prng, unsigned& t)
{
	// LLR (Log Likelyhood Ratio)
	const double bound = std::log((1-0.05) / 0.05);
	double LLR = 0;

	// Run an SPRT test (loop look infinite but terminates eventually with probability 1)
	for (t = 0; std::abs(LLR) < bound; ++t)
		LLR += llr_inc[prng.game_result()];

	return LLR >= bound;
}

void SPRT_average(unsigned nb_simu, const std::vector<double>& llr_inc, Result& r)
{
	PRNG prng(r.p);

	unsigned pass_cnt = 0;
	uint64_t sum_stop = 0;

	for (unsigned simu = 0; simu < nb_simu; ++simu) {
		unsigned t;
		pass_cnt += SPRT_one(llr_inc, prng, t);
		sum_stop += t;
	}

	r.pass = (double)pass_cnt / nb_simu;
	r.stop = (double)sum_stop / nb_simu;
}

int main(int argc, char **argv)
{
	// Parse parameters
	if (argc != 8) {
		std::cout << "Syntax:\nsprt elo_min elo_max elo_step nb_simu draw_elo bayes_elo0 bayes_elo1" << std::endl;
		return EXIT_FAILURE;
	}
	const double elo_min = atof(argv[1]), elo_max = atof(argv[2]), elo_step = atof(argv[3]);
	const double draw_elo = atof(argv[5]), bayes_elo0 = atof(argv[6]), bayes_elo1 = atof(argv[7]);
	const unsigned nb_simu = atoll(argv[4]);

	// Calculate probability laws under H0 and H1
	Probability p0, p1;
	p0.set(bayes_elo0, draw_elo);
	p1.set(bayes_elo1, draw_elo);

	// Pre-calculate LLR increment for each game result
	const std::vector<double> llr_inc = {
		std::log(p1.loss / p0.loss),
		std::log(p1.draw() / p0.draw()),
		std::log(p1.win / p0.win)
	};

	// Prepare vector of results
	std::vector<Result> res;
	for (double elo = elo_min; elo <= elo_max; elo += elo_step) {
		Result r;
		r.elo = elo;
		r.bayes_elo = invert(r.elo, draw_elo);
		r.p.set(r.bayes_elo, draw_elo);
		res.push_back(r);
	}

	// Run iterations concurrently
	std::vector<std::thread> threads;
	for (auto& r : res)
		threads.push_back(std::thread(SPRT_average, nb_simu, llr_inc, std::ref(r)));
	for (auto& thread : threads)
		thread.join();

	// Display results
	std::cout << std::setw(8) << "Elo" << std::setw(10) << "BayesElo"
		<< std::setw(10) << "P(win)" << std::setw(10) << "P(loss)" << std::setw(10) << "P(draw)"
		<< std::setw(10) << "P(pass)" << std::setw(10) << "avg(stop)"
		<< std::endl;
	for (auto& r : res)
		r.print();

	return EXIT_SUCCESS;
}
