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

void ReadMazeFromTerminal(); // option 1 in generate maze menu
void GenerateRandomMaze(); // option 2 in generate maze menu
void SolveMaze();
void SolveTile(Agent *player, AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos);
void HighlightSolvedBlock(const mcpp::Coordinate &playerPos, mcpp::MinecraftConnection &mc);
void PrintSteps(int &counter, const mcpp::Coordinate &playerPos);
void UpdateCoordsAfterSolving(const AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos);
std::string coordDirToKey(const CoordDir& cd);
bool AllVisited(const mcpp::Coordinate cd, const AgentDirection &dir, std::vector<CoordDir> &visitedTiles);
void GetEnvStructure(mcpp::Coordinate &basePoint, mcpp::MinecraftConnection &mc, int &length, int &width);
void RemoveEnvironment();
void RestoreEnvironment();

int main(void){

    // bool mode = NORMAL_MODE;
    //read Mode

    mcpp::MinecraftConnection mc;
    printStartText();
    printMainMenu();
    
    int input;

    do {
        std::cin >> input;
        if (input == 1) {
            printGenerateMazeMenu();
            std::cin >> input;

            if (input == 1) {
                ReadMazeFromTerminal();
            } else if (input == 2) {
                mcpp::Coordinate basePoint;
                std::cin >> basePoint.x >> basePoint.y >> basePoint.z;
                // basePoint.y = mc.getHeight(basePoint.x, basePoint.z) + 1; // Set basePoint to be on top of the ground
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
                SolveMaze();
            } else if (input == 2) {
                // Placeholder for show escape route
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

    mc.doCommand("time set day"); 

    States curState = ST_Main;

    //State machine for menu        
    while (curState != ST_Exit)
    {
        //Do something
    }


    return EXIT_SUCCESS;

}

// Tony
void ReadMazeFromTerminal() {
    // Base point
    mcpp::MinecraftConnection mc;
    int x, y, z;
    std::cout << "Enter the basePoint of maze:" << std::endl;
    std::cin >> x >> y >> z;
    mcpp::Coordinate basePoint = mcpp::Coordinate(x, y + 1, z);

    // Length and Width
    int envLength, envWidth;
    std::cin >> envLength;
    do {
        std::cin >> envWidth;
        if (envWidth % 2 == 0) {
            std::cout << "Width must be an odd number. Please re-enter:" << std::endl;
        }
    } while (envWidth % 2 == 0);
    
    
    char envStructure [envLength][envWidth];
    char readChar;

    for (int i = 0; i < envLength; i++) {
        for (int j = 0; j < envLength; j++) {
            std::cin >> readChar;
            envStructure[i][j] = readChar;

        }
    }
    for (int i = 0; i < envLength; i++) {
        for (int j = 0; j < envWidth; j++) {
        if (envStructure[i][j] == 'x') {
            mc.setBlock(basePoint + mcpp::Coordinate(i, 0, j), mcpp::Blocks::BRICKS);
        } else {
            mc.setBlock(basePoint + mcpp::Coordinate(i, 0, j), mcpp::Blocks::AIR);
        }
    }
 }

    //TODO: Maze Structure
    std::vector<std::string> maze;
    std::cout << "Enter the maze structure:" << std::endl;
    std::string row;
    for (int i = 0; i < envLength; i++) {
        std::cin >> row;
        if (row.size() != static_cast<size_t>(envWidth)) {
            std::cout << "Invalid input. Row should have exactly " << envWidth<< " characters." << std::endl;
            i--;
        } else {
            maze.push_back(row);
        }
    }

}
// Ravi
void GenerateRandomMaze() {
    //TODO: Generate maze and print in console

    // dummy maze for solving, assume generated
    
}

/*
    Solves the perfect or looped maze using the right-hand-follow algorithm.
*/
void SolveMaze() {
    mcpp::MinecraftConnection mc;
    int counter = 1;

    // Find player position
    mcpp::Coordinate playerPos = mc.getPlayerPosition();

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
        SolveTile(player, dir, x, z, playerPos);

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
void HighlightSolvedBlock(const mcpp::Coordinate &playerPos, mcpp::MinecraftConnection &mc) {
    mc.setBlock(playerPos, mcpp::Blocks::LIME_CARPET);

    // 1 second sleep delay
    std::this_thread::sleep_for(std::chrono::seconds(1)); 

    // Remove highlighted block
    mc.setBlock(playerPos, mcpp::Blocks::AIR);
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

void SolveTile(Agent *player, AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos) {
    bool moved = false;
    while (!moved) {
        AgentDirection rightDir = player->turn(dir);

        // Try to turn right first
        if (player->canMove(x, z, rightDir, playerPos)) {
            std::cout << "Turning right." << std::endl;
            dir = rightDir;
            moved = true;
        } 

        // If it can move straight, do so
        else if (player->canMove(x, z, dir, playerPos)) {
            std::cout << "Moving straight." << std::endl;
            moved = true;
        } 
        else {
            std::cout << "Cannot move straight or right. Trying to turn left." << std::endl;

            // Can't move straight or right. Try to turn left
            for (int i = 0; i < 3; i++) {
                dir = player->turn(dir);
                if (player->canMove(x, z, dir, playerPos)) {
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
                                            mcpp::MinecraftConnection &mc, int &length, int &width) {
    
    // Calculate corners and then get heights
    mcpp::Coordinate corner1(basePoint.x, 0, basePoint.z);
    mcpp::Coordinate corner2(basePoint.x + length, 0, basePoint.z + width);
    auto envHeights = mc.getHeights(corner1, corner2);

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
    mcpp::Coordinate basePoint2 = mcpp::Coordinate(basePoint.x + length, basePoint.y, basePoint.z + width);
    return mc.getBlocks(basePoint, basePoint2);
}

/*
    Rebuilds the environment after the user exits the program. Uses the 3D vector returned from getEnvironment().
*/
void rebuildEnvironment(const mcpp::Coordinate& corner1, 
                        const std::vector<std::vector<std::vector<mcpp::BlockType>>>& savedEnvironment, 
                        mcpp::MinecraftConnection& mc) {
    
    // Assume [x][y][z]
    int xLen = savedEnvironment.size();
    int yLen = savedEnvironment[0].size(); // Assumes non-empty nested vectors
    int zLen = savedEnvironment[0][0].size(); // Assumes non-empty nested vectors

    for (int x = 0; x < xLen; x++) {
        for (int y = 0; y < yLen; y++) {
            for (int z = 0; z < zLen; ++z) {
                mcpp::Coordinate newCoord(corner1.x + x, corner1.y + y, corner1.z + z);
                mc.setBlock(newCoord, savedEnvironment[x][y][z]);
            }
        }
    }
}