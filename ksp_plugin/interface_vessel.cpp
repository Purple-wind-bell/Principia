#include "ksp_plugin/interface.hpp"

#include "journal/method.hpp"
#include "journal/profiles.hpp"

namespace principia {
namespace interface {

XYZ principia__VesselBinormal(Plugin const* const plugin,
                              char const* const vessel_guid) {
  journal::Method<journal::VesselBinormal> m({plugin, vessel_guid});
  return m.Return(
      ToXYZ(CHECK_NOTNULL(plugin)->VesselBinormal(vessel_guid).coordinates()));
}

AdaptiveStepParameters principia__VesselGetPredictionAdaptiveStepParameters(
    Plugin const* const plugin,
    char const* const vessel_guid) {
  journal::Method<journal::VesselGetPredictionAdaptiveStepParameters> m(
      {plugin, vessel_guid});
  return m.Return(ToAdaptiveStepParameters(
      GetVessel(plugin, vessel_guid)->prediction_adaptive_step_parameters()));
}

XYZ principia__VesselNormal(Plugin const* const plugin,
                            char const* const vessel_guid) {
  journal::Method<journal::VesselNormal> m({plugin, vessel_guid});
  return m.Return(
      ToXYZ(CHECK_NOTNULL(plugin)->VesselNormal(vessel_guid).coordinates()));
}

void principia__VesselSetPredictionAdaptiveStepParameters(
    Plugin const* const plugin,
    char const* const vessel_guid,
    AdaptiveStepParameters const adaptive_step_parameters) {
  journal::Method<journal::VesselSetPredictionAdaptiveStepParameters> m(
      {plugin, vessel_guid, adaptive_step_parameters});
  GetVessel(plugin, vessel_guid)
      ->set_prediction_adaptive_step_parameters(
          FromAdaptiveStepParameters(adaptive_step_parameters));
  return m.Return();
}

XYZ principia__VesselTangent(Plugin const* const plugin,
                             char const* const vessel_guid) {
  journal::Method<journal::VesselTangent> m({plugin, vessel_guid});
  return m.Return(
      ToXYZ(CHECK_NOTNULL(plugin)->VesselTangent(vessel_guid).coordinates()));
}

}  // namespace interface
}  // namespace principia