### Overview

SPRT is a simulator for Sequential Probability Ration Test, applied to chess
testing, using the BayesElo model.

### Compiling

* Code should compile with any `C++11` capable compiler (no dependancy).
* On Linux, or POSIX systems with g++, run `make.sh`.

### Syntax

`sprt BayesEloMin BayesEloMax BayesEloStep Simulations DrawElo BayesElo0 BayesElo1 Quantiles...`

* `BayesEloMin`, `BayesEloMax`, `BayesEloStep`: Range of elo values to test.
* `Simulations`: Number of simulation to run for every step.
* `DrawElo`: Parameter of the BayesElo model. To calibrate this value, use the
following formula: `DrawElo = 200.log10[(1-w)/w.(1-l)/l]`, where `w` and `l`
are the win and loss ratio.
* `BayesElo0`, `BayesElo1`: Hypothesis H0 and H1 of the test.
* `Quantiles...`: Quantile(s) of the stopping time distribution.

Note that alpha and beta are always 5%. You do not need to change these values.
Instead, modify the bounds (`BayesElo0`,`BayesElo1`).

### Example

	$ ./sprt -1 5 1 20000 250 0 4 0.25 0.5 0.75 0.9
	BayesElo     Elo   %Pass     Avg    Q25%    Q50%    Q75%    Q90%
	   -1.00   -0.62  0.0128   28982   15842   24199   36892   53050
	    0.00   -0.00  0.0515   40145   19796   31951   51911   77467
	    1.00    0.62  0.1888   55858   24872   43094   73794  113389
	    2.00    1.24  0.5031   65163   27763   48987   85860  134934
	    3.00    1.86  0.8149   55612   24676   42561   73334  112514
	    4.00    2.48  0.9506   39821   19647   31877   51575   76254
	    5.00    3.10  0.9875   28835   15796   24049   36825   52675

For example, +3 BayesElo with DrawElo = 250 corresponds to +1.86 Elo, and the
SPRT(0,4) test has 81.49% chance to pass, with an average run of 55612 games.
The remaining columns are quantiles of the stopping time distribution.
