﻿
#include "ksp_plugin/plugin.hpp"

#include <cmath>
#include <map>
#include <memory>

#include "geometry/permutation.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "physics/mock_n_body_system.hpp"
#include "quantities/si.hpp"
#include "testing_utilities/almost_equals.hpp"
#include "testing_utilities/death_message.hpp"
#include "testing_utilities/numerics.hpp"
#include "testing_utilities/solar_system.hpp"

using principia::geometry::Bivector;
using principia::geometry::Permutation;
using principia::physics::MockNBodySystem;
using principia::quantities::Abs;
using principia::quantities::Sqrt;
using principia::si::Day;
using principia::si::Radian;
using principia::si::AstronomicalUnit;
using principia::testing_utilities::AbsoluteError;
using principia::testing_utilities::RelativeError;
using principia::testing_utilities::AlmostEquals;
using principia::testing_utilities::DeathMessage;
using principia::testing_utilities::ICRFJ2000Ecliptic;
using principia::testing_utilities::SolarSystem;
using testing::Eq;
using testing::Ge;
using testing::Gt;
using testing::Lt;
using testing::StrictMock;
using testing::_;

namespace principia {
namespace ksp_plugin {

class TestablePlugin : public Plugin {
 public:
  // Takes ownership of |n_body_system|.
  // TODO(phl): We'd like to pass a |unique_ptr<>| but that seems to confuse
  // gMock.
  TestablePlugin(Instant const& initial_time,
                 Index const sun_index,
                 GravitationalParameter const& sun_gravitational_parameter,
                 Angle const& planetarium_rotation,
                 MockNBodySystem<Barycentre>* n_body_system)
      : Plugin(initial_time,
               sun_index,
               sun_gravitational_parameter,
               planetarium_rotation) {
    n_body_system_.reset(n_body_system);
  }
};

class PluginTest : public testing::Test {
 protected:
  PluginTest()
      : looking_glass_(Permutation<ICRFJ2000Ecliptic, AliceSun>::XZY),
        solar_system_(SolarSystem::AtСпутник1Launch(
            SolarSystem::Accuracy::kMajorBodiesOnly)),
        bodies_(solar_system_->massive_bodies()),
        initial_time_(solar_system_->trajectories().front()->last_time()),
        sun_gravitational_parameter_(
            bodies_[SolarSystem::kSun]->gravitational_parameter()),
        planetarium_rotation_(1 * Radian) {
    n_body_system_ = new MockNBodySystem<Barycentre>();
    plugin_ = std::make_unique<StrictMock<TestablePlugin>>(
                  initial_time_,
                  SolarSystem::kSun,
                  sun_gravitational_parameter_,
                  planetarium_rotation_,
                  n_body_system_);
  }

  void InsertAllSolarSystemBodies() {
    for (std::size_t index = SolarSystem::kSun + 1;
         index < bodies_.size();
         ++index) {
      Index const parent_index = SolarSystem::parent(index);
      Displacement<AliceSun> const from_parent_position = looking_glass_(
          solar_system_->trajectories()[index]->last_position() -
          solar_system_->trajectories()[parent_index]->last_position());
      Velocity<AliceSun> const from_parent_velocity = looking_glass_(
          solar_system_->trajectories()[index]->last_velocity() -
          solar_system_->trajectories()[parent_index]->last_velocity());
      plugin_->InsertCelestial(index,
                               bodies_[index]->gravitational_parameter(),
                               parent_index,
                               from_parent_position,
                               from_parent_velocity);
    }
  }

  Permutation<ICRFJ2000Ecliptic, AliceSun> looking_glass_;
  MockNBodySystem<Barycentre>* n_body_system_;  // Not owned.
  std::unique_ptr<SolarSystem> solar_system_;
  SolarSystem::Bodies bodies_;
  Instant initial_time_;
  GravitationalParameter sun_gravitational_parameter_;
  Angle planetarium_rotation_;

