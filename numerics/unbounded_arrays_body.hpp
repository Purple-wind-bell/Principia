#pragma once

#include "numerics/unbounded_arrays.hpp"

#include <cmath>
#include <utility>
#include <vector>

#include "quantities/elementary_functions.hpp"

namespace principia {
namespace numerics {
namespace _unbounded_arrays {
namespace internal {

using namespace principia::quantities::_elementary_functions;

template<class T>
template<class U, class... Args>
void uninitialized_allocator<T>::construct(U* const p, Args&&... args) {
  ::new ((void*)p) U(std::forward<Args>(args)...);  // NOLINT
}

template<typename Scalar_>
UnboundedVector<Scalar_>::UnboundedVector(int const size)
    : data_(size, Scalar{}) {}

template<typename Scalar_>
UnboundedVector<Scalar_>::UnboundedVector(int const size, uninitialized_t)
    : data_(size) {}

template<typename Scalar_>
UnboundedVector<Scalar_>::UnboundedVector(std::initializer_list<Scalar> data)
    : data_(std::move(data)) {}

template<typename Scalar_>
template<int size_>
UnboundedVector<Scalar_>::UnboundedVector(
    FixedVector<Scalar, size_> const& data)
    : data_(data.begin(), data.end()) {}

template<typename Scalar_>
void UnboundedVector<Scalar_>::Extend(int const extra_size) {
  DCHECK_LE(0, extra_size);
  data_.resize(data_.size() + extra_size, Scalar{});
}

template<typename Scalar_>
void UnboundedVector<Scalar_>::Extend(int const extra_size, uninitialized_t) {
  DCHECK_LE(0, extra_size);
  data_.resize(data_.size() + extra_size);
}

template<typename Scalar_>
void UnboundedVector<Scalar_>::Extend(std::initializer_list<Scalar> data) {
  std::move(data.begin(), data.end(), std::back_inserter(data_));
}

template<typename Scalar_>
void UnboundedVector<Scalar_>::EraseToEnd(int const begin_index) {
  data_.erase(data_.begin() + begin_index, data_.end());
}

template<typename Scalar_>
Scalar_ UnboundedVector<Scalar_>::Norm() const {
  return Sqrt(Norm²());
}

template<typename Scalar_>
Square<Scalar_> UnboundedVector<Scalar_>::Norm²() const {
  Square<Scalar> norm²{};
  for (auto const c : data_) {
    norm² += c * c;
  }
  return norm²;
}

template<typename Scalar_>
int UnboundedVector<Scalar_>::size() const {
  return data_.size();
}

template<typename Scalar_>
typename std::vector<Scalar_>::const_iterator UnboundedVector<Scalar_>::begin()
    const {
  return data_.cbegin();
}

template<typename Scalar_>
typename std::vector<Scalar_>::const_iterator UnboundedVector<Scalar_>::end()
    const {
  return data_.cend();
}

template<typename Scalar_>
Scalar_& UnboundedVector<Scalar_>::operator[](int const index) {
  DCHECK_LE(0, index);
  DCHECK_LT(index, size());
  return data_[index];
}

template<typename Scalar_>
Scalar_ const& UnboundedVector<Scalar_>::operator[](int const index) const {
  DCHECK_LE(0, index);
  DCHECK_LT(index, size());
  return data_[index];
}

template<typename Scalar_>
bool UnboundedVector<Scalar_>::operator==(UnboundedVector const& right) const {
  return data_ == right.data_;
}

template<typename Scalar_>
bool UnboundedVector<Scalar_>::operator!=(UnboundedVector const& right) const {
  return data_ != right.data_;
}

template<typename Scalar_>
UnboundedMatrix<Scalar_>::UnboundedMatrix(int const rows, int const columns)
    : rows_(rows),
      columns_(columns),
      data_(rows_ * columns_, Scalar{}) {}

template<typename Scalar_>
UnboundedMatrix<Scalar_>::UnboundedMatrix(int const rows, int const columns,
                                          uninitialized_t)
    : rows_(rows),
      columns_(columns),
      data_(rows_ * columns_) {}


template<typename Scalar_>
UnboundedMatrix<Scalar_>::UnboundedMatrix(std::initializer_list<Scalar_> data)
    : rows_(Sqrt(data.size())),
      columns_(Sqrt(data.size())),
      data_(std::move(data)) {
  CHECK_EQ(data.size(), rows_ * columns_);
}

template<typename Scalar_>
UnboundedMatrix<Scalar_>::UnboundedMatrix(int const rows, int const columns,
                                          std::initializer_list<Scalar> data)
    : rows_(rows),
      columns_(columns),
      data_(std::move(data)) {
  CHECK_EQ(data.size(), rows_ * columns_);
}

template<typename Scalar_>
UnboundedMatrix<Scalar_>::UnboundedMatrix(
    TransposedView<UnboundedMatrix<Scalar>> const& view)
    : UnboundedMatrix(view.rows(), view.columns(), uninitialized) {
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < columns_; ++j) {
      (*this)(i, j) = view(i, j);
    }
  }
}

template<typename Scalar_>
int UnboundedMatrix<Scalar_>::rows() const {
  return rows_;
}

template<typename Scalar_>
int UnboundedMatrix<Scalar_>::columns() const {
  return columns_;
}

template<typename Scalar_>
int UnboundedMatrix<Scalar_>::size() const {
  return rows_ * columns_;
}

template<typename Scalar_>
Scalar_ UnboundedMatrix<Scalar_>::FrobeniusNorm() const {
  Square<Scalar> Σᵢⱼaᵢⱼ²{};
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < columns_; ++j) {
      Σᵢⱼaᵢⱼ² += Pow<2>((*this)(i, j));
    }
  }
  return Sqrt(Σᵢⱼaᵢⱼ²);
}

