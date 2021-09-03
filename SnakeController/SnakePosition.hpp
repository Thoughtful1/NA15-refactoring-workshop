#pragma once
#include <sstream>

namespace Snake
{

struct Position
{
    int x, y;

    bool operator==(Position const& rhs) const { return x == rhs.x and y == rhs.y; }
    
    Position retrievePosition(std::istream& istr);
    Position readFoodPosition(std::istream& istr);   
    bool checkPositionControl(std::istream& istr, char control); 
};

Position Position::retrievePosition(std::istream& istr){
    Position position;
    istr >> position.x >> position.y;
    return position;
}
bool checkPositionControl(std::istream& istr, char control)
{
    char input;
    istr >> input;
    return input == control;
}

Position Position::readFoodPosition(std::istream& istr)
{

    if (not this->checkPositionControl(istr, 'F')) {
        throw std::logic_error("Bad configuration of Snake::SnakePosition.");
    }
    Position snakeposition;    
    return snakeposition.retrievePosition(istr);
}
} // namespace Snake
