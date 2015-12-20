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
        std::cout << "Syntax:\nsprt BayesEloMin BayesEloMax BayesEloStep Simulations DrawElo "
            "BayesElo0 BayesElo1 Quantiles..." << std::endl;
        return EXIT_FAILURE;
    }
    const double bayesEloMin = atof(argv[1]), bayesEloMax = atof(argv[2]), bayesEloStep = atof(argv[3]);
    const double drawElo = atof(argv[5]), bayesElo0 = atof(argv[6]), bayesElo1 = atof(argv[7]);
    const size_t simulations = atoll(argv[4]);
    std::vector<double> quantiles;
    for (int i = 8; i < argc; ++i)
        quantiles.push_back(atof(argv[i]));

    // Run SPRT::average() concurrently for each BayesElo value
    std::vector<std::future<SPRT::Result>> results;
    for (double bayesElo = bayesEloMin; bayesElo <= bayesEloMax; bayesElo += bayesEloStep)
        results.push_back(std::async(std::launch::async, SPRT::average, simulations, bayesElo,
            drawElo, bayesElo0, bayesElo1, quantiles));

    // Display header
    std::cout << std::setw(8) << "BayesElo" << std::setw(8) << "Elo" << std::setw(8) << "%Pass"
        << std::setw(8) << "Avg";
    for (auto& qp : quantiles) {
        std::ostringstream os;
        os << 'Q' << 100 * qp << '%';
        std::cout << std::setw(8) << os.str();
    }
    std::cout << std::endl;

    // Display results (when ready)
    for (auto& r : results) {
        const SPRT::Result _r = r.get();
        std::cout << std::fixed << std::setprecision(2) << std::setw(8) << _r.p.bayes_elo()
            << std::setw(8) << _r.p.elo() << std::setprecision(4) << std::setw(8) << _r.pass
            << std::setprecision(0) << std::setw(8) << _r.stop;
        for (auto& qv : _r.quantileValue)
            std::cout << std::setw(8) << qv;
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