template<typename Scalar_>
Scalar_& UnboundedMatrix<Scalar_>::operator()(
    int const row, int const column) {
  DCHECK_LE(0, row);
  DCHECK_LT(row, rows_);
  DCHECK_LE(0, column);
  DCHECK_LT(column, columns_);
  return data_[row * columns_ + column];
}

template<typename Scalar_>
Scalar_ const& UnboundedMatrix<Scalar_>::operator()(
    int const row, int const column) const {
  DCHECK_LE(0, row);
  DCHECK_LT(row, rows_);
  DCHECK_LE(0, column);
  DCHECK_LT(column, columns_);
  return data_[row * columns_ + column];
}

template<typename Scalar_>
bool UnboundedMatrix<Scalar_>::operator==(UnboundedMatrix const& right) const {
  return data_ == right.data_;
}

template<typename Scalar_>
bool UnboundedMatrix<Scalar_>::operator!=(UnboundedMatrix const& right) const {
  return !(*this == right);
}

template<typename Scalar_>
template<typename LScalar, typename RScalar>
Product<Scalar_, Product<LScalar, RScalar>>
UnboundedMatrix<Scalar_>::operator()(
    UnboundedVector<LScalar> const& left,
    UnboundedVector<RScalar> const& right) const {
  return TransposedView{left} * (*this * right);  // NOLINT
}

template<typename Scalar_>
UnboundedMatrix<Scalar_>
UnboundedMatrix<Scalar_>::Identity(int const rows, int const columns) {
  UnboundedMatrix<Scalar> m(rows, columns);
  for (int i = 0; i < rows; ++i) {
    m(i, i) = 1;
  }
  return m;
}

template<typename Scalar_>
UnboundedLowerTriangularMatrix<Scalar_>::UnboundedLowerTriangularMatrix(
    int const rows)
    : rows_(rows),
      data_(rows_ * (rows_ + 1) / 2, Scalar{}) {}

template<typename Scalar_>
UnboundedLowerTriangularMatrix<Scalar_>::UnboundedLowerTriangularMatrix(
    int const rows,
    uninitialized_t)
    : rows_(rows),
      data_(rows_ * (rows_ + 1) / 2) {}

template<typename Scalar_>
UnboundedLowerTriangularMatrix<Scalar_>::UnboundedLowerTriangularMatrix(
    std::initializer_list<Scalar> data)
    : rows_(static_cast<int>(std::lround((-1 + Sqrt(8 * data.size())) * 0.5))),
      data_(std::move(data)) {
  DCHECK_EQ(data_.size(), rows_ * (rows_ + 1) / 2);
}

template<typename Scalar_>
UnboundedLowerTriangularMatrix<Scalar_>::UnboundedLowerTriangularMatrix(
    TransposedView<UnboundedUpperTriangularMatrix<Scalar>> const& view)
    : UnboundedLowerTriangularMatrix(view.rows(), uninitialized) {
  for (int i = 0; i < rows(); ++i) {
    for (int j = 0; j <= i; ++j) {
      (*this)(i, j) = view(i, j);
    }
  }
}

