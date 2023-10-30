#include "Agent.h"

Agent::Agent(mcpp::Coordinate &startLoc)
{
    this->startLoc = startLoc;
}

/**
 * Checks if an agent can move in a given direction. Achieved through checking if the block in the 
 * given direction is an air block. If it is, the agent can move. If not, the agent cannot move and 
 * will need to change its direction using turn().
 * Used in the SolveTile function.
 * @param x The x coordinate of the agent
 * @param z The z coordinate of the agent
 * @param dir The direction the agent is facing
 * @param loc The location of the agent
 * @param mc The Minecraft connection
 * @return True if the agent can move, false otherwise.
*/
bool Agent::canMove(int x, int z, AgentDirection& dir, mcpp::Coordinate &loc, mcpp::MinecraftConnection* mc) {
    
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

mcpp::Coordinate Agent::findNeighbour(AgentDirection& dir, mcpp::Coordinate& loc, 
                                     mcpp::MinecraftConnection* mc) {
    mcpp::Coordinate neighbour(0, 0, 0);

    if (dir == UP) {
        neighbour = mcpp::Coordinate(loc.x + 1, loc.y, loc.z);
    } else if (dir == RIGHT) {
        neighbour = mcpp::Coordinate(loc.x, loc.y, loc.z + 1);
    } else if (dir == DOWN) {
        neighbour = mcpp::Coordinate(loc.x - 1, loc.y, loc.z);
    } else if (dir == LEFT) {
        neighbour = mcpp::Coordinate(loc.x, loc.y, loc.z - 1);
    }
    return neighbour;
}

/**
 * Turns the agent in a given direction. Achieved through incrementing the direction by 1,
 * and then modding by 4 to ensure the direction is within the range of 0-3.
 * Used inside of the SolveTile function.
 * @param dir The direction the agent is facing
 * @return The new direction the agent is facing
*/
AgentDirection Agent::turn(AgentDirection& dir) {
    return static_cast<AgentDirection>((dir + 1) % 4);
}

/**
 * Sets the start location of the agent.
 * @param startLoc The start location of the agent
*/
void Agent::setStartLoc(mcpp::Coordinate &startLoc) {
    this->startLoc = startLoc;
}

/**
 * Gets the start location of the agent.
 * @return The start location of the agent
*/
mcpp::Coordinate Agent::getStartLoc() {
    return startLoc;
}

Agent::~Agent()
{
    std::cout << "Agent destructor successfully called." << std::endl;
}