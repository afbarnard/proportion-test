// Copyright (c) 2010 Aubrey Barnard.  This is free software.  See
// LICENSE for details.

/*
 * Statistical tests of two proportions.
 *
 * Also includes code for using proportion test as a predicate for Yap.
 */

#include <stdio.h>
#include <math.h>

/*
 * Binomial distribution PDF.
 *
 * Here's the math for the following code.
 * B(k; n, p) = n! / (k! * (n - k)!) * p^k * (1 - p)^(n - k) [1]
 *
 * log(B(k; n, p)) = log(n!) - log(k! * (n - k)!) + log(p^k * (1 - p)^(n - k)) [2]
 * = log(n!) - (log(k!) + log((n - k)!)) + log(p^k) + log((1 - p)^(n - k))
 * = log(n!) - log(k!) - log((n - k)!) + k * log(p) + (n - k) * log(1 - p)
 * = log(gamma(n + 1)) - log(gamma(k + 1)) - log(gamma(n - k + 1)) + k * log(p) + (n - k) * log(1 - p)
 * = lgamma(n + 1) - lgamma(k + 1) - lgamma(n - k + 1) + k * log(p) + (n - k) * log(1 - p)
 *
 * B(k; n, p) = exp(lgamma(n + 1) - lgamma(k + 1) - lgamma(n - k + 1) + k * log(p) + (n - k) * log(1 - p))
 *
 * [1] http://en.wikipedia.org/wiki/Binomial_distribution
 * [2] Credit for the log approach goes to SciPy.
 */
double binomial(int number_successes, int number_trials, double success_prob)
{
  return exp(
	     lgamma((double) (number_trials + 1)) -
	     lgamma((double) (number_successes + 1)) -
	     lgamma((double) (number_trials - number_successes + 1)) +
	     (double) number_successes * log(success_prob) +
	     (double) (number_trials - number_successes) * log(1.0 - success_prob)
	     );
}


/*
 * Binomial distribution integral (like CDF, but for arbitrary start and
 * end points). Uses sums, not incomplete beta function. End points
 * included.
 */
double binomial_integral(int start, int end, int number_trials, double success_prob)
{
  double sum = 0.0;
  for (int number_successes = start; number_successes <= end; number_successes++) {
    sum += binomial(number_successes, number_trials, success_prob);
  }
  return sum;
}


/*
 * Calculates the p-value for a two-tailed binomial test with the given
 * arguments.
 */
double binomial_test_twotailed(int number_successes, int number_trials, double success_prob)
{
  //printf("successes: %d trials: %d prob: %.12g\n", number_successes, number_trials, success_prob);
  // Calculate the effect size and the other limit
  double mean = success_prob * (double) number_trials;
  double opposite_successes = 2.0 * mean - (double) number_successes;

  // Make sure the limits are on the correct sides of each other
  // Round opposite_successes away from the mean
  int left_limit;
  int right_limit;
  if (opposite_successes < (double) number_successes) {
    left_limit = (int) floor(opposite_successes);
    right_limit = number_successes;
  } else {
    left_limit = number_successes;
    right_limit = (int) ceil(opposite_successes);
  }

  // Calculate the values in the tails
  double left_tail = binomial_integral(0, left_limit, number_trials, success_prob);
  double right_tail = binomial_integral(right_limit, number_trials, number_trials, success_prob);
  //printf("mean: %.12g left_limit: %d right_limit: %d\n", mean, left_limit, right_limit);
  //printf("left_tail: %.12g right_tail: %.12g\n", left_tail, right_tail);
  // The p-value is the sum of the two tails
  return left_tail + right_tail;
}


/*
 * Calculates the p-value for a two-tailed binomial test for the given
 * proportions versus the null hypothesis of equal proportions.
 */
double binomial_test(int proportion1, int proportion2)
{
  return binomial_test_twotailed(proportion1, proportion1 + proportion2, 0.5);
}


/*
 * Calculates the CDF for the chi-square distribution with one degree of
 * freedom.
 *
 * Here's the math for the following code.
 * gammai is the lower incomplete gamma function
 * chisq_cdf(x; k) = gammai(k / 2, x / 2) / gamma(k / 2) [1]
 * chisq_cdf(x; 1) = gammai(1 / 2, x / 2) / gamma(1 / 2)
 * gammai(1 / 2, x) = sqrt(pi) * erf(sqrt(x)) [2]
 * gamma(1 / 2) = sqrt(pi) [3]
 * chisq_cdf(x; 1) = (sqrt(pi) * erf(sqrt(x / 2))) / sqrt(pi)
 * = erf(sqrt(x / 2))
 *
 * [1] http://en.wikipedia.org/wiki/Chi-square_distribution
 * [2] http://en.wikipedia.org/wiki/Incomplete_Gamma_function
 * [3] http://en.wikipedia.org/wiki/Gamma_function
 */
double chisquare_cdf_1df(double x)
{
  return erf(sqrt(x / 2.0));
}


/*
 * Calculates the complement of the CDF (1 - CDF(x)) for the chi-square
 * distribution with one degree of freedom.
 */
double chisquare_cdfc_1df(double x)
{
  return erfc(sqrt(x / 2.0));
}


/*
 * Calculates the p-value for a chi square test for the given
 * proportions versus the null hypothesis of equal proportions.
 */
double chisquare_test(int proportion1, int proportion2)
{
  // Calculate the statistic. Write it out because it's just 2 proportions.
  // Use 0.5 frequency because that's the null hypothesis
  int total = proportion1 + proportion2;
  double expected = total / 2.0;
  double diff1 = (double) proportion1 - expected;
  double diff2 = (double) proportion2 - expected;
  double statistic = diff1 * diff1 / expected + diff2 * diff2 / expected;

  // Call out to calculate the p-value
  double p_value = chisquare_cdfc_1df(statistic);
  //printf("statistic: %.8g p-value: %.8g\n", statistic, p_value);
  return p_value;
}


/*
 * Calculates the p-value for a statistical test of the given
 * proportions versus the null hypothesis of equal proportions. If the
 * sum of the proportions is greater than 200 a chi-square test is used,
 * otherwise a two-tailed binomial test is used.
 */
double proportion_test(int proportion1, int proportion2)
{
  if ((proportion1 + proportion2) > 200) {
    return chisquare_test(proportion1, proportion2);
  } else {
    return binomial_test(proportion1, proportion2);
  }
}


// Only include the Yap stuff if not testing so I don't have to link in Yap for testing
#ifndef PROPORTION_TEST_TEST_C

#include <YapInterface.h>


/*
 * Implements the above proportion test as a Yap predicate.
 *
 * There are 2 input arguments to the predicate: number of positives
 * covered (int), number of negatives covered (int). There is 1 output
 * argument: p-value (float).
 */
YAP_Bool proportion_test_pred(void)
{
  // Convert the input arguments to integers
  YAP_Int positives = YAP_IntOfTerm(YAP_ARG1);
  YAP_Int negatives = YAP_IntOfTerm(YAP_ARG2);

  // Compute the test
  double p_value = proportion_test(positives, negatives);

  // Return the result
  YAP_Term p_value_term = YAP_MkFloatTerm(p_value);
  return YAP_Unify(YAP_ARG3, p_value_term);
}


/*
 * Register the defined C predicates with Yap.
 */
void initialize_predicates()
{
  // Give it the same name as the C function
  YAP_UserCPredicate("proportion_test", &proportion_test_pred, 3L);
}


#endif // End excluding Yap while testing
