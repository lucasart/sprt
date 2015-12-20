#include <algorithm>
#include "sprt.h"

namespace SPRT {

bool one(const double llrInc[3], PRNG& prng, size_t& stop)
{
    // LLR (Log Likelyhood Ratio)
    const double bound = std::log((1 - 0.05) / 0.05);
    double LLR = 0;

    // Run an SPRT test (loop look infinite but terminates eventually with probability 1)
    for (stop = 0; std::abs(LLR) < bound; ++stop)
        LLR += llrInc[prng.game_result()];

    return LLR >= bound;
}

Result average(size_t simulations, const double llrInc[3], double bayesElo, double drawElo,
    const std::vector<double>& quantiles)
{
    Result r(bayesElo, drawElo);
    PRNG prng(r.p);

    size_t passCount = 0, stopSum = 0;

    std::vector<size_t> stop(simulations);
    for (size_t s = 0; s < simulations; ++s) {
        passCount += one(llrInc, prng, stop[s]);
        stopSum += stop[s];
    }

    if (!quantiles.empty()) {
        std::sort(stop.begin(), stop.end());
        for (auto& qp : quantiles)
            r.quantileValue.push_back(stop[(simulations - 1) * qp + 0.5]);
    }

    r.pass = double(passCount) / simulations;
    r.stop = double(stopSum) / simulations;
    return r;
}

} // namespace SPRT
