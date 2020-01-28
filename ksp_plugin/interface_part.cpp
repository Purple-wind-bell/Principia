﻿
#include "ksp_plugin/interface.hpp"

#include <algorithm>
#include <chrono>

#include "geometry/grassmann.hpp"
#include "journal/method.hpp"
#include "journal/profiles.hpp"
#include "ksp_plugin/identification.hpp"
#include "ksp_plugin/iterators.hpp"
#include "quantities/named_quantities.hpp"
#include "quantities/quantities.hpp"
#include "quantities/si.hpp"

namespace principia {
namespace interface {

using geometry::Bivector;
using quantities::Force;
using quantities::Torque;
using ksp_plugin::PartId;
using quantities::si::Kilo;
using quantities::si::Newton;
using quantities::si::Metre;
using quantities::si::Radian;

void __cdecl principia__PartAddIntrinsicForceAndTorque(
    Plugin* const plugin,
    PartId const part_id,
    XYZ const force_in_kilonewtons,
    XYZ const torque_in_kilonewton_metre) {
  journal::Method<journal::PartAddIntrinsicForceAndTorque> m(
      {plugin, part_id, force_in_kilonewtons, torque_in_kilonewton_metre});
  CHECK_NOTNULL(plugin)->AddPartIntrinsicForceAndTorque(
      part_id,
      Vector<Force, World>(FromXYZ(force_in_kilonewtons) * Kilo(Newton)),
      Bivector<Torque, World>(FromXYZ(torque_in_kilonewton_metre) *
                              Kilo(Newton) * Metre * Radian));
  return m.Return();
}

void __cdecl principia__PartClearIntrinsicForcesAndTorques(
    Plugin* const plugin,
    PartId const part_id) {
  journal::Method<journal::PartClearIntrinsicForcesAndTorques> m({plugin,
                                                                  part_id});
  CHECK_NOTNULL(plugin)->ClearPartIntrinsicForcesAndTorques(part_id);
  return m.Return();
}

QP __cdecl principia__PartGetActualDegreesOfFreedom(Plugin const* const plugin,
                                                    PartId const part_id,
                                                    Origin const origin) {
  journal::Method<journal::PartGetActualDegreesOfFreedom> m(
      {plugin, part_id, origin});
  CHECK_NOTNULL(plugin);
  return m.Return(ToQP(
      plugin->GetPartActualDegreesOfFreedom(
          part_id,
          plugin->BarycentricToWorld(
              origin.reference_part_is_unmoving,
              origin.reference_part_id,
              origin.reference_part_is_at_origin
                  ? std::nullopt
                  : std::make_optional(
                        FromXYZ<Position<World>>(
                            origin.main_body_centre_in_world))))));
}

void __cdecl principia__PartSetApparentRigidMotion(
    Plugin* const plugin,
    PartId const part_id,
    QP const degrees_of_freedom,
    WXYZ const rotation,
    XYZ const angular_velocity,
    QP const main_body_degrees_of_freedom) {
  journal::Method<journal::PartSetApparentRigidMotion> m(
      {plugin,
       part_id,
       degrees_of_freedom,
       rotation,
       angular_velocity,
       main_body_degrees_of_freedom});
  CHECK_NOTNULL(plugin);
  plugin->SetPartApparentRigidMotion(
      part_id,
      MakePartRigidMotion(degrees_of_freedom, rotation, angular_velocity),
      FromQP<DegreesOfFreedom<World>>(main_body_degrees_of_freedom));
  return m.Return();
}

}  // namespace interface
}  // namespace principia
