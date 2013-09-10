// Copyright (c) 2010 Aubrey Barnard.  This is free software.  See
// LICENSE for details.

/*
 * Tests the functions in proportionTest.c.
 */

#define PROPORTION_TEST_TEST_C

#include <stdlib.h>
#include <math.h>

#include <check.h>

#include "proportionTest.c"

#define SIG_DIGITS 10


// Function to compare doubles (handles exponents)
int compare_doubles(double expected, double actual, int significant_digits)
{
  // Get the exponent for each double
  int expected_exp = (int) floor(log10(expected));
  int actual_exp = (int) floor(log10(actual));
  int exp = (expected_exp < actual_exp ? expected_exp : actual_exp);

  // Compare to within 10^(exp - significant_digits)
  double tolerance = pow(10.0, (double) (exp - significant_digits));
  double difference = fabs(expected - actual);
  //printf("expected_exp: %i actual_exp: %i exp: %i tolerance: %.2g difference: %.15g\n", expected_exp, actual_exp, exp, tolerance, difference);
  return (difference < tolerance);
}


// Asserts two doubles are equal for unit testing
void assert_equals(double expected, double actual, int significant_digits)
{
  if (!compare_doubles(expected, actual, significant_digits)) {
    fail("Doubles not equal: Expected: %.12g Actual: %.12g", expected, actual);
  }
}


// Test data for binomial distribution PDF
#define test_binomial_data_length 15

// Generated with following python code:
// (1-5): nt = random.randint(1, 1000); ns = random.randint(0, nt); (ns, nt, random.random())
// (6-10): nt = random.randint(1, 100); ns = random.randint(0, nt); (ns, nt, random.random())
// (11-15): nt = random.randint(1, 10); ns = random.randint(0, nt); (ns, nt, random.random())
int test_binomial_data_successes[] = {
  392,
  172,
  174,
  162,
  185,

  53,
  43,
  11,
  24,
  2,

  4,
  0,
  3,
  2,
  1
};

int test_binomial_data_trials[] = {
  620,
  485,
  629,
  351,
  196,

  54,
  77,
  11,
  57,
  2,

  10,
  1,
  6,
  6,
  1
};

double test_binomial_data_prob[] = {
  0.81119835850039301,
  0.56586248581055776,
  0.31834635978808645,
  0.63385150309939231,
  0.37044608582094984,

  0.14310979691793868,
  0.85737100412326572,
  0.41907865288461432,
  0.066572549173138817,
  0.49146400180019212,

  0.64328116418045522,
  0.33919458730124885,
  0.42610226057295575,
  0.64190743415040585,
  0.73409989547983279
};

// Generated using R with options(digits=15): dbinom(successes, trials, prob)
double test_binomial_data_answer[] = {
  8.58994240095824e-26,
  3.89126507976308e-21,
  0.00267495784579504,
  1.96584040659893e-11,
  3.11204257196861e-65,

  8.2374976754006e-44,
  1.90514859509277e-10,
  7.00246475120943e-05,
  4.44729047409375e-14,
  0.241536865065459,

  0.074093291291514,
  0.660805412698751,
  0.292465775892408,
  0.101628915178332,
  0.734099895479833
};


// Test the binomial distribution PDF calculations
START_TEST(test_binomial)
{
  for (int test_index = 0; test_index < test_binomial_data_length; test_index++) {
    double result = binomial(test_binomial_data_successes[test_index], test_binomial_data_trials[test_index], test_binomial_data_prob[test_index]);
    //printf("Expected: %.15g Actual: %.15g\n", test_binomial_data_answer[test_index], result);
    assert_equals(test_binomial_data_answer[test_index], result, SIG_DIGITS);
  }
}
END_TEST;


// Test data for binomial_integral
#define test_binomial_integral_data_length 9

// Generated with the following Python:
// nt = random.randint(2, 1000); start = random.randint(0, nt - 1); (start, random.randint(start + 1, nt), nt, random.random())
// Again 3 groups for 1000, 100, 10
int test_binomial_integral_data_start[] = {
  53,
  473,
  271,

  5,
  32,
  28,

  7,
  3,
  2
};

int test_binomial_integral_data_end[] = {
  416,
  491,
  317,

  46,
  52,
  40,

  9,
  8,
  5
};

int test_binomial_integral_data_trials[] = {
  817,
  514,
  343,

  85,
  57,
  97,

  9,
  9,
  5
};

double test_binomial_integral_data_prob[] = {
  0.50258268609630641,
  0.027414451278221286,
  0.9408201930093304,

  0.15271239212919374,
  0.44934653940215386,
  0.82166879021551376,

  0.4115965021751895,
  0.51216129787667342,
  0.86212696681350454
};

// Generated using R with options(digits=15): pbinom(end, trials, prob) - pbinom(start - 1, trials, prob)
double test_binomial_integral_data_answer[] = {
  0.65984740219146,
  0.0,
  0.119114220132441,

  0.997826988693654,
  0.0588579463007108,
  2.76347687604193e-19,

  0.0296444371533974,
  0.919130073559746,
  0.998392571428867
};


// Test the binomial integral 
START_TEST(test_binomial_integral)
{
  for (int test_index = 0; test_index < test_binomial_integral_data_length; test_index++) {
    double result = binomial_integral(test_binomial_integral_data_start[test_index], test_binomial_integral_data_end[test_index], test_binomial_integral_data_trials[test_index], test_binomial_integral_data_prob[test_index]);
    //printf("Expected: %.15g Actual: %.15g\n", test_binomial_integral_data_answer[test_index], result);
    assert_equals(test_binomial_integral_data_answer[test_index], result, SIG_DIGITS);
  }
}
END_TEST;


// Test data for two-tailed binomial test
#define test_binomial_twotailed_data_length 9

