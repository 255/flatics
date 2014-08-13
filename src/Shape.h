#ifndef FLATICS_SHAPE_H
#define FLATICS_SHAPE_H

#include "PointMass.h"

namespace flatics {

template<typename Scalar, class Vec>
class Shape : public PointMass<Scalar, Vec> {
protected:
  Shape(Scalar mass, const Vec& position = Vec(), const Vec& velocity = Vec()) : PointMass<Scalar, Vec>(mass, position, velocity) {}

public:

  // bounding box functions...
  virtual Scalar minX() const = 0;
  virtual Scalar maxX() const = 0;
  virtual Scalar minY() const = 0;
  virtual Scalar maxY() const = 0;
};

}

#endif // FLATICS_SHAPE_H
