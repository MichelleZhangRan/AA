#include "linear_programming/lp/lp_compilation_error_setup.hpp"

BOOST_FIXTURE_TEST_SUITE(lp_compilation_error, compilation_error_setup)

BOOST_AUTO_TEST_CASE(lp_compilation_error_2_div_X) {
#ifdef _COMPILATION_ERROR_TEST_
    expr = 2 / X;
#else
    expr = 2 * X;
#endif
}

BOOST_AUTO_TEST_SUITE_END()
