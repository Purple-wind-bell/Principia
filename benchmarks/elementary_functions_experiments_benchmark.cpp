// .\Release\x64\benchmarks.exe --benchmark_repetitions=3 --benchmark_filter=BM_Experiment  // NOLINT(whitespace/line_length)

#include <array>
#include <cmath>
#include <cstdint>
#include <random>
#include <utility>

#include "benchmark/benchmark.h"
#include "numerics/double_precision.hpp"
#include "numerics/scale_b.hpp"
#include "quantities/elementary_functions.hpp"
#include "quantities/numbers.hpp"  // 🧙 For π.

namespace principia {
namespace functions {

// TODO(phl): The polynomials in this file should use class
// |PolynomialInMonomialBasis|.
// TODO(phl): Study the effect of rounding the polynomial coefficients to
// machine numbers.

using namespace principia::numerics::_double_precision;
using namespace principia::numerics::_scale_b;
using namespace principia::quantities::_elementary_functions;

using Value = double;
using Argument = double;

constexpr Argument x_min = π / 6;  // The sinus is greater than 1/2.
constexpr Argument x_max = π / 4;  // Upper bound after argument reduction.
static constexpr std::int64_t number_of_iterations = 1000;

// A helper class to benchmark implementations with various table spacings, and
// compare the effect of reduced table spacing with the effect of increased
// polynomial degrees.
template<Argument table_spacing>
class TableSpacingImplementation {
 public:
  void Initialize();

  Value Sin(Argument x);
  Value Cos(Argument x);

 private:
  // Despite the name these are not accurate values, but for the purposes of
  // benchmarking it doesn't matter.
  struct AccurateValues {
    Argument x;
    Value sin_x;
    Value cos_x;
  };

  Value SinPolynomial(Argument x);
  Value CosPolynomial(Argument x);

  std::array<AccurateValues,
             static_cast<std::int64_t>(x_max / table_spacing) + 1>
      accurate_values_;
};

// A helper class to benchmark an implementation with multiple tables.  Each
// table covers a binade of the value of the Sin function, and tables closer to
// 0 have smaller spacing.
class MultiTableImplementation {
 public:
  static constexpr Argument max_table_spacing = 2.0 / 1024.0;
  static constexpr std::int64_t number_of_tables = 9;

  // ArcSin(2^-(n + 1)) for n in [0, 8] rounded towards positive infinity.  The
  // entry at index n has n leading zeroes in the mantissa of its sinus.
  static constexpr std::array<Argument, number_of_tables> cutoffs{
      0x1.0C152382D7366p-1,
      0x1.02BE9CE0B87CEp-2,
      0x1.00ABE0C129E1Fp-3,
      0x1.002ABDE95361Ap-4,
      0x1.000AABDE0B9C9p-5,
      0x1.0002AABDDE94Dp-6,
      0x1.0000AAABDDE0Cp-7,
      0x1.00002AAABDDDFp-8,
      0x1.00000AAAABDDEp-9};

  // The spacing between arguments above each cutoff.
  static constexpr std::array<Argument, number_of_tables> table_spacings{
      ScaleB(max_table_spacing, 0),
      ScaleB(max_table_spacing, -1),
      ScaleB(max_table_spacing, -2),
      ScaleB(max_table_spacing, -3),
      ScaleB(max_table_spacing, -4),
      ScaleB(max_table_spacing, -5),
      ScaleB(max_table_spacing, -6),
      ScaleB(max_table_spacing, -7),
      ScaleB(max_table_spacing, -8)};

  void Initialize();

  Value Sin(Argument x);
  Value Cos(Argument x);

 private:
  // Despite the name these are not accurate values, but for the purposes of
  // benchmarking it doesn't matter.
  struct AccurateValues {
    Argument x;
    Value sin_x;
    Value cos_x;
  };

  void SelectCutoff(Argument x, std::int64_t& index, double& cutoff);

  Value SinPolynomial(Argument x);
  // |i| is the index of the binade in |cutoffs_|,
  Value CosPolynomial(std::int64_t i, Argument x);

