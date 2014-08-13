#ifndef SPACE_H
#define SPACE_H

#include "Circle.h"
#include "Utility.h"

#include <vector>
#include <iostream>
#include <random>
#include <functional>
#include <unordered_set>

// TODO: synchronizing access to objects vector, since
// creating new objects may result in trying to move an uninitialized object
#include <atomic>
#include <mutex>
#include <thread>

namespace flatics {

template<typename Scalar, class Vec>
class Space {
public:
  // TODO: for debug only
  size_t ops = 0;

  enum BoundaryMode {
    NONE,
    WRAP,
    BOUNCE,
  };

private:
  typedef Circle<Scalar, Vec> Object;

  Scalar width_, height_;
  std::vector<Object> objects_;

  BoundaryMode boundary_mode_;
  bool object_gravity_;
  std::mutex mutex_;

  bool wrapBoundaries(Object& obj) {
    if (obj.position().x <= 0)
      obj.setPosition(width_ + obj.position().x, obj.position().y);
    else if (obj.position().x >= width_)
      obj.setPosition(obj.position().x - width_, obj.position().y);
    else if (obj.position().y <= 0)
      obj.setPosition(obj.position().x, height_ + obj.position().y);
    else if (obj.position().y >= height_)
      obj.setPosition(obj.position().x, obj.position().y - height_);

    return false;
  }

  bool bounceBoundaries(Object& obj, Scalar restitution_factor = 1.0) {
    if (obj.minX() <= 0) {
      if (obj.velocity().x < 0)
        obj.bounceVertical(restitution_factor);

      obj.translateX(-obj.minX());
    } else if (obj.maxX() >= width_) {
      if (obj.velocity().x > 0)
        obj.bounceVertical(restitution_factor);

      obj.translateX(width_ - obj.maxX());
    } if (obj.minY() <= 0) {
      if (obj.velocity().y < 0)
        obj.bounceHorizontal(restitution_factor);

      obj.translateY(-obj.minY());
    } else if (obj.maxY() >= height_) {
      if (obj.velocity().y > 0)
        obj.bounceHorizontal(restitution_factor);

      obj.translateY(height_ - obj.maxY());
    }

    return false; // TODO: make these void functions...
  }

  static void applyGravity(Object& obj1, Object& obj2) {
    Vec r = obj2.position() - obj1.position();

    // guard against objects on top of each other causing infinite gravity
    if (r) {
      Scalar forceMagnitude = G * obj1.mass() * obj2.mass() / r.squared();

      Vec force(forceMagnitude, r);

      obj1.addExternalForce(force);
      obj2.addExternalForce(-force);
    }
  }

public:
  // TODO: make this private...
  // gravity's acceleration vector
  Vec global_gravity_;

  Space(size_t width, size_t height, BoundaryMode boundaryMode = BoundaryMode::BOUNCE, const Vec& gravity = Vec())
      : width_(width), height_(height), boundary_mode_(boundaryMode), object_gravity_(true), global_gravity_(gravity) {
  }

  const std::vector<Object>& objects() { return objects_; }

  void addRandomCircle() {
    std::lock_guard<std::mutex> lock(mutex_);

    // TODO: clean this up
    static std::uniform_real_distribution<Scalar> radius(3, 15);

    Scalar rad = radius(randGen);
    Scalar mass = rad * rad;

    static std::normal_distribution<Scalar> velocity(0.0, 200);

    static std::uniform_real_distribution<Scalar> xVal(101, width_-101);
    static std::uniform_real_distribution<Scalar> yVal(101, height_-101);

    objects_.emplace_back(rad, mass, Vec(xVal(randGen), yVal(randGen)), Vec(velocity(randGen), velocity(randGen)));

    std::cout << "Just created " << objects_.back() << " for a total of " << objects_.size() << std::endl;
  }

  void addRandomCircle(Scalar x, Scalar y, Scalar mass = 0, Scalar rad = 0) {
    std::lock_guard<std::mutex> lock(mutex_);

    // TODO: clean this up
    static std::uniform_real_distribution<Scalar> radius(5, 10);

    if (rad == 0)
      rad = radius(randGen);

    if (mass == 0)
      mass = rad * rad;

    objects_.emplace_back(rad, mass, Vec(x, y), Vec());
  }

  template<typename... Args>
  void addCircle(Args&&... args) {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_.emplace_back(args...);
  }

  void update(Scalar dt) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (objects_.empty())
      return;

    // TODO: for testing only
    size_t compareCount = 0;

    // gravity and collisions
    for (size_t i = 0; i < objects_.size()-1; ++i)
    for (size_t j = i+1; j < objects_.size(); ++j) {
      Object& bad1 = objects_[i];
      Object& bad2 = objects_[j];

      compareCount++;

      if (Object::distance(bad1, bad2) <= bad1.radius() + bad2.radius()) {
        Object::collide(bad1, bad2);
        Object::unoverlap(bad1, bad2);
      }

      if (object_gravity_)
        applyGravity(bad1, bad2);
    }

    ops = compareCount;

    for (Object& obj : objects_) {

      // TODO: move this check elsewhere?
      if (boundary_mode_ == BOUNCE) {
        bounceBoundaries(obj);
      } else if (boundary_mode_ == WRAP) {
        wrapBoundaries(obj);
      }

      obj.update(dt, global_gravity_);
    }
  }

  Scalar energy() const {
    Scalar total = 0;

    for (const Object& obj : objects_) {
      total += obj.energy();
    }

    return total;
  }

  Vec momentum() const {
    Vec total;

    for (const Object& obj : objects_) {
      total += obj.momentum();
    }

    return total;
  }

  void report() const {
    std::cout << "Gravity: " << global_gravity_ << std::endl
              << "Kinetic energy: " << energy() << " J " << std::endl
              << "Momentum: " << momentum() << " N*s" << std::endl
              << "|Momentum|: " << momentum().length() << " N*s\n" << std::endl;
  }

  void energize(Scalar ratio) {
    for (Object& obj : objects_) {
      obj.scaleVelocity(ratio);
    }
  }

  void halt() {
    for (Object& obj : objects_) {
      obj.setVelocity(Vec());
    }
  }

  void clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    objects_.clear();
  }
};

template<typename Scalar, class Vec>
std::ostream& operator<<(std::ostream& os, const Space<Scalar, Vec>& space) {
  for (const auto& obj : space.objects_)
    os << obj << std::endl;

  return os;
}

}

#endif //SPACE_H
