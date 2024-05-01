#pragma once

#include <functional>
#include <vector>

#include "absl/status/statusor.h"
#include "boost/multiprecision/cpp_bin_float.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#include "numerics/polynomial_in_monomial_basis.hpp"

namespace principia {
namespace functions {
namespace _accurate_table_generator {
namespace internal {

using namespace boost::multiprecision;
using namespace principia::numerics::_polynomial_in_monomial_basis;

using AccurateFunction = std::function<cpp_bin_float_50(cpp_rational const&)>;
template<int degree>
using AccuratePolynomial =
    PolynomialInMonomialBasis<cpp_rational, cpp_rational, degree>;

template<std::int64_t zeroes>
cpp_rational ExhaustiveSearch(std::vector<AccurateFunction> const& functions,
                              cpp_rational const& starting_argument);

template<std::int64_t zeroes>
std::vector<cpp_rational> ExhaustiveMultisearch(
    std::vector<AccurateFunction> const& functions,
    std::vector<cpp_rational> const& starting_arguments);

// |functions| have arguments at distance |T/N| of |near_argument|.
template<std::int64_t zeroes>
absl::StatusOr<cpp_rational> SimultaneousBadCaseSearch(
  std::array<AccurateFunction, 2> const& functions,
  std::array<AccuratePolynomial<2>, 2> const& polynomials,
  cpp_rational const& near_argument,
  std::int64_t M,
  std::int64_t N,
  std::int64_t T);

}  // namespace internal

using internal::AccuratePolynomial;
using internal::ExhaustiveMultisearch;
using internal::ExhaustiveSearch;
using internal::SimultaneousBadCaseSearch;

}  // namespace _accurate_table_generator
}  // namespace functions
}  // namespace principia

#include "functions/accurate_table_generator_body.hpp"
