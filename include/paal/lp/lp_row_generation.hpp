//=======================================================================
// Copyright (c)
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file lp_row_generation.hpp
 * @brief
 * @author Piotr Godlewski
 * @version 1.0
 * @date 2013-06-04
 */
#ifndef LP_ROW_GENERATION_HPP
#define LP_ROW_GENERATION_HPP

#include "paal/lp/lp_base.hpp"
#include "paal/lp/problem_type.hpp"

namespace paal {
namespace lp {

/**
 * @class lp_row_generation
 *
 * @brief Finds an extreme point solution to the LP using row generation.
 */
class lp_row_generation {
  public:
    /**
     * Finds an extreme point solution to the LP using row genereation:
     * solves the initial LP and then ask the separation oracle if the found
     * solution is a feasible solution to the complete problem. If not,
     * adds a new row (generated by the oracle) to the LP and reoptimizes it.
     * This procedure is iterated until a feasible solution to the full LP
     * is found.
     */
    template <typename Problem, typename LP>
    problem_type operator()(Problem & problem, LP & lp, problem_type prob_type) {
        auto & oracle = problem.get_oracle();
        while (prob_type == OPTIMAL && !oracle.feasible_solution(problem, lp)) {
            oracle.add_violated_constraint(problem, lp);
            prob_type = lp.resolve_simplex(DUAL);
        }
        return prob_type;
    }
};

/**
 * @class row_generation_solve_lp
 *
 * @brief Finds an extreme point solution to the LP using row generation.
 */
class row_generation_solve_lp {
  public:
    /**
     * Finds an extreme point solution to the LP using row genereation.
     */
    template <typename Problem, typename LP>
    problem_type operator()(Problem &problem, LP &lp) {
        return m_row_generation(problem, lp, lp.solve_simplex(PRIMAL));
    }

  private:
    lp_row_generation m_row_generation;
};

/**
 * @class row_generation_resolve_lp
 *
 * @brief Finds an extreme point solution to the LP using the row generation
* technique.
 */
class row_generation_resolve_lp {
  public:
    /**
     * Finds an extreme point solution to the LP using the row genereation
     * technique.
     */
    template <typename Problem, typename LP>
    problem_type operator()(Problem &problem, LP &lp) {
        return m_row_generation(problem, lp, lp.resolve_simplex(PRIMAL));
    }

  private:
    lp_row_generation m_row_generation;
};

} // lp
} // paal

#endif /* LP_ROW_GENERATION_HPP */
