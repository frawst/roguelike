#include "bat.h"

#include <cmath>

Bat::Bat(double x, double y) :
  Entity("enemies.png", 8, x, y),
  state_(State::Waiting),
  cx_(0), cy_(0), clockwise_(true) {}

void Bat::ai(const Dungeon&, const Entity& player) {
  if (state_ == State::Waiting) {
    const double dx = x_ - player.x();
    const double dy = y_ - player.y();
    radius_ = std::hypot(dx, dy);
    if (radius_ < kAttackRadius) {
      timer_ = 0;
      state_ = State::Flying;
      cx_ = player.x();
      cy_ = player.y();

      std::random_device rd;
      std::uniform_int_distribution<int> r(0, 1);
      clockwise_ = r(rd) == 0;
    }
  }
}

void Bat::update(const Dungeon&, unsigned int elapsed) {
  timer_ += elapsed;

  if (state_ == State::Flying) {
    double angle = std::atan2(y_ - cy_, x_ - cx_);
    angle += elapsed * kFlyingSpeed * (clockwise_ ? 1 : -1);
    x_ = std::cos(angle) * radius_ + cx_;
    y_ = std::sin(angle) * radius_ + cy_;

    if (timer_ > kFlyTime) {
      state_ = State::Resting;
      timer_ = 0;
    }

  } else if (state_ == State::Resting && timer_ > kRestTime) {
    state_ = State::Waiting;
    timer_ = 0;
  }
}

void Bat::draw(Graphics& graphics, int xo, int yo) const {
  Entity::draw(graphics, xo, yo);
#ifndef NDEBUG
  if (cx_ != 0 || cy_ != 0) {
    graphics.draw_line(x_ - xo, y_ - yo, cx_ - xo, cy_ - yo, 0x0000ffff);
  }
#endif
}

int Bat::sprite_number() const {
  return state_ == State::Flying ? 4 + (timer_ / 100) % 2 : 6;
}