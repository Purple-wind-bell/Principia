// Warning!  This file was generated by running a program (see project |tools|).
// If you change it, the changes will be lost the next time the generator is
// run.  You should change the generator instead.

extern "C"
struct NavigationFrameParameters {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  int extension;
  int centre_index;
  int primary_index;
  int secondary_index;
};

static_assert(std::is_pod<NavigationFrameParameters>::value,
              "NavigationFrameParameters is used for interfacing");

extern "C"
struct XYZ {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  double x;
  double y;
  double z;
};

static_assert(std::is_pod<XYZ>::value,
              "XYZ is used for interfacing");

extern "C"
struct AdaptiveStepParameters {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  std::int64_t integrator_kind;
  std::int64_t max_steps;
  double length_integration_tolerance;
  double speed_integration_tolerance;
};

static_assert(std::is_pod<AdaptiveStepParameters>::value,
              "AdaptiveStepParameters is used for interfacing");

extern "C"
struct BodyGeopotentialElement {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  char const* degree;
  char const* order;
  char const* cos;
  char const* j;
  char const* sin;
};

static_assert(std::is_pod<BodyGeopotentialElement>::value,
              "BodyGeopotentialElement is used for interfacing");

extern "C"
struct BodyParameters {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  char const* name;
  char const* gravitational_parameter;
  char const* reference_instant;
  char const* min_radius;
  char const* mean_radius;
  char const* max_radius;
  char const* axis_right_ascension;
  char const* axis_declination;
  char const* reference_angle;
  char const* angular_frequency;
  char const* reference_radius;
  char const* j2;
  BodyGeopotentialElement const* const* geopotential;
};

static_assert(std::is_pod<BodyParameters>::value,
              "BodyParameters is used for interfacing");

extern "C"
struct Burn {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  double thrust_in_kilonewtons;
  double specific_impulse_in_seconds_g0;
  NavigationFrameParameters frame;
  double initial_time;
  XYZ delta_v;
  bool is_inertially_fixed;
};

static_assert(std::is_pod<Burn>::value,
              "Burn is used for interfacing");

extern "C"
struct ConfigurationAccuracyParameters {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  char const* fitting_tolerance;
  char const* geopotential_tolerance;
};

static_assert(std::is_pod<ConfigurationAccuracyParameters>::value,
              "ConfigurationAccuracyParameters is used for interfacing");

extern "C"
struct ConfigurationFixedStepParameters {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  char const* fixed_step_size_integrator;
  char const* integration_step_size;
};

static_assert(std::is_pod<ConfigurationFixedStepParameters>::value,
              "ConfigurationFixedStepParameters is used for interfacing");

extern "C"
struct ConfigurationAdaptiveStepParameters {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  char const* adaptive_step_size_integrator;
  char const* length_integration_tolerance;
  char const* speed_integration_tolerance;
};

static_assert(std::is_pod<ConfigurationAdaptiveStepParameters>::value,
              "ConfigurationAdaptiveStepParameters is used for interfacing");

extern "C"
struct FlightPlanAdaptiveStepParameters {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  std::int64_t integrator_kind;
  std::int64_t generalized_integrator_kind;
  std::int64_t max_steps;
  double length_integration_tolerance;
  double speed_integration_tolerance;
};

static_assert(std::is_pod<FlightPlanAdaptiveStepParameters>::value,
              "FlightPlanAdaptiveStepParameters is used for interfacing");

extern "C"
struct KeplerianElements {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  double eccentricity;
  double semimajor_axis;
  double mean_motion;
  double inclination_in_degrees;
  double longitude_of_ascending_node_in_degrees;
  double argument_of_periapsis_in_degrees;
  double mean_anomaly;
};

static_assert(std::is_pod<KeplerianElements>::value,
              "KeplerianElements is used for interfacing");

extern "C"
struct NavigationManoeuvre {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  Burn burn;
  double initial_mass_in_tonnes;
  double final_mass_in_tonnes;
  double mass_flow;
  double duration;
  double final_time;
  double time_of_half_delta_v;
  double time_to_half_delta_v;
};

static_assert(std::is_pod<NavigationManoeuvre>::value,
              "NavigationManoeuvre is used for interfacing");

extern "C"
struct NavigationManoeuvreFrenetTrihedron {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  XYZ binormal;
  XYZ normal;
  XYZ tangent;
};

