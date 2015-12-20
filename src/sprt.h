#include <vector>
#include "stat.h"

namespace SPRT {

struct Result {
    Probability p;
    double pass, stop;
    std::vector<size_t> quantileValue;

    Result(double bayesElo, double drawElo) { p.set(bayesElo, drawElo); }
};

Result average(size_t simulations, double bayesElo, double drawElo, double bayesElo0,
    double bayesElo1, const std::vector<double>& quantiles);

} // namespace SPRT
