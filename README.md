### Overview

SPRT is a simulator for Sequential Probability Ration Test, applied to chess
testing, using the BayesElo model.

### Compiling

* On Linux, simply run `make.sh`
* On other POSIX compliant platforms, I don't expect many problems
* Otherwise, requires a standard compliant C99 compiler (eg. mingw for Windows)

### Usage

**Syntax**

    sprt elo_min elo_max elo_step nb_simu draw_elo elo0 elo1

* `elo_min`, `elo_max`, `elo_step`: This is the range of elo values to test,
expressed in BayesElo units.
* `nb_simu`: Number of simulation to run for every step.
* `draw_elo`: Parameter of the BayesElo model, used to reduce the problem to 1
dimension, where we can apply SPRT. To calibrate this value, use the following
formula: `draw_elo = 200.log10[(1-w)/w.(1-l)/l]`.
* `elo0`, `elo1`: Hypothesis H0 and H1 of the test, expressed in BayesElo units.

Note that alpha and beta are always 5%. Do not change these values in the code.
Instead, play with the bounds (elo0,elo1).

Note also that SPRT is an infinite test, although probability of long runs decays
very fast. To perform a truncated SPRT, modify this line of code:

    #define T       128000	// truncate at 128k games

Decision will then be based on which bound is closest to the current LLR
(log-likelyhood ratio).

**Sample session**

    $ ./sprt -2 6 1 10000 250 0 4
    BayesELO,ELO,P(pass),avg(stop)
    -2.00,-1.24,0.0025,22083
    -1.00,-0.62,0.0119,28961
    0.00,-0.00,0.0499,40417
    1.00,0.62,0.1892,56089
    2.00,1.24,0.4962,66266
    3.00,1.86,0.8161,55588
    4.00,2.48,0.9520,40370
    5.00,3.10,0.9888,28896
    6.00,3.72,0.9968,22014

For example, +2.00 BayesElo corresponds to +1.24 ELO (for draw_elo = 250). In this
scenario, the SPRT(elo0=0,elo1=4) test has 49.6% probability to pass (choose H1).
