// .\Release\x64\benchmarks.exe --benchmark_repetitions=3 --benchmark_filter=LatticesBenchmark  // NOLINT(whitespace/line_length)

#include <random>

#include "benchmark/benchmark.h"
#include "boost/multiprecision/cpp_int.hpp"
#include "numerics/fixed_arrays.hpp"
#include "numerics/lattices.hpp"

namespace principia {
namespace numerics {

using namespace boost::multiprecision;
using namespace principia::numerics::_fixed_arrays;
using namespace principia::numerics::_lattices;

constexpr std::int64_t number_of_lattices = 1000;

template<typename Element, int max_element>
class LatticesBenchmark : public benchmark::Fixture {
 protected:
  using Lattice = FixedMatrix<Element, 5, 4>;

  void SetUp(benchmark::State& state) {
    std::mt19937_64 random(42);
    std::uniform_int_distribution<> uniformly_at(-max_element, max_element);

    for (std::int64_t l = 0; l < number_of_lattices; ++l) {
      auto& lattice = lattices_[l];
      for (std::int64_t i = 0; i < lattice.rows(); ++i) {
        for (std::int64_t j = 0; j < lattice.rows(); ++j) {
          lattice(i, j) = uniformly_at(random);
        }
      }
    }
  }

  void RunLenstraLenstraLovász(benchmark::State& state) {
    while (state.KeepRunningBatch(number_of_lattices)) {
      for (auto const& lattice : lattices_) {
        benchmark::DoNotOptimize(LenstraLenstraLovász(lattice));
      }
    }
  }

  std::array<Lattice, number_of_lattices> lattices_;
};

BENCHMARK_TEMPLATE_F(LatticesBenchmark,
                     LenstraLenstraLovászDouble6,
                     double, 1'000'000)(benchmark::State& state) {
  RunLenstraLenstraLovász(state);
}

BENCHMARK_TEMPLATE_F(LatticesBenchmark,
                     LenstraLenstraLovászDouble9,
                     double, 1'000'000'000)(benchmark::State& state) {
  RunLenstraLenstraLovász(state);
}

BENCHMARK_TEMPLATE_F(LatticesBenchmark,
                     LenstraLenstraLovászCppRational6,
                     cpp_rational, 1'000'000)(benchmark::State& state) {
  RunLenstraLenstraLovász(state);
}

BENCHMARK_TEMPLATE_F(LatticesBenchmark,
                     LenstraLenstraLovászCppRational9,
                     cpp_rational, 1'000'000'000)(benchmark::State& state) {
  RunLenstraLenstraLovász(state);
}

}  // namespace numerics
}  // namespace principia
