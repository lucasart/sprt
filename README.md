### Overview

SPRT is a simulator for Sequential Probability Ration Test, applied to chess
testing, using the BayesElo model.

### Compiling

* Code should compile with any `C++11` capable compiler (no dependancy).
* On Linux, or POSIX systems with g++, run `make.sh`.

### Syntax

`sprt BayesEloMin BayesEloMax BayesEloStep Simulations DrawElo BayesElo0 BayesElo1`

* `BayesEloMin`, `BayesEloMax`, `BayesEloStep`: Range of elo values to test.
* `Simulations`: Number of simulation to run for every step.
* `DrawElo`: Parameter of the BayesElo model. To calibrate this value, use the
following formula: `DrawElo = 200.log10[(1-w)/w.(1-l)/l]`, where `w` and `l`
are the win and loss ratio.
* `BayesElo0`, `BayesElo1`: Hypothesis H0 and H1 of the test.

Note that alpha and beta are always 5%. You do not need to change these values.
Instead, modify the bounds (`BayesElo0`,`BayesElo1`).

### Example

	$ ./sprt -1 5 1 20000 250 0 4
	BayesElo     Elo   %Pass     Avg    Q25%    Q50%    Q75%    Q90%
	   -1.00   -0.62  0.0128   28982   15842   24199   36893   53051
	    0.00   -0.00  0.0515   40145   19796   31951   51913   77481
	    1.00    0.62  0.1888   55858   24872   43094   73796  113419
	    2.00    1.24  0.5031   65163   27763   48987   85880  134951
	    3.00    1.86  0.8149   55612   24676   42561   73343  112581
	    4.00    2.48  0.9506   39821   19647   31877   51587   76262
	    5.00    3.10  0.9875   28835   15796   24049   36828   52699

For example, +2 BayesElo with DrawElo = 250 corresponds to +1.24 Elo, and the
SPRT(0,4) test has 50.31% chance to pass, with an average run of 63731 games.
The remaining columns are quantiles of the stopping time distribution.
