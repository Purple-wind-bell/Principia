#include "mathematica/mathematica.hpp"

#include <list>
#include <string>
#include <vector>

#include "astronomy/orbital_elements.hpp"
#include "geometry/frame.hpp"
#include "geometry/grassmann.hpp"
#include "geometry/named_quantities.hpp"
#include "geometry/point.hpp"
#include "geometry/quaternion.hpp"
#include "geometry/r3_element.hpp"
#include "gtest/gtest.h"
#include "numerics/fixed_arrays.hpp"
#include "physics/degrees_of_freedom.hpp"
#include "physics/discrete_trajectory.hpp"
#include "quantities/quantities.hpp"
#include "quantities/si.hpp"

namespace principia {
namespace mathematica {

using astronomy::OrbitalElements;
using geometry::Bivector;
using geometry::Displacement;
using geometry::Frame;
using geometry::Instant;
using geometry::Point;
using geometry::Quaternion;
using geometry::R3Element;
using geometry::Vector;
using geometry::Velocity;
using numerics::FixedVector;
using physics::DegreesOfFreedom;
using physics::DiscreteTrajectory;
using quantities::si::Metre;
using quantities::si::Radian;
using quantities::si::Second;

class MathematicaTest : public ::testing::Test {
 protected:
};

TEST_F(MathematicaTest, ToMathematica) {
  using F = Frame<enum class FTag>;
  {
    EXPECT_EQ(
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision]]",
        ToMathematica(std::vector<double>{2, 3}));
    EXPECT_EQ("List[]", ToMathematica(std::vector<int>{}));
  }
  {
    std::list<int> list;
    list.push_back(3);
    list.push_front(2);
    list.push_back(4);
    auto it = list.cbegin();
    ++it;
    ++it;
    EXPECT_EQ(
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision]]",
        ToMathematica(list.cbegin(), it));
    EXPECT_EQ("List[]", ToMathematica(it, it));
  }
  {
    EXPECT_EQ("SetPrecision[+3.00000000000000000*^+00,$MachinePrecision]",
              ToMathematica(3.0));
    EXPECT_EQ("SetPrecision[-2.00000000000000000*^+00,$MachinePrecision]",
              ToMathematica(-2.0));
    EXPECT_EQ("SetPrecision[-0.00000000000000000*^+00,$MachinePrecision]",
              ToMathematica(-0.0));
  }
  {
    EXPECT_EQ(
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision]]",
        ToMathematica(FixedVector<double, 2>({2, 3})));
    EXPECT_EQ("List[]", ToMathematica(FixedVector<int, 0>()));
  }
  {
    EXPECT_EQ(
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[-4.00000000000000000*^+00,$MachinePrecision]]",
        ToMathematica(R3Element<double>(2.0, 3.0, -4.0)));
  }
  {
    // TODO(phl): Use Quaternion[].
    EXPECT_EQ(
        "List["
        "SetPrecision[+1.00000000000000000*^+00,$MachinePrecision],"
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[-4.00000000000000000*^+00,$MachinePrecision]]]",
        ToMathematica(Quaternion(1.0, R3Element<double>(2.0, 3.0, -4.0))));
  }
  {
    // TODO(phl): Why two SetPrecision[]?
    EXPECT_EQ(
        "SetPrecision["
        "Quantity["
        "SetPrecision[+1.50000000000000000*^+00,$MachinePrecision],"
        "\" m s^-1\"],"
        "$MachinePrecision]",
        ToMathematica(1.5 * Metre / Second));
  }
  {
    EXPECT_EQ(
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[-4.00000000000000000*^+00,$MachinePrecision]]",
        ToMathematica(Vector<double, F>({2.0, 3.0, -4.0})));
  }
  {
    EXPECT_EQ(
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[-4.00000000000000000*^+00,$MachinePrecision]]",
        ToMathematica(Bivector<double, F>({2.0, 3.0, -4.0})));
  }
  {
    EXPECT_EQ(
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[-4.00000000000000000*^+00,$MachinePrecision]]",
        ToMathematica(Point<Vector<double, F>>() +
                      Vector<double, F>({2.0, 3.0, -4.0})));
  }
  {
    // TODO(phl): Why no units?
    EXPECT_EQ(
        "List["
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[-4.00000000000000000*^+00,$MachinePrecision]],"
        "List["
        "SetPrecision[-1.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[-5.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+8.00000000000000000*^+00,$MachinePrecision]]]",
        ToMathematica(DegreesOfFreedom<F>(
            F::origin +
                Displacement<F>({2.0 * Metre, 3.0 * Metre, -4.0 * Metre}),
            Velocity<F>({-1.0 * Metre / Second,
                         -5.0 * Metre / Second,
                         8.0 * Metre / Second}))));
  }
  {
    EXPECT_EQ(
        "List["
        "SetPrecision["
        "Quantity[SetPrecision[+1.00000000000000000*^+00,$MachinePrecision],"
        "\" m\"],$MachinePrecision],"
        "SetPrecision["
        "Quantity[SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "\" s\"],$MachinePrecision],"
        "SetPrecision["
        "Quantity[SetPrecision[+3.00000000000000000*^+00,$MachinePrecision],"
        "\" m s^-1\"],$MachinePrecision]]",
        ToMathematica(std::tuple{1 * Metre, 2 * Second, 3 * Metre / Second}));
  }
  {
    DiscreteTrajectory<F> trajectory;
    trajectory.Append(
        Instant(),
        DegreesOfFreedom<F>(
            F::origin +
                Displacement<F>({2.0 * Metre, 3.0 * Metre, -4.0 * Metre}),
            Velocity<F>({-1.0 * Metre / Second,
                         -5.0 * Metre / Second,
                         8.0 * Metre / Second})));
    EXPECT_EQ(
        "List[SetPrecision[+0.00000000000000000*^+00,$MachinePrecision],"
        "List["
        "List["
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[-4.00000000000000000*^+00,$MachinePrecision]],"
        "List["
        "SetPrecision[-1.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[-5.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+8.00000000000000000*^+00,$MachinePrecision]]]]",
        ToMathematica(*trajectory.begin()));
  }
  {
    OrbitalElements::EquinoctialElements elements{
        Instant(), 1 * Metre, 2, 3, 4 * Radian, 5, 6, 7, 8};
    EXPECT_EQ(
        "List["
        "SetPrecision[+0.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+1.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+2.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+3.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+4.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+5.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+6.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+7.00000000000000000*^+00,$MachinePrecision],"
        "SetPrecision[+8.00000000000000000*^+00,$MachinePrecision]]"
    , ToMathematica(elements));
  }
  {
    EXPECT_EQ("foo\"bar", ToMathematica("foo\"bar"));
  }
}

}  // namespace mathematica
}  // namespace principia