/**
 * @file knapsack_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-09-20
 */

#define BOOST_TEST_MODULE knapsack_test
#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/range/irange.hpp>
#include <boost/fusion/include/for_each.hpp>

#include "paal/dynamic/knapsack.hpp"
#include "paal/dynamic/knapsack_0_1.hpp"
#include "paal/dynamic/knapsack_fptas.hpp"
#include "paal/dynamic/knapsack_0_1_fptas.hpp"
#include "paal/utils/double_rounding.hpp"

#include "utils/logger.hpp"

namespace pd = paal::detail;
using namespace paal;


std::vector<int> sizes{1,2,3,1,2,4,1,10};
int capacity = 6;
std::vector<int> values{3,2,65,1,2,3,1,23};
auto objects = boost::irange(size_t(0), sizes.size());
auto sizesFunctor = paal::utils::make_ArrayToFunctor(sizes);
auto valuesFunctor = paal::utils::make_ArrayToFunctor(values);
static const double OPT = 130;
static const double OPT_0_1 = 70;
static const double OPT_CAP = 6;
static const double EPSILON = 1./4.;
static const double VALUE_MULTIPLIER = 1. - EPSILON;
static const double SIZE_MULTIPLIER = 1. + EPSILON;
static const utils::Compare<double> compare(0.001);
    
template <typename MaxValue> 
void check(MaxValue maxValue, const std::vector<int> & result, pd::NoZeroOneTag, double valMultiplier = 1, double capMultiplier = 1) {
    BOOST_CHECK(compare.ge(maxValue.first, OPT * valMultiplier));
    BOOST_CHECK(compare.ge(OPT_CAP * capMultiplier, maxValue.second));
    LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOG_COPY_DEL(result.begin(), result.end(), " ");
    LOGLN("");
}

template <typename MaxValue> 
void check_no_output(MaxValue maxValue, const std::vector<int> & result, pd::ZeroOneTag, double valMultiplier = 1, double capMultiplier = 1) {
    BOOST_CHECK(compare.ge(maxValue.first, OPT_0_1 * valMultiplier));
    BOOST_CHECK(compare.ge(OPT_CAP * capMultiplier, maxValue.second));
    LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOGLN("");
}

template <typename MaxValue> 
void check(MaxValue maxValue, const std::vector<int> & result, pd::ZeroOneTag, double valMultiplier = 1, double capMultiplier = 1) {
    BOOST_CHECK(compare.ge(maxValue.first, OPT_0_1 * valMultiplier));
    BOOST_CHECK(compare.ge(OPT_CAP * capMultiplier, maxValue.second));
    LOGLN("Max value " << maxValue.first << ", Total size "  << maxValue.second);
    LOG_COPY_DEL(result.begin(), result.end(), " ");
    LOGLN("");
}

