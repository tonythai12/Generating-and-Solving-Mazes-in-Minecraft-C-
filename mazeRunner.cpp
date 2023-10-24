#include <iostream>

#include <mcpp/mcpp.h>

#include "menuUtils.h"
#include "Maze.h"
#include "Agent.h"

// For sleep and time duration
#include <thread>   
#include <chrono> 

// For tracking tiles that have been solved (for looped mazes)
#include <map>
#include <algorithm>

#define NORMAL_MODE 0
#define TESTING_MODE 1

enum States{
    ST_Main,
    ST_GetMaze,
    ST_SolveMaze,
    ST_Creators,
    ST_Exit
};

/*
    Custom data structure for storing an agent's coordinates and direction faced.
    This struct is used in the AllVisited() function, to determine if a particular tile has been
    visited more than N times.
*/
struct CoordDir {
    mcpp::Coordinate coord;
    AgentDirection dir;
};

Maze* ReadMazeFromTerminal(mcpp::MinecraftConnection* mc, Maze* terminalMaze);
void GenerateRandomMaze();
void SolveMaze(mcpp::MinecraftConnection* mc);
void SolveTile(Agent *player, AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos,
                mcpp::MinecraftConnection* mc);
void HighlightSolvedBlock(const mcpp::Coordinate &playerPos, mcpp::MinecraftConnection* mc);
void PrintSteps(int &counter, const mcpp::Coordinate &playerPos);
void UpdateCoordsAfterSolving(const AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos);
std::string coordDirToKey(const CoordDir& cd);
bool AllVisited(const mcpp::Coordinate cd, const AgentDirection &dir, std::vector<CoordDir> &visitedTiles);
std::vector<std::vector<std::vector<mcpp::BlockType>>> getEnvironment(mcpp::Coordinate &basePoint, 
                                            mcpp::MinecraftConnection* mc, int &length, int &width);
void flattenEnvironment(const mcpp::Coordinate& corner1, const mcpp::Coordinate& corner2, 
                       mcpp::MinecraftConnection* mc);
void rebuildEnvironment(const mcpp::Coordinate& corner1, 
                        const std::vector<std::vector<std::vector<mcpp::BlockType>>>& savedEnvironment, 
                        mcpp::MinecraftConnection* mc);
void SolveManually(mcpp::MinecraftConnection* mc, Maze* terminalMaze);

int main(void){

    // bool mode = NORMAL_MODE;
    //read Mode

    mcpp::MinecraftConnection* mc = new mcpp::MinecraftConnection();
    Maze* terminalMaze = nullptr;
    printStartText();
    printMainMenu();
    
    int input;

    do {
        std::cin >> input;

        if (input == 1) {
            printGenerateMazeMenu();
            std::cin >> input;

            if (input == 1) {
                terminalMaze = ReadMazeFromTerminal(mc, terminalMaze);

            } else if (input == 2) {
                mcpp::Coordinate basePoint;
                std::cin >> basePoint.x >> basePoint.y >> basePoint.z;
                Maze maze(basePoint, 7, 5, NORMAL_MODE);
                maze.generateMaze();

            } else if (input == 3) {
                printMainMenu();

            } else {
                std::cout << "Invalid input. Please try again." << std::endl;
            }

        } else if (input == 2) {
            // Placeholder for build maze menu
            continue;

        } else if (input == 3) {
            printSolveMazeMenu();
            std::cin >> input;

            if (input == 1) {
                if (terminalMaze) {
                    SolveManually(mc, terminalMaze);
                }
                else {
                    std::cout << "Please generate a maze first." << std::endl;
                }

            } else if (input == 2) {
                SolveMaze(mc);
                //start = mc->getPlayerPosition();
                // mcpp::Coordinate coord = mc->getPlayerPosition();
                // int h = 7;
                // int w = 5;
                // mcpp::Coordinate coord2 = mcpp::Coordinate(coord.x + h, coord.y, coord.z + w);
                // auto savedEnv = getEnvironment(coord, mc, h, w);
                // flattenEnvironment(coord, coord2, mc);
                // rebuildEnvironment(coord, savedEnv, mc);
                continue;
            } else if (input == 3) {
                printMainMenu();
            } else {
                std::cout << "Invalid input. Please try again." << std::endl;
            }
        } else if (input == 4) {
            printTeamInfo();
        } else if (input == 5) {
            printExitMassage();
        } else {
            // Re-prompts the user for input repeatedly
            std::cout << "Invalid input. Please try again." << std::endl;
        }
    } while (input != 5);

    mc->doCommand("time set day"); 

    States curState = ST_Main;

    //State machine for menu        
    while (curState != ST_Exit)
    {
        //Do something
    }

    delete mc;
    if (terminalMaze) {
        delete terminalMaze;
    }
    return EXIT_SUCCESS;

}

