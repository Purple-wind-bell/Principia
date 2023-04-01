#pragma once

#include "physics/similar_motion.hpp"

namespace principia {
namespace physics {
namespace _similar_motion {
namespace internal {

template<typename FromFrame, typename ToFrame>
template<typename ThroughFrame>
SimilarMotion<FromFrame, ToFrame>::SimilarMotion(
    RigidMotion<FromFrame, ThroughFrame> const& rigid_motion,
    Homothecy<double, ThroughFrame, ToFrame> const& dilatation,
    Variation<double> const& dilatation_rate)
    : rigid_motion_(RigidMotion<ThroughFrame, Through>::Identity() *
                    rigid_motion),
      dilatation_(dilatation *
                  Homothecy<double, Through, ThroughFrame>::Identity()),
      dilatation_rate_(dilatation_rate) {}

template<typename FromFrame, typename ToFrame>
DegreesOfFreedom<ToFrame> SimilarMotion<FromFrame, ToFrame>::operator()(
    DegreesOfFreedom<FromFrame> const& degrees_of_freedom) const {
  auto const degrees_of_freedom_in_through = rigid_motion_(degrees_of_freedom);
  auto const& qᴿ = degrees_of_freedom_in_through.position();
  auto const& q̇ᴿ = degrees_of_freedom_in_through.velocity();
  return {dilatation_(qᴿ - Through::origin),
          dilatation_rate_ * (qᴿ - Through::origin) + dilatation_(q̇ᴿ)};
}

}  // namespace internal
}  // namespace _similar_motion
}  // namespace physics
}  // namespace principia
