#pragma once

#include "SnakePosition.hpp"
#include "SnakeDimension.hpp"
#include <memory>

namespace Snake
{

class World
{
public:
    World(Dimension dimension, Position food);

    void setFoodPosition(Position position);
    Position getFoodPosition() const;
    std::unique_ptr<World> readWorld(std::istream& istr);

    bool contains(Position position) const;
private:
    Position m_foodPosition;
    Dimension m_dimension;
};

} // namespace Snake