static_assert(std::is_pod<NavigationManoeuvreFrenetTrihedron>::value,
              "NavigationManoeuvreFrenetTrihedron is used for interfacing");

extern "C"
struct Origin {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  bool reference_part_is_at_origin;
  bool reference_part_is_unmoving;
  uint32_t reference_part_id;
  XYZ main_body_centre_in_world;
};

static_assert(std::is_pod<Origin>::value,
              "Origin is used for interfacing");

extern "C"
struct QP {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  XYZ q;
  XYZ p;
};

static_assert(std::is_pod<QP>::value,
              "QP is used for interfacing");

extern "C"
struct Status {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  int error;
  char const* message;
};

static_assert(std::is_pod<Status>::value,
              "Status is used for interfacing");

extern "C"
struct WXYZ {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  double w;
  double x;
  double y;
  double z;
};

static_assert(std::is_pod<WXYZ>::value,
              "WXYZ is used for interfacing");

extern "C"
struct QPRW {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  QP qp;
  WXYZ r;
  XYZ w;
};

static_assert(std::is_pod<QPRW>::value,
              "QPRW is used for interfacing");

extern "C"
struct XY {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  double x;
  double y;
};

static_assert(std::is_pod<XY>::value,
              "XY is used for interfacing");

extern "C"
struct Interval {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  double min;
  double max;
};

static_assert(std::is_pod<Interval>::value,
              "Interval is used for interfacing");

extern "C"
struct OrbitalElements {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  double sidereal_period;
  double nodal_period;
  double anomalistic_period;
  double nodal_precession;
  Interval mean_semimajor_axis;
  Interval mean_eccentricity;
  Interval mean_inclination;
  Interval mean_longitude_of_ascending_nodes;
  Interval mean_argument_of_periapsis;
  Interval mean_periapsis_distance;
  Interval mean_apoapsis_distance;
  Interval radial_distance;
};

static_assert(std::is_pod<OrbitalElements>::value,
              "OrbitalElements is used for interfacing");

extern "C"
struct OrbitRecurrence {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  int nuo;
  int dto;
  int cto;
  int number_of_revolutions;
  double equatorial_shift;
  double base_interval;
  double grid_interval;
  int subcycle;
};

static_assert(std::is_pod<OrbitRecurrence>::value,
              "OrbitRecurrence is used for interfacing");

extern "C"
struct EquatorialCrossings {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  Interval longitudes_reduced_to_ascending_pass;
  Interval longitudes_reduced_to_descending_pass;
};

static_assert(std::is_pod<EquatorialCrossings>::value,
              "EquatorialCrossings is used for interfacing");

extern "C"
struct OrbitGroundTrack {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  EquatorialCrossings equatorial_crossings;
};

static_assert(std::is_pod<OrbitGroundTrack>::value,
              "OrbitGroundTrack is used for interfacing");

extern "C"
struct OrbitAnalysis {
  static void* operator new(std::size_t size) {
    return ::operator new(size);
  };
  double progress_of_next_analysis;
  int const* primary_index;
  double mission_duration;
  OrbitalElements const* elements;
  OrbitRecurrence const* recurrence;
  OrbitGroundTrack const* ground_track;
  OrbitAnalysis* elements_address;
  OrbitAnalysis* recurrence_address;
  OrbitAnalysis* ground_track_address;
};

static_assert(std::is_pod<OrbitAnalysis>::value,
              "OrbitAnalysis is used for interfacing");

extern "C" PRINCIPIA_DLL
void __cdecl principia__AdvanceTime(
    Plugin* const plugin,
    double const t,
    double const planetarium_rotation);

extern "C" PRINCIPIA_DLL
XYZ __cdecl principia__AngularMomentumFromAngularVelocity(
    XYZ const world_angular_velocity,
    XYZ const moments_of_inertia_in_tonnes,
    WXYZ const principal_axes_rotation,
    WXYZ const part_rotation);

extern "C" PRINCIPIA_DLL
WXYZ __cdecl principia__CameraReferenceRotation(
    Plugin* const plugin);

extern "C" PRINCIPIA_DLL
void __cdecl principia__CatchUpLaggingVessels(
    Plugin* const plugin,
    Iterator** const collided_vessels);

