#include <iostream>
#include <iomanip>
#include <vector>
#include <future>
#include <algorithm>
#include <array>
#include <sstream>
#include "stat.h"

// Quantiles of the stopping time distribution to display
const std::vector<double> quantile_probability = {0.1, 0.25, 0.5, 0.75, 0.9};

struct Result {
	double elo, bayes_elo;
	Probability p;
	double pass, stop;
	std::vector<size_t> quantile_value;

	Result(double _elo, double draw_elo) {
		elo = _elo;
		bayes_elo = invert(elo, draw_elo);
		p.set(bayes_elo, draw_elo);
	}

	static void header() {
		std::cout << std::setw(8) << "Elo" << std::setw(10) << "BayesElo"
			<< std::setw(10) << "%Pass" << std::setw(10) << "Avg run";
		for (auto& qp : quantile_probability) {
			std::ostringstream os;
			os << 'Q' << 100 * qp << '%';
			std::cout << std::setw(10) << os.str();
		}
		std::cout << std::endl;
	}

	void print() const {
		std::cout << std::fixed << std::setprecision(2)
			<< std::setw(8) << elo << std::setw(10) << bayes_elo
			<< std::setprecision(4) << std::setw(10) << pass
			<< std::setprecision(0) << std::setw(10) << stop;
		for (auto& qv : quantile_value)
			std::cout << std::setw(10) << qv;
		std::cout << std::endl;
	}
};

bool SPRT_one(const std::array<double, 3> llr_inc, PRNG& prng, size_t& t)
{
	// LLR (Log Likelyhood Ratio)
	const double bound = std::log((1-0.05) / 0.05);
	double LLR = 0;

	// Run an SPRT test (loop look infinite but terminates eventually with probability 1)
	for (t = 0; std::abs(LLR) < bound; ++t)
		LLR += llr_inc[prng.game_result()];

	return LLR >= bound;
}

Result SPRT_average(size_t nb_simu, const std::array<double, 3> llr_inc, double elo, double draw_elo)
{
	Result r(elo, draw_elo);
	PRNG prng(r.p);

	size_t pass_cnt = 0, sum_stop = 0;

	std::vector<size_t> t(nb_simu);
	for (size_t s = 0; s < nb_simu; ++s) {
		pass_cnt += SPRT_one(llr_inc, prng, t[s]);
		sum_stop += t[s];
	}

	std::sort(t.begin(), t.end());
	for (auto& qp : quantile_probability)
		r.quantile_value.push_back(t[size_t(nb_simu * qp + 0.5)]);

	r.pass = double(pass_cnt) / nb_simu;
	r.stop = double(sum_stop) / nb_simu;
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
	const size_t nb_simu = atoll(argv[4]);

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
