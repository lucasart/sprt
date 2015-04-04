### Overview

SPRT is a simulator for Sequential Probability Ration Test, applied to chess
testing, using the BayesElo model.

### Compiling

* On Linux, simply run `make.sh`
* On other POSIX compliant platforms, I don't expect many problems
* Otherwise, requires a standard compliant C99 compiler (eg. mingw for Windows)

### Usage

Run without parameters to get syntax:

    $ ./sprt 
    7 parameters required: elo_min elo_max elo_step nb_simu draw_elo elo0 elo1

**Meaning of the parameters**

* `elo_min`, `elo_max`, `elo_step`: This is the range of elo values to test,
expressed in BayesElo units.
* `nb_simu`: Number of simulation to run for every step.
* `draw_elo`: Parameter of the BayesElo model, used to reduce the problem to 1
dimension, where we can apply SPRT. To calibrate this value, use the following
formula: `draw_elo = 200.log10[(1-w)/w.(1-l)/l]`.
* `elo0`, `elo1`: Hypothesis H0 and H1 of the test, expressed in BayesElo units.
