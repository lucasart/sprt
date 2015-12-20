#pragma once
#include <cstdint>
#include <cmath>

struct Probability {
    double win, loss;
    double draw() const { return 1 - win - loss; }

    double score() const { return win + draw() / 2; }
    double elo()   const { return -400 * std::log10(1 / score() - 1); }

    double draw_elo()  const { return 200 * std::log10((1 - win) / win * (1 - loss) / loss); }
    double bayes_elo() const { return 200 * std::log10(win / loss * (1 - loss) / (1 - win)); }

    void set(double bayesElo, double drawElo);
};

enum {LOSS, DRAW, WIN};

// Game result generator
class PRNG {
    uint64_t a, b, c, d;
    Probability p;
    uint64_t rand64();
public:
    PRNG(const Probability& p);
    int game_result();
};
