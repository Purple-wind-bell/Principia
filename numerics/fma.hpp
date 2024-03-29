#pragma once

#include <immintrin.h>

#include "base/cpuid.hpp"
#include "base/macros.hpp"  // 🧙 For PRINCIPIA_USE_FMA_IF_AVAILABLE.

namespace principia {
namespace numerics {
namespace _fma {
namespace internal {

using namespace principia::base::_cpuid;

// With clang, using FMA requires VEX-encoding everything; see #3019.
#if PRINCIPIA_COMPILER_MSVC
constexpr bool CanEmitFMAInstructions = true;
#else
constexpr bool CanEmitFMAInstructions = false;
#endif

// The functions in this file unconditionally wrap the appropriate intrinsics.
// The caller may only use them if |UseHardwareFMA| is true.
#if PRINCIPIA_USE_FMA_IF_AVAILABLE()
#if PRINCIPIA_CAN_OVERRIDE_FMA_USAGE_AT_RUNTIME
inline bool DefaultUseHardwareFMA =
    CanEmitFMAInstructions && HasCPUFeatures(CPUFeatureFlags::FMA);
inline bool InternalUseHardwareFMA = DefaultUseHardwareFMA;
class FMAPreventer {
 public:
  FMAPreventer() {
    InternalUseHardwareFMA = false;
  }
  ~FMAPreventer() {
    InternalUseHardwareFMA = DefaultUseHardwareFMA;
  }
};
inline bool const& UseHardwareFMA = InternalUseHardwareFMA;
#else
inline bool const UseHardwareFMA =
    CanEmitFMAInstructions && HasCPUFeatures(CPUFeatureFlags::FMA);
class FMAPreventer;  // Undefined.
#endif
#else
inline bool const UseHardwareFMA = false;
#endif

// ⟦ab + c⟧.
inline double FusedMultiplyAdd(double a, double b, double c);

// ⟦ab - c⟧.
inline double FusedMultiplySubtract(double a, double b, double c);

// ⟦-ab + c⟧.
inline double FusedNegatedMultiplyAdd(double a, double b, double c);

// ⟦-ab - c⟧.
inline double FusedNegatedMultiplySubtract(double a, double b, double c);

}  // namespace internal

using internal::CanEmitFMAInstructions;
using internal::FMAPreventer;
using internal::FusedMultiplyAdd;
using internal::FusedMultiplySubtract;
using internal::FusedNegatedMultiplyAdd;
using internal::FusedNegatedMultiplySubtract;
using internal::UseHardwareFMA;

}  // namespace _fma
}  // namespace numerics
}  // namespace principia

#include "numerics/fma_body.hpp"
