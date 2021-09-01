#pragma once

#include <list>
#include <memory>
#include <stdexcept>

#include "IEventHandler.hpp"
#include "SnakeInterface.hpp"

class Event;
class IPort;

namespace Snake
{
struct ConfigurationError : std::logic_error
{
    ConfigurationError();
};

struct UnexpectedEventException : std::runtime_error
{
    UnexpectedEventException();
};

class Controller : public IEventHandler
{
public:
    Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config);

    Controller(Controller const& p_rhs) = delete;
    Controller& operator=(Controller const& p_rhs) = delete;

    void receive(std::unique_ptr<Event> e) override;
    

private:
    struct Segment
    {
        int x;
        int y;
        int ttl;
    };

    IPort& m_displayPort;
    IPort& m_foodPort;
    IPort& m_scorePort;

    std::pair<int, int> m_mapDimension;
    std::pair<int, int> m_foodPosition;
    void ScorePointOrGetLostOrRemoveSegment(bool& lost, Segment& newHead, std::pair<int, int>& m_foodPosition, std::pair<int, int>& m_mapDimension,
    IPort& m_scorePort, IPort& m_foodPort, IPort& m_displayPort);   
    void clearOldAndPlaceNewFood(bool& requestedFoodCollidedWithSnake, std::pair<int, int>& m_foodPosition, IPort& m_displayPort, 
    IPort& m_foodPort, FoodInd& receivedFood);
    void searchListofSegments(std::list<Segment>& m_segments, Segment& newHead, IPort& m_scorePort, bool& lost);
    void introduceNewHead(std::list<Segment>& m_segments, Segment& newHead, DisplayInd& placeNewHead, bool& lost, IPort& m_displayPort);
    void catchBadCastingOfRequestedFoodType(std::list<Segment>& m_segments,std::unique_ptr<Event>& e, DisplayInd& placeNewFood, IPort& m_displayPort, 
    IPort& m_foodPort, std::pair<int, int>& m_foodPosition);
    void ForLoopCheckIfSegmentsAreCollidingwithFood(std::list<Segment>& m_segments, bool& requestedFoodCollidedWithSnake, FoodInd& receivedFood);
    Direction m_currentDirection;
    std::list<Segment> m_segments;
    DisplayInd l_evt;
    DisplayInd clearOldFood;
    DisplayInd placeNewFood;
    DisplayInd placeNewHead;
};

} // namespace Snake
