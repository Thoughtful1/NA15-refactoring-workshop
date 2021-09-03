#include "SnakeController.hpp"

#include <algorithm>
#include <sstream>

#include "EventT.hpp"
#include "IPort.hpp"

namespace Snake
{
ConfigurationError::ConfigurationError()
    : std::logic_error("Bad configuration of Snake::Controller.")
{}

UnexpectedEventException::UnexpectedEventException()
    : std::runtime_error("Unexpected event received!")
{}
void World::updateFoodPosition(int x, int y, std::function<void()> clearPolicy, Controller& controller, Segment& segment, Snake& snake)
{
    if (segment.isSegmentAtPosition(x, y, snake.m_segments) || controller.isPositionOutsideMap(x,y,this->m_mapDimension)) {
        controller.m_foodPort.send(std::make_unique<EventT<FoodReq>>());
        return;
    }

    clearPolicy();
    this->sendPlaceNewFood(x, y, controller.m_displayPort);
}
void World::sendClearOldFood(IPort& m_displayPort)
{
    DisplayInd clearOldFood;
    clearOldFood.x = m_foodPosition.first;
    clearOldFood.y = m_foodPosition.second;
    clearOldFood.value = Cell_FREE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(clearOldFood));
}


void World::sendPlaceNewFood(int x, int y, IPort& m_displayPort)
{
    m_foodPosition = std::make_pair(x, y);

    DisplayInd placeNewFood;
    placeNewFood.x = x;
    placeNewFood.y = y;
    placeNewFood.value = Cell_FOOD;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewFood));
}


Controller::Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config)
    : m_displayPort(p_displayPort),
      m_foodPort(p_foodPort),
      m_scorePort(p_scorePort),
      m_paused(false)
{
    std::istringstream istr(p_config);
    char w, f, s, d;

    int width, height, length;
    int foodX, foodY;
    World world;
    istr >> w >> width >> height >> f >> foodX >> foodY >> s;

    if (w == 'W' and f == 'F' and s == 'S') {
        
        world.m_mapDimension = std::make_pair(width, height);
        world.m_foodPosition = std::make_pair(foodX, foodY);

        istr >> d;
        switch (d) {
            case 'U':
                m_currentDirection = Direction_UP;
                break;
            case 'D':
                m_currentDirection = Direction_DOWN;
                break;
            case 'L':
                m_currentDirection = Direction_LEFT;
                break;
            case 'R':
                m_currentDirection = Direction_RIGHT;
                break;
            default:
                throw ConfigurationError();
        }
        istr >> length;

        while (length--) {
            Segment seg;
            istr >> seg.x >> seg.y;
            this->snake.m_segments.push_back(seg);
        }
    } else {
        throw ConfigurationError();
    }
}

void Controller::handleFoodInd(std::unique_ptr<Event> e)
{
    auto receivedFood = payload<FoodInd>(*e);
    Controller& controllerRef=*this;
    this->world.updateFoodPosition(receivedFood.x, receivedFood.y, []{}, controllerRef, segment, snake);
}

void Controller::handleFoodResp(std::unique_ptr<Event> e)
{
    auto requestedFood = payload<FoodResp>(*e);
    Controller& controllerRef=*this;
    this->world.updateFoodPosition(requestedFood.x, requestedFood.y, []{}, controllerRef, segment, snake);
}

void Controller::handlePauseInd(std::unique_ptr<Event> e)
{
    m_paused = not m_paused;
}


void Controller::handleDirectionInd(std::unique_ptr<Event> e)
{
    auto direction = payload<DirectionInd>(*e).direction;

    if (perpendicular(m_currentDirection, direction)) {
        m_currentDirection = direction;
    }
}

