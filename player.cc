#include "player.h"

Player::Player(int x, int y) :
  Entity("player.png", 4, x, y, 12),
  weapons_("weapons.png", 2, kTileSize, kTileSize),
  ui_("ui.png", 5, kTileSize, kTileSize),
  gold_(0) {}

void Player::move(Player::Direction direction) {
  if (state_ != State::Attacking) {
    facing_ = direction;
    state_ = State::Walking;
  }
}

void Player::stop() {
  if (state_ == State::Walking) {
    state_ = State::Waiting;
  }
}

bool Player::interact(Dungeon& dungeon) {
  auto p = dungeon.grid_coords(x_, y_);
  switch (facing_) {
    case Direction::North: --p.second; break;
    case Direction::South: ++p.second; break;
    case Direction::West: --p.first; break;
    case Direction::East: ++p.first; break;
  }

  return dungeon.interact(p.first, p.second);
}

void Player::attack() {
  state_ = State::Attacking;
  timer_ = 0;
}

// Helper to lock walking to half-tile grid
std::pair<double, double> grid_walk(double delta, double minor, int grid) {
  const double dmin = minor - 8 * (int)(minor / 8);
  if (dmin > grid) {
    if (delta > 8 - dmin) {
      return { delta - 8 + dmin, 8 - dmin };
    } else {
      return { 0, delta };
    }
  } else {
    if (delta > dmin) {
      return { delta - dmin, -dmin };
    } else {
      return { 0, -delta };
    }
  }
}

void Player::update(const Dungeon& dungeon, unsigned int elapsed) {
  if (iframes_ > 0) {
    iframes_ -= elapsed;
    if (iframes_ <= 0) iframes_ = 0;
  }

  if (state_ == State::Walking) {
    const double delta = kSpeed * elapsed;
    std::pair<double, double> d;
    double dx = 0, dy = 0;

    switch (facing_) {
      case Direction::North:
      case Direction::South:
        d = grid_walk(delta, x_, kHalfTile / 2);
        dx = d.second;
        dy = d.first * (facing_ == Direction::North ? -1 : 1);
        break;

      case Direction::West:
      case Direction::East:
        d = grid_walk(delta, y_, kHalfTile / 2);
        dx = d.first * (facing_ == Direction::West ? -1 : 1);
        dy = d.second;
        break;
    }

    x_ += dx;
    y_ += dy;

    if (dungeon.box_walkable(collision_box())) {
      timer_ = (timer_ + elapsed) % 1000;
    } else {
      x_ -= dx;
      y_ -= dy;
    }

  } else if (state_ == State::Attacking) {
    timer_ += elapsed;
    if (timer_ > kAttackTime) {
      state_ = State::Waiting;
      timer_ = 0;
    }
  }
}

void Player::draw(Graphics& graphics, int xo, int yo) const {
  if (iframes_ > 0 && (iframes_ / 32) % 2 == 0) return;

  const int x = x_ - kHalfTile - xo;
  const int y = y_ - kHalfTile - yo;

  if (facing_ == Direction::North) draw_weapon(graphics, xo, yo);
  sprites_.draw_ex(graphics, sprite_number(), x, y, facing_ == Direction::West, 0, 0, 0);
  if (facing_ != Direction::North) draw_weapon(graphics, xo, yo);

#ifndef NDEBUG
  hit_box().draw(graphics, 0xff0000ff, false, xo, yo);
#endif

}

void Player::draw_hud(Graphics& graphics, int xo, int yo) const {
  const int hearts = maxhp_ / 4;

  for (int h = 0; h < hearts; ++h) {
    const int hx = graphics.width() - kTileSize * (8 - h % 8);
    const int hy = kTileSize * (h / 8);
    const int n = (curhp_ > (h + 1) * 4) ? 4 : (curhp_ < h * 4) ? 0 : curhp_ - h * 4;
    ui_.draw(graphics, n, hx, hy);
  }
}

int Player::sprite_number() const {
  int d = 0;

  switch (facing_) {
    case Direction::North: d = 0; break;
    case Direction::South: d = 2; break;
    default: d = 1; break;
  }

  switch (state_) {
    case State::Attacking:
      return 12 + d;

    case State::Waiting:
    case State::Walking:
      return d * 4 + timer_ / 250;

    default:
      return 0;
  }
}

Rect Player::collision_box() const {
  return { x_ - kHalfTile, y_, x_ + kHalfTile - 1, y_ + kHalfTile - 1};
}

Rect Player::hit_box() const {
  return { x_ - kHalfTile + 2, y_ + 2, x_ + kHalfTile - 2, y_ + kHalfTile - 2 };
}

Rect Player::attack_box() const {
  if (state_ == State::Attacking) {
    double sx = x_;
    double sy = y_;
    double w = kTileSize;
    double h = kTileSize;

    switch (facing_) {
      case Direction::North:
        sx -= kHalfTile;
        sy -= 2 * kHalfTile;
        w = kHalfTile;
        break;

      case Direction::South:
        w = kHalfTile;
        break;

      case Direction::West:
        h = kHalfTile;
        sx -= kTileSize;
        break;

      case Direction::East:
        h = kHalfTile;
        break;
    }

    return { sx, sy, sx + w, sy + h };
  } else {
    return { 0, 0, 0, 0 };
  }
}

void Player::draw_weapon(Graphics& graphics, int xo, int yo) const {
  const Rect weapon = attack_box();
  int wx = weapon.left - xo;
  int wy = weapon.top - yo;

  if (weapon.height() != 0) {
    int weapon_sprite = 0;
    switch (facing_) {
      case Direction::North:
        weapon_sprite = 0;
        wx -= 4;
        break;
      case Direction::South:
        weapon_sprite = 1;
        wx -= 4;
        break;
      case Direction::West:
        weapon_sprite = 2;
        wy -= 4;
        break;
      case Direction::East:
        weapon_sprite = 3;
        wy -= 4;
        break;
    }

    // TODO better handling of weapon sprite positioning
    weapons_.draw(graphics, weapon_sprite, wx, wy);
#ifndef NDEBUG
    weapon.draw(graphics, 0x0000ffff, false, xo, yo);
#endif
  }
}
