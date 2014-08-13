// Definition of the Vector2 class
template<typename Scalar>
inline Vector2<Scalar>::Vector2(Scalar initial) : x(initial), y(initial) {}

template<typename Scalar>
inline Vector2<Scalar>::Vector2(Scalar xValue, Scalar yValue) : x(xValue), y(yValue) {}

template<typename Scalar>
inline Vector2<Scalar>::Vector2(const Scalar array[2]) : x(array[0]), y(array[1]) {}

template<typename Scalar>
inline Vector2<Scalar>::Vector2(Scalar length, const Vector2<Scalar>& direction) {
  Scalar originalLength = direction.length();

  x = direction.x/originalLength * length;
  y = direction.y/originalLength * length;
}

template<typename Scalar>
inline Vector2<Scalar>& Vector2<Scalar>::operator=(const Vector2<Scalar>& other) {
  x = other.x;
  y = other.y;
  return *this;
}

template<typename Scalar>
inline Vector2<Scalar>& Vector2<Scalar>::operator-=(const Vector2<Scalar>& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

template<typename Scalar>
inline Vector2<Scalar>& Vector2<Scalar>::operator+=(const Vector2<Scalar>& other) {
  x += other.x;
  y += other.y;
  return *this;
}

template<typename Scalar>
inline Vector2<Scalar>& Vector2<Scalar>::operator*=(Scalar n) {
  x *= n;
  y *= n;
  return *this;
}

template<typename Scalar>
inline Vector2<Scalar>& Vector2<Scalar>::operator/=(Scalar n) {
  x /= n;
  y /= n;
  return *this;
}

template<typename Scalar>
inline bool Vector2<Scalar>::operator==(const Vector2<Scalar>& other) const {
  return (x == other.x) && (y == other.y);
}

template<typename Scalar>
inline bool Vector2<Scalar>::operator!=(const Vector2<Scalar>& other) const {
  return (x != other.x) || (y != other.y);
}

template<typename Scalar>
inline Vector2<Scalar> Vector2<Scalar>::operator+(const Vector2<Scalar>& other) const {
  return Vector2(x + other.x, y + other.y);
}

template<typename Scalar>
inline Vector2<Scalar> Vector2<Scalar>::operator-(const Vector2<Scalar>& other) const {
  return Vector2(x - other.x, y - other.y);
}

template<typename Scalar>
inline Vector2<Scalar> Vector2<Scalar>::operator-() const {
  return Vector2(-x, -y);
}

template<typename Scalar>
inline Vector2<Scalar> Vector2<Scalar>::operator*(Scalar n) const {
  return Vector2(x * n, y * n);
}

template<typename Scalar>
inline Vector2<Scalar> Vector2<Scalar>::operator/(Scalar n) const {
  return Vector2(x / n, y / n);
}

template<typename Scalar>
inline Scalar Vector2<Scalar>::dotProduct(const Vector2<Scalar>& other) const {
  return x * other.x + y  * other.y;
}

template<typename Scalar>
inline Scalar Vector2<Scalar>::dotProduct(const Vector2<Scalar>& left, const Vector2<Scalar>& right) {
  return left.x * right.x + left.y * right.y;
}

template<typename Scalar>
template<typename T>
inline T Vector2<Scalar>::length() const {
  return sqrt(x*x + y*y);
}

template<typename Scalar>
template<typename T>
inline T Vector2<Scalar>::distance(const Vector2<Scalar>& vec1, const Vector2<Scalar>& vec2) {
  Scalar dx = vec1.x - vec2.x;
  Scalar dy = vec1.y - vec2.y;

  return sqrt(dx*dx + dy*dy);
}

template<typename Scalar>
inline Scalar Vector2<Scalar>::squared() const {
  return x*x + y*y;
}

template<typename Scalar>
inline Vector2<Scalar> Vector2<Scalar>::normalized() const {
  return Vector2(x/length(), y/length());
}

// TODO: make sure this works...
template<typename Scalar>
inline Scalar Vector2<Scalar>::angle() const {
  return atan2(y, x);
}

template<typename Scalar>
inline void Vector2<Scalar>::clear() {
  x = 0;
  y = 0;
}

// TODO: TEST THESE THREE FUNCTIONS
template<typename Scalar>
inline void Vector2<Scalar>::addLength(Scalar delta) {
  // cos(arctan(y/x)) == x/hypotenuse
  if (x != 0 || y != 0) {
    Scalar hypotenuse = length();
    x += x/hypotenuse * delta;
    y += y/hypotenuse * delta;
  } else {
    x = delta;
  }
}

template<typename Scalar>
inline void Vector2<Scalar>::setLength(Scalar newLength) {
  Scalar len = length();
  // cos(arctan(y/x)) == x/length
  x = x/len * newLength;
  y = y/len * newLength;
}



// TODO: DO I NEED THIS????
template<typename Scalar>
inline Scalar Vector2<Scalar>::distancePointToLine(const Vector2<Scalar>& point, const Vector2<Scalar>& linePt1, const Vector2<Scalar>& linePt2) {
  // handle vertical lines
  if (linePt1.x == linePt2.x) {
    return abs(point.x - linePt1.x);
  }

  Scalar m = (linePt1.y - linePt2.y) / (linePt1.x - linePt2.x);
  Scalar b = linePt1.y - m*linePt1.x;
/*
  std::cout << linePt1 << " and " << linePt2 << " are y = " << m << "*x + " << b << std::endl;
  std::cout << "\tdistance from " << point << " to that line is " << abs(point.y - m*point.x - b) / sqrt(m*m + 1) << std::endl;
*/
  // Thanks http://math.ucsd.edu/~wgarner/math4c/derivations/distance/distptline.htm
  return abs(point.y - m*point.x - b / sqrt(m*m + 1));
}

// two corners on a diagonal of a rectangle
// TODO: DO I NEED THIS????
template<typename Scalar>
inline bool Vector2<Scalar>::withinRectangle(const Vector2<Scalar>& point, const Vector2<Scalar>& r1, const Vector2<Scalar>& r2) {
  /*
  bool withinX = r1.x < r2.x ? (point.x >= r1.x && point.x <= r2.x) : (point.x >= r2.x && point.x <= r1.x);
  bool withinY = r1.y < r2.y ? (point.y >= r1.y && point.y <= r2.y) : (point.y >= r2.y && point.y <= r1.y);

  return withinX && withinY;
  */

  return (r1.x < r2.x ? (point.x >= r1.x && point.x <= r2.x) : (point.x >= r2.x && point.x <= r1.x))
    && (r1.y < r2.y ? (point.y >= r1.y && point.y <= r2.y) : (point.y >= r2.y && point.y <= r1.y));
}

// TODO: DO I NEED THIS????
template<typename Scalar>
inline bool Vector2<Scalar>::closeToRectangle(const Vector2<Scalar>& point, const Vector2<Scalar>& origR1, const Vector2<Scalar>& origR2, Scalar distance) {
  Vector2<Scalar> r1, r2;

  if (origR1.x < origR1.x) {
    r1.x = origR1.x - distance;
    r2.x = origR2.x + distance;
  } else {
    r1.x = origR1.x + distance;
    r2.x = origR2.x - distance;
  }

  if (origR1.y < origR1.y) {
    r1.y = origR1.y - distance;
    r2.y = origR2.y + distance;
  } else {
    r1.y = origR1.y + distance;
    r2.y = origR2.y - distance;
  }

  return withinRectangle(point, r1, r2);
}

template<typename Scalar>
inline Vector2<Scalar> operator*(Scalar n, const Vector2<Scalar>& vec) {
  return Vector2<Scalar>(n * vec.x, n * vec.y);
}

template<typename Scalar>
inline std::ostream& operator<<(std::ostream& os, const Vector2<Scalar>& vec) {
  return os << '[' << vec.x << ' ' << vec.y << ']';
}
