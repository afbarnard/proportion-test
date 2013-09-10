Proportion Test
===============


Have a look around!  This repository is not an active project so much as
a dump of some code I was working with a few years ago that might be
useful or relevant to someone.

The code implements testing an observed proportion against a null
distribution.  Both binomial and chi-square tests are implemented.  The
code also includes an experimental/prototypical xUnit testing framework
for Prolog and a foreign function interface to C implementations of the
same statistical tests.

This code exists because statistical code for Prolog is hard to find.
At the time I was experimenting with different scoring functions for
evaluating rules in Aleph (an inductive logic programming (ILP) system,
which is a rule learning system for supervised machine learning tasks).
Basically, a rule is evaluated based on how many positive (correct)
examples it covers (#p) versus how many negative (false) examples it
covers (#n).  The standard way to score a rule is the difference #p-#n.
Another way to assess a rule, and what I was researching, was to test
the proportion #p/(#p+#n) against some expected prior like 1/2 or 1/10.
Aleph is implemented in Prolog and typically runs on Yap, so a Prolog
implementation of these statistical tests was needed (and could not be
found) and thus this code was born.

If this sounds like interesting research, let me know.  With the right
data, there's probably a paper in this somewhere.


Copyright (c) 2013 Aubrey Barnard.  This is free software.  See
LICENSE for details.