void Controller::receive(std::unique_ptr<Event> e)
{
    switch (e->getMessageId()) {
        case TimeoutInd::MESSAGE_ID:
            if (!m_paused) {
                Controller& controllerRef=*this;
                return segment.handleTimeoutInd(segment, snake, controllerRef, world, controllerRef.m_currentDirection);
            }
            return;
        case DirectionInd::MESSAGE_ID:
            if (!m_paused) {
                return handleDirectionInd(std::move(e));
            }
            return;
        case FoodInd::MESSAGE_ID:
            return handleFoodInd(std::move(e));
        case FoodResp::MESSAGE_ID:
            return handleFoodResp(std::move(e));
        case PauseInd::MESSAGE_ID:
            return handlePauseInd(std::move(e));
        default:
            throw UnexpectedEventException();
    }
}
bool Controller::isPositionOutsideMap(int x, int y,  std::pair<int, int>& m_mapDimension) const
{
    return x < 0 or y < 0 or x >= m_mapDimension.first or y >= m_mapDimension.second;
}
bool Segment::isSegmentAtPosition(int x, int y, std::list<Segment>& m_segments) const
{
    return m_segments.end() !=  std::find_if(m_segments.cbegin(), m_segments.cend(),
        [x, y](auto const& segment){ return segment.x == x and segment.y == y; });
}

namespace
{
bool isHorizontal(Direction direction)
{
    return Direction_LEFT == direction or Direction_RIGHT == direction;
}

bool isVertical(Direction direction)
{
    return Direction_UP == direction or Direction_DOWN == direction;
}

bool isPositive(Direction direction)
{
    return (isVertical(direction) and Direction_DOWN == direction)
        or (isHorizontal(direction) and Direction_RIGHT == direction);
}

bool perpendicular(Direction dir1, Direction dir2)
{
    return isHorizontal(dir1) == isVertical(dir2);
}
} // namespace

Segment Segment::calculateNewHead(std::list<Segment>& m_segments, Direction& m_currentDirection)
{
    Segment const& currentHead = m_segments.front();

    Segment newHead;
    newHead.x = currentHead.x + (isHorizontal(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);
    newHead.y = currentHead.y + (isVertical(m_currentDirection) ? isPositive(m_currentDirection) ? 1 : -1 : 0);

    return newHead;
}

void Segment::addHeadSegment(Segment const& newHead, Snake& snake, IPort& m_displayPort)
{
    snake.m_segments.push_front(newHead);

    DisplayInd placeNewHead;
    placeNewHead.x = newHead.x;
    placeNewHead.y = newHead.y;
    placeNewHead.value = Cell_SNAKE;

    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(placeNewHead));
}

void Segment::removeTailSegmentIfNotScored(Segment& newHead, World& world, Controller& controller, Snake& snake)
{
    if (std::make_pair(newHead.x, newHead.y) == world.m_foodPosition) {
        controller.m_scorePort.send(std::make_unique<EventT<ScoreInd>>());
        controller.m_foodPort.send(std::make_unique<EventT<FoodReq>>());
    } else {
        removeTailSegment(snake.m_segments, controller.m_displayPort);
    }
}

void Segment::updateSegmentsIfSuccessfullMove(Segment& newHead, World& world, Controller& controller, Snake snake)
{
    if (isSegmentAtPosition(newHead.x, newHead.y, snake.m_segments) or controller.isPositionOutsideMap(newHead.x, newHead.y,  world.m_mapDimension)) {
        controller.m_scorePort.send(std::make_unique<EventT<LooseInd>>());
    } else {
        addHeadSegment(newHead, snake, controller.m_displayPort);
        removeTailSegmentIfNotScored(newHead, world, controller, snake);
    }
}

void Segment::handleTimeoutInd(Segment& segment, Snake& const snake, Controller controller, World world, Direction& const m_currentDirection)
{
    Segment newHeadSegment= calculateNewHead(snake.m_segments, m_currentDirection);
    segment.updateSegmentsIfSuccessfullMove(newHeadSegment, world, controller, snake);
}

void Segment::removeTailSegment(std::list<Segment>& m_segments, IPort& m_displayPort)
{
    auto tail = m_segments.back();

    DisplayInd l_evt;
    l_evt.x = tail.x;
    l_evt.y = tail.y;
    l_evt.value = Cell_FREE;
    m_displayPort.send(std::make_unique<EventT<DisplayInd>>(l_evt));

    m_segments.pop_back();
}




} // namespace Snake
