/**
 * @file suffix_array.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-08-29
 */
#ifndef SUFFIX_ARRAY_HPP
#define SUFFIX_ARRAY_HPP
#include "boost/range/irange.hpp"

#include "boost/range/irange.hpp"

/*
 * algorithm from:
 *
 * http://www.cs.cmu.edu/~guyb/realworld/papersS04/KaSa03.pdf
 *
 */

namespace paal {
namespace suffix_arrays {

/**
 * @param Letter a1
 * @param int a2
 * @param Letter b1
 * @param int b2
 * @brief return true if pair (a1,a2) is smaller than pair (b1,b2) in
 * lexicographic order
 * and false otherwise
 * @tparam Letter
 */
// class suffix_array{
template <typename Letter>
inline bool leq(Letter a1, int a2, Letter b1,
                int b2) { // lexic. order for pairs
    return (a1 < b1 || (a1 == b1 && a2 <= b2));
}
/**
 * @param int a1
 * @param Letter a2
 * @param int a3
 * @param int b1
 * @param Letter b2
 * @param int b3
 * @brief return true if triple (a1,a2,a3) is smaller than triple (b1,b2,b3) in
 * lexicographic order
 * and false otherwise
 * @tparam Letter
 */
template <typename Letter>
inline bool leq(Letter a1, Letter a2, int a3, Letter b1, Letter b2, int b3) {
    return (a1 < b1 || (a1 == b1 && leq(a2, a3, b2, b3)));
}
// stably sort sortFrom[0..n-1] to sortTo[0..n-1] with keys in 0..K from r
template <typename Iterator>
static void radix_pass(std::vector<int> const &sortFrom,
                       std::vector<int> &sortTo, Iterator r, int n,
                       int maxLetter) { // count occurrences
    std::vector<int> c(maxLetter + 1);
    for (auto i : boost::irange(0, n)) {
        c[*(r + sortFrom[i])]++; // count occurrences
    }
    int sum = 0;
    for (auto i : boost::irange(0, maxLetter + 1)) { // exclusive prefix sums
        int t = c[i];
        c[i] = sum;
        sum += t;
    }
    for (auto i : boost::irange(0, n)) {
        sortTo[c[*(r + sortFrom[i])]++] = sortFrom[i]; // sort
    }
}

/**
 *
 * @brief
 * require text[n]=text[n+1]=text[n+2]=0, n>=2
 * fill suffix_array
 * suffix_array[i] contains the starting position of the i-1'th smallest suffix
* in Word
 * @tparam Letter
 * @param vector<Letter> text - text
 * @param vector<int> place for suffix_array
 * @param Letter max_letter optional parameter max_letter in alphabet
 */
// find the suffix array SA of text[0..n-1] in {1..maxLetter}^n
template <typename Letter>
void _suffix_array(std::vector<Letter> &text, std::vector<int> &SA,
                   Letter maxLetter = 0) {
    int n = text.size() - 3;
    int n0 = (n + 2) / 3, n1 = (n + 1) / 3, n2 = n / 3, n02 = n0 + n2;
    text.resize(text.size() + 3);
    std::vector<int> text12;
    std::vector<int> SA12;
    std::vector<int> text0;
    std::vector<int> SA0;
    if (maxLetter == 0)
        for (auto i : text) {
            if (maxLetter < i) {
                maxLetter = i;
            }
        }
    // generate positions of mod 1 and mod  2 suffixes
    // the "+(n0-n1)" adds a dummy mod 1 suffix if n%3 == 1
    for (auto i : boost::irange(0, n + (n0 - n1))) {
        if (i % 3 != 0) {
            text12.push_back(i);
        }
    }
    SA0.resize(n0);
    SA12.resize(n02 + 3);
    text12.resize(n02 + 3);
    // lsb radix sort the mod 1 and mod 2 triples
    radix_pass(text12, SA12, text.begin() + 2, n02, maxLetter);
    radix_pass(SA12, text12, text.begin() + 1, n02, maxLetter);
    radix_pass(text12, SA12, text.begin(), n02, maxLetter);

    // find lexicographic names of triples
    int name = 0;
    Letter c0 = Letter(), c1 = Letter(), c2 = Letter();
    for (auto i : boost::irange(0, n02)) {
        if (text[SA12[i]] != c0 || text[SA12[i] + 1] != c1 ||
            text[SA12[i] + 2] != c2 || name == 0) {
            name++;
            c0 = text[SA12[i]];
            c1 = text[SA12[i] + 1];
            c2 = text[SA12[i] + 2];
        }
        if (SA12[i] % 3 == 1) {
            text12[SA12[i] / 3] = name; // left half
        } else {
            text12[SA12[i] / 3 + n0] = name; // right half
        }
    }

    // recurse if names are not yet unique
    if (name < n02) {
        _suffix_array<int>(text12, SA12 /*, n02*/,
                           name); // parametrized by int intentionally
        // store unique names in s12 using the suffix array
        for (auto i : boost::irange(0, n02)) {
            text12[SA12[i]] = i + 1;
        }
    } else { // generate the suffix array of s12 directly
        for (auto i : boost::irange(0, n02)) {
            SA12[text12[i] - 1] = i;
        }
    }

    // stably sort the mod 0 suffixes from SA12 by their first character
    for (auto i : boost::irange(0, n02)) {
        if (SA12[i] < n0) {
            text0.push_back(3 * SA12[i]);
        }
    }
    radix_pass(text0, SA0, text.begin(), n0, maxLetter);
    auto GetI = [&](int t)->int {
        return SA12[t] < n0 ? SA12[t] * 3 + 1 : (SA12[t] - n0) * 3 + 2;
    };

    // merge sorted SA0 suffixes and sorted SA12 suffixes
    auto p = SA0.begin();
    int t = n0 - n1;
    for (auto k = SA.begin(); k < SA.begin() + n; k++) {
        int i = GetI(t); // pos of current offset 12 suffix
        int j = (*p);    // pos of current offset 0  suffix
        if (SA12[t] < n0
                ? leq(text[i], text12[SA12[t] + n0], text[j], text12[j / 3])
                : leq(text[i], text[i + 1], text12[SA12[t] - n0 + 1], text[j],
                      text[j + 1], text12[j / 3 + n0])) { // suffix from SA12 is
                                                          // smaller
            (*k) = i;
            t++;
            if (t == n02) { // done --- only SA0 suffixes left
                k++;
                if (p < SA0.end()) {
                    k = std::copy(p, SA0.end(), k);
                    p = SA0.end();
                }
            }
        } else {
            (*k) = j;
            p++;
            if (p == SA0.end()) { // done --- only SA12 suffixes left
                for (k++; t < n02; t++, k++) {
                    (*k) = GetI(t);
                }
            }
        }
    }
}
;

/**
 *
 * @brief
 * require text.size()>=2
 * fill suffix_array
 * suffix_array[i] contains the starting position of the i-1'th smallest suffix
* in Word
 * @tparam Letter
 * @param vector<Letter> text - text
 * @param vector<int> place for suffix_array
 * @param Letter max_letter optional parameter max_letter in alphabet
 */
// find the suffix array SA of text[0..n-1] in {1..maxLetter}^n
template <typename Letter>
void suffix_array(std::vector<Letter> &text, std::vector<int> &SA,
                  Letter maxLetter = 0) {
    text.resize(text.size() + 3);
    _suffix_array<Letter>(text, SA, maxLetter);
    text.resize(text.size() - 3);
}
;

}      //!suffix_arrays
}      //!paal
#endif /*SUFFIX_ARRAY_HPP*/
