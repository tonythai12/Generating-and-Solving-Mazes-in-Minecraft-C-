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
 *  Custom data structure for storing an agent's coordinates and direction faced.
 *  This struct is used in the AllVisited() function, to determine if a particular tile has been
 *  visited more than N times.
 */
struct CoordDir {
    mcpp::Coordinate coord;
    AgentDirection dir;
};

Maze* ReadMazeFromTerminal(mcpp::MinecraftConnection* mc, Maze*& terminalMaze);
void GenerateRandomMaze();
void SolveMaze(mcpp::MinecraftConnection* mc, Agent*& player);
void SolveTile(Agent*& player, AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos,
                mcpp::MinecraftConnection* mc);
void HighlightSolvedBlock(const mcpp::Coordinate &playerPos, mcpp::MinecraftConnection* mc);
void PrintSteps(int &counter, const mcpp::Coordinate &playerPos);
void UpdateCoordsAfterSolving(const AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos);
std::string coordDirToKey(const CoordDir& cd);
bool AllVisited(const mcpp::Coordinate cd, const AgentDirection &dir, std::vector<CoordDir> &visitedTiles);
void SolveManually(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, Agent*& player);
int getValidInt(const std::string& errorMsg);
bool validateMazeDimensions(const std::vector<std::string>& rows, int envLength, int envWidth);

int main(void) {

    // bool mode = NORMAL_MODE;
    //read Mode

    mcpp::MinecraftConnection* mc = new mcpp::MinecraftConnection();
    Maze* terminalMaze = nullptr;
    Agent* player = nullptr;
    std::vector<std::vector<std::vector<mcpp::BlockType>>> environment;

    printStartText();
    
    int input;

    States curState = ST_Main;

    while (curState != ST_Exit) {
        if (curState == ST_Main) {
            printMainMenu();
            std::cin >> input;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Input Error: Enter an integer between 1 and 5 ...." << std::endl;
            } else if (input == 1) {
                curState = ST_GetMaze;
            } else if (input == 2) {
                if (terminalMaze) {
                    environment = terminalMaze->getEnvironment(mc);
                    terminalMaze->FlattenAndBuild(mc);
                } else {
                    std::cout << "Please generate a maze first." << std::endl;
                }
                curState = ST_Main;
            } else if (input == 3) {
                curState = ST_SolveMaze;
            } else if (input == 4) {
                curState = ST_Creators;
            } else if (input == 5) {
                curState = ST_Exit;
            } else {
                std::cout << "Input Error: Enter a number between 1 and 5 ...." << std::endl;
            }
        }

        while (curState == ST_GetMaze) {
            printGenerateMazeMenu();
            std::cin >> input;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Input Error: Enter an integer between 1 and 3 ...." << std::endl;
            } else if (input == 1) {
                ReadMazeFromTerminal(mc, terminalMaze);
                curState = ST_Main;
            } else if (input == 2) {
                mcpp::Coordinate basePoint;
                std::cin >> basePoint.x >> basePoint.y >> basePoint.z;
                // @ravisidhu007 change the following to match the new maze constructor
                // Maze maze(basePoint, 13, 13, NORMAL_MODE);
                // maze.generateMaze();
                curState = ST_Main;
            } else if (input == 3) {
                curState = ST_Main;
            } else {
                std::cout << "Input Error: Enter a number between 1 and 3 ...." << std::endl;
            }
        }

        while (curState == ST_SolveMaze) {
            printSolveMazeMenu();
            std::cin >> input;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Input Error: Enter an integer between 1 and 3 ...." << std::endl;

            } else if (input == 1) {
                if (terminalMaze) {
                    SolveManually(mc, terminalMaze, player);
                } else {
                    std::cout << "Please generate a maze first." << std::endl;
                }
                curState = ST_Main;
            } else if (input == 2) {
                if (player) {
                    SolveMaze(mc, player);
                } else {
                    std::cout << "Please initialise a player first." << std::endl;
                }
                curState = ST_Main;
            } else if (input == 3) {
                curState = ST_Main;
            } else {
                std::cout << "Input Error: Enter a number between 1 and 5 ...." << std::endl;
            }
        }

        if (curState == ST_Creators) {
            printTeamInfo();
            curState = ST_Main;
        }
    }
    printExitMassage();

    if (terminalMaze) {
        // terminalMaze->UndoMaze(mc);
        terminalMaze->flattenEnvironment(mc);
        terminalMaze->rebuildEnvironment(mc, environment);
        delete terminalMaze;
    }
    if (player) {
        delete player;
    }
    delete mc;
    mc = nullptr;
    terminalMaze = nullptr;
    player = nullptr;
    return EXIT_SUCCESS;

}

int getValidInt(const std::string& errorMsg) {
    int input;
    while (true) {
        std::cin >> input;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << errorMsg << std::endl;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer
            return input;
        }
    }
}

