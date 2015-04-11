#include <iostream>
#include <iomanip>
#include <vector>
#include <future>
#include <algorithm>
#include <array>
#include "stat.h"

struct Result {
	double elo, bayes_elo;
	Probability p;
	double pass, stop;
	unsigned q10, q25, q50, q75, q90;

	Result(double _elo, double draw_elo) {
		elo = _elo;
		bayes_elo = invert(elo, draw_elo);
		p.set(bayes_elo, draw_elo);
	}

	static void header() {
		std::cout << std::setw(8) << "Elo" << std::setw(10) << "BayesElo"
			<< std::setw(10) << "%Pass" << std::setw(10) << "Avg run"
			<< std::setw(10) << "Q10%" << std::setw(10) << "Q25%"
			<< std::setw(10) << "Median" << std::setw(10) << "Q75%"
			<< std::setw(10) << "Q90%" << std::endl;
	}

	void print() const {
		std::cout << std::fixed << std::setprecision(2)
			<< std::setw(8) << elo << std::setw(10) << bayes_elo
			<< std::setprecision(4) << std::setw(10) << pass
			<< std::setprecision(0) << std::setw(10) << stop
			<< std::setw(10) << q10 << std::setw(10) << q25
			<< std::setw(10) << q50 << std::setw(10) << q75
			<< std::setw(10) << q90 << std::endl;
	}
};

bool SPRT_one(const std::array<double, 3> llr_inc, PRNG& prng, unsigned& t)
{
	// LLR (Log Likelyhood Ratio)
	const double bound = std::log((1-0.05) / 0.05);
	double LLR = 0;

	// Run an SPRT test (loop look infinite but terminates eventually with probability 1)
	for (t = 0; std::abs(LLR) < bound; ++t)
		LLR += llr_inc[prng.game_result()];

	return LLR >= bound;
}

Result SPRT_average(unsigned nb_simu, const std::array<double, 3> llr_inc, double elo, double draw_elo)
{
	Result r(elo, draw_elo);
	PRNG prng(r.p);

	size_t pass_cnt = 0;
	uint64_t sum_stop = 0;

	std::vector<unsigned> t(nb_simu);
	for (size_t s = 0; s < nb_simu; ++s) {
		pass_cnt += SPRT_one(llr_inc, prng, t[s]);
		sum_stop += t[s];
	}

	std::sort(t.begin(), t.end());
	r.q10 = t[nb_simu / 10];
	r.q25 = t[nb_simu / 4];
	r.q50 = t[nb_simu / 2];
	r.q75 = t[nb_simu * 3 / 4];
	r.q90 = t[nb_simu * 9 / 10];

	r.pass = (double)pass_cnt / nb_simu;
	r.stop = (double)sum_stop / nb_simu;
	return r;
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
	const std::array<double, 3> llr_inc = {
		std::log(p1.loss() / p0.loss()),
		std::log(p1.draw() / p0.draw()),
		std::log(p1.win() / p0.win())
	};

	// Run SPRT_average() concurrently for each elo value
	std::vector<std::future<Result>> results;
	for (double elo = elo_min; elo <= elo_max; elo += elo_step)
		results.push_back(std::async(std::launch::async, SPRT_average, nb_simu, llr_inc, elo, draw_elo));

	// Display results
	Result::header();
	for (auto& r : results)
		r.get().print();

	return EXIT_SUCCESS;
}
