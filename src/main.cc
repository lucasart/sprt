#include <iostream>
#include <iomanip>
#include <vector>
#include <future>
#include <algorithm>
#include <sstream>
#include "stat.h"

// Quantiles of the stopping time distribution to display
const std::vector<double> QuantileProbability = {.25, .5, .75, .9};

struct Result {
    Probability p;
    double pass, stop;
    std::vector<size_t> quantile_value;

    Result(double bayesElo, double drawElo) { p.set(bayesElo, drawElo); }

    void print() const {
        std::cout << std::fixed << std::setprecision(2) << std::setw(8) << p.bayes_elo()
            << std::setprecision(2) << std::setw(8) << p.elo()
            << std::setprecision(4) << std::setw(8) << pass
            << std::setprecision(0) << std::setw(8) << stop;
        for (auto& qv : quantile_value)
            std::cout << std::setw(8) << qv;
        std::cout << std::endl;
    }
};

bool SPRT_one(const double llr_inc[3], PRNG& prng, size_t& t)
{
    // LLR (Log Likelyhood Ratio)
    const double bound = std::log((1-0.05) / 0.05);
    double LLR = 0;

    // Run an SPRT test (loop look infinite but terminates eventually with probability 1)
    for (t = 0; std::abs(LLR) < bound; ++t)
        LLR += llr_inc[prng.game_result()];

    return LLR >= bound;
}

Result SPRT_average(size_t simulations, const double llr_inc[3], double bayesElo, double drawElo)
{
    Result r(bayesElo, drawElo);
    PRNG prng(r.p);

    size_t pass_cnt = 0, sum_stop = 0;

    std::vector<size_t> t(simulations);
    for (size_t s = 0; s < simulations; ++s) {
        pass_cnt += SPRT_one(llr_inc, prng, t[s]);
        sum_stop += t[s];
    }

    std::sort(t.begin(), t.end());
    for (auto& qp : QuantileProbability)
        r.quantile_value.push_back(t[size_t(simulations * qp + 0.5)]);

    r.pass = double(pass_cnt) / simulations;
    r.stop = double(sum_stop) / simulations;
    return r;
}

int main(int argc, char **argv)
{
    // Parse parameters
    if (argc != 8) {
        std::cout << "Syntax:\nsprt BayesEloMin BayesEloMax BayesEloStep Simulations DrawElo BayesElo0 BayesElo1" << std::endl;
        return EXIT_FAILURE;
    }
    const double bayesElo_min = atof(argv[1]), bayesElo_max = atof(argv[2]), bayesElo_step = atof(argv[3]);
    const double drawElo = atof(argv[5]), bayesElo0 = atof(argv[6]), bayesElo1 = atof(argv[7]);
    const size_t simulations = atoll(argv[4]);

    // Calculate probability laws under H0 and H1
    Probability p0, p1;
    p0.set(bayesElo0, drawElo);
    p1.set(bayesElo1, drawElo);

    // Pre-calculate LLR increment for each game result
    const double llr_inc[3] = {
        std::log(p1.loss / p0.loss),
        std::log(p1.draw() / p0.draw()),
        std::log(p1.win / p0.win)
    };

    // Run SPRT_average() concurrently for each elo value
    std::vector<std::future<Result>> results;
    for (double bayesElo = bayesElo_min; bayesElo <= bayesElo_max; bayesElo += bayesElo_step)
        results.push_back(std::async(std::launch::async, SPRT_average, simulations, llr_inc, bayesElo, drawElo));

    // Display header
    std::cout << std::setw(8) << "BayesElo" << std::setw(8) << "Elo" << std::setw(8) << "%Pass" << std::setw(8) << "Avg";
    for (auto& qp : QuantileProbability) {
        std::ostringstream os;
        os << 'Q' << 100 * qp << '%';
        std::cout << std::setw(8) << os.str();
    }
    std::cout << std::endl;

    // Display results
    for (auto& r : results)
        r.get().print();

    return EXIT_SUCCESS;
}
