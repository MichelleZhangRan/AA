/**
 * @file local_search_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#include "utils/logger.hpp"
#include "utils/simple_single_local_search_components.hpp"

#include "paal/local_search/custom_components.hpp"

#include <boost/test/unit_test.hpp>

#include <vector>

namespace ls = paal::local_search;
using namespace paal;

namespace {
ON_LOG(int i = 0;)

bool log_action(int s) {
    LOGLN("f(" << s << ") \t" << f(s) << " after " << ++i);
    return true;
}
;

} // anonymous namespace
BOOST_AUTO_TEST_SUITE(local_search)

BOOST_AUTO_TEST_CASE(local_search_first_improving_test) {
    // printing
    int solution(0);
    LOGLN("f(" << solution << ") \t" << f(solution));
    utils::always_false nop;

    // search
    BOOST_CHECK(ls::local_search(solution, ls::first_improving_strategy{},
                                 log_action, nop, search_comps()));
    BOOST_CHECK_EQUAL(solution, 6);
}

BOOST_AUTO_TEST_CASE(local_search_best_improving_test) {
    // printing
    int s(0);
    LOGLN("f(" << s << ") \t" << f(s));
    ON_LOG(i = 0);
    utils::always_false nop;

    // search
    BOOST_CHECK(ls::local_search(s, ls::best_improving_strategy{}, log_action,
                                 nop, search_comps()));
    BOOST_CHECK_EQUAL(s, 6);
}

BOOST_AUTO_TEST_CASE(local_search_best_test) {
    // printing
    int s(0);
    LOGLN("f(" << s << ") \t" << f(s));
    ON_LOG(i = 0);

    auto stop_after_five_iterations = paal::utils::make_not_functor(
        paal::local_search::stop_condition_count_limit{ 5 });
    auto on_success =
        paal::utils::make_and_functor(log_action, stop_after_five_iterations);

    // search
    BOOST_CHECK(ls::best(s, on_success, search_comps{}));
    BOOST_CHECK_EQUAL(s, 6);
}

BOOST_AUTO_TEST_SUITE_END()
