#include "Space.h"
#include "Circle.h"
#include "Vector2.h"
#include "Utility.h"

#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <thread>

template<typename SFMLType, typename FlaticsType>
sf::Vector2<SFMLType> toSf(flatics::Vector2<FlaticsType> vec) {
  return sf::Vector2<SFMLType>(vec.x, vec.y);
}

void updateInAThreadTest(flatics::Space<double, flatics::Vector2<double> >* space) {
  using namespace flatics;
  using namespace std::chrono;

  high_resolution_clock::time_point printTimerStart = high_resolution_clock::now();

  double dt = 2e-6;

  uint64_t start = cycleCount();
  uint64_t end;

  while (1) {
    space->update(dt);

    end = cycleCount();
    dt = static_cast<double>(end - start) / TICK_RATE;
    start = cycleCount();

    duration<double> printTime = duration_cast<duration<double> >(high_resolution_clock::now() - printTimerStart);

    if (printTime.count() >= 2.0) {
      std::cout << "FLATICS TIME-->Ops: " << space->ops << ", FPS: " << 1.0d/dt << ", so frame took " << dt << " seconds." << std::endl;
      printTimerStart = high_resolution_clock::now();
    }
  }
}

int main() {
  using namespace flatics;
  using namespace std::chrono;
  using namespace std;

  typedef double Scalar;

  typedef Vector2<double> Vec;

  typedef Space<double, Vec> Space;

  // yeah, these won't be hardcoded someday
  const Scalar HEIGHT = 900;
  const Scalar WIDTH = 1600;

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "flatics!", sf::Style::Default, settings);
  window.setFramerateLimit(60);

  const Scalar RADIUS = 25;

  const Vec start0 = {600, 150};
  const Vec start1 = {700, 400};
  const Vec start2 = {200, 500};
  const Vec start3 = {800, 500};
  const Vec start4 = {900, 500};
  const Vec start5 = {1000, 600};

  const Vec planet = {800, 500};
  const Vec satellite = {800, 200};
  const Vec satelliteMotion = {150, 0};

  const Vec moveUp = {0, -100};
  const Vec moveDown = {0, 1};
  const Vec moveRight = {200, 0};
  const Vec moveLeft = {-70, 0};
  const Vec still = {0, 0};

  Space space(WIDTH, HEIGHT);


  space.addCircle(50, 1e17, planet, still);
  //space.addCircle(50, 1e17, start5, still);
  space.addCircle(3, 100,  satellite, satelliteMotion);

  space.addCircle(RADIUS, RADIUS*RADIUS, start2, moveRight);