// Again 3 groups for 1000, 100, 10
int test_binomial_twotailed_data_successes[] = {
  133,
  541,
  272,

  49,
  8,
  14,

  4,
  3,
  8
};

int test_binomial_twotailed_data_trials[] = {
  393,
  547,
  479,

  65,
  32,
  34,

  8,
  3,
  9
};

double test_binomial_twotailed_data_prob[] = {
  0.73549094368402157,
  0.072119113246073119,
  0.99272523232985577,

  0.21873573703754501,
  0.93714825640860089,
  0.92874809760063326,

  0.6495356843124559,
  0.19134931424597135,
  0.30584324339755709
};

double test_binomial_twotailed_data_answer[] = {
  1.08719497880240e-60,
  0.0,
  2.37645111542111e-303,

  6.2068549823656e-20,
  9.23457859750034e-23,
  5.92640629517219e-15,

  0.463037655923138,
  0.00700617095935884,
  0.00050170465408135
};


// Test the two-tailed binomial test
START_TEST(test_binomial_twotailed)
{
  for (int test_index = 0; test_index < test_binomial_twotailed_data_length; test_index++) {
    double result = binomial_test_twotailed(test_binomial_twotailed_data_successes[test_index], test_binomial_twotailed_data_trials[test_index], test_binomial_twotailed_data_prob[test_index]);
    //printf("Expected: %.15g Actual: %.15g\n", test_binomial_twotailed_data_answer[test_index], result);
    assert_equals(test_binomial_twotailed_data_answer[test_index], result, SIG_DIGITS);
  }
}
END_TEST;
// Had problem with right tail of 298, 320, 0.893808573358. R says 0.0085303750983766. I say 0.0146481499441656. Hmm.


// Test data for proportion tests
#define test_proportions_data_length 20

int test_proportions[20][2] = {
  {5347, 5970},
  {7595, 6073},
  {7708, 9930},
  {8526, 6552},
  {7208, 6305},

  {598, 79},
  {284, 446},
  {762, 989},
  {18, 942},
  {531, 978},

  {78, 97},
  {67, 38},
  {6, 63},
  {82, 22},
  {33, 21},

  {4, 7},
  {3, 5},
  {7, 9},
  {1, 3},
  {4, 10}
};

// Generated by R with options(digits=15): binom.test(c(prop1, prop2), p=0.5, alternative="two.sided")$p.value
double test_proportions_binom_ans[] = {
  4.96569444330095e-09,
  9.00819116371014e-39,
  6.10564978996164e-63,
  2.96299054376206e-58,
  8.34142595979523e-15,

  1.48682339486739e-99,
  2.19369487864294e-09,
  6.36701897425784e-08,
  1.33504491190077e-251,
  6.05986681095489e-31,

  0.173443727973984,
  0.00601608151524935,
  4.47352983649895e-13,
  2.56830196979963e-09,
  0.133674235364105,

  0.548828125,
  0.7265625,
  0.803619384765625,
  0.625,
  0.1795654296875
};

double test_proportions_chisq_ans[] = {
  4.73328564072474e-09,
  9.59929941647448e-39,
  7.80457958627232e-63,
  3.76489477935706e-58,
  7.97052931445024e-15,

  1.59824761055216e-88,
  2.02375222637581e-09,
  5.80233104521966e-08,
  2.02776677867658e-195,
  1.21606957458038e-30,

  0.150926950066716,
  0.00465319720901331,
  6.79060945202553e-12,
  4.01718815444534e-09,
  0.102470434859750,

  0.365712296281513,
  0.479500122186953,
  0.617075077451974,
  0.317310507862914,
  0.108809430040546
};


// Test binomial test with proportions and 0.5 probability
START_TEST(test_binomial_test)
{
  for (int test_index = 0; test_index < test_proportions_data_length; test_index++) {
    double result = binomial_test(test_proportions[test_index][0], test_proportions[test_index][1]);
    //printf("Expected: %.15g Actual: %.15g\n", test_proportions_binom_ans[test_index], result);
    // Relax the accuracy requirement slightly for the first 5 proportions which are so large for an exact test
    int digits = (test_index < 5 ? SIG_DIGITS - 1 : SIG_DIGITS);
    assert_equals(test_proportions_binom_ans[test_index], result, digits);
  }
}
END_TEST;


// Test chisquare test with proportions and 0.5 probability
START_TEST(test_chisquare_test)
{
  for (int test_index = 0; test_index < test_proportions_data_length; test_index++) {
    double result = chisquare_test(test_proportions[test_index][0], test_proportions[test_index][1]);
    //printf("Expected: %.15g Actual: %.15g\n", test_proportions_chisq_ans[test_index], result);
    assert_equals(test_proportions_chisq_ans[test_index], result, SIG_DIGITS);
  }
}
END_TEST;


// Define a suite for these tests
Suite * proportionTestTest_suite()
{
  // Create a suite with a single test case
  Suite * suite = suite_create("proportionTestTest");
  TCase * test_case = tcase_create("proportionTestTest");
  suite_add_tcase(suite, test_case);

  // Add all the tests to the test case
  tcase_add_test(test_case, test_binomial);
  tcase_add_test(test_case, test_binomial_integral);
  tcase_add_test(test_case, test_binomial_twotailed);
  tcase_add_test(test_case, test_binomial_test);
  tcase_add_test(test_case, test_chisquare_test);

  return suite;
}


// Main
int main()
{
  // Create all the suites to run
  Suite * proportion_suite = proportionTestTest_suite();

  // Set up the runner and run the tests
  SRunner * runner = srunner_create(proportion_suite);
  srunner_run_all(runner, CK_NORMAL);
  int number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  // Return success or failure
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