template<typename Scalar_>
void UnboundedLowerTriangularMatrix<Scalar_>::Extend(int const extra_rows) {
  rows_ += extra_rows;
  data_.resize(rows_ * (rows_ + 1) / 2, Scalar{});
}

template<typename Scalar_>
void UnboundedLowerTriangularMatrix<Scalar_>::Extend(int const extra_rows,
                                                     uninitialized_t) {
  rows_ += extra_rows;
  data_.resize(rows_ * (rows_ + 1) / 2);
}

template<typename Scalar_>
void UnboundedLowerTriangularMatrix<Scalar_>::Extend(
    std::initializer_list<Scalar> data) {
  std::move(data.begin(), data.end(), std::back_inserter(data_));
  rows_ = static_cast<int>(std::lround((-1 + Sqrt(8 * data_.size())) * 0.5));
  DCHECK_EQ(data_.size(), rows_ * (rows_ + 1) / 2);
}

template<typename Scalar_>
void UnboundedLowerTriangularMatrix<Scalar_>::EraseToEnd(
    int const begin_row_index) {
  rows_ = begin_row_index;
  data_.erase(data_.begin() + begin_row_index * (begin_row_index + 1) / 2,
              data_.end());
}

template<typename Scalar_>
int UnboundedLowerTriangularMatrix<Scalar_>::rows() const {
  return rows_;
}

template<typename Scalar_>
int UnboundedLowerTriangularMatrix<Scalar_>::columns() const {
  return rows_;
}

template<typename Scalar_>
int UnboundedLowerTriangularMatrix<Scalar_>::size() const {
  return data_.size();
}

template<typename Scalar_>
Scalar_& UnboundedLowerTriangularMatrix<Scalar_>::operator()(
    int const row, int const column) {
  DCHECK_LE(0, column);
  DCHECK_LE(column, row);
  DCHECK_LT(row, rows_);
  return data_[row * (row + 1) / 2 + column];
}

template<typename Scalar_>
Scalar_ const& UnboundedLowerTriangularMatrix<Scalar_>::operator()(
    int const row, int const column) const {
  DCHECK_LE(0, column);
  DCHECK_LE(column, row);
  DCHECK_LT(row, rows_);
  return data_[row * (row + 1) / 2 + column];
}

template<typename Scalar_>
bool UnboundedLowerTriangularMatrix<Scalar_>::operator==(
    UnboundedLowerTriangularMatrix const& right) const {
  return rows_ == right.rows_ && data_ == right.data_;
}

template<typename Scalar_>
bool UnboundedLowerTriangularMatrix<Scalar_>::operator!=(
    UnboundedLowerTriangularMatrix const& right) const {
  return !(*this == right);
}

template<typename Scalar_>
UnboundedUpperTriangularMatrix<Scalar_>::UnboundedUpperTriangularMatrix(
    int const columns)
    : columns_(columns),
      data_(columns_ * (columns_ + 1) / 2, Scalar{}) {}

template<typename Scalar_>
UnboundedUpperTriangularMatrix<Scalar_>::UnboundedUpperTriangularMatrix(
    int const columns,
    uninitialized_t)
    : columns_(columns),
      data_(columns_ * (columns_ + 1) / 2) {}

template<typename Scalar_>
UnboundedUpperTriangularMatrix<Scalar_>::UnboundedUpperTriangularMatrix(
    std::initializer_list<Scalar> const& data)
    : columns_(
          static_cast<int>(std::lround((-1 + Sqrt(8 * data.size())) * 0.5))),
      data_(Transpose(data,
                      /*current_columns=*/0,
                      /*extra_columns=*/columns_)) {
  DCHECK_EQ(data_.size(), columns_ * (columns_ + 1) / 2);
}

template<typename Scalar_>
UnboundedUpperTriangularMatrix<Scalar_>::UnboundedUpperTriangularMatrix(
    TransposedView<UnboundedLowerTriangularMatrix<Scalar>> const& view)
    : UnboundedUpperTriangularMatrix<Scalar>(view.columns(), uninitialized) {
  for (int i = 0; i < rows(); ++i) {
    for (int j = i; j < columns(); ++j) {
      (*this)(i, j) = view(i, j);
    }
  }
}