  // Because the interval [π / 6, π / 4] is shorter than the next one below, the
  // maximum value is reached between the first two cutoffs.
  static constexpr std::int64_t table_size =
      static_cast<std::int64_t>((cutoffs[0] - cutoffs[1]) / table_spacings[1]);

  std::array<std::int64_t, number_of_tables> one_over_table_spacings_;
  std::array<std::array<AccurateValues, table_size>, number_of_tables>
      accurate_values_;
};

//TODO(phl) comment
class SingleTableImplementation {
 public:
  static constexpr Argument table_spacing = 2.0 / 1024.0;

  // ArcSin[1/8], rounded towards infinity.  Two more leading zeroes than the
  // high binade.
  // TODO(phl): Rigourous error analysis needed to check that this is the right
  // cutoff.
  static constexpr Argument cutoff = 0x1.00ABE0C129E1Fp-3;

  // ArcSin[1/512], rounded towards infinity.
  static constexpr Argument min_argument = 0x1.00000AAAABDDEp-9;

  void Initialize();

  Value Sin(Argument x);
  Value Cos(Argument x);

 private:
  // Despite the name these are not accurate values, but for the purposes of
  // benchmarking it doesn't matter.
  struct AccurateValues {
    Argument x;
    Value sin_x;
    Value cos_x;
  };

  static constexpr Value cos_polynomial_0 = -0.499999999999999999999872434553;

  Value SinPolynomial(Argument x);
  Value CosPolynomial(Argument x);
  Value CosPolynomial2(Argument x);

