#pragma once

#include "physics/barycentric_rotating_reference_frame.hpp"

#include <algorithm>
#include <utility>
#include <vector>
#include <set>
#include <string>

#include "geometry/barycentre_calculator.hpp"
#include "geometry/r3x3_matrix.hpp"
#include "quantities/quantities.hpp"
#include "quantities/si.hpp"

namespace principia {
namespace physics {
namespace _barycentric_rotating_reference_frame {
namespace internal {

using namespace principia::geometry::_grassmann;
using namespace principia::geometry::_orthogonal_map;
using namespace principia::geometry::_r3x3_matrix;
using namespace principia::quantities::_elementary_functions;
using namespace principia::quantities::_named_quantities;
using namespace principia::quantities::_quantities;
using namespace principia::quantities::_si;

template<typename InertialFrame, typename ThisFrame>
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::
    BarycentricRotatingReferenceFrame(
        not_null<Ephemeris<InertialFrame> const*> ephemeris,
        not_null<MassiveBody const*> primary,
        not_null<MassiveBody const*> secondary)
    : BarycentricRotatingReferenceFrame(
          ephemeris,
          std::vector<not_null<MassiveBody const*>>{primary},
          std::vector<not_null<MassiveBody const*>>{secondary}) {}

template<typename InertialFrame, typename ThisFrame>
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::
    BarycentricRotatingReferenceFrame(
        not_null<Ephemeris<InertialFrame> const*> ephemeris,
        std::vector<not_null<MassiveBody const*>> primaries,
        std::vector<not_null<MassiveBody const*>> secondaries)
    : ephemeris_(std::move(ephemeris)),
      primaries_(std::move(primaries)),
      secondaries_(std::move(secondaries)),
      primary_gravitational_parameter_(
          std::accumulate(primaries_.begin(),
                          primaries_.end(),
                          GravitationalParameter{},
                          [](GravitationalParameter const sum,
                             not_null<MassiveBody const*> const body) {
                            return sum + body->gravitational_parameter();
                          })),
      secondary_gravitational_parameter_(
          std::accumulate(secondaries_.begin(),
                          secondaries_.end(),
                          GravitationalParameter{},
                          [](GravitationalParameter const sum,
                             not_null<MassiveBody const*> const body) {
                            return sum + body->gravitational_parameter();
                          })) {
  absl::btree_set<not_null<MassiveBody const*>> primary_set(primaries_.begin(),
                                                            primaries_.end());
  absl::btree_set<not_null<MassiveBody const*>> secondary_set(
      secondaries_.begin(), secondaries_.end());
  absl::btree_set<not_null<MassiveBody const*>> intersection;
  std::set_intersection(primary_set.begin(),
                        primary_set.end(),
                        secondary_set.begin(),
                        secondary_set.end(),
                        std::inserter(intersection, intersection.begin()));
  auto const names = [](auto const& bodies) {
    return absl::StrJoin(
        bodies,
        ",",
        [](std::string* const out, not_null<MassiveBody const*> const body) {
          out->append(body->name());
        });
  };
  CHECK_GE(primaries_.size(), 1) << names(primaries_);
  CHECK_EQ(primary_set.size(), primaries_.size()) << names(primaries_);
  CHECK_GE(secondaries_.size(), 1) << names(secondaries_);
  CHECK_EQ(secondary_set.size(), secondaries_.size()) << names(secondaries_);
  CHECK_EQ(intersection.size(), 0) << names(intersection);
}

template<typename InertialFrame, typename ThisFrame>
std::vector<not_null<MassiveBody const*>> const&
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::primaries() const {
  return primaries_;
}

template<typename InertialFrame, typename ThisFrame>
std::vector<not_null<MassiveBody const*>> const&
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::secondaries()
    const {
  return secondaries_;
}

template<typename InertialFrame, typename ThisFrame>
template<int degree>
Derivative<Position<InertialFrame>, Instant, degree>
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::
    primary_derivative(Instant const& t) const {
  BarycentreCalculator<Derivative<Position<InertialFrame>, Instant, degree>,
                       GravitationalParameter>
      result;
  for (not_null const primary : primaries_) {
    if constexpr (degree == 0) {
      result.Add(ephemeris_->trajectory(primary)->EvaluatePosition(t),
                 primary->gravitational_parameter());
    } else if constexpr (degree == 1) {
      result.Add(ephemeris_->trajectory(primary)->EvaluateVelocity(t),
                 primary->gravitational_parameter());
    } else if constexpr (degree == 2) {
      result.Add(
          ephemeris_->ComputeGravitationalAccelerationOnMassiveBody(primary, t),
          primary->gravitational_parameter());
    } else {
      static_assert(degree == 3);
      result.Add(ephemeris_->ComputeGravitationalJerkOnMassiveBody(primary, t),
                 primary->gravitational_parameter());
    }
  }
  return result.Get();
}

template<typename InertialFrame, typename ThisFrame>
template<int degree>
Derivative<Position<InertialFrame>, Instant, degree>
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::
    secondary_derivative(Instant const& t) const {
  BarycentreCalculator<Derivative<Position<InertialFrame>, Instant, degree>,
                       GravitationalParameter>
      result;
  for (not_null const secondary : secondaries_) {
    if constexpr (degree == 0) {
      result.Add(ephemeris_->trajectory(secondary)->EvaluatePosition(t),
                 secondary->gravitational_parameter());
    } else if constexpr (degree == 1) {
      result.Add(ephemeris_->trajectory(secondary)->EvaluateVelocity(t),
                 secondary->gravitational_parameter());
    } else if constexpr (degree == 2) {
      result.Add(ephemeris_->ComputeGravitationalAccelerationOnMassiveBody(
                     secondary, t),
                 secondary->gravitational_parameter());
    } else {
      static_assert(degree == 3);
      result.Add(
          ephemeris_->ComputeGravitationalJerkOnMassiveBody(secondary, t),
          secondary->gravitational_parameter());
    }
  }
  return result.Get();
}

template<typename InertialFrame, typename ThisFrame>
Instant BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::t_min()
    const {
  // We depend on all bodies via the gravitational acceleration.
  return ephemeris_->t_min();
}

template<typename InertialFrame, typename ThisFrame>
Instant BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::t_max()
    const {
  // We depend on all bodies via the gravitational acceleration.
  return ephemeris_->t_max();
}

template<typename InertialFrame, typename ThisFrame>
RigidMotion<InertialFrame, ThisFrame>
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::ToThisFrameAtTime(
    Instant const& t) const {
  auto const r₁ = primary_derivative<0>(t);
  auto const ṙ₁ = primary_derivative<1>(t);
  auto const r̈₁ = primary_derivative<2>(t);
  auto const r₂ = secondary_derivative<0>(t);
  auto const ṙ₂ = secondary_derivative<1>(t);
  auto const r̈₂ = secondary_derivative<2>(t);
  return ToThisFrame({r₁, ṙ₁, r̈₁}, {r₂, ṙ₂, r̈₂});
}

template<typename InertialFrame, typename ThisFrame>
void BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::
WriteToMessage(not_null<serialization::ReferenceFrame*> const message) const {
  auto* const extension = message->MutableExtension(
      serialization::BarycentricRotatingReferenceFrame::extension);
  for (not_null const primary : primaries_) {
    extension->add_primary(
        ephemeris_->serialization_index_for_body(primary));
  }
  for (not_null const secondary : secondaries_) {
    extension->add_secondary(
        ephemeris_->serialization_index_for_body(secondary));
  }
}

template<typename InertialFrame, typename ThisFrame>
not_null<std::unique_ptr<
    BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>>>
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::ReadFromMessage(
    not_null<Ephemeris<InertialFrame> const*> const ephemeris,
    serialization::BarycentricRotatingReferenceFrame const& message) {
  std::vector<not_null<MassiveBody const*>> primaries;
  primaries.reserve(message.primary().size());
  for (int const primary : message.primary()) {
    primaries.push_back(ephemeris->body_for_serialization_index(primary));
  }
  std::vector<not_null<MassiveBody const*>> secondaries;
  secondaries.reserve(message.secondary().size());
  for (int const secondary : message.secondary()) {
    secondaries.push_back(ephemeris->body_for_serialization_index(secondary));
  }
  return std::make_unique<BarycentricRotatingReferenceFrame>(
      ephemeris, std::move(primaries), std::move(secondaries));
}

template<typename InertialFrame, typename ThisFrame>
Vector<Acceleration, InertialFrame>
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::
GravitationalAcceleration(Instant const& t,
                          Position<InertialFrame> const& q) const {
  return ephemeris_->ComputeGravitationalAccelerationOnMasslessBody(q, t);
}

template<typename InertialFrame, typename ThisFrame>
SpecificEnergy BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::
GravitationalPotential(Instant const& t,
                       Position<InertialFrame> const& q) const {
  return ephemeris_->ComputeGravitationalPotential(q, t);
}

template<typename InertialFrame, typename ThisFrame>
AcceleratedRigidMotion<InertialFrame, ThisFrame>
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::MotionOfThisFrame(
    Instant const& t) const {
  auto const r₁ = primary_derivative<0>(t);
  auto const ṙ₁ = primary_derivative<1>(t);
  auto const r̈₁ = primary_derivative<2>(t);
  auto const r₁⁽³⁾ = primary_derivative<3>(t);
  auto const r₂ = secondary_derivative<0>(t);
  auto const ṙ₂ = secondary_derivative<1>(t);
  auto const r̈₂ = secondary_derivative<2>(t);
  auto const r₂⁽³⁾ = secondary_derivative<3>(t);

  auto const to_this_frame = ToThisFrame({r₁, ṙ₁, r̈₁}, {r₂, ṙ₂, r̈₂});

  Displacement<InertialFrame> const r = r₂ - r₁;
  Velocity<InertialFrame> const ṙ = ṙ₂ - ṙ₁;
  Vector<Acceleration, InertialFrame> const r̈ = r̈₂ - r̈₁;
  Vector<Jerk, InertialFrame> const r⁽³⁾ = r₂⁽³⁾ - r₁⁽³⁾;

  Trihedron<Length, ArealSpeed> orthogonal;
  Trihedron<double, double> orthonormal;
  Trihedron<Length, ArealSpeed, 1> 𝛛orthogonal;
  Trihedron<double, double, 1> 𝛛orthonormal;
  Trihedron<Length, ArealSpeed, 2> 𝛛²orthogonal;
  Trihedron<double, double, 2> 𝛛²orthonormal;

  Base::ComputeTrihedra(r, ṙ, orthogonal, orthonormal);
  Base::ComputeTrihedraDerivatives(r, ṙ, r̈,
                                   orthogonal, orthonormal,
                                   𝛛orthogonal, 𝛛orthonormal);
  Base::ComputeTrihedraDerivatives2(r, ṙ, r̈, r⁽³⁾,
                                    orthogonal, orthonormal,
                                    𝛛orthogonal, 𝛛orthonormal,
                                    𝛛²orthogonal, 𝛛²orthonormal);

  auto const angular_acceleration_of_to_frame =
      Base::ComputeAngularAcceleration(
          orthonormal, 𝛛orthonormal, 𝛛²orthonormal);

  Vector<Acceleration, InertialFrame> const acceleration_of_to_frame_origin =
      Barycentre(std::pair{r̈₁, r̈₂},
                 std::pair{primary_gravitational_parameter_,
                           secondary_gravitational_parameter_});
  return AcceleratedRigidMotion<InertialFrame, ThisFrame>(
             to_this_frame,
             angular_acceleration_of_to_frame,
             acceleration_of_to_frame_origin);
}

template<typename InertialFrame, typename ThisFrame>
RigidMotion<InertialFrame, ThisFrame>
BarycentricRotatingReferenceFrame<InertialFrame, ThisFrame>::ToThisFrame(
    Derivatives<Position<InertialFrame>, Instant, 3> const& primary_derivative,
    Derivatives<Position<InertialFrame>, Instant, 3> const&
        secondary_derivative) const {
  auto [r₁, ṙ₁, r̈₁] = primary_derivative;
  auto [r₂, ṙ₂, r̈₂] = secondary_derivative;
  DegreesOfFreedom<InertialFrame> const primary_degrees_of_freedom = {r₁, ṙ₁};
  DegreesOfFreedom<InertialFrame> const secondary_degrees_of_freedom = {r₂, ṙ₂};
  Rotation<InertialFrame, ThisFrame> rotation =
          Rotation<InertialFrame, ThisFrame>::Identity();
  AngularVelocity<InertialFrame> angular_velocity;
  RigidReferenceFrame<InertialFrame, ThisFrame>::ComputeAngularDegreesOfFreedom(
      primary_degrees_of_freedom,
      secondary_degrees_of_freedom,
      r̈₁,
      r̈₂,
      rotation,
      angular_velocity);

  DegreesOfFreedom<InertialFrame> const barycentre_degrees_of_freedom =
      Barycentre(std::pair{DegreesOfFreedom<InertialFrame>{r₁, ṙ₁},
                           DegreesOfFreedom<InertialFrame>{r₂, ṙ₂}},
                 std::pair{primary_gravitational_parameter_,
                           secondary_gravitational_parameter_});
  RigidTransformation<InertialFrame, ThisFrame> const rigid_transformation(
      barycentre_degrees_of_freedom.position(),
      ThisFrame::origin,
      rotation.template Forget<OrthogonalMap>());
  return RigidMotion<InertialFrame, ThisFrame>(
      rigid_transformation,
      angular_velocity,
      barycentre_degrees_of_freedom.velocity());
}

}  // namespace internal
}  // namespace _barycentric_rotating_reference_frame
}  // namespace physics
}  // namespace principia