// Tony
Maze* ReadMazeFromTerminal(mcpp::MinecraftConnection* mc, Maze* terminalMaze) {
    // Base point
    int x, y, z;
    std::cout << "Enter the basePoint of maze (X Y Z):" << std::endl;
    std::cin >> x;
    std::cin >> y;
    std::cin >> z;
    mcpp::Coordinate basePoint = mcpp::Coordinate(x, y + 1, z);

    // Length and Width
    unsigned int envLength, envWidth;
    std::cout << "Enter the size of the environment (H W):" << std::endl;
    std::cin >> envLength;
    do {
        std::cin >> envWidth;
        if (envWidth % 2 == 0) {
            std::cout << "Width must be an odd number. Please re-enter:" << std::endl;
        }
    } while (envWidth % 2 == 0);
    
    char envStructure [envLength][envWidth];
    terminalMaze->setBasePoint(basePoint);
    terminalMaze->setLength(envLength);
    terminalMaze->setWidth(envWidth);
    char readChar;

    std::cout << "Enter the environment structure:" << std::endl;

    for (int i = 0; i < envLength; i++) {
        for (int j = 0; j < envWidth; j++) {
            std::cin >> readChar;
            envStructure[i][j] = readChar;
        }
    }

    for (int i = 0; i < envLength; i++) {
        for (int j = 0; j < envWidth; j++) {
            if (envStructure[i][j] == 'x') {
                mc->setBlock(basePoint + mcpp::Coordinate(i, 0, j), mcpp::Blocks::BRICKS);
            } else {
                mc->setBlock(basePoint + mcpp::Coordinate(i, 0, j), mcpp::Blocks::AIR);
            }
        }
    }

    //TODO: Maze Structure
    // std::vector<std::string> maze;
    // std::cout << "Enter the maze structure:" << std::endl;
    // std::string row;
    // for (int i = 0; i < envLength; i++) {
    //     std::cin >> row;
    //     if (row.size() != static_cast<size_t>(envWidth)) {
    //         std::cout << "Invalid input. Row should have exactly " << envWidth<< " characters." << std::endl;
    //         i--;
    //     } else {
    //         maze.push_back(row);
    //     }
    // }

    return terminalMaze;

}
// Ravi
void GenerateRandomMaze() {
    //TODO: Generate maze and print in console

    // dummy maze for solving, assume generated
    
}

/*
    Solves the perfect or looped maze using the right-hand-follow algorithm.
*/
void SolveMaze(mcpp::MinecraftConnection* mc) {
    // mcpp::MinecraftConnection mc;
    int counter = 1;

    // Find player position
    mcpp::Coordinate playerPos = mc->getPlayerPosition();

    // Initialise player @ playerPos
    Agent *player = new Agent(playerPos);

    // Initialise variables for x and z coordinates
    int x = playerPos.x;
    int z = playerPos.z;

    // Condition for while-loop
    bool solvedMaze = false;

    // Initialise direction
    AgentDirection dir = UP;

    /* 
        Using a vector instead of a linked list to store all visitedTiles due always appending to the end
        of the vector. Further, we are occasionally checking all elements. No insertions or deletions
        are occuring in arbitrary positions. As such, a vector would be more time and space efficient.
    */
    std::vector<CoordDir> visitedTiles;

    while (!solvedMaze) {
        // Solve the current tile
        SolveTile(player, dir, x, z, playerPos, mc);

        // Update coordinates for the next tile to be solved
        UpdateCoordsAfterSolving(dir, x, z, playerPos);

        // Print out the coordinates of the solved tile
        PrintSteps(counter, playerPos);

        // Highlight the solved tile
        HighlightSolvedBlock(playerPos, mc);

        // now work out exit condition
        solvedMaze = AllVisited(playerPos, dir, visitedTiles);
    }

    delete player;
}
/*
    Prints the coordinates of the solved tile in the console.

    Part of the SolveMaze() function.
*/
void PrintSteps(int &counter, const mcpp::Coordinate &playerPos) {
    std::cout << "Step [" << counter << "]: (" << playerPos.x << ", " << playerPos.y << 
        ", " << playerPos.z << ")" << std::endl;
    counter++;
}