/*
  space.addCircle(RADIUS, RADIUS*RADIUS,   start3, still);
  space.addCircle(RADIUS, 10,   150, -5);
  space.addCircle(RADIUS, 101,   start4, moveLeft);
*/

  const sf::Color COLORS[] = {
    sf::Color::Green,
    sf::Color::White,
    sf::Color::Red,
    sf::Color::Blue,
    sf::Color::Yellow,
    sf::Color::Magenta,
    sf::Color::Cyan,
  };

  /* Just testing this out... remove this code later
  sf::ConvexShape polygon;
  polygon.setPointCount(3);
  polygon.setPoint(0, toSf<float>(pt1));
  polygon.setPoint(1, toSf<float>(pt2));
  polygon.setPoint(2, toSf<float>(pt3));
  polygon.setOutlineColor(sf::Color::White);
  polygon.setOutlineThickness(1);
  polygon.setPosition(toSf<float>(pos));

  Vec w_pt1 = pt1 + pos;
  Vec w_pt2 = pt2 + pos;
  Vec w_pt3 = pt3 + pos;
  */

  high_resolution_clock::time_point frameStart = high_resolution_clock::now();

  unsigned short timer = 0;

  // START THE PHYSICS THREAD
  std::thread physics(updateInAThreadTest, &space);
  physics.detach();

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::MouseButtonPressed:
        std::cout << " You clicked " << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            space.addRandomCircle(event.mouseButton.x, event.mouseButton.y, 1e17f, 30);
          else
            space.addRandomCircle(event.mouseButton.x, event.mouseButton.y);
          /*
                shapes.emplace_back(space._objs.back().radius());
                shapes.back().setFillColor(COLORS[space._objs.size() % 7]);
          */
        }
        break;
      case sf::Event::KeyPressed: {
        const Scalar INCREMENT = 10.f;

        switch (event.key.code) {
        case sf::Keyboard::Add:
          space.energize(1.25);
          break;
        case sf::Keyboard::Subtract:
          space.energize(0.75);
          break;
        case sf::Keyboard::Down:
          space.global_gravity_.y += INCREMENT;
          break;
        case sf::Keyboard::Up:
          space.global_gravity_.y -= INCREMENT;
          break;
        case sf::Keyboard::Left:
          space.global_gravity_.x -= INCREMENT;
          break;
        case sf::Keyboard::Right:
          space.global_gravity_.x += INCREMENT;
          break;
        case sf::Keyboard::Space:
          space.global_gravity_.x = 0;
          space.global_gravity_.y = EARTH_GRAVITY_ACCEL;
          break;
        case sf::Keyboard::Numpad0:
        case sf::Keyboard::Num0:
          space.global_gravity_.x = 0;
          space.global_gravity_.y = 0;
          break;
        case sf::Keyboard::X:
          space.halt();
          break;
        case sf::Keyboard::P:
        case sf::Keyboard::N:
          space.addRandomCircle();
          /*
          shapes.emplace_back(space._objs.back().radius());
          shapes.back().setFillColor(COLORS[space._objs.size() % 7]);
          */
          break;
        case sf::Keyboard::Delete:
          space.clear();
        case sf::Keyboard::R:
          space.report();
        default:
          break;
        }

      }
      default:
        break;
      }
    }



    {
      window.clear();
      //for (const auto& obj : space.objects()) {
      for (size_t i = 0; i < space.objects().size(); ++i) {
        const auto& obj(space.objects()[i]);
        /*
            // collisions with triangle
            if (Vec::closeToRectangle(obj.position(), w_pt1, w_pt2, RADIUS) && Vec::distancePointToLine(obj.position(), w_pt1, w_pt2) <= RADIUS) {
              obj.bounce(pt1-pt2);
              std::cout << obj.position() << " just hit side 1 <" << w_pt1 << ", " << w_pt2 << ">!\n";
            } else if (Vec::closeToRectangle(obj.position(), w_pt3, w_pt1, RADIUS) && Vec::distancePointToLine(obj.position(), w_pt3, w_pt1) <= RADIUS) {
              obj.bounce(pt1-pt3);
              std::cout << obj.position() << " just hit side 3 <" << w_pt3 << ", " << w_pt1 << ">!\n";
            } else if (Vec::closeToRectangle(obj.position(), w_pt3, w_pt2, RADIUS) && Vec::distancePointToLine(obj.position(), w_pt3, w_pt2) <= RADIUS) {
              obj.bounce(pt2-pt3);
              std::cout << obj.position() << " just hit side 2 <" << w_pt2 << ", " << w_pt3 << ">!\n";
            }
        */

        //s++->setPosition(obj.position().x - obj.radius(), obj.position().y - obj.radius());
        sf::CircleShape shape(obj.radius());
        shape.setFillColor(COLORS[i % 7]);
        shape.setPosition(obj.position().x - obj.radius(), obj.position().y - obj.radius());
        window.draw(shape);
      }
    }

    window.display();

    high_resolution_clock::time_point frameEnd = high_resolution_clock::now();
    duration<double> frameTime = duration_cast<duration<double> >(frameEnd - frameStart);
    frameStart = high_resolution_clock::now();

    if (timer++ == 1024) {
      timer = 0;
      std::cout << "DRAWING TIME-->Ops: " << space.ops << ", FPS: " << 1.0d/frameTime.count() << ", so frame took " << frameTime.count() << " seconds." << std::endl;
    }
  }

  return 0;
}