  std::unique_ptr<StrictMock<TestablePlugin>> plugin_;
};

TEST_F(PluginTest, Initialization) {
  InsertAllSolarSystemBodies();
  plugin_->EndInitialization();
  for (std::size_t index = SolarSystem::kSun + 1;
       index < bodies_.size();
       ++index) {
    Index const parent_index = SolarSystem::parent(index);
    EXPECT_THAT(solar_system_->trajectories()[index]->last_position() -
                solar_system_->trajectories()[parent_index]->last_position(),
                AlmostEquals(looking_glass_.Inverse()(
                    plugin_->CelestialDisplacementFromParent(index)),
                    250000));
    EXPECT_THAT(solar_system_->trajectories()[index]->last_velocity() -
                solar_system_->trajectories()[parent_index]->last_velocity(),
                AlmostEquals(looking_glass_.Inverse()(
                    plugin_->CelestialParentRelativeVelocity(index)),
                    1000));
  }
}

TEST_F(PluginTest, InsertCelestialError) {
  Displacement<AliceSun> const from_parent_position = looking_glass_(
      solar_system_->trajectories().front()->last_position() -
      solar_system_->trajectories().front()->last_position());
  Velocity<AliceSun> const from_parent_velocity = looking_glass_(
      solar_system_->trajectories().front()->last_velocity() -
      solar_system_->trajectories().front()->last_velocity());
  EXPECT_DEATH({
    InsertAllSolarSystemBodies();
    plugin_->EndInitialization();
    plugin_->InsertCelestial(42,
                             bodies_.front()->gravitational_parameter(),
                             SolarSystem::kSun,
                             from_parent_position,
                             from_parent_velocity);
  }, DeathMessage("before the end of initialization"));
  EXPECT_DEATH({
    InsertAllSolarSystemBodies();
    plugin_->InsertCelestial(42,
                             bodies_.front()->gravitational_parameter(),
                             1729,
                             from_parent_position,
                             from_parent_velocity);
  }, DeathMessage("No body at index"));
  EXPECT_DEATH({
    InsertAllSolarSystemBodies();
    plugin_->InsertCelestial(SolarSystem::kEarth,
                             bodies_.front()->gravitational_parameter(),
                             SolarSystem::kSun,
                             from_parent_position,
                             from_parent_velocity);
  }, DeathMessage("Body already exists"));
}

TEST_F(PluginTest, VesselInsertionAtInitialization) {
  GUID const guid = "Test Satellite";
  InsertAllSolarSystemBodies();
  plugin_->EndInitialization();
  bool const inserted = plugin_->InsertOrKeepVessel(guid,
                                                    SolarSystem::kEarth);
  EXPECT_TRUE(inserted);
  Displacement<AliceSun> const displacement({3111.0 * Kilo(Metre),
                                             4400.0 * Kilo(Metre),
                                             3810.0 * Kilo(Metre)});
  auto const tangent = displacement * Bivector<double, AliceSun>({1, 2, 3});
  Vector<double, AliceSun> unit_tangent = tangent / tangent.Norm();
  EXPECT_THAT(InnerProduct(unit_tangent, displacement / displacement.Norm()),
              Eq(0));
  // This yields a circular orbit.
  Velocity<AliceSun> const velocity =
      Sqrt(bodies_[SolarSystem::kEarth]->gravitational_parameter() /
               displacement.Norm()) * unit_tangent;
  plugin_->SetVesselStateOffset(guid, displacement, velocity);
  EXPECT_THAT(
      AbsoluteError(plugin_->VesselDisplacementFromParent(guid), displacement),
      Lt(DBL_EPSILON * AstronomicalUnit));
  EXPECT_THAT(plugin_->VesselParentRelativeVelocity(guid),
              AlmostEquals(velocity));
}

// Checks that the plugin correctly uses its 10-second-step history even when
// advanced with smaller timesteps.  At the moment this is done by comparing
// the results with systems integrated with a small step and a 10-second step.
// This is too much of an integration test, and it will break all the time (it
// already broke when merging CL #188).
// TODO(egg): this test should be rewritten using gmock to mock the integrator.
// This will make it much faster and far less likely to break.
// TODO(egg): release-only for now, this is ridiculously slow on debug.
// NOTE(phl: All the king's men and all the king's horses couldn't put this test
// together again.
#if 0
TEST_F(PluginTest, AdvanceTime) {
  InsertAllSolarSystemBodies();
  plugin_->EndInitialization();
  NBodySystem<ICRFJ2000Ecliptic> system;
  SPRKIntegrator<Length, Speed> integrator;
  integrator.Initialize(integrator.Order5Optimal());
  Instant const tmax = initial_time_ + 1000.0 * Second;
  Time const δt = 0.02 * Second;
  for (Instant t = initial_time_ + δt; t <= tmax; t = t + δt) {
    Angle const angle =
        t + δt <= tmax ? ((t - initial_time_) / (2 * Second)) * Radian
                       : 1 * Radian;
    plugin_->AdvanceTime(t, angle);
#pragma warning(push)
#pragma warning(disable : 4566)  // Stringification of Unicode identifiers.
    EXPECT_THAT(plugin_->HistoryTime() + plugin_->kΔt, Ge(t));
#pragma warning(pop)
    system.Integrate(integrator,
                     t,
                     plugin_->kΔt,  // Δt
                     0,  // sampling_period
                     true,  // tmax_is_exact
                     solar_system_->trajectories());
  }
  std::unique_ptr<SolarSystem> symplectic_system =
      SolarSystem::AtСпутник1Launch(SolarSystem::Accuracy::kMajorBodiesOnly);
  system.Integrate(integrator,
                   solar_system_->trajectories().front()->last_time(),
                   plugin_->kΔt,  // Δt
                   0,  // sampling_period
                   true,  // tmax_is_exact
                   symplectic_system->trajectories());

  for (std::size_t index = SolarSystem::kSun + 1;
       index < bodies_.size();
       ++index) {
    Displacement<AliceSun> position;
    Velocity<AliceSun> velocity;
    for (int i = index; i != SolarSystem::kSun; i = SolarSystem::parent(i)) {
      position += plugin_->CelestialDisplacementFromParent(i);
      velocity += plugin_->CelestialParentRelativeVelocity(i);
    }
    // Check that the results of the integration by |AdvanceTime| match a
    // trajectory integrated with a timestep of |kΔt| better than one stepped at
    // every call to |AdvanceTime|.
    Displacement<ICRFJ2000Ecliptic> const reference_position =
          solar_system_->trajectories()[index]->last_position() -
          solar_system_->trajectories()[SolarSystem::kSun]->last_position();
    Velocity<ICRFJ2000Ecliptic> const reference_velocity =
          solar_system_->trajectories()[index]->last_velocity() -
          solar_system_->trajectories()[SolarSystem::kSun]->last_velocity();
    Displacement<ICRFJ2000Ecliptic> const symplectic_position =
          symplectic_system->trajectories()[index]->last_position() -
          symplectic_system->trajectories()[SolarSystem::kSun]->last_position();
    Velocity<ICRFJ2000Ecliptic> const symplectic_velocity =
          symplectic_system->trajectories()[index]->last_velocity() -
          symplectic_system->trajectories()[SolarSystem::kSun]->last_velocity();
    double const low_position_error =
        RelativeError(looking_glass_(symplectic_position), position);
    double const low_velocity_error =
        RelativeError(looking_glass_(symplectic_velocity), velocity);
    double const high_position_error =
        RelativeError(looking_glass_(reference_position), position);
    double const high_velocity_error =
        RelativeError(looking_glass_(reference_velocity), velocity);
    EXPECT_THAT(low_position_error, Lt(1E-12));
    EXPECT_THAT(low_velocity_error, Lt(1E-12));
    if (index != SolarSystem::kEarth &&
        index != SolarSystem::kVenus &&
        index != SolarSystem::kMars) {
      EXPECT_THAT(high_position_error, Gt(1.2 * low_position_error))
          << SolarSystem::name(index);
      EXPECT_THAT(high_velocity_error, Gt(1.2 * low_velocity_error))
          << SolarSystem::name(index);
    }
  }
}
#endif

}  // namespace ksp_plugin
}  // namespace principia
