### Overview

SPRT is a simulator for Sequential Probability Ration Test, applied to chess
testing, using the BayesElo model.

### Compiling

* On Linux, simply run `make.sh`
* On other POSIX compliant platforms, I don't expect many problems
* Otherwise, requires a standard compliant C99 compiler (eg. mingw for Windows)

### Syntax

`sprt elo_min elo_max elo_step nb_simu draw_elo bayes_elo0 bayes_elo1`

* `elo_min`, `elo_max`, `elo_step`: Range of elo values to test.
* `nb_simu`: Number of simulation to run for every step
* `draw_elo`: Parameter of the BayesElo model. To calibrate this value, use the
following formula: `draw_elo = 200.log10[(1-w)/w.(1-l)/l]`, where `w` and `l`
are the win and loss ratio
* `bayes_elo0`, `bayes_elo1`: Hypothesis H0 and H1 of the test, **expressed in
   BayesElo**

Note that alpha and beta are always 5%. You do not need to change these values.
Instead, modify the bounds (elo0,elo1).

### Example

	$ ./sprt -1 5 1 20000 250 0 4
	     Elo  BayesElo     %Pass   Avg run      Q10%      Q25%      Q50%      Q75%      Q90%
	   -1.00     -1.61    0.0061     24261     10063     14010     20749     30534     42928
	    0.00      0.00    0.0515     40145     13206     19796     31951     51913     77481
	    1.00      1.61    0.3648     63731     16648     27113     48055     83959    130388
	    2.00      3.23    0.8592     51876     15017     23537     39990     68310    104624
	    3.00      4.84    0.9855     30237     11339     16339     25137     38463     55596
	    4.00      6.45    0.9989     19996      8936     12194     17459     24845     34464
	    5.00      8.07    1.0000     14669      7393      9693     13227     18006     23812

For example, +1 ELO with draw_elo = 250 corresponds to +1.61 BayesElo, and the
SPRT(0,4) test has 36.5% probability to pass, with an average run of 63,731
games. The last 5 columns are quantiles of the stopping time distribution.