extern "C" PRINCIPIA_DLL
QP __cdecl principia__CelestialFromParent(
    Plugin const* const plugin,
    int const celestial_index);

extern "C" PRINCIPIA_DLL
double __cdecl principia__CelestialInitialRotationInDegrees(
    Plugin const* const plugin,
    int const celestial_index);

extern "C" PRINCIPIA_DLL
WXYZ __cdecl principia__CelestialRotation(
    Plugin const* const plugin,
    int const index);

extern "C" PRINCIPIA_DLL
double __cdecl principia__CelestialRotationPeriod(
    Plugin const* const plugin,
    int const celestial_index);

extern "C" PRINCIPIA_DLL
WXYZ __cdecl principia__CelestialSphereRotation(
    Plugin const* const plugin);

extern "C" PRINCIPIA_DLL
QP __cdecl principia__CelestialWorldDegreesOfFreedom(
    Plugin const* const plugin,
    int const index,
    Origin const origin,
    double const time);

extern "C" PRINCIPIA_DLL
void __cdecl principia__ClearFlags();

extern "C" PRINCIPIA_DLL
void __cdecl principia__ClearTargetVessel(
    Plugin* const plugin);

extern "C" PRINCIPIA_DLL
void __cdecl principia__ClearWorldRotationalReferenceFrame(
    Plugin* const plugin);

extern "C" PRINCIPIA_DLL
double __cdecl principia__CurrentTime(
    Plugin const* const plugin);

extern "C" PRINCIPIA_DLL
void __cdecl principia__DeleteInterchange(
    void const** const native_pointer);

extern "C" PRINCIPIA_DLL
void __cdecl principia__DeletePlugin(
    Plugin const** const plugin);

extern "C" PRINCIPIA_DLL
void __cdecl principia__DeleteString(
    char const** const native_string);

extern "C" PRINCIPIA_DLL
void __cdecl principia__DeleteU16String(
    char16_t const** const native_string);

extern "C" PRINCIPIA_DLL
void __cdecl principia__DeserializePlugin(
    char const* const serialization,
    PushDeserializer** const deserializer,
    Plugin const** const plugin,
    char const* const compressor,
    char const* const encoder);

extern "C" PRINCIPIA_DLL
void __cdecl principia__EndInitialization(
    Plugin* const plugin);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__ExternalCelestialGetPosition(
    Plugin const* const plugin,
    int const body_index,
    double const time,
    XYZ* const position);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__ExternalCelestialGetSurfacePosition(
    Plugin const* const plugin,
    int const body_index,
    double const planetocentric_latitude_in_degrees,
    double const planetocentric_longitude_in_degrees,
    double const radius,
    double const time,
    XYZ* const position);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__ExternalFlowFreefall(
    Plugin const* const plugin,
    int const central_body_index,
    QP const world_body_centred_initial_degrees_of_freedom,
    double const t_initial,
    double const t_final,
    QP* const world_body_centred_final_degrees_of_freedom);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__ExternalGeopotentialGetCoefficient(
    Plugin const* const plugin,
    int const body_index,
    int const degree,
    int const order,
    XY* const coefficient);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__ExternalGeopotentialGetReferenceRadius(
    Plugin const* const plugin,
    int const body_index,
    double* const reference_radius);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__ExternalVesselGetPosition(
    Plugin const* const plugin,
    char const* const vessel_guid,
    double const time,
    XYZ* const position);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__ExternalGetNearestPlannedCoastDegreesOfFreedom(
    Plugin const* const plugin,
    int const central_body_index,
    char const* const vessel_guid,
    int const manoeuvre_index,
    XYZ const world_body_centred_reference_position,
    QP* const world_body_centred_nearest_degrees_of_freedom);

extern "C" PRINCIPIA_DLL
void __cdecl principia__FlightPlanCreate(
    Plugin const* const plugin,
    char const* const vessel_guid,
    double const final_time,
    double const mass_in_tonnes);

