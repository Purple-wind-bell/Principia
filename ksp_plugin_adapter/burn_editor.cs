﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace principia {
namespace ksp_plugin_adapter {

class BurnEditor {
  string name_ = "Manœuvre";
  DifferentialSlider Δv_tangent_;
  DifferentialSlider Δv_normal_;
  DifferentialSlider Δv_binormal_;
  DifferentialSlider initial_time_;

  const double Log10ΔvLowerRate = -3.0;
  const double Log10ΔvUpperRate = 3.5;

  // Returns the equivalent of the .NET >= 4 format
  // span.ToString(@"ddd \d hh \h mm \m\i\n ss.FFF \s").
  private string FormatTimeSpan (TimeSpan span) {
     return span.Days.ToString("000") + " d " +
            span.Hours.ToString("00") + " h " +
            span.Minutes.ToString("00") + " min " +
            span.Seconds.ToString("00") + " s";
  }

  public BurnEditor() {
    Δv_tangent_ = new DifferentialSlider(label            : "Δv tangent",
                                         unit             : "m / s",
                                         log10_lower_rate : Log10ΔvLowerRate,
                                         log10_upper_rate : Log10ΔvUpperRate);
    Δv_normal_ = new DifferentialSlider(label            : "Δv normal",
                                        unit             : "m / s",
                                        log10_lower_rate : Log10ΔvLowerRate,
                                        log10_upper_rate : Log10ΔvUpperRate);
    Δv_binormal_ = new DifferentialSlider(label            : "Δv binormal",
                                          unit             : "m / s",
                                          log10_lower_rate : Log10ΔvLowerRate,
                                          log10_upper_rate : Log10ΔvUpperRate);
    initial_time_ =
        new DifferentialSlider(
                label            : "t initial",
                unit             : null,
                log10_lower_rate : 0.0,
                log10_upper_rate : 7.0,
                min_value        : 0,
                max_value        : double.PositiveInfinity,
                format           : value => FormatTimeSpan(
                                                TimeSpan.FromSeconds(value)));
  }

  // Renders the |BurnEditor|.  Returns true if and only if the settings were
  // changed.
  public bool Render() {
    var old_skin = UnityEngine.GUI.skin;
    UnityEngine.GUI.skin = null;
    UnityEngine.GUILayout.BeginVertical();
    name_ = UnityEngine.GUILayout.TextField(
                name_,
                UnityEngine.GUILayout.ExpandWidth(true));
    bool changed = false;
    changed |= Δv_tangent_.Render();
    changed |= Δv_normal_.Render();
    changed |= Δv_binormal_.Render();
    changed |= initial_time_.Render();
    UnityEngine.GUILayout.Label(
        text       : "(from end of previous burn)",
        options    : UnityEngine.GUILayout.Width(250));
    UnityEngine.GUILayout.EndVertical();
    UnityEngine.GUI.skin = old_skin;
    return changed;
  }
}

}  // namespace ksp_plugin_adapter
}  // namespace principia
