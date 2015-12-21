#include <vector>
#include "stat.h"

namespace SPRT {

struct Result {
    Probability p;
    double passRate, stopAvg;
    std::vector<size_t> quantileValue;
};

Result average(size_t simulations, double bayesElo, double drawElo, double bayesElo0,
    double bayesElo1, const std::vector<double>& quantiles);

} // namespace SPRT
