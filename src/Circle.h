#ifndef FLATICS_CIRCLE_H
#define FLATICS_CIRCLE_H

#include "Shape.h"
#include "Utility.h"

#include <iostream>
#include <cmath>

namespace flatics {

template<typename Scalar, class Vec>
class Circle : public Shape<Scalar, Vec> {
private:
  Scalar radius_;

public:
  Circle(Scalar radius, Scalar mass, const Vec& position = Vec(), const Vec& velocity = Vec())
    : Shape<Scalar, Vec>(mass, position, velocity), radius_(radius) {}

  Scalar minX() const { return this->position_.x - radius_; };
  Scalar maxX() const { return this->position_.x + radius_; };
  Scalar minY() const { return this->position_.y - radius_; };
  Scalar maxY() const { return this->position_.y + radius_; };

  Scalar radius() const { return radius_; }

  // "bounce" off something with infinite mass
  // TODO: make sure this works...
  void bounce(const Vec& edge) {
    // get the projection and rejection of this vector on the other vector
    Vec unit = edge.normalized();
    Vec projection = Vec::dotProduct(this->velocity_, unit) * unit;
    Vec rejection = this->velocity_ - projection;

    // now, invert the rejection to have the velocity "bounce off" the surface
    this->velocity_ = projection - rejection;
  }

  /** Bounce off of a unit vector */
  void bounceUnitEdge(const Vec& unit) {
    // get the projection and rejection of this vector on the other vector
    Vec projection = Vec::dotProduct(this->velocity_, unit) * unit;
    Vec rejection = this->velocity_ - projection;

    // now, invert the rejection to have the velocity "bounce off" the surface
    this->velocity_ = projection - rejection;
  }

  void bounceHorizontal(Scalar restituton_factor = 1.0) {
    this->velocity_.y = -this->velocity_.y * restituton_factor;
  }

  void bounceVertical(Scalar restituton_factor = 1.0) {
    this->velocity_.x = -this->velocity_.x * restituton_factor;
  }

  // TODO: vector form is less efficient when compiled with -O2
  inline static Vec vectorCollision(Scalar cr, Scalar m1, const Vec& x1, const Vec& v1, Scalar m2, const Vec& x2, const Vec& v2) {
    Vec difference(x1 - x2);

    if (difference) {
      Scalar massRatio = (cr + 1)*m2 / (m1 + m2);
      Scalar projection = Vec::dotProduct(v1 - v2, difference) / difference.squared();

      return v1 - massRatio * projection * (x1 - x2);
    } else {
      return Vec();
    }
  }

  // TODO: this is not the right way to do this
  inline static void collide(Circle& obj1, Circle& obj2) {
    const Scalar& m1(obj1.mass());
    const Vec& x1(obj1.position());
    const Vec& v1(obj1.velocity());
    const Scalar& m2(obj2.mass());
    const Vec& x2(obj2.position());
    const Vec& v2(obj2.velocity());

    const double cr = 0.6;

    Vec difference(x1 - x2);

    if (difference) {
      // COMPARE METHODS
      // TODO: COMPARE EFFICIENCY OF METHODS

      /* TODO: remove this comparison code
      uint64_t vector_1 = cycleCount();

      Vec new_obj1_vel(vectorCollision(cr, obj1.mass_, obj1.position_, obj1.velocity_, obj2.mass_, obj2.position_, obj2.velocity_));
      Vec new_obj2_vel(vectorCollision(cr, obj2.mass_, obj2.position_, obj2.velocity_, obj1.mass_, obj1.position_, obj1.velocity_));
      Vec test(new_obj1_vel);

      uint64_t vector_2 = cycleCount();

//      std::cout << "COLLISION!" << std::endl;

      uint64_t linear_1 = cycleCount();
*/
      Vec unit_normal(difference.normalized());
      Vec unit_tangent(-unit_normal.y, unit_normal.x);

      Scalar v1_normal = unit_normal.dotProduct(v1);
      Scalar v2_normal = unit_normal.dotProduct(v2);

      Scalar v1_tangent = unit_tangent.dotProduct(v1);
      Scalar v2_tangent = unit_tangent.dotProduct(v2);

      Scalar v1_normal_f = PointMass<Scalar, Vec>::inelasticCollision(cr, m1, v1_normal, m2, v2_normal);
      Scalar v2_normal_f = PointMass<Scalar, Vec>::inelasticCollision(cr, m2, v2_normal, m1, v1_normal);

      obj1.velocity_ = v1_normal_f * unit_normal + v1_tangent * unit_tangent;
      obj2.velocity_ = v2_normal_f * unit_normal + v2_tangent * unit_tangent;

/*  // TODO: cleanup
      uint64_t linear_2 = cycleCount();
      std::cout << "METHOD 1 TOOK: " << vector_2 - vector_1 << " and GIVES: " << new_obj1_vel << " " << new_obj2_vel << std::endl;
      std::cout << "METHOD 2 TOOK: " << linear_2 - linear_1 << " and GIVES: " << obj1.velocity_ << " " << obj2.velocity_ << std::endl;
*/
    }
  }

/*
  // TODO: delete this
  static void collide(Circle& obj1, Circle& obj2) {
    Vec newObj1Velocity(vectorCollision(obj1.mass_, obj1.position_, obj1.velocity_, obj2.mass_, obj2.position_, obj2.velocity_));
    obj2.velocity_ = vectorCollision(obj2.mass_, obj2.position_, obj2.velocity_, obj1.mass_, obj1.position_, obj1.velocity_);
    obj1.velocity_ = newObj1Velocity;
  }
*/

  // TODO: This is not good... find a better way to avoid overlapping
  static void unoverlap(Circle& obj1, Circle& obj2) {
    Vec difference(obj1.position() - obj2.position());
    Scalar translateDistance = obj1.radius() + obj2.radius() - difference.length();

    if (translateDistance > 0) {
      difference.addLength(translateDistance); // TODO: should I add just a bit to the translate distance?

      // move the lass massive of the two objects
      if (obj1.mass() < obj2.mass()) {
        obj1.setPosition(obj2.position() + difference);
      } else {
        obj2.setPosition(obj1.position() - difference);
      }
    }
  }
};

template<typename Scalar, class Vec>
std::ostream& operator<<(std::ostream& os, const Circle<Scalar, Vec>& obj) {
  return os << "{ " << &obj << " is " << obj.mass() << " kg at " << obj.position() << " m | " << obj.velocity() << " m/s | " << obj.speed() << " m/s";
}

}

#endif //FLATICS_CIRCLE_H
