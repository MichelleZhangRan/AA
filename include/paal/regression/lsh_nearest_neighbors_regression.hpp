//=======================================================================
// Copyright (c) 2014 Andrzej Pacuk, Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file lsh_nearest_neighbors_regression.hpp
 * @brief
 * @author Andrzej Pacuk, Piotr Wygocki
 * @version 1.0
 * @date 2014-10-06
 */
#ifndef PAAL_LSH_NEAREST_NEIGHBOURS_REGRESSION_HPP
#define PAAL_LSH_NEAREST_NEIGHBOURS_REGRESSION_HPP

#include "paal/utils/accumulate_functors.hpp"
#include "paal/utils/hash.hpp"
#include "paal/utils/hash_functions.hpp"
#include "paal/utils/type_functions.hpp"

#include <boost/range/algorithm/transform.hpp>
#include <boost/range/combine.hpp>
#include <boost/range/empty.hpp>
#include <boost/range/size.hpp>

#include <functional>
#include <iterator>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace paal {


/**
 * @brief
 *
 * @tparam FunctionGenerator
 */
template <typename HashValue,
          typename FunctionGenerator = hash::default_hash_function_generator>
class hash_function_tuple_generator {
    using fun_t = pure_result_of_t<FunctionGenerator()>;
    using funs_t = std::vector<fun_t>;
    FunctionGenerator m_function_generator;
    unsigned m_hash_functions_per_point;
public:
    /**
     * @brief
     *
     * @param function_generator
     * @param hash_functions_per_point number of hash functions in single tuple
     */
    hash_function_tuple_generator(FunctionGenerator function_generator,
                                  unsigned hash_functions_per_point) :
        m_function_generator(std::forward<FunctionGenerator>(function_generator)),
        m_hash_functions_per_point(hash_functions_per_point) {
    }

    /**
     * @brief functor represting tuple of hash functions
     */
    class hash_function_tuple {
        funs_t m_hash_funs;
        mutable std::vector<HashValue> m_values;

    public:

        /**
         * @brief
         *
         * @param funs
         */
        hash_function_tuple(funs_t funs) : m_hash_funs(std::move(funs)) {
            m_values.reserve(m_hash_funs.size());
        }


        /**
         * @brief
         *
         * @tparam Point
         * @param point
         *
         * @return vector of hash functions results
         * This value is returned by reference, because in usual case the caller
         * is not going to store the result. When the program parameters are not tuned or
         * in very special cases this might be potentially inefficient.
         */
        template <typename Point>
        auto const & operator()(Point &&point) const {
            m_values.clear();
            //TODO, this function, in many cases (test for example) might actually return some transformation of point
            //this can be done since std::unordered_map::find is more general now.
            boost::transform(m_hash_funs, std::back_inserter(m_values),
                             [&](fun_t const &fun) { return fun(point); });

            return m_values;
        }
    };

    /**
     * @brief
     *
     * @return hash_function_tuple of m_hash_functions_per_point hash functions
     */
    //TODO change to auto, when it starts working
    hash_function_tuple operator()() {
        funs_t hash_funs;
        hash_funs.reserve(m_hash_functions_per_point);
        std::generate_n(std::back_inserter(hash_funs),
                        m_hash_functions_per_point,
                        std::ref(m_function_generator));

        return hash_function_tuple(std::move(hash_funs));
    }
};

/**
 * @brief
 *
 * @tparam FunctionGenerator
 * @param function_generator functor generating hash functions
 * @param hash_functions_per_point number of hash functions in single tuple
 *
 * @return
 */
template <typename HashValue, typename FunctionGenerator>
auto make_hash_function_tuple_generator(FunctionGenerator &&function_generator,
                                        unsigned hash_functions_per_point) {
    return hash_function_tuple_generator<HashValue, FunctionGenerator>(
                std::forward<FunctionGenerator>(function_generator),
                hash_functions_per_point);
}

/**
 * @brief For each query point counts average result of train points with hash
 * equal to query point's hash, basing on Locality-sensitve hashing.
 *
 * Example: <br>
   \snippet lsh_nearest_neighbors_regression_example.cpp LSH Nearest Neighbors Regression Example
 *
 * example file is lsh_nearest_neighbors_regression_example.cpp
 *
 * @tparam HashValue return type of functions generated by LshFunctionGenerator object
 * @tparam LshFunctionGenerator type of functor which generates proper LSH functions
 * @tparam HashForHashValue hash type to be used in hash maps
 */