extern "C" PRINCIPIA_DLL
void __cdecl principia__FlightPlanDelete(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
bool __cdecl principia__FlightPlanExists(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
FlightPlanAdaptiveStepParameters __cdecl principia__FlightPlanGetAdaptiveStepParameters(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
double __cdecl principia__FlightPlanGetActualFinalTime(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
double __cdecl principia__FlightPlanGetDesiredFinalTime(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
XYZ __cdecl principia__FlightPlanGetGuidance(
    Plugin const* const plugin,
    char const* const vessel_guid,
    int const index);

extern "C" PRINCIPIA_DLL
double __cdecl principia__FlightPlanGetInitialTime(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
NavigationManoeuvre __cdecl principia__FlightPlanGetManoeuvre(
    Plugin const* const plugin,
    char const* const vessel_guid,
    int const index);

extern "C" PRINCIPIA_DLL
NavigationManoeuvreFrenetTrihedron __cdecl principia__FlightPlanGetManoeuvreFrenetTrihedron(
    Plugin const* const plugin,
    char const* const vessel_guid,
    int const index);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__FlightPlanInsert(
    Plugin const* const plugin,
    char const* const vessel_guid,
    Burn const burn,
    int const index);

extern "C" PRINCIPIA_DLL
int __cdecl principia__FlightPlanNumberOfAnomalousManoeuvres(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
int __cdecl principia__FlightPlanNumberOfManoeuvres(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
int __cdecl principia__FlightPlanNumberOfSegments(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__FlightPlanRebase(
    Plugin const* const plugin,
    char const* const vessel_guid,
    double const mass_in_tonnes);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__FlightPlanRemove(
    Plugin const* const plugin,
    char const* const vessel_guid,
    int const index);

extern "C" PRINCIPIA_DLL
void __cdecl principia__FlightPlanRenderedApsides(
    Plugin const* const plugin,
    char const* const vessel_guid,
    int const celestial_index,
    XYZ const sun_world_position,
    int const max_points,
    Iterator** const apoapsides,
    Iterator** const periapsides);

extern "C" PRINCIPIA_DLL
void __cdecl principia__FlightPlanRenderedClosestApproaches(
    Plugin const* const plugin,
    char const* const vessel_guid,
    XYZ const sun_world_position,
    int const max_points,
    Iterator** const closest_approaches);

extern "C" PRINCIPIA_DLL
void __cdecl principia__FlightPlanRenderedNodes(
    Plugin const* const plugin,
    char const* const vessel_guid,
    XYZ const sun_world_position,
    int const max_points,
    Iterator** const ascending,
    Iterator** const descending);

extern "C" PRINCIPIA_DLL
Iterator* __cdecl principia__FlightPlanRenderedSegment(
    Plugin const* const plugin,
    char const* const vessel_guid,
    XYZ const sun_world_position,
    int const index);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__FlightPlanReplace(
    Plugin const* const plugin,
    char const* const vessel_guid,
    Burn const burn,
    int const index);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__FlightPlanSetAdaptiveStepParameters(
    Plugin const* const plugin,
    char const* const vessel_guid,
    FlightPlanAdaptiveStepParameters const flight_plan_adaptive_step_parameters);

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__FlightPlanSetDesiredFinalTime(
    Plugin const* const plugin,
    char const* const vessel_guid,
    double const final_time);

extern "C" PRINCIPIA_DLL
void __cdecl principia__ForgetAllHistoriesBefore(
    Plugin* const plugin,
    double const t);

extern "C" PRINCIPIA_DLL
void __cdecl principia__FreeVesselsAndPartsAndCollectPileUps(
    Plugin* const plugin,
    double const delta_t);

extern "C" PRINCIPIA_DLL
PileUpFuture* __cdecl principia__FutureCatchUpVessel(
    Plugin* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
void __cdecl principia__FutureWaitForVesselToCatchUp(
    Plugin* const plugin,
    PileUpFuture** const future,
    Iterator** const collided_vessels);

extern "C" PRINCIPIA_DLL
int __cdecl principia__GetBufferDuration();

extern "C" PRINCIPIA_DLL
int __cdecl principia__GetBufferedLogging();

extern "C" PRINCIPIA_DLL
int __cdecl principia__GetStderrLogging();

extern "C" PRINCIPIA_DLL
int __cdecl principia__GetSuppressedLogging();

extern "C" PRINCIPIA_DLL
int __cdecl principia__GetVerboseLogging();

extern "C" PRINCIPIA_DLL
void __cdecl principia__GetVersion(
    char const** const build_date,
    char const** const version);

extern "C" PRINCIPIA_DLL
bool __cdecl principia__HasEncounteredApocalypse(
    Plugin* const plugin,
    char const** const details);

extern "C" PRINCIPIA_DLL
bool __cdecl principia__HasVessel(
    Plugin* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
void __cdecl principia__InitializeEphemerisParameters(
    Plugin* const plugin,
    ConfigurationAccuracyParameters const& accuracy_parameters,
    ConfigurationFixedStepParameters const& fixed_step_parameters);

extern "C" PRINCIPIA_DLL
void __cdecl principia__InitializeHistoryParameters(
    Plugin* const plugin,
    ConfigurationFixedStepParameters const& parameters);

extern "C" PRINCIPIA_DLL
void __cdecl principia__InitializePsychohistoryParameters(
    Plugin* const plugin,
    ConfigurationAdaptiveStepParameters const& parameters);

extern "C" PRINCIPIA_DLL
void __cdecl principia__InsertCelestialAbsoluteCartesian(
    Plugin* const plugin,
    int const celestial_index,
    int const* const parent_index,
    BodyParameters const& body_parameters,
    char const* const x,
    char const* const y,
    char const* const z,
    char const* const vx,
    char const* const vy,
    char const* const vz);

extern "C" PRINCIPIA_DLL
void __cdecl principia__InsertCelestialJacobiKeplerian(
    Plugin* const plugin,
    int const celestial_index,
    int const* const parent_index,
    BodyParameters const& body_parameters,
    KeplerianElements const* const keplerian_elements);

extern "C" PRINCIPIA_DLL
void __cdecl principia__InsertOrKeepVessel(
    Plugin* const plugin,
    char const* const vessel_guid,
    char const* const vessel_name,
    int const parent_index,
    bool const loaded,
    bool* const inserted);

extern "C" PRINCIPIA_DLL
void __cdecl principia__InsertOrKeepLoadedPart(
    Plugin* const plugin,
    uint32_t const part_id,
    char const* const name,
    double const mass_in_tonnes,
    XYZ const centre_of_mass,
    XYZ const moments_of_inertia_in_tonnes,
    WXYZ const principal_axes_rotation,
    bool const is_solid_rocket_motor,
    char const* const vessel_guid,
    int const main_body_index,
    QP const main_body_world_degrees_of_freedom,
    QP const part_world_degrees_of_freedom,
    WXYZ const part_rotation,
    XYZ const part_angular_velocity,
    double const delta_t);

extern "C" PRINCIPIA_DLL
void __cdecl principia__InsertUnloadedPart(
    Plugin* const plugin,
    uint32_t const part_id,
    char const* const name,
    char const* const vessel_guid,
    QP const from_parent);

extern "C" PRINCIPIA_DLL
bool __cdecl principia__IteratorAtEnd(
    Iterator const* const iterator);

extern "C" PRINCIPIA_DLL
void __cdecl principia__IteratorDelete(
    Iterator** const iterator);

extern "C" PRINCIPIA_DLL
QP __cdecl principia__IteratorGetDiscreteTrajectoryQP(
    Iterator const* const iterator);

extern "C" PRINCIPIA_DLL
double __cdecl principia__IteratorGetDiscreteTrajectoryTime(
    Iterator const* const iterator);

extern "C" PRINCIPIA_DLL
XYZ __cdecl principia__IteratorGetDiscreteTrajectoryXYZ(
    Iterator const* const iterator);

extern "C" PRINCIPIA_DLL
Iterator* __cdecl principia__IteratorGetRP2LinesIterator(
    Iterator const* const iterator);

extern "C" PRINCIPIA_DLL
XY __cdecl principia__IteratorGetRP2LineXY(
    Iterator const* const iterator);

extern "C" PRINCIPIA_DLL
char const* __cdecl principia__IteratorGetVesselGuid(
    Iterator const* const iterator);

extern "C" PRINCIPIA_DLL
void __cdecl principia__IteratorIncrement(
    Iterator* const iterator);

extern "C" PRINCIPIA_DLL
void __cdecl principia__IteratorReset(
    Iterator* const iterator);

extern "C" PRINCIPIA_DLL
int __cdecl principia__IteratorSize(
    Iterator const* const iterator);

extern "C" PRINCIPIA_DLL
void __cdecl principia__LogError(
    char const* const file,
    int const line,
    char const* const text);

extern "C" PRINCIPIA_DLL
void __cdecl principia__LogFatal(
    char const* const file,
    int const line,
    char const* const text);

extern "C" PRINCIPIA_DLL
void __cdecl principia__LogInfo(
    char const* const file,
    int const line,
    char const* const text);

extern "C" PRINCIPIA_DLL
void __cdecl principia__LogWarning(
    char const* const file,
    int const line,
    char const* const text);

extern "C" PRINCIPIA_DLL
void __cdecl principia__MonitorSetName(
    int const i,
    char const* const name);

extern "C" PRINCIPIA_DLL
void __cdecl principia__MonitorStart(
    int const i);

extern "C" PRINCIPIA_DLL
void __cdecl principia__MonitorStop(
    int const i);

extern "C" PRINCIPIA_DLL
WXYZ __cdecl principia__NavballOrientation(
    Plugin const* const plugin,
    XYZ const sun_world_position,
    XYZ const ship_world_position);

extern "C" PRINCIPIA_DLL
Plugin* __cdecl principia__NewPlugin(
    char const* const game_epoch,
    char const* const solar_system_epoch,
    double const planetarium_rotation_in_degrees);

extern "C" PRINCIPIA_DLL
void __cdecl principia__PartApplyIntrinsicForce(
    Plugin* const plugin,
    uint32_t const part_id,
    XYZ const force_in_kilonewtons);

extern "C" PRINCIPIA_DLL
void __cdecl principia__PartApplyIntrinsicForceAtPosition(
    Plugin* const plugin,
    uint32_t const part_id,
    XYZ const force_in_kilonewtons,
    XYZ const lever_arm);

extern "C" PRINCIPIA_DLL
void __cdecl principia__PartApplyIntrinsicTorque(
    Plugin* const plugin,
    uint32_t const part_id,
    XYZ const torque_in_kilonewton_metre);

extern "C" PRINCIPIA_DLL
QPRW __cdecl principia__PartGetActualRigidMotion(
    Plugin const* const plugin,
    uint32_t const part_id,
    Origin const origin);

extern "C" PRINCIPIA_DLL
bool __cdecl principia__PartIsTruthful(
    Plugin const* const plugin,
    uint32_t const part_id);

extern "C" PRINCIPIA_DLL
void __cdecl principia__PartSetApparentRigidMotion(
    Plugin* const plugin,
    uint32_t const part_id,
    QP const degrees_of_freedom,
    WXYZ const rotation,
    XYZ const angular_velocity);

extern "C" PRINCIPIA_DLL
Planetarium* __cdecl principia__PlanetariumCreate(
    Plugin const* const plugin,
    XYZ const sun_world_position,
    XYZ const xyz_opengl_camera_x_in_world,
    XYZ const xyz_opengl_camera_y_in_world,
    XYZ const xyz_opengl_camera_z_in_world,
    XYZ const xyz_camera_position_in_world,
    double const focal,
    double const field_of_view);

extern "C" PRINCIPIA_DLL
void __cdecl principia__PlanetariumDelete(
    Planetarium const** const planetarium);

extern "C" PRINCIPIA_DLL
Iterator* __cdecl principia__PlanetariumPlotCelestialTrajectoryForPsychohistory(
    Planetarium const* const planetarium,
    Plugin const* const plugin,
    int const celestial_index,
    char const* const vessel_guid,
    double const max_history_length);

extern "C" PRINCIPIA_DLL
Iterator* __cdecl principia__PlanetariumPlotCelestialTrajectoryForPredictionOrFlightPlan(
    Planetarium const* const planetarium,
    Plugin const* const plugin,
    int const celestial_index,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
Iterator* __cdecl principia__PlanetariumPlotFlightPlanSegment(
    Planetarium const* const planetarium,
    Plugin const* const plugin,
    int const method,
    char const* const vessel_guid,
    int const index);

extern "C" PRINCIPIA_DLL
Iterator* __cdecl principia__PlanetariumPlotPrediction(
    Planetarium const* const planetarium,
    Plugin const* const plugin,
    int const method,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
Iterator* __cdecl principia__PlanetariumPlotPsychohistory(
    Planetarium const* const planetarium,
    Plugin const* const plugin,
    int const method,
    char const* const vessel_guid,
    double const max_history_length);

extern "C" PRINCIPIA_DLL
void __cdecl principia__PrepareToReportCollisions(
    Plugin* const plugin);

extern "C" PRINCIPIA_DLL
void __cdecl principia__RenderedPredictionApsides(
    Plugin const* const plugin,
    char const* const vessel_guid,
    int const celestial_index,
    XYZ const sun_world_position,
    int const max_points,
    Iterator** const apoapsides,
    Iterator** const periapsides);

extern "C" PRINCIPIA_DLL
void __cdecl principia__RenderedPredictionClosestApproaches(
    Plugin const* const plugin,
    char const* const vessel_guid,
    XYZ const sun_world_position,
    int const max_points,
    Iterator** const closest_approaches);

extern "C" PRINCIPIA_DLL
void __cdecl principia__RenderedPredictionNodes(
    Plugin const* const plugin,
    char const* const vessel_guid,
    XYZ const sun_world_position,
    int const max_points,
    Iterator** const ascending,
    Iterator** const descending);

extern "C" PRINCIPIA_DLL
void __cdecl principia__ReportGroundCollision(
    Plugin const* const plugin,
    uint32_t const part_id);

extern "C" PRINCIPIA_DLL
void __cdecl principia__ReportPartCollision(
    Plugin const* const plugin,
    uint32_t const part1_id,
    uint32_t const part2_id);

extern "C" PRINCIPIA_DLL
char const* __cdecl principia__SayHello();

extern "C" PRINCIPIA_DLL
Status* __cdecl principia__SayNotFound();

extern "C" PRINCIPIA_DLL
char const* __cdecl principia__SerializePlugin(
    Plugin const* const plugin,
    PullSerializer** const serializer,
    char const* const compressor,
    char const* const encoder);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetBufferDuration(
    int const seconds);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetBufferedLogging(
    int const max_severity);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetFlag(
    char const* const name,
    char const* const value);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetMainBody(
    Plugin* const plugin,
    int const index);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetPlottingFrame(
    Plugin* const plugin,
    NavigationFrameParameters const parameters);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetStderrLogging(
    int const min_severity);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetSuppressedLogging(
    int const min_severity);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetTargetVessel(
    Plugin* const plugin,
    char const* const vessel_guid,
    int const reference_body_index);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetVerboseLogging(
    int const level);

extern "C" PRINCIPIA_DLL
void __cdecl principia__SetWorldRotationalReferenceFrame(
    Plugin* const plugin,
    int const index);

extern "C" PRINCIPIA_DLL
XYZ __cdecl principia__UnmanageableVesselVelocity(
    Plugin const* const plugin,
    QP const degrees_of_freedom,
    int const celestial_index);

extern "C" PRINCIPIA_DLL
void __cdecl principia__UpdateCelestialHierarchy(
    Plugin const* const plugin,
    int const celestial_index,
    int const parent_index);

extern "C" PRINCIPIA_DLL
void __cdecl principia__UpdatePrediction(
    Plugin const* const plugin,
    char const* const vessel_guids);

extern "C" PRINCIPIA_DLL
XYZ __cdecl principia__VesselBinormal(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
QP __cdecl principia__VesselFromParent(
    Plugin const* const plugin,
    int const parent_index,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
OrbitAnalysis* __cdecl principia__VesselGetAnalysis(
    Plugin* const plugin,
    char const* const vessel_guid,
    int const* const revolutions_per_cycle,
    int const* const days_per_cycle,
    int const ground_track_revolution);

extern "C" PRINCIPIA_DLL
AdaptiveStepParameters __cdecl principia__VesselGetPredictionAdaptiveStepParameters(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
XYZ __cdecl principia__VesselNormal(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
void __cdecl principia__VesselRefreshAnalysis(
    Plugin* const plugin,
    char const* const vessel_guid,
    double const mission_duration);

extern "C" PRINCIPIA_DLL
void __cdecl principia__VesselSetPredictionAdaptiveStepParameters(
    Plugin const* const plugin,
    char const* const vessel_guid,
    AdaptiveStepParameters const adaptive_step_parameters);

extern "C" PRINCIPIA_DLL
XYZ __cdecl principia__VesselTangent(
    Plugin const* const plugin,
    char const* const vessel_guid);

extern "C" PRINCIPIA_DLL
XYZ __cdecl principia__VesselVelocity(
    Plugin const* const plugin,
    char const* const vessel_guid);

