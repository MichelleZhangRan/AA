#include "linear_programming/lp/lp_compilation_error_setup.hpp"

BOOST_FIXTURE_TEST_SUITE(lp_compilation_error, compilation_error_setup)

BOOST_AUTO_TEST_CASE(lp_compilation_error_X_div_Y) {
#ifdef _COMPILATION_ERROR_TEST_
    expr = X / Y;
#else
    expr = X + Y;
#endif
}

BOOST_AUTO_TEST_SUITE_END()
