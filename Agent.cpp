#include "Agent.h"

Agent::Agent(mcpp::Coordinate &startLoc)
{
    this->startLoc = startLoc;
}
/*
    Checks if an agent can move in a given direction. 

    Achieved through checking if the block in the given direction is an air block. 
    If it is, the agent can move. If not, the agent cannot move and will need to turn using turn().

    Used inside of SolveTile().

*/
bool Agent::canMove(int x, int z, AgentDirection dir, mcpp::Coordinate &loc, mcpp::MinecraftConnection* mc) {
    
    // Initialise default blockType
    mcpp::BlockType blockType(1);

    if (dir == UP) {
        blockType = mc->getBlock(mcpp::Coordinate(loc.x + 1, loc.y, loc.z));
    } else if (dir == RIGHT) {
        blockType = mc->getBlock(mcpp::Coordinate(loc.x, loc.y, loc.z + 1));
    } else if (dir == DOWN) {
        blockType = mc->getBlock(mcpp::Coordinate(loc.x - 1, loc.y, loc.z));
    } else if (dir == LEFT) {
        blockType = mc->getBlock(mcpp::Coordinate(loc.x, loc.y, loc.z - 1));
    } else {
        return false;
    }

    // If an air block is returned, canMove == true
    return blockType == mcpp::Blocks::AIR;
}

/*
    Turns the agent in a given direction. 

    Achieved through incrementing the direction by 1, 
    and then modding by 4 to ensure the direction is within the range of 0-3.

    Used inside of SolveTile().
*/
AgentDirection Agent::turn(AgentDirection dir) {
    return static_cast<AgentDirection>((dir + 1) % 4);
}

void Agent::setStartLoc(mcpp::Coordinate &startLoc) {
    this->startLoc = startLoc;
}

mcpp::Coordinate Agent::getStartLoc() {
    return startLoc;
}
void Agent::InitialisePlayer(mcpp::MinecraftConnection* mc) {
    mcpp::BlockType blockTypeEast = mc->getBlock(mcpp::Coordinate(startLoc.x + 1, startLoc.y, startLoc.z));
    mcpp::BlockType blockTypeWest = mc->getBlock(mcpp::Coordinate(startLoc.x - 1, startLoc.y, startLoc.z));
    mcpp::BlockType blockTypeNorth = mc->getBlock(mcpp::Coordinate(startLoc.x, startLoc.y, startLoc.z - 1));
    mcpp::BlockType blockTypeSouth = mc->getBlock(mcpp::Coordinate(startLoc.x, startLoc.y, startLoc.z + 1));

    if (blockTypeEast == mcpp::Blocks::BRICKS) {
        // Wall is to the Right, so face North (up)
        dir = UP;
    } else if (blockTypeWest == mcpp::Blocks::BRICKS) {
        // Wall is to the Left, so face South (down)
        dir = DOWN;
    } else if (blockTypeNorth == mcpp::Blocks::BRICKS) {
        // Wall is to the North, so face West (left)
        dir = LEFT;
    } else if (blockTypeSouth == mcpp::Blocks::BRICKS) {
        // Wall is to the South, so face East (right)
        dir = RIGHT;
    }
}

Agent::~Agent()
{
    std::cout << "Agent destructor successfully called." << std::endl;
}