/* 
    After a tile has been solved, this function increments or decrements the 'x' or 'z' coordinate, 
    depending on the direction that the agent is facing. Through this, the agent is able to move.

    Part of the SolveMaze() function.
*/
void UpdateCoordsAfterSolving(const AgentDirection &dir, int &x, int &z, 
                            mcpp::Coordinate &playerPos) {

    // Updates coords after moving in direction of dir
    if (dir == UP) {
        x++;
    } else if (dir == RIGHT) {
        z++;
    } else if (dir == DOWN) {
        x--;
    } else if (dir == LEFT) {
        z--;
    }
        
    // Updates playerPos with new coordinates
    playerPos.x = x;
    playerPos.z = z;      
}

/*
    Highlights the solved tile with a lime carpet block for 1 second.
    It then removes the lime carpet block by generating an air block in place of it.

    Part of the SolveMaze() function.
*/
void HighlightSolvedBlock(const mcpp::Coordinate &playerPos, mcpp::MinecraftConnection* mc) {
    mc->setBlock(playerPos, mcpp::Blocks::LIME_CARPET);

    // 1 second sleep delay
    std::this_thread::sleep_for(std::chrono::seconds(1)); 

    // Remove highlighted block
    mc->setBlock(playerPos, mcpp::Blocks::AIR);
    std::this_thread::sleep_for(std::chrono::seconds(1)); 
}

/*
    Solves the current tile by turning right, straight, left, or turning around. This is achieved
    through the right-hand wall-follower algorithm.

    If it can turn right it will. If not, it moves straight. If it can't move straight, 
    it will try to turn left. If it can't turn left, it will turn around. 

    This function will continue to execute until AllVisited() is true.

    Part of the SolveMaze() function.
*/
void SolveTile(Agent *player, AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos,
                mcpp::MinecraftConnection* mc) {
    bool moved = false;
    while (!moved) {
        AgentDirection rightDir = player->turn(dir);

        // Try to turn right first
        if (player->canMove(x, z, rightDir, playerPos, mc)) {
            std::cout << "Turning right." << std::endl;
            dir = rightDir;
            moved = true;
        } 

        // If it can move straight, do so
        else if (player->canMove(x, z, dir, playerPos, mc)) {
            std::cout << "Moving straight." << std::endl;
            moved = true;
        } 
        else {
            std::cout << "Cannot move straight or right. Trying to turn left." << std::endl;

            // Can't move straight or right. Try to turn left
            for (int i = 0; i < 3; i++) {
                dir = player->turn(dir);
                if (player->canMove(x, z, dir, playerPos, mc)) {
                    moved = true;
                    break;
                }
            }

            // If it still can't move, it means it has to turn around
            if (!moved) {
                std::cout << "Turning around." << std::endl;
                dir = player->turn(dir);
                dir = player->turn(dir);
                moved = true;
            }
        }
    }
}

/*
    Converts a CoordDir struct to a string, which is used as a key in a map.
    This is used in the AllVisited() function.
*/
std::string coordDirToKey(const CoordDir& cd) {
    return std::to_string(cd.coord.x) + "_" + std::to_string(cd.coord.y) + "_" 
    + std::to_string(cd.coord.z) + "_" + std::to_string(cd.dir);
}

