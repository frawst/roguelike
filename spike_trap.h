#pragma once

#include "entity.h"

class SpikeTrap : public Entity {
  public:

    SpikeTrap(double x, double y);

    void ai(const Dungeon& dungeon, const Entity& player) override;
    void update(const Dungeon& dungeon, unsigned int elapsed) override;

    Rect collision_box() const override;
    Rect attack_box() const override;

  private:

    static constexpr double kChargingSpeed = 0.15;
    static constexpr double kRetreatingSpeed = kChargingSpeed / 2;
    static constexpr int kHoldTime = 500;

    enum class State { Waiting, Charging, Hold, Retreating };

    State state_;

    int sprite_number() const override;
};