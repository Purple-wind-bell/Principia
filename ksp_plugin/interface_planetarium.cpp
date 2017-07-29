
#include "ksp_plugin/interface.hpp"

#include "geometry/affine_map.hpp"
#include "geometry/grassmann.hpp"
#include "geometry/orthogonal_map.hpp"
#include "geometry/perspective.hpp"
#include "geometry/rotation.hpp"
#include "geometry/rp2_point.hpp"
#include "glog/logging.h"
#include "journal/method.hpp"
#include "journal/profiles.hpp"
#include "ksp_plugin/frames.hpp"
#include "ksp_plugin/iterators.hpp"
#include "ksp_plugin/renderer.hpp"
#include "physics/discrete_trajectory.hpp"
#include "physics/rigid_motion.hpp"
#include "quantities/quantities.hpp"
#include "quantities/si.hpp"

namespace principia {
namespace interface {

using geometry::AffineMap;
using geometry::Multivector;
using geometry::OrthogonalMap;
using geometry::Perspective;
using geometry::Rotation;
using geometry::RP2Lines;
using ksp_plugin::Camera;
using ksp_plugin::Navigation;
using ksp_plugin::Planetarium;
using ksp_plugin::Renderer;
using ksp_plugin::TypedIterator;
using physics::DiscreteTrajectory;
using physics::RigidTransformation;
using quantities::Length;
using quantities::si::Metre;

Planetarium* principia__PlanetariumCreate(
    Plugin const* const plugin,
    XYZ const sun_world_position,
    XYZ const xyz_opengl_camera_x_in_world,
    XYZ const xyz_opengl_camera_y_in_world,
    XYZ const xyz_opengl_camera_z_in_world,
    XYZ const xyz_camera_position_in_world,
    double const focal) {
  journal::Method<journal::PlanetariumCreate> m({plugin,
                                                 sun_world_position,
                                                 xyz_opengl_camera_x_in_world,
                                                 xyz_opengl_camera_y_in_world,
                                                 xyz_opengl_camera_z_in_world,
                                                 xyz_camera_position_in_world,
                                                 focal});
  Renderer const& renderer = CHECK_NOTNULL(plugin)->renderer();

  Multivector<double, World, 1> const opengl_camera_x_in_world(
      FromXYZ(xyz_opengl_camera_x_in_world));
  Multivector<double, World, 1> const opengl_camera_y_in_world(
      FromXYZ(xyz_opengl_camera_y_in_world));
  Multivector<double, World, 2> const opengl_camera_z_in_world(
      FromXYZ(xyz_opengl_camera_z_in_world));
  // Note the minus sign for z below because our convention with respect to the
  // orientation of z is opposite that of OpenGL.
  Rotation<Camera, World> const camera_to_world_rotation(
      opengl_camera_x_in_world,
      opengl_camera_y_in_world,
      -opengl_camera_z_in_world);
  Position<World> const camera_position_in_world =
      FromXYZ<Position<World>>(xyz_camera_position_in_world);

  RigidTransformation<Camera, World> const
      camera_to_world_affine_map(Camera::origin,
                                 camera_position_in_world,
                                 camera_to_world_rotation.Forget());
  RigidTransformation<World, Navigation> const
      world_to_plotting_affine_map =
          renderer.WorldToPlotting(plugin->CurrentTime(),
                                   FromXYZ<Position<World>>(sun_world_position),
                                   plugin->PlanetariumRotation());

  Planetarium::Parameters parameters(/*sphere_radius_multiplier=*/1.05);
  Perspective<Navigation, Camera, Length, OrthogonalMap> perspective(
      world_to_plotting_affine_map * camera_to_world_affine_map,
      focal * Metre);

  return m.Return(plugin->NewPlanetarium(parameters, perspective).release());
}

void principia__PlanetariumDelete(
    Planetarium const** const planetarium) {
  journal::Method<journal::PlanetariumDelete> m({planetarium}, {planetarium});
  CHECK_NOTNULL(planetarium);
  TakeOwnership(planetarium);
  return m.Return();
}

Iterator* principia__PlanetariumPlotPrediction(
    Planetarium const* const planetarium,
    Plugin const* const plugin,
    char const* const vessel_guid) {
  journal::Method<journal::PlanetariumPlotPrediction> m({planetarium,
                                                         plugin,
                                                         vessel_guid});
  CHECK_NOTNULL(plugin);
  CHECK_NOTNULL(planetarium);
  auto const& prediction = plugin->GetVessel(vessel_guid)->prediction();
  auto const rp2_lines = planetarium->PlotMethod0(prediction.Begin(),
                                                  prediction.End(),
                                                  plugin->CurrentTime());
  return m.Return(new TypedIterator<RP2Lines<Length, Camera>>(rp2_lines));
}

Iterator* principia__PlanetariumPlotPsychohistory(
    Planetarium const* const planetarium,
    Plugin const* const plugin,
    char const* const vessel_guid) {
  journal::Method<journal::PlanetariumPlotPsychohistory> m({planetarium,
                                                            plugin,
                                                            vessel_guid});
  CHECK_NOTNULL(plugin);
  CHECK_NOTNULL(planetarium);
  auto const& psychohistory = plugin->GetVessel(vessel_guid)->psychohistory();
  auto const rp2_lines = planetarium->PlotMethod0(psychohistory.Begin(),
                                                  psychohistory.End(),
                                                  plugin->CurrentTime());
  return m.Return(new TypedIterator<RP2Lines<Length, Camera>>(rp2_lines));
}

}  // namespace interface
}  // namespace principia