//Knapsack 
BOOST_AUTO_TEST_CASE(Knapsack) {
    std::vector<int> result;
    LOGLN("Knapsack");
    auto maxValue = paal::knapsack(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check(maxValue, result, pd::NoZeroOneTag());
}

template <typename T>
std::string to_string(T) {
    return "";
}

std::string to_string(pd::IntegralValueAndSizeTag) {
    return "value and size";
}

std::string to_string(pd::IntegralValueTag) {
    return "value";
}

std::string to_string(pd::IntegralSizeTag) {
    return "size";
}

std::string to_string(pd::NoRetrieveSolutionTag) {
    return "without output";
}

std::string to_string(pd::ZeroOneTag) {
    return "0/1";
}


template <typename IntegralTag, typename IsZeroOne, typename RetrieveSolution = pd::RetrieveSolutionTag> 
void detail_knapsack() {
    std::vector<int> result;
    LOGLN("Knapsack " << to_string(IsZeroOne())  <<  " on " + to_string(IntegralTag()) + " " + to_string(RetrieveSolution()));
    auto maxValue = pd::knapsack(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor,
            IsZeroOne(),
            IntegralTag(),
            RetrieveSolution());

    check(maxValue, result, IsZeroOne());
}


BOOST_AUTO_TEST_CASE(KnapsackOverloads) {
    detail_knapsack<pd::IntegralValueTag,        pd::NoZeroOneTag>();
    detail_knapsack<pd::IntegralSizeTag,         pd::NoZeroOneTag>();
    detail_knapsack<pd::IntegralValueAndSizeTag, pd::NoZeroOneTag>();
    detail_knapsack<pd::IntegralValueTag,        pd::ZeroOneTag, pd::RetrieveSolutionTag>();
    detail_knapsack<pd::IntegralSizeTag,         pd::ZeroOneTag, pd::RetrieveSolutionTag>();
    detail_knapsack<pd::IntegralValueAndSizeTag, pd::ZeroOneTag, pd::RetrieveSolutionTag>();
    detail_knapsack<pd::IntegralValueTag,        pd::ZeroOneTag, pd::NoRetrieveSolutionTag>();
    detail_knapsack<pd::IntegralSizeTag,         pd::ZeroOneTag, pd::NoRetrieveSolutionTag>();
    detail_knapsack<pd::IntegralValueAndSizeTag, pd::ZeroOneTag, pd::NoRetrieveSolutionTag>();
}


//Knapsack 0/1
BOOST_AUTO_TEST_CASE(Knapsack_0_1) {
    std::vector<int> result;
    LOGLN("Knapsack 0/1");
    auto maxValue = paal::knapsack_0_1(std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);
    check(maxValue, result, pd::ZeroOneTag());
}

BOOST_AUTO_TEST_CASE(Knapsack_0_1_no_output) {
    std::vector<int> result;
    LOGLN("Knapsack 0/1 no output");
    auto maxValue = paal::knapsack_0_1_no_output(std::begin(objects), std::end(objects), 
            capacity,
            sizesFunctor, 
            valuesFunctor);

    check_no_output(maxValue, result, pd::ZeroOneTag());
}

//Knapsack fptas value
BOOST_AUTO_TEST_CASE(Knapsack_fptas_value) {
    std::vector<int> result;
    LOGLN("Knapsack fptas value");
    auto maxValue = paal::knapsack_on_value_fptas(EPSILON, std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check(maxValue, result, pd::NoZeroOneTag(), VALUE_MULTIPLIER);
}

//TODO this tests is very weak because it runs standard algorithm no fptas
//Knapsack fptas size
BOOST_AUTO_TEST_CASE(Knapsack_fptas_size) {
    std::vector<int> result;
    LOGLN("Knapsack fptas size");
    auto maxValue = paal::knapsack_on_size_fptas(EPSILON, std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check(maxValue, result, pd::NoZeroOneTag(), 1., SIZE_MULTIPLIER);
}

//Knapsack 0/1: no output iterator size fptas
BOOST_AUTO_TEST_CASE(Knapsack_0_1_no_output_size_fptas) {
    std::vector<int> result;
    LOGLN("Knapsack 0/1 no output size fptas");
    auto maxValue = paal::knapsack_0_1_no_output_on_size_fptas(EPSILON, std::begin(objects), std::end(objects), 
            capacity,
            sizesFunctor, 
            valuesFunctor);

    check_no_output(maxValue, result, pd::ZeroOneTag(), 1, SIZE_MULTIPLIER);
}

//Knapsack 0/1: no output iterator value fptas
BOOST_AUTO_TEST_CASE(Knapsack_0_1_no_output_value_fptas) {
    std::vector<int> result;
    LOGLN("Knapsack 0/1 no output value fptas");
    auto maxValue = paal::knapsack_0_1_no_output_on_value_fptas(EPSILON, std::begin(objects), std::end(objects), 
            capacity,
            sizesFunctor, 
            valuesFunctor);

    check_no_output(maxValue, result, pd::ZeroOneTag(), VALUE_MULTIPLIER);
}

//Knapsack 0/1 value fptas
BOOST_AUTO_TEST_CASE(Knapsack_0_1_value_fptas) {
    std::vector<int> result;
    LOGLN("Knapsack 0/1 on value fptas");
    auto maxValue = paal::knapsack_0_1_on_value_fptas(EPSILON, std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check(maxValue, result, pd::ZeroOneTag(), VALUE_MULTIPLIER);
}

//Knapsack 0/1  size fptas
BOOST_AUTO_TEST_CASE(Knapsack_0_1_size_fptas) {
    std::vector<int> result;
    LOGLN("Knapsack 0/1 on size fptas");
    auto maxValue = paal::knapsack_0_1_on_size_fptas(EPSILON, std::begin(objects), std::end(objects), 
            capacity,
            std::back_inserter(result), 
            sizesFunctor, 
            valuesFunctor);

    check(maxValue, result, pd::ZeroOneTag(), 1, SIZE_MULTIPLIER);
}

