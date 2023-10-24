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

Agent::~Agent()
{
    std::cout << "Agent destroyed" << std::endl;
}