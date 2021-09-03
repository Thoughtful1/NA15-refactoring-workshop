#pragma once

#include <list>
#include <memory>
#include <functional>
#include <stdexcept>

#include "IEventHandler.hpp"
#include "SnakeInterface.hpp"

class Event;
class IPort;

namespace Snake
{

struct World
{
    std::pair<int, int> m_mapDimension;
    std::pair<int, int> m_foodPosition;   
    void sendPlaceNewFood(int x, int y, IPort& m_displayPort);
    void updateFoodPosition(int x, int y, std::function<void()> clearPolicy, Controller& controller, Segment& segment, Snake& snake);
    void sendClearOldFood(IPort& m_displayPort);
};

struct Snake
{
    std::list<Segment> m_segments;
};


struct ConfigurationError : std::logic_error
{
    ConfigurationError();
};

struct UnexpectedEventException : std::runtime_error
{
    UnexpectedEventException();
};

struct Segment
{
    int x, y;
    void addHeadSegment(Segment const& newHead, Snake& snake, IPort& m_displayPort);
    void removeTailSegmentIfNotScored(Segment& newHead, World& world, Controller& controller, Snake& snake);
    void removeTailSegment(std::list<Segment>& m_segments, IPort& m_displayPort);
    bool isSegmentAtPosition(int x, int y, std::list<Segment>& m_segments) const;
    Segment calculateNewHead(std::list<Segment>& m_segments, Direction& m_currentDirection);
    void updateSegmentsIfSuccessfullMove(Segment& newHead, World& world, Controller& controller, Snake snake);
    void handleTimeoutInd(Segment& segment, Snake& const snake, Controller controller, World world, Direction& const m_currentDirection);
};




class Controller : public IEventHandler
{
public:
    Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config);

    //Controller(Controller const& p_rhs) = delete;
    Controller& operator=(Controller const& p_rhs) = delete;

    void receive(std::unique_ptr<Event> e) override;

    IPort& m_displayPort;
    IPort& m_foodPort;
    IPort& m_scorePort;
    bool isPositionOutsideMap(int x, int y, std::pair<int, int>& m_mapDimension) const;
private:


    // std::pair<int, int> m_mapDimension;
    // std::pair<int, int> m_foodPosition;

    // struct Segment
    // {
    //     int x;
    //     int y;
    // };

    Snake snake;
    World world;
    Segment segment;
    //std::list<Segment> m_segments;
    Direction m_currentDirection;

    
    void handleDirectionInd(std::unique_ptr<Event>);
    void handleFoodInd(std::unique_ptr<Event>);
    void handleFoodResp(std::unique_ptr<Event>);
    void handlePauseInd(std::unique_ptr<Event>);

    // bool isSegmentAtPosition(int x, int y) const;
    // Segment calculateNewHead() const;
    // void updateSegmentsIfSuccessfullMove(Segment const& newHead);
    // void addHeadSegment(Segment const& newHead);
    // void removeTailSegmentIfNotScored(Segment const& newHead);
    // void removeTailSegment();

    

    // void updateFoodPosition(int x, int y, std::function<void()> clearPolicy);
    // void sendClearOldFood();
    // void sendPlaceNewFood(int x, int y);

    bool m_paused;
};

} // namespace Snake