template<typename Scalar_>
void UnboundedUpperTriangularMatrix<Scalar_>::Extend(int const extra_columns) {
  columns_ += extra_columns;
  data_.resize(columns_ * (columns_ + 1) / 2, Scalar{});
}

template<typename Scalar_>
void UnboundedUpperTriangularMatrix<Scalar_>::Extend(int const extra_columns,
                                                     uninitialized_t) {
  columns_ += extra_columns;
  data_.resize(columns_ * (columns_ + 1) / 2);
}

template<typename Scalar_>
void UnboundedUpperTriangularMatrix<Scalar_>::Extend(
    std::initializer_list<Scalar> const& data) {
  int const new_columns = static_cast<int>(
      std::lround((-1 + Sqrt(8 * (data_.size() + data.size()))) * 0.5));
  auto transposed_data = Transpose(data,
                                   /*current_columns=*/columns_,
                                   /*extra_columns=*/new_columns - columns_);
  columns_ = new_columns;
  std::move(transposed_data.begin(),
            transposed_data.end(),
            std::back_inserter(data_));
  DCHECK_EQ(data_.size(), columns_ * (columns_ + 1) / 2);
}

template<typename Scalar_>
void UnboundedUpperTriangularMatrix<Scalar_>::EraseToEnd(
    int const begin_column_index) {
  columns_ = begin_column_index;
  data_.erase(data_.begin() + begin_column_index * (begin_column_index + 1) / 2,
              data_.end());
}

template<typename Scalar_>
int UnboundedUpperTriangularMatrix<Scalar_>::rows() const {
  return columns_;
}

template<typename Scalar_>
int UnboundedUpperTriangularMatrix<Scalar_>::columns() const {
  return columns_;
}

template<typename Scalar_>
int UnboundedUpperTriangularMatrix<Scalar_>::size() const {
  return data_.size();
}

template<typename Scalar_>
Scalar_& UnboundedUpperTriangularMatrix<Scalar_>::operator()(
    int const row, int const column) {
  DCHECK_LE(0, row);
  DCHECK_LE(row, column);
  DCHECK_LT(column, columns_);
  return data_[column * (column + 1) / 2 + row];
}

template<typename Scalar_>
Scalar_ const& UnboundedUpperTriangularMatrix<Scalar_>::operator()(
    int const row, int const column) const {
  DCHECK_LE(0, row);
  DCHECK_LE(row, column);
  DCHECK_LT(column, columns_);
  return data_[column * (column + 1) / 2 + row];
}

template<typename Scalar_>
bool UnboundedUpperTriangularMatrix<Scalar_>::operator==(
    UnboundedUpperTriangularMatrix const& right) const {
  return columns_ == right.columns_ && data_ == right.data_;
}

template<typename Scalar_>
bool UnboundedUpperTriangularMatrix<Scalar_>::operator!=(
    UnboundedUpperTriangularMatrix const& right) const {
  return !(*this == right);
}

template<typename Scalar_>
auto
UnboundedUpperTriangularMatrix<Scalar_>::Transpose(
    std::initializer_list<Scalar> const& data,
    int const current_columns,
    int const extra_columns) ->
  std::vector<Scalar, uninitialized_allocator<Scalar>> {
  // |data| is a trapezoidal slice at the end of the matrix.  This is
  // inconvenient to index, so we start by constructing a rectangular array with
  // |extra_columns| columns and |current_columns + extra_columns| rows padded
  // with junk.
  std::vector<Scalar, uninitialized_allocator<Scalar>> padded;
  {
    padded.reserve(2 * data.size());  // An overestimate.
    int row = 0;
    int column = 0;
    for (auto it = data.begin(); it != data.end();) {
      if (row <= current_columns + column) {
        padded.push_back(*it);
        ++it;
      } else {
        padded.emplace_back();
      }
      ++column;
      if (column == extra_columns) {
        column = 0;
        ++row;
      }
    }
  }

  // Scan the padded array by column and append the part above the diagonal to
  // the result.
  std::vector<Scalar, uninitialized_allocator<Scalar>> result;
  result.reserve(data.size());
  int const number_of_rows = current_columns + extra_columns;
  for (int column = 0; column < extra_columns; ++column) {
    for (int row = 0; row < number_of_rows; ++row) {
      if (row <= current_columns + column) {
        result.push_back(padded[row * extra_columns + column]);
      }
    }
  }

  return result;
}

