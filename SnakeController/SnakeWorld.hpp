#pragma once

#include <utility>
#include "SnakeDimension.hpp"
#include "SnakePosition.hpp"


namespace Snake
{

class World
{
public:
    World(Dimension & dimension, Position& food);

    void setFoodPosition(Position& position);
    Position& getFoodPosition();
    Dimension& getDimension();


    bool contains(Position& position) const;

private:
    Position m_foodPosition;
    Dimension m_dimension;
};

} // namespace Snake