template <typename HashValue,
          typename LshFunctionGenerator = hash_function_tuple_generator<HashValue>,
          //TODO default value here supposed to be std::hash
          typename HashForHashValue = range_hash<HashValue>>
class lsh_nearest_neighbors_regression {
    LshFunctionGenerator m_lsh_function_generator;

    //TODO template param QueryResultType
    using res_accu_t = average_accumulator<>;
    using map_t = std::unordered_map<HashValue, res_accu_t, HashForHashValue>;
    using lsh_fun_t = pure_result_of_t<LshFunctionGenerator()>;

    ///hash maps containing average result for each hash key
    std::vector<map_t> m_hash_maps;
    ///hash functions
    std::vector<lsh_fun_t> m_hashes;

    ///average result of all train points
    average_accumulator<> m_avg;

public:

    /**
     * @brief initializes model and trains model using train points and results
     *
     * @tparam TrainPoints
     * @tparam TrainResults
     * @param train_points
     * @param train_results
     * @param passes number of used LSH functions
     * @param lsh_function_generator functor generating proper LSH functions
     * @param threads_count does not work yet (algoritm is currently only single threaded)
     */
    template <typename TrainPoints, typename TrainResults>
    lsh_nearest_neighbors_regression(
            TrainPoints &&train_points, TrainResults &&train_results,
            unsigned passes,
            LshFunctionGenerator &&lsh_function_generator,
            unsigned threads_count) :
        m_lsh_function_generator(lsh_function_generator),
        m_hash_maps(passes) {

        m_hashes.reserve(passes);
        std::generate_n(std::back_inserter(m_hashes), passes,
                    std::ref(lsh_function_generator));

        update(std::forward<TrainPoints>(train_points),
               std::forward<TrainResults>(train_results),
               threads_count);
    }

    /**
     * @brief constructor using maximal supported number of concurrent threads
     *
     * @tparam TrainPoints
     * @tparam TrainResults
     * @param train_points
     * @param train_results
     * @param passes number of used LSH functions
     * @param lsh_function_generator functor generating proper LSH functions
     */
    template <typename TrainPoints, typename TrainResults>
    lsh_nearest_neighbors_regression(
             TrainPoints &&train_points, TrainResults &&train_results,
             unsigned passes,
             LshFunctionGenerator &&lsh_function_generator
             ) :
        lsh_nearest_neighbors_regression(train_points, train_results, passes,
                lsh_function_generator, std::thread::hardware_concurrency()) {
    }

    /**
     * @brief trains model
     *
     * @tparam TrainPoints
     * @tparam TrainResults
     * @param train_points
     * @param train_results
     * @param threads_count does not work yet (algoritm is currently only single threaded)
     */
    template <typename TrainPoints, typename TrainResults>
    void update(TrainPoints &&train_points, TrainResults &&train_results,
                unsigned threads_count) {
        //TODO This is nice solution because train_points range is Single-Pass,
        //it would be nice to keep this when threads are added
        for (auto &&train_point_result : boost::combine(train_points, train_results)) {
            auto const &point = boost::get<0>(train_point_result);
            auto const &res = boost::get<1>(train_point_result);
            m_avg.add_value(res);

            for (auto &&map_and_fun : boost::combine(m_hash_maps, m_hashes)) {
                auto &map = boost::get<0>(map_and_fun);
                auto const &fun = boost::get<1>(map_and_fun);
                map[fun(point)].add_value(res);
            }
        }
    }

    /**
     * @brief trains model using maximal supported number of concurrent threads
     *
     * @tparam TrainPoints
     * @tparam TrainResults
     * @param train_points
     * @param train_results
     */
    template <typename TrainPoints, typename TrainResults>
    void update(TrainPoints &&train_points, TrainResults &&train_results) {
        update(std::forward<TrainPoints>(train_points),
               std::forward<TrainResults>(train_results),
               std::thread::hardware_concurrency());
    }

