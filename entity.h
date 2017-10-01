#pragma once

#include <string>

#include "graphics.h"
#include "spritemap.h"

#include "dungeon.h"
#include "rect.h"

class Entity {
  public:

    enum class Direction { North, South, East, West };

    static Direction reverse_direction(Direction d);
    static std::pair<double, double> delta_direction(Direction d, double amount);

    Entity(std::string sprites, int cols, double x, double y, int hp);

    double x() const;
    double y() const;
    void set_position(double x, double y);

    virtual void ai(const Dungeon& dungeon, const Entity& target);
    virtual void update(const Dungeon& dungeon, unsigned int elapsed);
    virtual void draw(Graphics& graphics, int xo, int yo) const;
    virtual bool dead() const;

    virtual void hit();

    virtual Rect collision_box() const;
    virtual Rect hit_box() const;
    virtual Rect attack_box() const;
    virtual Rect defense_box() const;

  protected:

    static constexpr int kTileSize = 16;
    static constexpr int kHalfTile = kTileSize / 2;
    static constexpr int kIFrameTime = 500;

    enum class State { Waiting, Walking, Attacking, Holding, Retreating };

    SpriteMap sprites_;
    double x_, y_;
    Direction facing_;
    State state_;
    int timer_, iframes_;
    int maxhp_, curhp_;
    bool dead_;

    virtual int sprite_number() const;
};
