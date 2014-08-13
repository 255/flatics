#ifndef FLATICS_POINTMASS_H
#define FLATICS_POINTMASS_H

namespace flatics {

template <typename Scalar, class Vec>
class PointMass {
private:
  Vec net_external_force_;

protected:
  Scalar mass_;
  Vec position_;
  Vec velocity_;

  PointMass(Scalar mass, const Vec& position = Vec(), const Vec& velocity = Vec())
      : mass_(mass), position_(position), velocity_(velocity) {}


  inline static Scalar elasticCollision(Scalar m1, Scalar v1, Scalar m2, Scalar v2) {
    //return ((m1 - m2)/(m1 + m2)) * v1 + (2*m2 / (m1 + m2)) * v2;
    return ((m1 - m2)*v1 + 2*m2*v2) / (m1 + m2);
  }

  // cr == coefficient of restitution, and Cr is in [0, 1]
  // elastic is inelastic with Cr = 1
  inline static Scalar inelasticCollision(Scalar cr, Scalar m1, Scalar v1, Scalar m2, Scalar v2) {
    //return ((m1 - m2)/(m1 + m2)) * v1 + (2*m2 / (m1 + m2)) * v2;
    return (cr*m2*(v2 - v1) + m1*v1 + m2*v2) / (m1 + m2);
  }

public:
  /*
   *  Updating position and velocity
   *
   *  - update position (from velocity): position += velocity * dt
   *  - update velocity (from external forces): velocity += (force / mass) * dt
   *
   *  dt: how much time has passed
   *  force: the net force vector acting on the object
   */
  /** Update the position and velocity of the point mass while applying a constant acceleration */
  void update(Scalar dt, const Vec& acceleration) {
    position_ += velocity_ * dt;
    velocity_ += (net_external_force_ / mass_ + acceleration) * dt;
    net_external_force_.clear();
  }

  /** Update the position and velocity of the point mass */
  void update(Scalar dt) {
    position_ += velocity_ * dt;
    velocity_ += net_external_force_ / mass_ * dt;
    net_external_force_.clear();
  }

  Scalar mass() const { return mass_; }

  const Vec& velocity() const { return velocity_; }

  void setVelocity(const Vec& velocity) { velocity_ = velocity; }

  void scaleVelocity(Scalar factor) { velocity_ *= factor; }

  /** Position of the center of mass */
  const Vec& position() const { return position_; }

  void setPosition(const Vec& position) { position_ = position; }

  void setPosition(Scalar x, Scalar y) {
    position_.x = x;
    position_.y = y;
  }

  void setX(Scalar x) { position_.x = x; }

  void setY(Scalar y) { position_.y = y; }

  const Vec& translate(Vec delta) { return position_ += delta; }

  Scalar translateX(Scalar dx) { return position_.x += dx; }

  Scalar translateY(Scalar dy) { return position_.y += dy; }

  Scalar speed() const { return velocity_.length(); }

  Vec momentum() const { return mass_ * velocity_; }

  Scalar energy() const {
    return mass_ * velocity_.squared() / 2;
  }

  Scalar distance(const PointMass& other) const {
    return Vec::distance(position_, other.position_);
  }

  static Scalar distance(const PointMass& obj1, const PointMass& obj2) {
    return Vec::distance(obj1.position(), obj2.position());
  }

  // TODO: this is not the right way to do collisions
  static void collide(PointMass& obj1, PointMass& obj2) {
    // calculate linear collisions for each dimension
    Scalar obj1_new_x = oneDimensionalCollision(obj1.mass_, obj1.velocity_.x, obj2.mass_, obj2.velocity_.x);
    Scalar obj1_new_y = oneDimensionalCollision(obj1.mass_, obj1.velocity_.y, obj2.mass_, obj2.velocity_.y);

    obj2.velocity_.x = oneDimensionalCollision(obj2.mass_, obj2.velocity_.x, obj1.mass_, obj1.velocity_.x);
    obj2.velocity_.y = oneDimensionalCollision(obj2.mass_, obj2.velocity_.y, obj1.mass_, obj1.velocity_.y);

    obj1.velocity_.x = obj1_new_x;
    obj1.velocity_.y = obj1_new_y;
  }

  /** Sum up external forces. */
  void addExternalForce(const Vec& force) {
    net_external_force_ += force;
  }
};

}

#endif // FLATICS_OBJECT_H
