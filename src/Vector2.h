#ifndef FLATICS_VECTOR2_H
#define FLATICS_VECTOR2_H

#include <cmath>
#include <iostream>

namespace flatics {

/** A vector of 2 dimensions */
template<typename Scalar>
class Vector2 {
public:
  Scalar x;
  Scalar y;

  /** Initialize vector to set value */
  Vector2(Scalar initial = {});

  /** Initialize vector to set value */
  Vector2(Scalar xValue, Scalar yValue);

  /** Initialize vector from an array */
  Vector2(const Scalar array[2]);

  /** Create a vector of specified length in the same direction as the provided vector */
  Vector2(Scalar length, const Vector2<Scalar>& direction);

  /** Assign the values of one vector to this vector */
  Vector2<Scalar>& operator=(const Vector2<Scalar>& other);

  Vector2<Scalar>& operator-=(const Vector2<Scalar>& other);

  Vector2<Scalar>& operator+=(const Vector2<Scalar>& other);

  Vector2<Scalar>& operator*=(Scalar n);

  Vector2<Scalar>& operator/=(Scalar n);

  bool operator==(const Vector2<Scalar>& other) const;

  bool operator!=(const Vector2<Scalar>& other) const;

  /** cast a vector to a bool: true if non-zero, false if zero */
  operator bool() { return x != 0 || y != 0 ? true : false; }

  bool isFinite() { return isfinite(x) && isfinite(y); }

  Vector2<Scalar> operator+(const Vector2<Scalar>& other) const;

  Vector2<Scalar> operator-(const Vector2<Scalar>& other) const;

  Vector2<Scalar> operator-() const;

  Vector2<Scalar> operator*(Scalar n) const;

  Vector2<Scalar> operator/(Scalar n) const;

  template<typename T = Scalar>
  T length() const;

  template<typename T = Scalar>
  static T distance(const Vector2<Scalar>& vec1, const Vector2<Scalar>& vec2);

  Scalar dotProduct(const Vector2<Scalar>& other) const;

  static Scalar dotProduct(const Vector2<Scalar>& left, const Vector2<Scalar>& right);

  /** The magnitude of the vector squared, or the vector's dot product with itself */
  Scalar squared() const;

  Vector2<Scalar> normalized() const;

  // TODO: make sure this works...
  /** The angle with respect to the x-axis */
  Scalar angle() const;

  /** Clear the vector (set it to 0) */
  void clear();

  /** Add to the the length but not the angle of the vector */
  void addLength(Scalar delta);

  /** Scale the length but not the angle of the vector */
  void scaleLength(Scalar f);

  /** Set the length but not the angle of the vector */
  void setLength(Scalar newLength);

  /** Distance from point to line defined by two points */
  static Scalar distancePointToLine(const Vector2<Scalar>& point, const Vector2<Scalar>& linePt1, const Vector2<Scalar>& linePt2);

  // two corners on a diagonal of a rectangle
  static bool withinRectangle(const Vector2<Scalar>& point, const Vector2<Scalar>& r1, const Vector2<Scalar>& r2);

  static bool closeToRectangle(const Vector2<Scalar>& point, const Vector2<Scalar>& origR1, const Vector2<Scalar>& origR2, Scalar distance);
}; // class Vector2

template<typename Scalar>
Vector2<Scalar> operator*(Scalar n, const Vector2<Scalar>& vec);

template<typename Scalar>
std::ostream& operator<<(std::ostream& os, const Vector2<Scalar>& vec);

#include "Vector2.inl"

} // namespace flatics

#endif //FLATICS_VECTOR2_H
