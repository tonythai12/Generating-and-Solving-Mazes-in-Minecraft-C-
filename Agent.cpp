#include "Agent.h"

Agent::Agent(mcpp::Coordinate &startLoc)
{
    this->startLoc = startLoc;
}

bool Agent::canMove(int x, int z, AgentDirection dir, mcpp::Coordinate &loc) {

    mcpp::MinecraftConnection mc;
    
    // Initialise default blockType
    mcpp::BlockType blockType(1);

    if (dir == UP) {
        blockType = mc.getBlock(mcpp::Coordinate(loc.x + 1, loc.y, loc.z));
    } else if (dir == RIGHT) {
        blockType = mc.getBlock(mcpp::Coordinate(loc.x, loc.y, loc.z + 1));
    } else if (dir == DOWN) {
        blockType = mc.getBlock(mcpp::Coordinate(loc.x - 1, loc.y, loc.z));
    } else if (dir == LEFT) {
        blockType = mc.getBlock(mcpp::Coordinate(loc.x, loc.y, loc.z - 1));
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