#ifndef FLATICS_POINTMASS_H
#define FLATICS_POINTMASS_H

namespace flatics {

template <typename Scalar, class Vec>
class PointMass {
protected:
    Scalar _mass;
    Vec _position;
    Vec _velocity;

    Object(Scalar mass, const Vec& position = Vec(), const Vec& velocity = Vec())
        : _mass(mass), _position(position), _velocity(velocity) {}

public:
    /*
     *  Updating position and velocity
     *
     *  - update position (from velocity): position += velocity * dt
     *  - update velocity (from external forces): velocity += (force / mass) * dt
     *
     *  @param dt how much time has passed
     *  @param force the net force vector acting on the object
     */
    // TODO: Seriously think about the best way to do this
    void moveWithForceAcceleration(Scalar dt, const Vec& force, const Vec& acceleration) {
        this->_position += this->_velocity * dt;
        this->_velocity += (force / this->_mass + acceleration) * dt;
    }

    void moveWithForce(Scalar dt, const Vec& force) {
        this->_position += this->_velocity * dt;
        this->_velocity += force / this->_mass * dt;
    }

    void moveWithAcceleration(Scalar dt, const Vec& acceleration) {
        this->_position += this->_velocity * dt;
        this->_velocity += acceleration * dt;
    }

    void move(Scalar dt) {
        this->_position += this->_velocity * dt;
    }

    Scalar mass() const { return _mass; }

    const Vec& velocity() const { return _velocity; }

    void setVelocity(const Vec& velocity) { _velocity = velocity; }

    /** This is NOT a linear thing -- each dimension is scaled separately */
    void scaleVelocity(Scalar factor) { _velocity *= factor; }

    /** Position of the center of mass */
    const Vec& position() const { return _position; }

    Scalar speed() const { return _velocity.length(); }

    Vec momentum() const { return _mass * _velocity; }

    Scalar energy() const {
        return _mass * _velocity.squared() / 2;
    }
};

}

#endif // FLATICS_OBJECT_H
