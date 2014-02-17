﻿using KSP;
using NewtonianPhysics;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace Principia {
  [KSPAddon(KSPAddon.Startup.Flight, false)]
  public class Core : MonoBehaviour {
    // TODO(robin): This class is a mess. The calculations, both for renderering
    // and moving vessels around, should be done elsewhere.
    protected Rect mainWindowPosition;
    protected Rect referenceFrameWindowPosition;
    private const double Day = 24 * Hour;
    private const double Hour = 60 * Minute;
    private const double Minute = 60 * Second;
    private const double PredictionLength = 1 * Day;
    private const double Second = 1;
    private const double Year = 365.25 * Day;
    private Vector3d a;
    private Vector3d activeVesselAccumulatedVelocity;
    private Vector3d activeVesselProperAcceleration;
    private Vector3d activeVesselProperAcceleration2;
    private double[] activeVesselProperAccelerations = new double[100];
    private Vector3d activeVesselProperObtAcceleration;
    private Vector3d activeVesselVelocity, activeVesselObtVelocity;
    private Dictionary<string, Body> bodies;
    private Vector3d coriolis, gravity, centrifugal;
    private Vector3d geometricAcceleration;
    private Vector3d geometricAcceleration2;
    private MapRenderer mapRenderer;
    private bool predictTrajectories = false;
    private Vector3d q;
    private CelestialBody referenceBody;
    private ReferenceFrameType referenceFrameType;
    private int samplingIndex;
    private bool simulate = false;
    private Vector3d[] standardBasis = { new Vector3d { x = 1, y = 0, z = 0 },
                                         new Vector3d { x = 0, y = 1, z = 0 },
                                         new Vector3d { x = 0, y = 0, z = 1 } };
    private NBodySystem system;
    private Dictionary<string, LineRenderer> trajectories;
    private Vector3d v;
    private enum ReferenceFrameType {
      [Description("Body-Centric Inertial")]
      Inertial,
      [Description("Body-Centric Rotating")]
      Surface,
      [Description("Barycentric Co-Rotating")]
      CoRotating
    }

    #region Unity Methods

    private void FixedUpdate() {
      if (!FlightGlobals.ready) {
        return;
      }
      Vessel activeVessel = FlightGlobals.ActiveVessel;
      a = new Vector3d {
        x = activeVessel.acceleration.x,
        y = activeVessel.acceleration.y,
        z = activeVessel.acceleration.z
      };
      v = new Vector3d {
        x = activeVessel.orbit.vel.x,
        y = activeVessel.orbit.vel.y,
        z = activeVessel.orbit.vel.z
      };
      double UT = Planetarium.GetUniversalTime();
      QuaternionD rotation = Planetarium.Rotation;
      {
        activeVesselAccumulatedVelocity = Vector3d.zero;
        double totalMass = 0;
        foreach (Part part in activeVessel.parts) {
          activeVesselAccumulatedVelocity +=
            (Vector3d)part.rb.velocity * (double)part.rb.mass;
          totalMass += (double)part.rb.mass;
        }
        activeVesselAccumulatedVelocity /= totalMass;
        activeVesselAccumulatedVelocity += Krakensbane.GetFrameVelocity();

        // Yet another geometric acceleration.
        CelestialBody primary = activeVessel.orbit.referenceBody;
        Vector3d vesselPosition = activeVessel.findLocalCenterOfMass();
        Vector3d vesselVelocity =
          ((Vector3d)activeVessel.rootPart.rb.GetPointVelocity(vesselPosition))
          + Krakensbane.GetFrameVelocity();
        geometricAcceleration2 =
          FlightGlobals.getGeeForceAtPosition(vesselPosition)
          + FlightGlobals.getCoriolisAcc(vesselVelocity, primary)
          + FlightGlobals.getCentrifugalAcc(vesselPosition, primary);

        activeVesselProperObtAcceleration
          = (activeVessel.obt_velocity - activeVesselObtVelocity)
          / TimeWarp.fixedDeltaTime - (gravity + coriolis / 2 + centrifugal);
        activeVesselObtVelocity = activeVessel.obt_velocity;

        Vector3d newVelocity =
          activeVessel.rootPart.rb.GetPointVelocity(activeVessel.orbitDriver.driverTransform.TransformPoint(activeVessel.orbitDriver.localCoM));
        newVelocity += Krakensbane.GetFrameVelocity();
        activeVesselProperAcceleration = (newVelocity - activeVesselVelocity)
                              / TimeWarp.fixedDeltaTime - geometricAcceleration;
        activeVesselProperAcceleration2 = (newVelocity - activeVesselVelocity)
                             / TimeWarp.fixedDeltaTime - geometricAcceleration2;
        activeVesselVelocity = newVelocity;
        activeVesselProperAccelerations[samplingIndex]
          = activeVesselProperAcceleration.magnitude;
        samplingIndex = (samplingIndex + 1)
          % activeVesselProperAccelerations.Length;
        // Now we compute the geometric acceleration which will be applied by
        // Unity over the next Euler step.
        geometricAcceleration =
          FlightGlobals.getGeeForceAtPosition(vesselPosition)
          + FlightGlobals.getCoriolisAcc(vesselVelocity, primary)
          + FlightGlobals.getCentrifugalAcc(vesselPosition, primary);

        gravity = FlightGlobals.getGeeForceAtPosition(vesselPosition);
        coriolis = FlightGlobals.getCoriolisAcc(vesselVelocity, primary);
        centrifugal = FlightGlobals.getCentrifugalAcc(vesselPosition, primary);

        q = primary.position - vesselPosition;
      }
      if (simulate) {
#if TRACE
        print("Initiating simulation step...");
#endif
        system.Evolve(UT, 10);
        foreach (CelestialBody body in FlightGlobals.Bodies) {
          if (body.name != "Sun") {
#if TRACE
            print("Updating " + body.name + "...");
#endif
            Body secondary, primary;
            bodies.TryGetValue(body.name, out secondary);
            bodies.TryGetValue(body.referenceBody.name, out primary);
            Vector3d position = secondary.q.ToVector() - primary.q.ToVector();
            Vector3d velocity = secondary.v.ToVector() - primary.v.ToVector();
            Orbit original = body.orbitDriver.orbit;
            Orbit copy = new Orbit(original.inclination,
                                   original.eccentricity,
                                   original.semiMajorAxis,
                                   original.LAN,
                                   original.argumentOfPeriapsis,
                                   original.meanAnomalyAtEpoch,
                                   original.epoch,
                                   original.referenceBody);
            copy.UpdateFromStateVectors((rotation * position.xzy).xzy,
                                        (rotation * velocity.xzy).xzy,
                                        copy.referenceBody,
                                        UT);
            body.orbit.inclination = copy.inclination;
            body.orbit.eccentricity = copy.eccentricity;
            body.orbit.semiMajorAxis = copy.semiMajorAxis;
            body.orbit.LAN = copy.LAN;
            body.orbit.argumentOfPeriapsis = copy.argumentOfPeriapsis;
            body.orbit.meanAnomalyAtEpoch = copy.meanAnomalyAtEpoch;
            body.orbit.epoch = copy.epoch;
            body.orbit.referenceBody = copy.referenceBody;
            body.orbit.Init();
            body.orbit.UpdateFromUT(UT);
            body.CBUpdate();
          }
        }
        foreach (Vessel vessel in FlightGlobals.Vessels) {
          if (vessel.situation != Vessel.Situations.LANDED &&
            vessel.situation != Vessel.Situations.SPLASHED &&
            vessel.situation != Vessel.Situations.PRELAUNCH) {
#if TRACE
            print("Updating " + vessel.name + "...");
#endif
            Body secondary, primary;
            bodies.TryGetValue(vessel.id.ToString(), out secondary);
            bodies.TryGetValue(vessel.orbit.referenceBody.name,
                               out primary);
            Vector3d position = secondary.q.ToVector()
                                - primary.q.ToVector();
            Vector3d velocity = secondary.v.ToVector()
                                - primary.v.ToVector();
            vessel.orbit.UpdateFromStateVectors(
              (rotation * position.xzy).xzy,
              (rotation * velocity.xzy).xzy,
              vessel.orbit.referenceBody, UT);
          }
        }
      }
      if (predictTrajectories) {
        system.AdvancePredictions(tmax: UT + 1 * Day,
                                  maxTimestep: 10 * Second,
                                  samplingPeriod: 10);
      }
    }
    private void OnDestroy() {
      RenderingManager.RemoveFromPostDrawQueue(3, new Callback(DrawGUI));
    }
    private void Start() {
      print("Principia: Starting...");
      bodies = new Dictionary<string, Body>();
      trajectories = new Dictionary<string, LineRenderer>();
      referenceBody = Planetarium.fetch.Sun;
      referenceFrameType = ReferenceFrameType.Inertial;
      RenderingManager.AddToPostDrawQueue(3, new Callback(DrawGUI));
      if ((mainWindowPosition.x == 0)
          && (mainWindowPosition.y == 0)) {
        mainWindowPosition = new Rect(Screen.width / 2,
                                      Screen.height / 2,
                                      10, 10);
      }
      if ((referenceFrameWindowPosition.x == 0)
          && (referenceFrameWindowPosition.y == 0)) {
        referenceFrameWindowPosition = new Rect(Screen.width / 3,
                                                Screen.height / 3,
                                                10, 10);
      }
      mapRenderer = MapRenderer.CreateAndAttach(DrawTrajectories);
      print("Principia: Started.");
    }
    private void Update() {
      DrawTrajectories();
    }

    #endregion Unity Methods

    #region GUI

    private void DrawGUI() {
      GUI.skin = HighLogic.Skin;
      mainWindowPosition = GUILayout.Window(1,
                                   mainWindowPosition,
                                   MainWindow,
                                   "Traces of Various Descriptions",
                                   GUILayout.MinWidth(500));
      referenceFrameWindowPosition = GUILayout.Window(2,
                                     referenceFrameWindowPosition,
                                     ReferenceFrameWindow,
                                     "Reference Frame",
                                     GUILayout.MinWidth(500));
    }
    private void MainWindow(int windowID) {
      GUIStyle mySty = new GUIStyle(GUI.skin.button);
      mySty.normal.textColor = mySty.focused.textColor = Color.white;
      mySty.hover.textColor = mySty.active.textColor = Color.yellow;
      mySty.onNormal.textColor
        = mySty.onFocused.textColor
        = mySty.onHover.textColor
        = mySty.onActive.textColor = Color.green;
      mySty.padding = new RectOffset(8, 8, 8, 8);

      GUILayout.BeginVertical();
      GUILayout.TextArea("q: " + q.x.ToString("F3") + ", "
                               + q.y.ToString("F3") + ", "
                               + q.z.ToString("F3") + " ("
                               + q.magnitude.ToString("F3") + ")",
                               GUILayout.ExpandWidth(true));
      GUILayout.TextArea("v: " + v.x.ToString("F3") + ", "
                               + v.y.ToString("F3") + ", "
                               + v.z.ToString("F3") + " ("
                               + v.magnitude.ToString("F3") + ")",
                               GUILayout.ExpandWidth(true));
      GUILayout.TextArea("a: " + a.x.ToString("F3") + ", "
                               + a.y.ToString("F3") + ", "
                               + a.z.ToString("F3") + " ("
                               + a.magnitude.ToString("F3") + ")",
                               GUILayout.ExpandWidth(true));
      GUILayout.TextArea("Planetarium rotation: "
                         + Planetarium.Rotation.w.ToString("F5") + " + "
                         + Planetarium.Rotation.x.ToString("F5") + " i + "
                         + Planetarium.Rotation.y.ToString("F5") + " j + "
                         + Planetarium.Rotation.z.ToString("F5") + " k",
                         GUILayout.ExpandWidth(true));
      if (GUILayout.Button(simulate ? "Switch to Keplerian"
                                    : "Switch to Newtonian",
                           mySty, GUILayout.ExpandWidth(true))) {
        simulate = !simulate;
        double UT = Planetarium.GetUniversalTime();
        bodies.Clear();
        foreach (CelestialBody body in FlightGlobals.Bodies) {
          print("Adding body " + body.name + "...");
          bodies.Add(body.name, body.ToBody());
        }
        foreach (Vessel vessel in FlightGlobals.Vessels) {
          if (vessel.situation == Vessel.Situations.ESCAPING ||
              vessel.situation == Vessel.Situations.ORBITING ||
              vessel.situation == Vessel.Situations.SUB_ORBITAL) {
            print("Adding vessel " + vessel.name + "...");
            bodies.Add(vessel.id.ToString(), vessel.ToBody());
          }
        }
        system = new NBodySystem(bodies.Values.ToArray(), UT);
      }
      if (GUILayout.Button(
                    predictTrajectories ? "Stop Plotting" : "Plot Trajectories",
                    mySty, GUILayout.ExpandWidth(true))) {
        predictTrajectories = !predictTrajectories;
        double UT = Planetarium.GetUniversalTime();
        if (predictTrajectories) {
          foreach (Vessel vessel in FlightGlobals.Vessels) {
            if (vessel.situation == Vessel.Situations.ESCAPING ||
                vessel.situation == Vessel.Situations.ORBITING ||
                vessel.situation == Vessel.Situations.SUB_ORBITAL) {
              print("Adding vessel " + vessel.name + " trajectory...");
              GameObject lineObject = new GameObject("Line");
              // TODO(robin): Switch to layer 31, use a mesh, draw the lines in
              // 2D if MapView.Draw3DLines is false (needs a MapViewLine class).
              lineObject.layer = 9;
              LineRenderer line = lineObject.AddComponent<LineRenderer>();
              line.transform.parent = null;
              line.material = MapView.fetch.orbitLinesMaterial;
              line.SetColors(Color.blue, Color.red);
              line.useWorldSpace = true;
              trajectories.Add(vessel.id.ToString(), line);
            }
          }
          system.RecalculatePredictions(tmax: UT + 1 * Day,
                                        maxTimestep: 10 * Second,
                                        samplingPeriod: 10);
        } else {
          foreach (LineRenderer line in trajectories.Values) {
            Destroy(line);
          }
          trajectories.Clear();
        }
      }
      if (simulate) {
        GUILayout.TextArea("Simulating n-body physics...");
        Body kerbin;
        bodies.TryGetValue("Kerbin", out kerbin);
        CelestialBody minmus = FlightGlobals.Bodies[3];
        GUILayout.TextArea("qKerbin: " + kerbin.q.ToVector().ToString("F5"));
        GUILayout.TextArea("vKerbin: " + kerbin.v.ToVector().ToString("F5"));
        GUILayout.TextArea("ωMinmus: " + minmus.angularVelocity.ToString("F5"));
      }
      GUILayout.TextArea("MapView.Draw3DLines: "
                         + MapView.Draw3DLines.ToString());
      GUILayout.TextArea("ActiveVessel.GetWorldPos3D: " +
                     FlightGlobals.ActiveVessel.GetWorldPos3D().ToString("F5"));
      GUILayout.TextArea("Krakensbane.GetFrameVelocity: " +
                         Krakensbane.GetFrameVelocity().ToString("F5"));
      GUILayout.TextArea("FlightGlobals.ActiveVessel.rb_velocity: "
           + ((Vector3d)FlightGlobals.ActiveVessel.rb_velocity).ToString("F5"));
      GUILayout.TextArea("FlightGlobals.ActiveVessel.rootPart.rb.velocity: "
        + ((Vector3d)FlightGlobals.ActiveVessel.rootPart.rb.velocity)
                                                               .ToString("F5"));
      GUILayout.TextArea("FlightGlobals.ActiveVessel.perturbation: "
          + ((Vector3d)FlightGlobals.ActiveVessel.perturbation).ToString("F5"));
      GUILayout.TextArea("FlightGlobals.ActiveVessel.geeForce: "
                          + FlightGlobals.ActiveVessel.geeForce.ToString());
      GUILayout.TextArea("FlightGlobals.ActiveVessel.geeForce_immediate: "
                + FlightGlobals.ActiveVessel.geeForce_immediate.ToString());
      GUILayout.TextArea("Proper Acceleration: "
        + activeVesselProperAcceleration.ToString("F9"));
      GUILayout.TextArea("Peak Proper Acceleration: "
        + activeVesselProperAccelerations.Max().ToString("F9"));
      GUILayout.TextArea("Proper Acceleration 2: "
        + activeVesselProperAcceleration2.ToString("F9"));
      GUILayout.TextArea("Proper Obt Acceleration: "
        + activeVesselProperObtAcceleration.ToString("F9"));

      GUILayout.TextArea("Root velocity: "
        + activeVesselVelocity.ToString("F9"));
      GUILayout.TextArea("Accumulated velocity: "
        + activeVesselAccumulatedVelocity.ToString("F9"));
      GUILayout.TextArea("obt_velocity: "
        + activeVesselObtVelocity.ToString("F9"));

      GUILayout.TextArea("gravity: " + gravity.ToString("F9"));
      GUILayout.TextArea("centrifugal: " + centrifugal.ToString("F9"));
      GUILayout.TextArea("coriolis: " + coriolis.ToString("F9"));
      string integrators = "";
      foreach (Part part in FlightGlobals.ActiveVessel.parts) {
        integrators += part.partName + ": "
          + (part.flightIntegrator == null ? "null; " : "not null; ");
      }
      GUILayout.TextArea(integrators);
      GUILayout.EndVertical();

      GUI.DragWindow(new Rect(left: 0f, top: 0f, width: 10000f, height: 20f));
    }
    private void ReferenceFrameWindow(int windowID) {
      GUIStyle mySty = new GUIStyle(GUI.skin.button);
      mySty.normal.textColor = mySty.focused.textColor = Color.white;
      mySty.hover.textColor = mySty.active.textColor = Color.yellow;
      mySty.onNormal.textColor
        = mySty.onFocused.textColor
        = mySty.onHover.textColor
        = mySty.onActive.textColor = Color.green;
      mySty.padding = new RectOffset(8, 8, 8, 8);

      GUILayout.BeginHorizontal();
      GUILayout.BeginVertical(GUILayout.Width(250));
      foreach (CelestialBody body in FlightGlobals.Bodies) {
        if (GUILayout.Toggle(referenceBody.name == body.name,
                             body.name)
            && referenceBody.name != body.name) {
          referenceBody = body;
        }
      }
      GUILayout.EndVertical();
      GUILayout.BeginVertical(GUILayout.ExpandWidth(true));
      foreach (ReferenceFrameType type in
               Enum.GetValues(typeof(ReferenceFrameType))) {
        if (GUILayout.Toggle(referenceFrameType == type,
                             type.Description(),
                             GUILayout.ExpandWidth(true))
            && referenceFrameType != type
            && referenceBody.name != "Sun") {
          referenceFrameType = type;
        }
      }
      GUILayout.EndVertical();
      GUILayout.EndHorizontal();

      GUI.DragWindow(new Rect(left: 0f, top: 0f, width: 10000f, height: 20f));
    }

    #endregion GUI

    private void DrawTrajectories() {
      if (MapView.MapIsEnabled && predictTrajectories) {
        QuaternionD rotation = Planetarium.Rotation;
        PlanetariumCamera camera = (PlanetariumCamera)
          GameObject.FindObjectOfType(typeof(PlanetariumCamera));
        double UT = Planetarium.GetUniversalTime();
        foreach (Vessel v in FlightGlobals.Vessels) {
          // TODO(robin): There are too many nested expressions here, and I've
          // caught quite a few bugs from misplaced parentheses. Declare more
          // variables so this part is at least vaguely readable.
          if (v.situation != Vessel.Situations.LANDED &&
            v.situation != Vessel.Situations.SPLASHED &&
            v.situation != Vessel.Situations.PRELAUNCH) {
            LineRenderer line;
            Body vessel;
            Body reference;
            Body primary = null;              // The silly language forbids the
            CelestialBody primaryBody = null; // use of 'unassigned' variables.
            trajectories.TryGetValue(v.id.ToString(), out line);
            bodies.TryGetValue(v.id.ToString(), out vessel);
            bodies.TryGetValue(referenceBody.name, out reference);
            if (referenceFrameType == ReferenceFrameType.CoRotating) {
              primaryBody = referenceBody.orbit.referenceBody;
              bodies.TryGetValue(primaryBody.name, out primary);
            }
            line.SetVertexCount(vessel.predictedTrajectory.Count);
            line.enabled = true;
            Vector3d currentVesselPosition = v.GetWorldPos3D();
            line.SetWidth((0.01f * camera.Distance), (0.01f * camera.Distance));
#pragma warning disable 618 // Obsolete.
            // Quaternion.AxisAngle uses radians (which we want, like and use)
            // and is deprecated because of that for some mindbogglingly stupid
            // reason. We use it nonetheless.
            // We also use Quaternion rather than QuaternionD because KSP's
            // QuaternionD seems broken (maybe it doesn't implement deprecated
            // functions).
            // TODO(robin): Make a versor; I wouldn't want to trust Unity with a
            // calculation even if it were done in double precision.
            for (int i = 0; i < vessel.predictedTrajectory.Count; ++i) {
              switch (referenceFrameType) {
                case ReferenceFrameType.CoRotating:
                  Vector3d predictedBarycenter =
                    (reference.predictedTrajectory[i].q.ToVector().xzy
                    * referenceBody.Mass
                    + primary.predictedTrajectory[i].q.ToVector().xzy
                    * primaryBody.Mass)
                  / (referenceBody.Mass + primaryBody.Mass);
                  Vector3d currentBarycenter =
                    (reference.q.ToVector().xzy * referenceBody.Mass
                    + primary.q.ToVector().xzy * primaryBody.Mass)
                    / (referenceBody.Mass + primaryBody.Mass);
                  line.SetPosition(i, ScaledSpace.LocalToScaledSpace(
                    rotation * (
                    Quaternion.FromToRotation(
                      reference.predictedTrajectory[i].q.ToVector().xzy
                        - predictedBarycenter,
                      reference.q.ToVector().xzy - currentBarycenter)
                    * (vessel.predictedTrajectory[i].q.ToVector().xzy
                      - predictedBarycenter) + currentBarycenter
                    - vessel.q.ToVector().xzy) + currentVesselPosition));
                  break;
                case ReferenceFrameType.Inertial:
                  line.SetPosition(i, ScaledSpace.LocalToScaledSpace(
                    rotation * (vessel.predictedTrajectory[i].q.ToVector().xzy
                             - reference.predictedTrajectory[i].q.ToVector().xzy
                             + reference.q.ToVector().xzy
                             - vessel.q.ToVector().xzy)
                    + currentVesselPosition));
                  break;
                case ReferenceFrameType.Surface:
                  line.SetPosition(i, ScaledSpace.LocalToScaledSpace(
                    rotation * (
                    Quaternion.AxisAngle(referenceBody.angularVelocity, (float)(
                                         referenceBody.angularV * (UT -
                                              vessel.predictedTrajectory[i].t)))
                      * (vessel.predictedTrajectory[i].q.ToVector().xzy
                        - reference.predictedTrajectory[i].q.ToVector().xzy)
                      + reference.q.ToVector().xzy - vessel.q.ToVector().xzy)
                    + currentVesselPosition));
                  break;
              }
            }
#pragma warning restore 618
          }
        }
      }
    }
  }
}