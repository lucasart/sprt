#include <iostream>
#include <iomanip>
#include <vector>
#include <future>
#include <sstream>
#include "sprt.h"

int main(int argc, char **argv)
{
    // Parse parameters
    if (argc < 8) {
        std::cout << "Syntax:\nsprt BayesEloMin BayesEloMax BayesEloStep Simulations DrawElo BayesElo0 BayesElo1 Quantiles..." << std::endl;
        return EXIT_FAILURE;
    }
    const double bayesElo_min = atof(argv[1]), bayesElo_max = atof(argv[2]), bayesElo_step = atof(argv[3]);
    const double drawElo = atof(argv[5]), bayesElo0 = atof(argv[6]), bayesElo1 = atof(argv[7]);
    const size_t simulations = atoll(argv[4]);
    std::vector<double> quantiles;
    for (int i = 8; i < argc; ++i)
        quantiles.push_back(atof(argv[i]));

    // Calculate probability laws under H0 and H1
    Probability p0, p1;
    p0.set(bayesElo0, drawElo);
    p1.set(bayesElo1, drawElo);

    // Pre-calculate LLR increment for each game result
    const double llrInc[3] = {
        std::log(p1.loss / p0.loss),
        std::log(p1.draw() / p0.draw()),
        std::log(p1.win / p0.win)
    };

    // Run SPRT::average() concurrently for each elo value
    std::vector<std::future<SPRT::Result>> results;
    for (double bayesElo = bayesElo_min; bayesElo <= bayesElo_max; bayesElo += bayesElo_step)
        results.push_back(std::async(std::launch::async, SPRT::average, simulations, llrInc, bayesElo, drawElo, quantiles));

    // Display header
    std::cout << std::setw(8) << "BayesElo" << std::setw(8) << "Elo" << std::setw(8) << "%Pass" << std::setw(8) << "Avg";
    for (auto& qp : quantiles) {
        std::ostringstream os;
        os << 'Q' << 100 * qp << '%';
        std::cout << std::setw(8) << os.str();
    }
    std::cout << std::endl;

    // Display results
    for (auto& r : results) {
        const SPRT::Result _r = r.get();
        std::cout << std::fixed << std::setprecision(2) << std::setw(8) << _r.p.bayes_elo() << std::setw(8) << _r.p.elo()
            << std::setprecision(4) << std::setw(8) << _r.pass << std::setprecision(0) << std::setw(8) << _r.stop;
        for (auto& qv : _r.quantileValue)
            std::cout << std::setw(8) << qv;
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
