#pragma once

#include "physics/degrees_of_freedom.hpp"

#include <string>
#include "geometry/barycentre_calculator.hpp"
#include "geometry/pair.hpp"

namespace principia {
namespace physics {
namespace _degrees_of_freedom {
namespace internal {

using namespace principia::geometry::_barycentre_calculator;

template<typename Frame>
DegreesOfFreedom<Frame>::DegreesOfFreedom(Position<Frame> const& position,
                                          Velocity<Frame> const& velocity)
    : Pair<Position<Frame>, Velocity<Frame>>(position, velocity) {}

template<typename Frame>
DegreesOfFreedom<Frame>::DegreesOfFreedom(
    Pair<Position<Frame>, Velocity<Frame>> const& base)
    : Pair<Position<Frame>, Velocity<Frame>>(base) {}

template<typename Frame>
DegreesOfFreedom<Frame> DegreesOfFreedom<Frame>::ReadFromMessage(
    serialization::Pair const& message)
  requires serializable<Frame> {
  return Pair<Position<Frame>, Velocity<Frame>>::ReadFromMessage(message);
}

template<typename Frame>
Position<Frame> const& DegreesOfFreedom<Frame>::position() const {
  return this->t1_;
}

template<typename Frame>
Velocity<Frame> const& DegreesOfFreedom<Frame>::velocity() const {
  return this->t2_;
}

template<typename Frame>
RelativeDegreesOfFreedom<Frame>::RelativeDegreesOfFreedom(
    Displacement<Frame> const& displacement,
    Velocity<Frame> const& velocity)
    : Pair<Displacement<Frame>, Velocity<Frame>>(displacement, velocity) {}

template<typename Frame>
RelativeDegreesOfFreedom<Frame>::RelativeDegreesOfFreedom(
    Pair<Displacement<Frame>, Velocity<Frame>> const& base)
    : Pair<Displacement<Frame>, Velocity<Frame>>(base) {}

template<typename Frame>
Displacement<Frame> const&
RelativeDegreesOfFreedom<Frame>::displacement() const {
  return this->t1_;
}

template<typename Frame>
Velocity<Frame> const& RelativeDegreesOfFreedom<Frame>::velocity() const {
  return this->t2_;
}

template<typename Frame>
std::string DebugString(DegreesOfFreedom<Frame> const& degrees_of_freedom) {
  return "{" + DebugString(degrees_of_freedom.position()) + ", " +
         DebugString(degrees_of_freedom.velocity()) + "}";
}

template<typename Frame>
std::string DebugString(
    RelativeDegreesOfFreedom<Frame> const& relative_degrees_of_freedom) {
  return "{" + DebugString(relative_degrees_of_freedom.displacement()) + ", " +
         DebugString(relative_degrees_of_freedom.velocity()) + "}";
}

template<typename Frame>
std::ostream& operator<<(std::ostream& out,
                         DegreesOfFreedom<Frame> const& degrees_of_freedom) {
  out << DebugString(degrees_of_freedom);
  return out;
}

template<typename Frame>
std::ostream& operator<<(
    std::ostream& out,
    RelativeDegreesOfFreedom<Frame> const& relative_degrees_of_freedom) {
  out << DebugString(relative_degrees_of_freedom);
  return out;
}

}  // namespace internal
}  // namespace _degrees_of_freedom
}  // namespace physics

namespace base {
namespace _mappable {
namespace internal {

using namespace principia::geometry::_pair;
using namespace principia::physics::_degrees_of_freedom;

template<typename Functor, typename Frame>
typename Mappable<Functor, RelativeDegreesOfFreedom<Frame>>::type
Mappable<Functor, RelativeDegreesOfFreedom<Frame>>::Do(
    Functor const& functor,
    RelativeDegreesOfFreedom<Frame> const& relative) {
  return type(functor(relative.displacement()), functor(relative.velocity()));
}

}  // namespace internal
}  // namespace _mappable
}  // namespace base
}  // namespace principia
