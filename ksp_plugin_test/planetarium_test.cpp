﻿
#include "ksp_plugin/planetarium.hpp"

#include <vector>

#include "geometry/affine_map.hpp"
#include "geometry/grassmann.hpp"
#include "geometry/linear_map.hpp"
#include "geometry/named_quantities.hpp"
#include "geometry/rotation.hpp"
#include "gtest/gtest.h"
#include "physics/massive_body.hpp"
#include "physics/mock_continuous_trajectory.hpp"
#include "physics/mock_dynamic_frame.hpp"
#include "physics/mock_ephemeris.hpp"
#include "physics/rigid_motion.hpp"
#include "physics/rotating_body.hpp"
#include "quantities/numbers.hpp"
#include "quantities/elementary_functions.hpp"
#include "quantities/si.hpp"
#include "testing_utilities/vanishes_before.hpp"

namespace principia {
namespace ksp_plugin {
namespace internal_planetarium {

using geometry::AffineMap;
using geometry::AngularVelocity;
using geometry::Bivector;
using geometry::Displacement;
using geometry::LinearMap;
using geometry::Rotation;
using geometry::Vector;
using geometry::Velocity;
using physics::MassiveBody;
using physics::MockContinuousTrajectory;
using physics::MockDynamicFrame;
using physics::MockEphemeris;
using physics::RigidMotion;
using physics::RigidTransformation;
using physics::RotatingBody;
using quantities::Cos;
using quantities::Sin;
using quantities::Sqrt;
using quantities::Time;
using quantities::si::Kilogram;
using quantities::si::Metre;
using quantities::si::Radian;
using quantities::si::Second;
using testing_utilities::VanishesBefore;
using ::testing::_;
using ::testing::AllOf;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Return;
using ::testing::ReturnRef;

class PlanetariumTest : public ::testing::Test {
 protected:
  Instant const t0_;
};

TEST_F(PlanetariumTest, PlotMethod0) {
  // A circular trajectory around the origin.
  DiscreteTrajectory<Barycentric> discrete_trajectory;
  for (Time t; t <= 10 * Second; t += 1 * Second) {
    DegreesOfFreedom<Barycentric> const degrees_of_freedom(
        Barycentric::origin +
            Displacement<Barycentric>(
                {10 * Metre * Sin(2 * π * t * Radian / (10 * Second)),
                 10 * Metre * Cos(2 * π * t * Radian / (10 * Second)),
                 0 * Metre}),
        Velocity<Barycentric>());
    discrete_trajectory.Append(t0_ + t, degrees_of_freedom);
  }

  // The camera is located as {0, 20, 0} and is looking along -y.
  Perspective<Navigation, Camera, Length, OrthogonalMap> const perspective(
      AffineMap<Navigation, Camera, Length, OrthogonalMap>(
          Navigation::origin +
              Displacement<Navigation>({0 * Metre, 20 * Metre, 0 * Metre}),
          Camera::origin,
          Rotation<Navigation, Camera>(
              Vector<double, Navigation>({1, 0, 0}),
              Vector<double, Navigation>({0, 0, 1}),
              Bivector<double, Navigation>({0, -1, 0})).Forget()),
      /*focal=*/5 * Metre);

  MockDynamicFrame<Barycentric, Navigation> plotting_frame;
  EXPECT_CALL(plotting_frame, ToThisFrameAtTime(_))
      .WillRepeatedly(Return(RigidMotion<Barycentric, Navigation>(
          RigidTransformation<Barycentric, Navigation>::Identity(),
          AngularVelocity<Barycentric>(),
          Velocity<Barycentric>())));

  // A body of radius 1 m located at the origin.
  RotatingBody<Barycentric> const body(
      MassiveBody::Parameters(1 * Kilogram),
      RotatingBody<Barycentric>::Parameters(
          /*mean_radius=*/1 * Metre,
          /*reference_angle=*/0 * Radian,
          /*reference_instant=*/t0_,
          /*angular_frequency=*/10 * Radian / Second,
          /*ascension_of_pole=*/0 * Radian,
          /*declination_of_pole=*/π / 2 * Radian));
  std::vector<not_null<MassiveBody const*>> const bodies = {&body};
  MockContinuousTrajectory<Barycentric> continuous_trajectory;
  MockEphemeris<Barycentric> ephemeris;
  EXPECT_CALL(ephemeris, bodies()).WillRepeatedly(ReturnRef(bodies));
  EXPECT_CALL(ephemeris, trajectory(_))
      .WillRepeatedly(Return(&continuous_trajectory));
  EXPECT_CALL(continuous_trajectory, EvaluatePosition(_))
      .WillRepeatedly(Return(Barycentric::origin));

  Planetarium planetarium(perspective, &ephemeris, &plotting_frame);
  auto const rp2_points =
      planetarium.PlotMethod0(discrete_trajectory.Begin(),
                              discrete_trajectory.End(),
                              t0_ + 10 * Second);

  for (auto const& rp2_point : rp2_points) {
    // The following limit is obtained by elementary geometry by noticing that
    // the circle is viewed from the camera under an angle of π / 6.
    EXPECT_THAT(rp2_point.x(), AllOf(Ge(-5.0 / Sqrt(3.0) * Metre),
                                     Le(5.0 / Sqrt(3.0) * Metre)));
    EXPECT_THAT(rp2_point.y(), VanishesBefore(1 * Metre, 6, 13));
  }
}

}  // namespace internal_planetarium
}  // namespace ksp_plugin
}  // namespace principia