/*
    Checks if a tile has been visited more than N times with the same agent direction. 
    If so, it returns true. Otherwise, it returns false. 
    
    Used as an exit condition for SolveMaze().
*/
bool AllVisited(mcpp::Coordinate coord, const AgentDirection &dir, std::vector<CoordDir> &visitedTiles) {
    CoordDir currentTile {coord, dir};
    visitedTiles.push_back(currentTile);

    bool isSolved = false;

    std::map<std::string, int> tileCounter;
    int N = 2;

    // Count occurrences of each tile with direction
    for (const auto& tile : visitedTiles) {
        std::string key = coordDirToKey(tile);
        tileCounter[key]++;
    }

    // Check if any tile with direction has been visited more than N times
    for (const auto& entry : tileCounter) {
        if (entry.second > N) {
            isSolved = true; // Exit condition
            break;
        }
    }

    return isSolved;

}
/*
    Retrieves the blockTypes of the environment within the specified bounds.
    Stored in a 3D vector, which will be used to restore the environment after solving the maze.
*/
std::vector<std::vector<std::vector<mcpp::BlockType>>> getEnvironment(mcpp::Coordinate &basePoint, 
                                            mcpp::MinecraftConnection* mc, int &length, int &width) {
    
    // Calculate corners and then get heights
    mcpp::Coordinate corner1(basePoint.x, basePoint.y, basePoint.z);
    mcpp::Coordinate corner2(basePoint.x + length, basePoint.y, basePoint.z + width);
    auto envHeights = mc->getHeights(corner1, corner2);

    // Find min and max y-values
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();
    for (const auto& row : envHeights) {
        for (int height : row) {
            if (height < minY) minY = height;
            if (height > maxY) maxY = height;
        }
    }

    // Get all blocks using the min/max y-values
    mcpp::Coordinate loc1 = mcpp::Coordinate(basePoint.x, minY, basePoint.z);
    mcpp::Coordinate loc2 = mcpp::Coordinate(basePoint.x + length, maxY, basePoint.z + width);
    auto blocks = mc->getBlocks(loc1, loc2);

    return blocks;
}

/*
    Rebuilds the environment after the user exits the program. Uses the 3D vector returned from getEnvironment().
*/
void rebuildEnvironment(const mcpp::Coordinate& corner1, 
                        const std::vector<std::vector<std::vector<mcpp::BlockType>>>& savedEnvironment, 
                        mcpp::MinecraftConnection* mc) {
    
    // Format is [y][x][z]
    int yLen = savedEnvironment.size();
    int xLen = savedEnvironment[0].size();
    int zLen = savedEnvironment[0][0].size(); 

    for (int y = 0; y < yLen; y++) {
        for (int x = 0; x < xLen; x++) {
            for (int z = 0; z < zLen; ++z) {
                mcpp::Coordinate newCoord(corner1.x + x, corner1.y + y, corner1.z + z);
                mc->setBlock(newCoord, savedEnvironment[y][x][z]);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }
}

/*
    Flattens the environment within the specified bounds. 
*/
void flattenEnvironment(const mcpp::Coordinate& corner1, const mcpp::Coordinate& corner2, 
                       mcpp::MinecraftConnection* mc) {

    // Heights of the environment at (x, z) (as y-coords are different for each pair)
    auto heights = mc->getHeights(corner1, corner2);
    int floorLevel = corner1.y;
    
    // Assume [x][z] for testing
    for (int x = 0; x < heights.size(); x++) {
        for (int z = 0; z < heights[x].size(); z++) {
            int highestBlockY = heights[x][z];

            // Set blocks above the floorLevel and up to the highest block to AIR
            for (int y = floorLevel + 1; y <= highestBlockY; y++) {
                mcpp::Coordinate coord(corner1.x + x, y, corner1.z + z);
                mc->setBlock(coord, mcpp::Blocks::AIR);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            // Set the block at floorLevel to GRASS
            mcpp::Coordinate coord(corner1.x + x, floorLevel, corner1.z + z);
            mc->setBlock(coord, mcpp::Blocks::GRASS);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // Set blocks below floorLevel to GRASS
            for (int y = highestBlockY; y < floorLevel; y++) {
                mcpp::Coordinate coord(corner1.x + x, y, corner1.z + z);
                mc->setBlock(coord, mcpp::Blocks::GRASS);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }
}

void SolveManually(mcpp::MinecraftConnection* mc, Maze* terminalMaze) {
    // Initialise random seed
    std::srand(std::time(0));

    // Retrieve required maze data
    mcpp::Coordinate basePoint = terminalMaze->getBasePoint();
    int length = terminalMaze->getLength();
    int width = terminalMaze->getWidth();

    // Add all 'walkable' tiles to a vector
    std::vector<mcpp::Coordinate> walkableCoords;
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            mcpp::Coordinate currentCoord = basePoint + mcpp::Coordinate(i, 0, j);
            if (mc->getBlock(currentCoord) == mcpp::Blocks::AIR) {
                walkableCoords.push_back(currentCoord);
            }
        }
    }

    // Set player position to a random walkable tile & allow them to solve manually
    if (!walkableCoords.empty()) {
        int randomIndex = std::rand() % walkableCoords.size();
        mc->setPlayerPosition(walkableCoords[randomIndex]);

    } else {
        std::cout << "No walkable tile found!" << std::endl;
    }


}
