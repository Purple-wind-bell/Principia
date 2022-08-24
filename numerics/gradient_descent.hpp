
#pragma once

#include <functional>

#include "geometry/grassmann.hpp"
#include "geometry/named_quantities.hpp"
#include "quantities/named_quantities.hpp"
#include "quantities/quantities.hpp"

namespace principia {
namespace numerics {
namespace internal_gradient_descent {

using geometry::Position;
using geometry::Vector;
using quantities::Derivative;
using quantities::Length;

template<typename Value, typename Frame>
using Field = std::function<Value(Position<Frame> const&)>;

template<typename Scalar, typename Frame>
using Gradient = Vector<Derivative<Scalar, Length>, Frame>;

template<typename Scalar, typename Frame>
Position<Frame> BroydenFletcherGoldfarbShanno(
    Position<Frame> const& start_position,
    Field<Scalar, Frame> const& f,
    Field<Gradient<Scalar, Frame>, Frame> const& grad_f,
    Length const& tolerance);

}  // namespace internal_gradient_descent

using internal_gradient_descent::BroydenFletcherGoldfarbShanno;

}  // namespace numerics
}  // namespace principia

#include "numerics/gradient_descent_body.hpp"