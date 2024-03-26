#pragma once

#include <vector>
#include <utility>

#include "quantities/concepts.hpp"
#include "quantities/named_quantities.hpp"

namespace principia {
namespace geometry {
namespace _barycentre_calculator {
namespace internal {

using namespace principia::quantities::_concepts;
using namespace principia::quantities::_named_quantities;


template<affine Point, homogeneous_field Scalar>
  requires homogeneous_vector_space<Difference<Point>, Scalar>
class BarycentreCalculator;

// |Vector| must be a vector space over the field |Scalar|.
template<typename Vector, typename Scalar>
  requires homogeneous_vector_space<Vector, Scalar>
class BarycentreCalculator<Vector, Scalar> final {
 public:
  BarycentreCalculator() = default;

  void Add(Vector const& vector, Scalar const& weight);
  Vector Get() const;

  // The sum of the weights added so far.
  Scalar const& weight() const;

 private:
  bool empty_ = true;
  Product<Vector, Scalar> weighted_sum_;
  Scalar weight_;
};

// |T| is anything for which a specialization of BarycentreCalculator exists.
template<typename T, typename Scalar>
T Barycentre(std::pair<T, T> const& ts,
             std::pair<Scalar, Scalar> const& weights);
template<typename T, typename Scalar, template<typename...> class Container>
T Barycentre(Container<T> const& ts, Container<Scalar> const& weights);

}  // namespace internal

using internal::Barycentre;
using internal::BarycentreCalculator;

}  // namespace _barycentre_calculator
}  // namespace geometry
}  // namespace principia

#include "geometry/barycentre_calculator_body.hpp"
