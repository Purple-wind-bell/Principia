#pragma once

#include "numerics/concepts.hpp"

namespace principia {
namespace numerics {
namespace _lattices {
namespace internal {

using namespace principia::numerics::_concepts;

//TODO(phl):Should this be integral?
template<typename Matrix>
  requires two_dimensional<Matrix>
Matrix LenstraLenstraLov�sz(Matrix const& L);

}  // namespace internal

using internal::LenstraLenstraLov�sz;

}  // namespace _lattices
}  // namespace numerics
}  // namespace principia

#include "numerics/lattices_body.hpp"