    /**
     * @brief queries model
     *
     * @tparam QueryPoints
     * @tparam OutputIterator
     * @param query_points
     * @param result
     * @param threads_count does not work yet (algoritm is currently only single threaded)
     */
    template <typename QueryPoints, typename OutputIterator>
    void test(QueryPoints &&query_points, OutputIterator result,
              unsigned threads_count) const {
        assert(!m_avg.empty());
        //TODO This is nice solution because query_points range is Single-Pass,
        //it would be nice to keep this when threads are added
        for (auto &&query_point : query_points) {
            average_accumulator<> avg;
            for(auto && map_and_fun : boost::combine(m_hash_maps, m_hashes)) {
                auto const &map = boost::get<0>(map_and_fun);
                auto const &fun = boost::get<1>(map_and_fun);
                auto got = map.find(fun(query_point));
                if (got != map.end()) {
                    avg.add_value(got->second.get_average_unsafe());
                }
            }
            *result = avg.get_average(m_avg.get_average());
            ++result;
        }
    }


    /**
     * @brief queries model using maximal supported number of concurrent threads
     *
     * @tparam QueryPoints
     * @tparam OutputIterator
     * @param query_points
     * @param result
     */
    template <typename QueryPoints, typename OutputIterator>
    void test(QueryPoints &&query_points, OutputIterator result) const {
        test(std::forward<QueryPoints>(query_points),
              result, std::thread::hardware_concurrency());
    }
};

/**
 * @brief this is the most general version of the make_lsh_nearest_neighbors_regression,
 *        It takes any hash function generator.
 *
 * @tparam TrainPoints
 * @tparam TrainResults
 * @tparam LshFunctionGenerator
 * @param train_points
 * @param train_results
 * @param passes number of used LSH functions
 * @param lsh_function_generator functor generating proper LSH functions
 * @param threads_count does not work yet (algoritm is currently only single threaded)
 *
 * @return lsh_nearest_neighbors_regression model
 */
template <typename TrainPoints, typename TrainResults,
          typename LshFunctionGenerator>
auto make_lsh_nearest_neighbors_regression(
             TrainPoints &&train_points, TrainResults &&train_results,
             unsigned passes,
             LshFunctionGenerator &&lsh_function_generator,
             unsigned threads_count = std::thread::hardware_concurrency()) {
    using lsh_fun = pure_result_of_t<LshFunctionGenerator()>;
    using hash_result = pure_result_of_t<lsh_fun(range_to_ref_t<TrainPoints>)>;
    return lsh_nearest_neighbors_regression<hash_result, LshFunctionGenerator>(
            std::forward<TrainPoints>(train_points),
            std::forward<TrainResults>(train_results),
            passes,
            std::forward<LshFunctionGenerator>(lsh_function_generator),
            threads_count);
}


/**
 * @brief This is the special version  of make_lsh_nearest_neighbors_regression.
 *        This version assumes that hash function is concatenation (tuple) of several hash functions.
 *        In this function user provide Function generator for the inner  functions only.
 *
 *
 *
 * @tparam TrainPoints
 * @tparam TrainResults
 * @tparam FunctionGenerator
 * @param train_points
 * @param train_results
 * @param passes
 * @param function_generator
 * @param hash_functions_per_point
 * @param threads_count
 *
 * @return
 */
template <typename TrainPoints, typename TrainResults,
          typename FunctionGenerator>
auto make_lsh_nearest_neighbors_regression_tuple_hash(
             TrainPoints &&train_points, TrainResults &&train_results,
             unsigned passes,
             FunctionGenerator &&function_generator,
             unsigned hash_functions_per_point,
             unsigned threads_count = std::thread::hardware_concurrency()) {
    using lsh_fun = pure_result_of_t<FunctionGenerator()>;
    using hash_result_single = pure_result_of_t<lsh_fun(range_to_ref_t<TrainPoints>)>;
    auto tuple_lsh = paal::make_hash_function_tuple_generator<hash_result_single>(
                    std::forward<FunctionGenerator>(function_generator),
                    hash_functions_per_point);
    return make_lsh_nearest_neighbors_regression(
            std::forward<TrainPoints>(train_points),
            std::forward<TrainResults>(train_results),
            passes,
            std::move(tuple_lsh),
            threads_count);
}

} //! paal

#endif // PAAL_LSH_NEAREST_NEIGHBOURS_REGRESSION_HPP
