#include "SnakeWorld.hpp"
#include "SnakeController.cpp"
namespace Snake
{



World::World(Dimension dimension, Position food)
    : m_foodPosition(food),
      m_dimension(dimension)
{}

void World::setFoodPosition(Position position)
{
    m_foodPosition = position;
}

Position World::getFoodPosition() const
{
    return m_foodPosition;
}

bool World::contains(Position position) const
{
    return m_dimension.isInside(position);
}
std::unique_ptr<World> readWorld(std::istream& istr)
{
    if (not checkControl(istr, 'W')) {
        throw Snake::ConfigurationError();
    }
    Dimension snakeGameDimensions;
    Position snakePosition;
    auto worldDimension = snakeGameDimensions.readWorldDimension(istr);
    auto foodPosition = snakePosition.readFoodPosition(istr);
    return std::make_unique<World>(worldDimension, foodPosition);
}
} // namespace Snake
