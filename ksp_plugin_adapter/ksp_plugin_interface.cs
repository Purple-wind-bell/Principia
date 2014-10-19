﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace principia {
namespace ksp_plugin_adapter {

[KSPAddon(startup : KSPAddon.Startup.Flight, once : false)]
public partial class PluginAdapter : UnityEngine.MonoBehaviour {

  private const string kDllPath = "GameData/Principia/principia.dll";

  [StructLayout(LayoutKind.Sequential)]
  private struct XYZ {
    double x, y, z;
    public static explicit operator XYZ(Vector3d v) {
      return new XYZ {x = v.x, y = v.y, z = v.z};
    }
    public static explicit operator Vector3d(XYZ v) {
      return new Vector3d {x = v.x, y = v.y, z = v.z};
    }
  };

  // Logging.

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void InitGoogleLogging();

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void LogInfo(
      [MarshalAs(UnmanagedType.LPStr)] String message);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void LogWarning(
      [MarshalAs(UnmanagedType.LPStr)] String message);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void LogError(
      [MarshalAs(UnmanagedType.LPStr)] String message);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void LogFatal(
      [MarshalAs(UnmanagedType.LPStr)] String message);

  // Plugin interface.

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern IntPtr SayHello();

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern IntPtr NewPlugin(
      double initial_time,
      int sun_index,
      double sun_gravitational_parameter,
      double planetarium_rotation_in_degrees);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void DeletePlugin(ref IntPtr plugin);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void InsertCelestial(
      IntPtr plugin,
      int celestial_index,
      double gravitational_parameter,
      int parent_index,
      XYZ from_parent_position,
      XYZ from_parent_velocity);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void EndInitialization(IntPtr plugin);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void UpdateCelestialHierarchy(IntPtr plugin,
                                                      int celestial_index,
                                                      int parent_index);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern bool InsertOrKeepVessel(
      IntPtr plugin,
      [MarshalAs(UnmanagedType.LPStr)] String guid,
      int parent_index);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void SetVesselStateOffset(
      IntPtr plugin,
      [MarshalAs(UnmanagedType.LPStr)] String guid,
      XYZ from_parent_position,
      XYZ from_parent_velocity);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern void AdvanceTime(IntPtr plugin, 
                                         double t,
                                         double planetarium_rotation);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern XYZ VesselDisplacementFromParent(
      IntPtr plugin,
      [MarshalAs(UnmanagedType.LPStr)] String guid);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern XYZ VesselParentRelativeVelocity(
      IntPtr plugin,
      [MarshalAs(UnmanagedType.LPStr)] String guid);

  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern XYZ CelestialDisplacementFromParent(
      IntPtr plugin,
      int celestial_index);
  
  [DllImport(dllName           : kDllPath,
             CallingConvention = CallingConvention.Cdecl)]
  private static extern XYZ CelestialParentRelativeVelocity(
      IntPtr plugin,
      int celestial_index);
}

}  // namespace ksp_plugin_adapter
}  // namespace principia
