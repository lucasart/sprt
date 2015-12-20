#include <vector>
#include "stat.h"

namespace SPRT {

struct Result {
    Probability p;
    double pass, stop;
    std::vector<size_t> quantileValue;

    Result(double bayesElo, double drawElo) { p.set(bayesElo, drawElo); }
};

bool one(const double llrInc[3], PRNG& prng, size_t& stop);
Result average(size_t simulations, const double llrInc[3], double bayesElo, double drawElo,
    const std::vector<double>& quantiles);

} // namespace SPRT