bool validateMazeDimensions(const std::vector<std::string>& rows, int envLength, int envWidth) {
    bool isValid = true;

    if (rows.size() != envLength) {
        isValid = false;
    } else {
        for (const auto& row : rows) {
            if (isValid && row.length() != envWidth) {
                isValid = false;
            }
        }
    }
    return isValid;
}
// Tony
Maze* ReadMazeFromTerminal(mcpp::MinecraftConnection* mc, Maze*& terminalMaze) {
    // Base point
    int x, y, z;
    std::cout << "Enter the basePoint of maze:" << std::endl;
    x = getValidInt("Input Error: Enter an integer:");
    y = getValidInt("Input Error: Enter an integer:");
    z = getValidInt("Input Error: Enter an integer:");
    mcpp::Coordinate basePoint = mcpp::Coordinate(x, y + 1, z);

    // Length and Width
    int envLength, envWidth;
    std::cout << "Enter the length and width of maze:" << std::endl;

    do {
        envLength = getValidInt("Input Error: Enter an odd-numbered integer. ");
        envWidth = getValidInt("Input Error: Enter an odd-numbered integer. ");
        bool isNotOddInput = false;

        if (envLength % 2 == 0 || envLength <= 0) {
            std::cout << "Input Error: Length must be a positive odd integer. ";
            isNotOddInput = true;
        }
        
        if (envWidth % 2 == 0 || envWidth <= 0) {
            std::cout << "Input Error: Width must be an odd-numbered integer. ";
            isNotOddInput = true;
        }

        if (isNotOddInput) {
            std::cout << "Please re-enter both length and width:" << std::endl;
        }

    } while (envLength % 2 == 0 || envWidth % 2 == 0);
    
    std::vector<std::vector<char>> mazeStructure;
    std::vector<std::string> rows;

    bool validMaze = false;
    while (!validMaze) { // Outer loop to repeat until a valid maze is entered
        std::cout << "Enter the environment structure:" << std::endl;
        rows.clear();
        
        for (int i = 0; i < envLength; ++i) {
            std::string row;
            std::getline(std::cin, row);
            rows.push_back(row);
        }

        if (validateMazeDimensions(rows, envLength, envWidth)) {
            validMaze = true;  // Update the control variable to exit the loop
        } else {
            std::cout << "The dimensions of the maze structure you entered do not match the expected dimensions. Please re-enter." << std::endl;
        }
    }
    // Resize 2d vector into correct dimensions
    mazeStructure.resize(envLength, std::vector<char>(envWidth)); 

    for (int i = 0; i < envLength; i++) {
        for (int j = 0; j < envWidth; j++) {
            mazeStructure[i][j] = rows[i][j];
        }
    }
    terminalMaze = new Maze(basePoint, envLength, envWidth, mazeStructure);
    std::cout << "Maze read successfully" << std::endl;
    terminalMaze->PrintMaze();

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
void SolveMaze(mcpp::MinecraftConnection* mc, Agent*& player) {
    // mcpp::MinecraftConnection mc;
    int counter = 1;

    // Find player position
    mcpp::Coordinate playerPos = player->getStartLoc();

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
void SolveTile(Agent*& player, AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos,
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

void SolveManually(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, Agent*& player) {
    // Initialise random seed
    std::srand(std::time(0));

    // Retrieve required maze data
    mcpp::Coordinate basePoint = terminalMaze->getBasePoint();
    int length = terminalMaze->getLength();
    int width = terminalMaze->getWidth();

    // Add all 'walkable' tiles to a vector
    std::vector<mcpp::Coordinate> walkableCoords;

    mcpp::Coordinate corner1 = mcpp::Coordinate(basePoint.x, basePoint.y + 1, basePoint.z);
    mcpp::Coordinate corner2 = mcpp::Coordinate(basePoint.x + length, basePoint.y + 1, basePoint.z + width);
    auto blocks = mc->getBlocks(corner1, corner2);

    int yLen = blocks.size();
    int xLen = blocks[0].size();
    int zLen = blocks[0][0].size(); 

    for (int y = 0; y < yLen; y++) {
        for (int x = 0; x < xLen; x++) {
            for (int z = 0; z < zLen; z++) {
                if (blocks[y][x][z] == mcpp::Blocks::AIR) {
                    walkableCoords.push_back(mcpp::Coordinate(basePoint.x + x, basePoint.y, basePoint.z + z));
                }
            }
        }
    }

    // Set player position to a random walkable tile & allow them to solve manually
    if (!walkableCoords.empty()) {
        int randomIndex = std::rand() % walkableCoords.size();
        mcpp::Coordinate walkableTile = walkableCoords[randomIndex];
        mc->setPlayerPosition(walkableTile);
        player = new Agent(walkableTile);
    } else {
        std::cout << "No walkable tile found!" << std::endl;
    }
}