template<typename LScalar, typename RScalar>
Product<LScalar, RScalar> InnerProduct(UnboundedVector<LScalar> const& left,
                                       UnboundedVector<RScalar> const& right) {
  return TransposedView{left} * right;  // NOLINT
}

template<typename Scalar>
UnboundedVector<double> Normalize(UnboundedVector<Scalar> const& vector) {
  return vector / vector.Norm();
}

template<typename LScalar, typename RScalar>
UnboundedMatrix<Product<LScalar, RScalar>> SymmetricProduct(
    UnboundedVector<LScalar> const& left,
    UnboundedVector<RScalar> const& right) {
  CHECK_EQ(left.size(), right.size());
  UnboundedMatrix<Product<LScalar, RScalar>> result(
      left.size(), right.size(), uninitialized);
  for (int i = 0; i < left.size(); ++i) {
    for (int j = 0; j < i; ++j) {
      auto const r = 0.5 * (left[i] * right[j] + left[j] * right[i]);
      result(i, j) = r;
      result(j, i) = r;
    }
    result(i, i) = left[i] * right[i];
  }
  return result;
}

template<typename Scalar>
UnboundedMatrix<Square<Scalar>> SymmetricSquare(
    UnboundedVector<Scalar> const& vector) {
  UnboundedMatrix<Square<Scalar>> result(
      vector.size(), vector.size(), uninitialized);
  for (int i = 0; i < vector.size(); ++i) {
    for (int j = 0; j < i; ++j) {
      auto const r = vector[i] * vector[j];
      result(i, j) = r;
      result(j, i) = r;
    }
    result(i, i) = Pow<2>(vector[i]);
  }
  return result;
}

template<typename Scalar>
UnboundedVector<Scalar> operator-(UnboundedVector<Scalar> const& right) {
  std::vector<Scalar> result;
  result.reserve(right.size());
  for (int i = 0; i < right.size(); ++i) {
    result[i] = -right[i];
  }
  return UnboundedVector<Scalar>(std::move(result));
}

