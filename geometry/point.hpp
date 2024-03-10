#pragma once

#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "base/not_null.hpp"
#include "base/concepts.hpp"
#include "geometry/barycentre_calculator.hpp"  // 🧙 For friendship.
#include "quantities/named_quantities.hpp"
#include "quantities/traits.hpp"
#include "serialization/geometry.pb.h"

namespace principia {
namespace geometry {
namespace _point {
namespace internal {

using namespace principia::base::_not_null;
using namespace principia::base::_concepts;
using namespace principia::quantities::_named_quantities;
using namespace principia::quantities::_traits;

// Point<Vector> is an affine space on the vector space Vector. Vector should
// be equipped with operators +, -, +=, -=, ==, !=, as well as Vector * Weight
// and Vector / Weight for any Weight used in Barycentre.
template<typename Vector>
class Point final {
 public:
  constexpr Point();

#if PRINCIPIA_COMPILER_MSVC && !__INTELLISENSE__
  // Explicitly define constexpr default copy and move constructors because
  // otherwise MSVC fails to initialize constant expressions.  In addition,
  // Intellisense gets confused by these (because of course MSVC and
  // Intellisense are different compilers and have bugs in different places).
  constexpr Point(Point const& other);
  constexpr Point(Point&& other);
  Point& operator=(Point const& other) = default;
  Point& operator=(Point&& other) = default;
#endif

  constexpr Vector operator-(Point const& from) const;

  constexpr Point operator+(Vector const& translation) const;
  constexpr Point operator-(Vector const& translation) const;

  Point& operator+=(Vector const& translation);
  Point& operator-=(Vector const& translation);

  constexpr bool operator==(Point const& right) const;
  constexpr bool operator!=(Point const& right) const;

  void WriteToMessage(not_null<serialization::Point*> message) const;
  static Point ReadFromMessage(serialization::Point const& message)
    requires serializable<Vector>;

 private:
  // This constructor allows for C++11 functional constexpr operators, and
  // possibly move-magic.
  // TODO(egg): We may want to reconsider this after we truly have C++14.
  constexpr explicit Point(Vector const& coordinates);

  Vector coordinates_;

  template<typename V>
  friend constexpr Point<V> operator+(V const& translation,
                                      Point<V> const& point);
  template<typename L, typename R>
  friend Point<Product<L, R>> FusedMultiplyAdd(L const& a, R const& b,
                                               Point<Product<L, R>> const& c);
  template<typename L, typename R>
  friend Point<Product<L, R>> FusedNegatedMultiplyAdd(
      L const& a, R const& b, Point<Product<L, R>> const& c);

  template<typename V>
  friend constexpr typename std::enable_if_t<is_quantity_v<V>, bool>
  operator<(Point<V> const& left, Point<V> const& right);
  template<typename V>
  friend constexpr typename std::enable_if_t<is_quantity_v<V>, bool>
  operator<=(Point<V> const& left, Point<V> const& right);
  template<typename V>
  friend constexpr typename std::enable_if_t<is_quantity_v<V>, bool>
  operator>=(Point<V> const& left, Point<V> const& right);
  template<typename V>
  friend constexpr typename std::enable_if_t<is_quantity_v<V>, bool>
  operator>(Point<V> const& left, Point<V> const& right);

  template<typename V>
  friend constexpr typename std::enable_if_t<is_quantity_v<V>, Point<V>>
  NextUp(Point<V> x);
  template<typename V>
  friend constexpr typename std::enable_if_t<is_quantity_v<V>, Point<V>>
  NextDown(Point<V> x);

  template<typename V>
  friend std::string DebugString(Point<V> const& point);

  template<typename V, typename S>
  friend class _barycentre_calculator::BarycentreCalculator;
};

template<typename Vector>
constexpr Point<Vector> operator+(Vector const& translation,
                                  Point<Vector> const& point);

template<typename L, typename R>
Point<Product<L, R>> FusedMultiplyAdd(L const& a,
                                      R const& b,
                                      Point<Product<L, R>> const& c);
template<typename L, typename R>
Point<Product<L, R>> FusedNegatedMultiplyAdd(L const& a,
                                             R const& b,
                                             Point<Product<L, R>> const& c);

template<typename Vector>
constexpr typename std::enable_if_t<is_quantity_v<Vector>, bool>
operator<(Point<Vector> const& left, Point<Vector> const& right);

template<typename Vector>
constexpr typename std::enable_if_t<is_quantity_v<Vector>, bool>
operator<=(Point<Vector> const& left, Point<Vector> const& right);

template<typename Vector>
constexpr typename std::enable_if_t<is_quantity_v<Vector>, bool>
operator>=(Point<Vector> const& left, Point<Vector> const& right);

template<typename Vector>
constexpr typename std::enable_if_t<is_quantity_v<Vector>, bool>
operator>(Point<Vector> const& left, Point<Vector> const& right);

template<typename Vector>
constexpr typename std::enable_if_t<is_quantity_v<Vector>, Point<Vector>>
NextUp(Point<Vector> x);
template<typename Vector>
constexpr typename std::enable_if_t<is_quantity_v<Vector>, Point<Vector>>
NextDown(Point<Vector> x);

template<typename Vector>
std::string DebugString(Point<Vector> const& point);

template<typename Vector>
std::ostream& operator<<(std::ostream& out, Point<Vector> const& point);

}  // namespace internal

using internal::Point;

}  // namespace _point

// Specialize BarycentreCalculator to make it applicable to Points.
namespace _barycentre_calculator {
namespace internal {

using namespace principia::geometry::_point;
using namespace principia::quantities::_named_quantities;
using namespace principia::quantities::_traits;

template<typename Vector, typename Weight>
class BarycentreCalculator<Point<Vector>, Weight> final {
 public:
  BarycentreCalculator() = default;

  void Add(Point<Vector> const& point, Weight const& weight);
  Point<Vector> Get() const;

  Weight const& weight() const;

 private:
  bool empty_ = true;
  Product<Vector, Weight> weighted_sum_;
  Weight weight_;
};

}  // namespace internal
}  // namespace _barycentre_calculator
}  // namespace geometry
}  // namespace principia

#include "geometry/point_body.hpp"
