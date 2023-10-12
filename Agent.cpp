#include "Agent.h"

Agent::Agent(mcpp::Coordinate startLoc)
{
    this->startLoc = startLoc;
}

bool Agent::canMove(int x, int z, AgentDirection dir) {
    
    // Initialise default blockType
    mcpp::BlockType blockType(1);

    if (dir == UP) {
        blockType = mc.getBlock(mcpp::Coordinate(startLoc.x + 1, startLoc.y, startLoc.z));
    } else if (dir == RIGHT) {
        blockType = mc.getBlock(mcpp::Coordinate(startLoc.x, startLoc.y, startLoc.z + 1));
    } else if (dir == DOWN) {
        blockType = mc.getBlock(mcpp::Coordinate(startLoc.x - 1, startLoc.y, startLoc.z));
    } else if (dir == LEFT) {
        blockType = mc.getBlock(mcpp::Coordinate(startLoc.x, startLoc.y, startLoc.z - 1));
    } else {
        return false;
    }

    // If an air block is returned, canMove == true
    return blockType == mcpp::Blocks::AIR;
}

AgentDirection Agent::turn(AgentDirection dir) {
    return static_cast<AgentDirection>((dir + 1) % 4);
}

Agent::~Agent()
{
    std::cout << "Agent destroyed" << std::endl;
}