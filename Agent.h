#ifndef ASSIGN3_AGENT_H
#define ASSIGN3_AGENT_H

#include <iostream>
#include <mcpp/mcpp.h>

#define MOVE_XPLUS mcpp::Coordinate(1,0,0)
#define MOVE_XMINUS mcpp::Coordinate(-1,0,0)
#define MOVE_ZPLUS mcpp::Coordinate(0,0,1)
#define MOVE_ZMINUS mcpp::Coordinate(0,0,-1)

enum solveAlgorithm{
        RIGHT_HAND_FOLLOW,
        BREATH_FIRST_SEARCH,
};

enum AgentOrientation{
    X_PLUS,
    Z_PLUS,
    X_MINUS,
    Z_MINUS
};

enum AgentDirection {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

class Agent
{

public:
    Agent(mcpp::Coordinate &startLoc);
    bool canMove(int x, int z, AgentDirection& dir, mcpp::Coordinate &loc, mcpp::MinecraftConnection* mc);
    AgentDirection turn(AgentDirection& dir);
    ~Agent();
    void setStartLoc(mcpp::Coordinate &startLoc);
    mcpp::Coordinate getStartLoc();
    AgentDirection getDirection() {return dir;}
    void setDirection(AgentDirection dir) {this->dir = dir;}
    mcpp::Coordinate Agent::findNeighbour(AgentDirection& dir, mcpp::Coordinate& loc, 
                                         mcpp::MinecraftConnection* mc);

private:
    /* data */
    mcpp::Coordinate startLoc;
    AgentDirection dir;
};



#endif //ASSIGN3_AGENT_H