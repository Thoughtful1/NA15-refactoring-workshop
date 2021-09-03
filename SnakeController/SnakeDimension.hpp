#pragma once

#include "SnakePosition.hpp"

namespace Snake
{

struct Dimension
{
    int width, height;
    Dimension readWorldDimension(std::istream& istr);

    bool isInside(Position position) const
    {
        return positiveLessThan(position.x, width)
            and positiveLessThan(position.y, height);
    }

private:
    bool positiveLessThan(int value, int max) const
    {
        return 0 <= value and value < max;
    }

    
};
Dimension Dimension::readWorldDimension(std::istream& istr){
    Dimension dimension;
    istr >> dimension.width >> dimension.height;
    return dimension;

    }

} // namespace Snake