template<typename Scalar>
UnboundedMatrix<Scalar> operator-(UnboundedMatrix<Scalar> const& right) {
  UnboundedMatrix<Scalar> result(right.rows(), right.columns(), uninitialized);
  for (int i = 0; i < right.rows(); ++i) {
    for (int j = 0; j < right.columns(); ++j) {
      result(i, j) = -right(i, j);
    }
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedVector<Sum<LScalar, RScalar>> operator+(
    UnboundedVector<LScalar> const& left,
    UnboundedVector<RScalar> const& right) {
  CHECK_EQ(left.size(), right.size());
  std::vector<Sum<LScalar, RScalar>> result;
  result.resize(right.size());
  for (int i = 0; i < right.size(); ++i) {
    result[i] = left[i] + right[i];
  }
  return UnboundedVector<Sum<LScalar, RScalar>>(std::move(result));
}

template<typename LScalar, typename RScalar>
UnboundedMatrix<Sum<LScalar, RScalar>> operator+(
    UnboundedMatrix<LScalar> const& left,
    UnboundedMatrix<RScalar> const& right) {
  CHECK_EQ(left.rows(), right.rows());
  CHECK_EQ(left.columns(), right.columns());
  UnboundedMatrix<Sum<LScalar, RScalar>> result(
      right.rows(), right.columns(), uninitialized);
  for (int i = 0; i < right.rows(); ++i) {
    for (int j = 0; j < right.columns(); ++j) {
      result(i, j) = left(i, j) + right(i, j);
    }
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedVector<Difference<LScalar, RScalar>> operator-(
    UnboundedVector<LScalar> const& left,
    UnboundedVector<RScalar> const& right) {
  CHECK_EQ(left.size(), right.size());
  std::vector<Sum<LScalar, RScalar>> result;
  result.resize(right.size());
  for (int i = 0; i < right.size(); ++i) {
    result[i] = left[i] - right[i];
  }
  return UnboundedVector<Difference<LScalar, RScalar>>(std::move(result));
}

template<typename LScalar, typename RScalar>
UnboundedMatrix<Difference<LScalar, RScalar>> operator-(
    UnboundedMatrix<LScalar> const& left,
    UnboundedMatrix<RScalar> const& right) {
  CHECK_EQ(left.rows(), right.rows());
  CHECK_EQ(left.columns(), right.columns());
  UnboundedMatrix<Sum<LScalar, RScalar>> result(
      right.rows(), right.columns(), uninitialized);
  for (int i = 0; i < right.rows(); ++i) {
    for (int j = 0; j < right.columns(); ++j) {
      result(i, j) = left(i, j) + right(i, j);
    }
  }
  return result;
}

template<typename Scalar>
UnboundedVector<Scalar>& operator+=(
    UnboundedVector<Scalar>& left,
    UnboundedVector<Scalar> const& right) {
  return left = left + right;
}

template<typename Scalar>
UnboundedMatrix<Scalar>& operator+=(
    UnboundedMatrix<Scalar>& left,
    UnboundedMatrix<Scalar> const& right) {
  return left = left + right;
}

template<typename Scalar>
UnboundedVector<Scalar>& operator-=(
    UnboundedVector<Scalar>& left,
    UnboundedVector<Scalar> const& right) {
  return left = left - right;
}

template<typename Scalar>
UnboundedMatrix<Scalar>& operator-=(
    UnboundedMatrix<Scalar>& left,
    UnboundedMatrix<Scalar> const& right) {
  return left = left - right;
}

template<typename LScalar, typename RScalar>
UnboundedVector<Product<LScalar, RScalar>> operator*(
    LScalar const& left,
    UnboundedVector<RScalar> const& right) {
  UnboundedVector<Product<LScalar, RScalar>> result(right.size(),
                                                    uninitialized);
  for (int i = 0; i < right.size(); ++i) {
    result[i] = left * right[i];
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedVector<Product<LScalar, RScalar>> operator*(
    UnboundedVector<LScalar> const& left,
    RScalar const& right) {
  UnboundedVector<Product<LScalar, RScalar>> result(left.size(),
                                                    uninitialized);
  for (int i = 0; i < left.size(); ++i) {
    result[i] = left[i] * right;
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedMatrix<Product<LScalar, RScalar>> operator*(
    LScalar const& left,
    UnboundedMatrix<RScalar> const& right) {
  UnboundedMatrix<Product<LScalar, RScalar>> result(right.rows(),
                                                    right.columns(),
                                                    uninitialized);
  for (int i = 0; i < right.rows(); ++i) {
    for (int j = 0; j < right.columns(); ++j) {
      result(i, j) = left * right(i, j);
    }
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedMatrix<Product<LScalar, RScalar>> operator*(
    UnboundedMatrix<LScalar> const& left,
    RScalar const& right) {
  UnboundedMatrix<Product<LScalar, RScalar>> result(left.rows(),
                                                    left.columns(),
                                                    uninitialized);
  for (int i = 0; i < left.rows(); ++i) {
    for (int j = 0; j < left.columns(); ++j) {
      result(i, j) = left(i, j) * right;
    }
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedVector<Quotient<LScalar, RScalar>> operator/(
    UnboundedVector<LScalar> const& left,
    RScalar const& right) {
  UnboundedVector<Quotient<LScalar, RScalar>> result(left.size(),
                                                     uninitialized);
  for (int i = 0; i < left.size(); ++i) {
    result[i] = left[i] / right;
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedMatrix<Quotient<LScalar, RScalar>> operator/(
    UnboundedMatrix<LScalar> const& left,
    RScalar const right) {
  UnboundedMatrix<Quotient<LScalar, RScalar>> result(left.rows(),
                                                     left.columns(),
                                                     uninitialized);
  for (int i = 0; i < left.rows(); ++i) {
    for (int j = 0; j < left.columns(); ++j) {
      result(i, j) = left(i, j) / right;
    }
  }
  return result;
}

template<typename Scalar>
UnboundedVector<Scalar>& operator*=(UnboundedVector<Scalar>& left,
                                    double const right) {
  return left = left * right;
}

template<typename Scalar>
UnboundedMatrix<Scalar>& operator*=(UnboundedMatrix<Scalar>& left,
                                    double const right) {
  return left = left * right;
}

template<typename Scalar>
UnboundedVector<Scalar>& operator/=(UnboundedVector<Scalar>& left,
                                    double const right) {
  return left = left / right;
}

template<typename Scalar>
UnboundedMatrix<Scalar>& operator/=(UnboundedMatrix<Scalar>& left,
                                    double const right) {
  return left = left / right;
}

template<typename LScalar, typename RScalar>
Product<LScalar, RScalar> operator*(
    TransposedView<UnboundedVector<LScalar>> const& left,
    UnboundedVector<RScalar> const& right) {
  CHECK_EQ(left.size(), right.size());
  Product<LScalar, RScalar> result{};
  for (int i = 0; i < left.size(); ++i) {
    result += left[i] * right[i];
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedMatrix<Product<LScalar, RScalar>> operator*(
    UnboundedVector<LScalar> const& left,
    TransposedView<UnboundedVector<RScalar>> const& right) {
  UnboundedMatrix<Product<LScalar, RScalar>> result(left.size(),
                                                    right.size(),
                                                    uninitialized);
  for (int i = 0; i < result.rows(); ++i) {
    for (int j = 0; j < result.columns(); ++j) {
      result(i, j) = left[i] * right[j];
    }
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedMatrix<Product<LScalar, RScalar>> operator*(
    UnboundedMatrix<LScalar> const& left,
    UnboundedMatrix<RScalar> const& right) {
  CHECK_EQ(left.columns(), right.rows());
  UnboundedMatrix<Product<LScalar, RScalar>> result(left.rows(),
                                                           right.columns());
  for (int i = 0; i < left.rows(); ++i) {
    for (int j = 0; j < right.columns(); ++j) {
      for (int k = 0; k < left.columns(); ++k) {
        result(i, j) += left(i, k) * right(k, j);
      }
    }
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedVector<Product<LScalar, RScalar>> operator*(
    UnboundedMatrix<LScalar> const& left,
    UnboundedVector<RScalar> const& right) {
  CHECK_EQ(left.columns(), right.size());
  UnboundedVector<Product<LScalar, RScalar>> result(left.rows());
  for (int i = 0; i < left.rows(); ++i) {
    auto& result_i = result[i];
    for (int j = 0; j < left.columns(); ++j) {
      result_i += left(i, j) * right[j];
    }
  }
  return result;
}

template<typename LScalar, typename RScalar>
UnboundedVector<Product<LScalar, RScalar>> operator*(
    TransposedView<UnboundedMatrix<LScalar>> const& left,
    UnboundedVector<RScalar> const& right) {
  CHECK_EQ(left.rows(), right.size());
  UnboundedVector<Product<LScalar, RScalar>> result(left.rows());
  for (int i = 0; i < left.rows(); ++i) {
    auto& result_i = result[i];
    for (int j = 0; j < left.columns(); ++j) {
      result_i += left(i, j) * right[j];
    }
  }
  return result;
}

template<typename Scalar>
std::ostream& operator<<(std::ostream& out,
                         UnboundedVector<Scalar> const& vector) {
  std::stringstream s;
  for (int i = 0; i < vector.size(); ++i) {
    s << (i == 0 ? "{" : "") << vector[i]
      << (i == vector.size() - 1 ? "}" : ", ");
  }
  out << s.str();
  return out;
}

template<typename Scalar>
std::ostream& operator<<(std::ostream& out,
                         UnboundedLowerTriangularMatrix<Scalar> const& matrix) {
  out << "rows: " << matrix.rows() << "\n";
  for (int i = 0; i < matrix.rows(); ++i) {
    out << "{";
    for (int j = 0; j <= i; ++j) {
      out << matrix(i, j);
      if (j < i) {
        out << ", ";
      }
    }
    out << "}\n";
  }
  return out;
}

template<typename Scalar>
std::ostream& operator<<(std::ostream& out,
                         UnboundedMatrix<Scalar> const& matrix) {
  out << "rows: " << matrix.rows() << " columns: " << matrix.columns() << "\n";
  for (int i = 0; i < matrix.rows(); ++i) {
    out << "{";
    for (int j = 0; j < matrix.columns(); ++j) {
      out << matrix(i, j);
      if (j < matrix.columns() - 1) {
        out << ", ";
      }
    }
    out << "}\n";
  }
  return out;
}

template<typename Scalar>
std::ostream& operator<<(std::ostream& out,
                         UnboundedUpperTriangularMatrix<Scalar> const& matrix) {
  out << "columns: " << matrix.columns_ << "\n";
  for (int i = 0; i < matrix.columns(); ++i) {
    out << "{";
    for (int j = i; j < matrix.columns(); ++j) {
      if (j > i) {
        out << ", ";
      }
      out << matrix(i, j);
    }
    out << "}\n";
  }
  return out;
}

}  // namespace internal
}  // namespace _unbounded_arrays
}  // namespace numerics
}  // namespace principia