  std::array<AccurateValues,
             static_cast<std::int64_t>(x_max / table_spacing) + 1>
      accurate_values_;
};

template<Argument table_spacing>
void TableSpacingImplementation<table_spacing>::Initialize() {
  int i = 0;
  for (Argument x = table_spacing / 2;
       x <= x_max + table_spacing / 2;
       x += table_spacing, ++i) {
    accurate_values_[i] = {.x = x,
                           .sin_x = std::sin(x),
                           .cos_x = std::cos(x)};
  }
}

template<Argument table_spacing>
FORCE_INLINE(inline)
Value TableSpacingImplementation<table_spacing>::Sin(Argument const x) {
  auto const i = static_cast<std::int64_t>(x * (1.0 / table_spacing));
  auto const& accurate_values = accurate_values_[i];
  auto const& x₀ = accurate_values.x;
  auto const& sin_x₀ = accurate_values.sin_x;
  auto const& cos_x₀ = accurate_values.cos_x;
  auto const h = x - x₀;
  auto const h² = h * h;
  auto const h³ = h² * h;
  auto const sin_x₀_plus_h_cos_x₀ = TwoProductAdd(cos_x₀, h, sin_x₀);
  return sin_x₀_plus_h_cos_x₀.value +
         ((sin_x₀ * h² * CosPolynomial(h²) + cos_x₀ * h³ * SinPolynomial(h²)) +
          sin_x₀_plus_h_cos_x₀.error);
}

template<Argument table_spacing>
FORCE_INLINE(inline)
Value TableSpacingImplementation<table_spacing>::Cos(Argument const x) {
  auto const i = static_cast<std::int64_t>(x * (1.0 / table_spacing));
  auto const& accurate_values = accurate_values_[i];
  auto const& x₀ = accurate_values.x;
  auto const& sin_x₀ = accurate_values.sin_x;
  auto const& cos_x₀ = accurate_values.cos_x;
  auto const h = x - x₀;
  auto const h² = h * h;
  auto const h³ = h² * h;
  auto const cos_x₀_minus_h_sin_x₀ = TwoProductNegatedAdd(sin_x₀, h, cos_x₀);
  return cos_x₀_minus_h_sin_x₀.value +
         ((cos_x₀ * h² * CosPolynomial(h²) - sin_x₀ * h³ * SinPolynomial(h²)) +
          cos_x₀_minus_h_sin_x₀.error);
}

template<Argument table_spacing>
Value TableSpacingImplementation<table_spacing>::SinPolynomial(
    Argument const x) {
  if constexpr (table_spacing == 2.0 / 256.0) {
    // 71 bits.
    return -0.166666666666666666666421797625 +
           0.00833333057503280528178543245797 * x;
  } else if constexpr (table_spacing == 2.0 / 1024.0) {
    // 85 bits.
    return -0.166666666666666666666666651721 +
           0.00833333316093951937646271666739 * x;
  }
}

template<Argument table_spacing>
Value TableSpacingImplementation<table_spacing>::CosPolynomial(
    Argument const x) {
  if constexpr (table_spacing == 2.0 / 256.0) {
    // 77 bits.
    return -0.499999999999999999999999974543 +
           x * (0.0416666666666633318024480868405 -
                0.00138888829905860875255146938745 * x);
  } else if constexpr (table_spacing == 2.0 / 1024.0) {
    // 72 bits.
    return -0.499999999999999999999872434553 +
           0.0416666654823785864634569932662 * x;
  }
}

void MultiTableImplementation::Initialize() {
  Argument current_x_max = x_max;
  Argument current_x_min;
  for (std::int64_t i = 0; i < number_of_tables; ++i) {
    current_x_min = cutoffs[i];
    one_over_table_spacings_[i] = 1.0 / table_spacings[i];
    Argument x = current_x_min + table_spacings[i] / 2;
    for (std::int64_t j = 0; j < table_size && x < current_x_max; ++j) {
      accurate_values_[i][j] = {.x = x,
                                .sin_x = std::sin(x),
                                .cos_x = std::cos(x)};
      x += table_spacings[i];
    }
    current_x_max = current_x_min;
  }
}

FORCE_INLINE(inline)
Value MultiTableImplementation::Sin(Argument const x) {
  std::int64_t i;
  Argument cutoff;
  SelectCutoff(x, i, cutoff);

  auto const j = static_cast<std::int64_t>((x - cutoff) *
                                           one_over_table_spacings_[i]);
  auto const& accurate_values = accurate_values_[i][j];
  auto const& x₀ = accurate_values.x;
  auto const& sin_x₀ = accurate_values.sin_x;
  auto const& cos_x₀ = accurate_values.cos_x;
  auto const h = x - x₀;
  auto const h² = h * h;
  auto const h³ = h² * h;
  auto const sin_x₀_plus_h_cos_x₀ = TwoProductAdd(cos_x₀, h, sin_x₀);
  return sin_x₀_plus_h_cos_x₀.value + ((sin_x₀ * h² * CosPolynomial(i, h²) +
                                        cos_x₀ * h³ * SinPolynomial(h²)) +
                                       sin_x₀_plus_h_cos_x₀.error);
}

FORCE_INLINE(inline)
Value MultiTableImplementation::Cos(Argument const x) {
  std::int64_t i;
  Argument cutoff;
  SelectCutoff(x, i, cutoff);

  auto const j = static_cast<std::int64_t>((x - cutoff) *
                                           one_over_table_spacings_[i]);
  auto const& accurate_values = accurate_values_[i][j];
  auto const& x₀ = accurate_values.x;
  auto const& sin_x₀ = accurate_values.sin_x;
  auto const& cos_x₀ = accurate_values.cos_x;
  auto const h = x - x₀;
  auto const h² = h * h;
  auto const h³ = h² * h;
  auto const cos_x₀_minus_h_sin_x₀ = TwoProductNegatedAdd(sin_x₀, h, cos_x₀);
  return cos_x₀_minus_h_sin_x₀.value + ((cos_x₀ * h² * CosPolynomial(i, h²) -
                                         sin_x₀ * h³ * SinPolynomial(h²)) +
                                        cos_x₀_minus_h_sin_x₀.error);
}

FORCE_INLINE(inline)
void MultiTableImplementation::SelectCutoff(Argument const x,
                                            std::int64_t& index,
                                            Argument& cutoff) {
  // The details of this code have a measurable performance impact.  It does on
  // average 2.30 comparisons.  That's more than a naive loop starting at
  // |k = 0| (which would do 2.28 comparisons) but it's faster in practice.
  if (x <= cutoffs[1]) {
    // Because the intervals are unequal, this loop does on average 1.93
    // comparisons.
    for (std::int64_t k = 2; k < cutoffs.size(); ++k) {
      if (cutoffs[k] <= x) {
        index = k;
        cutoff = cutoffs[k];
        break;
      }
    }
  } else if (cutoffs[0] <= x) {
    index = 0;
    cutoff = cutoffs[0];
  } else {
    index = 1;
    cutoff = cutoffs[1];
  }
}

Value MultiTableImplementation::SinPolynomial(Argument const x) {
  // 85 bits.  Works for all binades.
  return -0.166666666666666666666666651721 +
         0.00833333316093951937646271666739 * x;
}

Value MultiTableImplementation::CosPolynomial(std::int64_t const i,
                                              Argument const x) {
  // The polynomials for the highest two binades don't give us enough bits, so
  // we have to use 3 polynomials.
  // i == 1 goes first because it is the largest argument interval.
  if (i == 1) {
    // 78 bits.
    return -0.499999999999999999999998006790 +
           0.0416666663705946726372008045758 * x;
  } else if (i == 0) {
    // 72 bits.
    return -0.499999999999999999999872434553 +
           0.0416666654823785864634569932662 * x;
  } else {
    // 84 bits.
    return -0.499999999999999999999999968856 +
           0.0416666665926486697856340784849 * x;
  }
}

void SingleTableImplementation::Initialize() {
  int i = 0;
  for (Argument x = table_spacing / 2;
       x <= x_max + table_spacing / 2;
       x += table_spacing, ++i) {
    accurate_values_[i] = {.x = x,
                           .sin_x = std::sin(x),
                           .cos_x = std::cos(x)};
  }
}

FORCE_INLINE(inline)
Value SingleTableImplementation::Sin(Argument const x) {
  auto const i = static_cast<std::int64_t>(x * (1.0 / table_spacing));
  auto const& accurate_values = accurate_values_[i];
  auto const& x₀ = accurate_values.x;
  auto const& sin_x₀ = accurate_values.sin_x;
  auto const& cos_x₀ = accurate_values.cos_x;
  auto const h = x - x₀;
  auto const sin_x₀_plus_h_cos_x₀ = TwoProductAdd(cos_x₀, h, sin_x₀);
  if (cutoff <= x) {
    auto const h² = h * h;
    auto const h³ = h² * h;
    return sin_x₀_plus_h_cos_x₀.value + ((sin_x₀ * h² * CosPolynomial(h²) +
                                          cos_x₀ * h³ * SinPolynomial(h²)) +
                                         sin_x₀_plus_h_cos_x₀.error);
  } else {
    // TODO(phl): Error analysis of this computation.
    auto const h² = TwoProduct(h, h);
    auto const h³ = h².value * h;
    auto const h²_sin_x₀_cos_polynomial_0 =
        h² * TwoProduct(sin_x₀, cos_polynomial_0);
    auto const terms_up_to_h² = QuickTwoSum(sin_x₀_plus_h_cos_x₀.value,
                                            h²_sin_x₀_cos_polynomial_0.value);
    return terms_up_to_h².value +
           ((sin_x₀ * h².value * CosPolynomial2(h².value) +
             cos_x₀ * h³ * SinPolynomial(h².value)) +
            sin_x₀_plus_h_cos_x₀.error + h²_sin_x₀_cos_polynomial_0.error);
  }
}

FORCE_INLINE(inline)
Value SingleTableImplementation::Cos(Argument const x) {
  auto const i = static_cast<std::int64_t>(x * (1.0 / table_spacing));
  auto const& accurate_values = accurate_values_[i];
  auto const& x₀ = accurate_values.x;
  auto const& sin_x₀ = accurate_values.sin_x;
  auto const& cos_x₀ = accurate_values.cos_x;
  auto const h = x - x₀;
  auto const cos_x₀_minus_h_sin_x₀ = TwoProductNegatedAdd(sin_x₀, h, cos_x₀);
  if (cutoff <= x) {
    auto const h² = h * h;
    auto const h³ = h² * h;
    return cos_x₀_minus_h_sin_x₀.value + ((cos_x₀ * h² * CosPolynomial(h²) -
                                           sin_x₀ * h³ * SinPolynomial(h²)) +
                                          cos_x₀_minus_h_sin_x₀.error);
  } else {
    // TODO(phl): Error analysis of this computation.
    auto const h² = TwoProduct(h, h);
    auto const h³ = h².value * h;
    auto const h²_cos_x₀_cos_polynomial_0 =
        h² * TwoProduct(cos_x₀, cos_polynomial_0);
    auto const terms_up_to_h² = QuickTwoSum(cos_x₀_minus_h_sin_x₀.value,
                                            h²_cos_x₀_cos_polynomial_0.value);
    return terms_up_to_h².value +
           ((cos_x₀ * h².value * CosPolynomial2(h².value) -
             sin_x₀ * h³ * SinPolynomial(h².value)) +
            cos_x₀_minus_h_sin_x₀.error + h²_cos_x₀_cos_polynomial_0.error);
  }
}

Value SingleTableImplementation::SinPolynomial(
    Argument const x) {
  // 85 bits.
  return -0.166666666666666666666666651721 +
         0.00833333316093951937646271666739 * x;
}

Value SingleTableImplementation::CosPolynomial(
    Argument const x) {
  // 72 bits.
  return cos_polynomial_0 + 0.0416666654823785864634569932662 * x;
}

Value SingleTableImplementation::CosPolynomial2(
    Argument const x) {
  // 101 bits.
  return x * (0.04166666666666665363986848039146102332933 -
              0.001388888852024502693312293343727757316234 * x);
}

template<Argument table_spacing>
void BM_ExperimentSinTableSpacing(benchmark::State& state) {
  std::mt19937_64 random(42);
  std::uniform_real_distribution<> uniformly_at(x_min, x_max);

  TableSpacingImplementation<table_spacing> implementation;
  implementation.Initialize();

  Argument a[number_of_iterations];
  for (std::int64_t i = 0; i < number_of_iterations; ++i) {
    a[i] = uniformly_at(random);
  }

  Value v[number_of_iterations];
  while (state.KeepRunningBatch(number_of_iterations)) {
    for (std::int64_t i = 0; i < number_of_iterations; ++i) {
      v[i] = implementation.Sin(a[i]);
#if _DEBUG
      // The implementation is not accurate, but let's check that it's not
      // broken.
      auto const absolute_error = Abs(v[i] - std::sin(a[i]));
      CHECK_LT(absolute_error, 5.6e-17);
#endif
    }
    benchmark::DoNotOptimize(v);
  }
}

template<Argument table_spacing>
void BM_ExperimentCosTableSpacing(benchmark::State& state) {
  std::mt19937_64 random(42);
  std::uniform_real_distribution<> uniformly_at(x_min, x_max);

  TableSpacingImplementation<table_spacing> implementation;
  implementation.Initialize();

  Argument a[number_of_iterations];
  for (std::int64_t i = 0; i < number_of_iterations; ++i) {
    a[i] = uniformly_at(random);
  }

  Value v[number_of_iterations];
  while (state.KeepRunningBatch(number_of_iterations)) {
    for (std::int64_t i = 0; i < number_of_iterations; ++i) {
      v[i] = implementation.Cos(a[i]);
#if _DEBUG
      // The implementation is not accurate, but let's check that it's not
      // broken.
      auto const absolute_error = Abs(v[i] - std::cos(a[i]));
      CHECK_LT(absolute_error, 1.2e-16);
#endif
    }
    benchmark::DoNotOptimize(v);
  }
}

void BM_ExperimentSinMultiTable(benchmark::State& state) {
  std::mt19937_64 random(42);
  std::uniform_real_distribution<> uniformly_at(
      MultiTableImplementation::cutoffs
          [MultiTableImplementation::number_of_tables - 1],
      x_max);

  MultiTableImplementation implementation;
  implementation.Initialize();

  Argument a[number_of_iterations];
  for (std::int64_t i = 0; i < number_of_iterations; ++i) {
    a[i] = uniformly_at(random);
  }

  Value v[number_of_iterations];
  while (state.KeepRunningBatch(number_of_iterations)) {
    for (std::int64_t i = 0; i < number_of_iterations; ++i) {
      v[i] = implementation.Sin(a[i]);
#if _DEBUG
      // The implementation is not accurate, but let's check that it's not
      // broken.
      auto const absolute_error = Abs(v[i] - std::sin(a[i]));
      CHECK_LT(absolute_error, 1.2e-16);
#endif
    }
    benchmark::DoNotOptimize(v);
  }
}

void BM_ExperimentCosMultiTable(benchmark::State& state) {
  std::mt19937_64 random(42);
  std::uniform_real_distribution<> uniformly_at(
      MultiTableImplementation::cutoffs
          [MultiTableImplementation::number_of_tables - 1],
      x_max);

  MultiTableImplementation implementation;
  implementation.Initialize();

  Argument a[number_of_iterations];
  for (std::int64_t i = 0; i < number_of_iterations; ++i) {
    a[i] = uniformly_at(random);
  }

  Value v[number_of_iterations];
  while (state.KeepRunningBatch(number_of_iterations)) {
    for (std::int64_t i = 0; i < number_of_iterations; ++i) {
      v[i] = implementation.Cos(a[i]);
#if _DEBUG
      // The implementation is not accurate, but let's check that it's not
      // broken.
      auto const absolute_error = Abs(v[i] - std::cos(a[i]));
      CHECK_LT(absolute_error, 1.2e-16);
#endif
    }
    benchmark::DoNotOptimize(v);
  }
}

void BM_ExperimentSinSingleTable(benchmark::State& state) {
  std::mt19937_64 random(42);
  std::uniform_real_distribution<> uniformly_at(
      SingleTableImplementation::min_argument,
      x_max);

  SingleTableImplementation implementation;
  implementation.Initialize();

  Argument a[number_of_iterations];
  for (std::int64_t i = 0; i < number_of_iterations; ++i) {
    a[i] = uniformly_at(random);
  }

  Value v[number_of_iterations];
  while (state.KeepRunningBatch(number_of_iterations)) {
    for (std::int64_t i = 0; i < number_of_iterations; ++i) {
      v[i] = implementation.Sin(a[i]);
#if _DEBUG
      // The implementation is not accurate, but let's check that it's not
      // broken.
      auto const absolute_error = Abs(v[i] - std::sin(a[i]));
      CHECK_LT(absolute_error, 1.2e-16);
#endif
    }
    benchmark::DoNotOptimize(v);
  }
}

void BM_ExperimentCosSingleTable(benchmark::State& state) {
  std::mt19937_64 random(42);
  std::uniform_real_distribution<> uniformly_at(
      SingleTableImplementation::min_argument,
      x_max);

  SingleTableImplementation implementation;
  implementation.Initialize();

  Argument a[number_of_iterations];
  for (std::int64_t i = 0; i < number_of_iterations; ++i) {
    a[i] = uniformly_at(random);
  }

  Value v[number_of_iterations];
  while (state.KeepRunningBatch(number_of_iterations)) {
    for (std::int64_t i = 0; i < number_of_iterations; ++i) {
      v[i] = implementation.Cos(a[i]);
#if _DEBUG
      // The implementation is not accurate, but let's check that it's not
      // broken.
      auto const absolute_error = Abs(v[i] - std::cos(a[i]));
      CHECK_LT(absolute_error, 1.2e-16);
#endif
    }
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK_TEMPLATE(BM_ExperimentSinTableSpacing, 2.0 / 256.0)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_ExperimentSinTableSpacing, 2.0 / 1024.0)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_ExperimentCosTableSpacing, 2.0 / 256.0)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_TEMPLATE(BM_ExperimentCosTableSpacing, 2.0 / 1024.0)
    ->Unit(benchmark::kNanosecond);
BENCHMARK(BM_ExperimentSinMultiTable)->Unit(benchmark::kNanosecond);
BENCHMARK(BM_ExperimentCosMultiTable)->Unit(benchmark::kNanosecond);
BENCHMARK(BM_ExperimentSinSingleTable)->Unit(benchmark::kNanosecond);
BENCHMARK(BM_ExperimentCosSingleTable)->Unit(benchmark::kNanosecond);

}  // namespace functions
}  // namespace principia
