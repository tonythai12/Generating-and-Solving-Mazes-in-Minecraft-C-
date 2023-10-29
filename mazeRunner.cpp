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

void ReadMazeFromTerminal(mcpp::MinecraftConnection* mc, Maze*& terminalMaze);
void GenerateRandomMaze();
void SolveMaze(mcpp::MinecraftConnection* mc, Agent*& player);
void SolveTile(Agent*& player, AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos,
                mcpp::MinecraftConnection* mc);
void HighlightSolvedBlock(const mcpp::Coordinate &playerPos, mcpp::MinecraftConnection* mc);
void PrintSteps(int &counter, const mcpp::Coordinate &playerPos);
void UpdateCoordsAfterSolving(const AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos);
std::string coordDirToKey(const CoordDir& cd);
bool AllVisited(const mcpp::Coordinate cd, const AgentDirection &dir, std::vector<CoordDir>&        
                visitedTiles, mcpp::MinecraftConnection* mc);
void SolveManually(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, Agent*& player);
std::vector<int> getValidInts(const std::string& errorMsg);
bool validateMazeDimensions(const std::vector<std::string>& rows, int envLength, int envWidth);
void InitialisePlayer(mcpp::MinecraftConnection* mc, mcpp::Coordinate& startLoc, AgentDirection& dir);
bool CheckIfSolved(mcpp::Coordinate& coord, mcpp::MinecraftConnection* mc, AgentDirection dir);
bool validateMazeCharacters(const std::vector<std::string>& rows);

int main(int argc, char* argv[]) {

    // bool mode = NORMAL_MODE;
    
    // // Check for testing mode
    // for (int i = 1; i < argc; i++) {  
    //     std::string arg = argv[i];
    //     if (arg == "-testmode") {
    //         mode = TESTING_MODE;
    //     }
    // }

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
        terminalMaze->RestoreOldBlocksFirst(mc);
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

std::vector<int> getValidInts(const std::string& errorMsg) {
    std::string line;
    std::vector<int> inputs;

    while (true) {
        std::getline(std::cin, line);
        std::stringstream ss(line);

        int temp;
        bool fail = false;


        while (ss >> temp) {
            inputs.push_back(temp);
        }

        if (ss.fail() && !ss.eof()) {
            // Parsing failed before reaching end of string
            fail = true;
        }

        if (fail) {
            std::cout << errorMsg << std::endl;
        } else {
            return inputs;
        }
    }
}

bool validateMazeDimensions(const std::vector<std::string>& rows, int envLength, int envWidth) {
    bool isValid = true;

    if (static_cast<int>(rows.size()) != envLength) {
        isValid = false;
    } else {
        for (const auto& row : rows) {
            if (isValid && static_cast<int>(row.length()) != envWidth) {
                isValid = false;
            }
        }
    }
    return isValid;
}
bool validateMazeCharacters(const std::vector<std::string>& rows) {
    bool valid = true; // Initially assume that it's valid, then check for invalid characters
    for (const auto& row : rows) {
        for (char c : row) {
            if (c != 'x' && c != '.') {
                return false;
            }
        }
    }
    return valid;
}
// Tony
void ReadMazeFromTerminal(mcpp::MinecraftConnection* mc, Maze*& terminalMaze) {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::vector<int> inputs;

    do {
        std::cout << "Enter the basePoint of maze:" << std::endl;
        inputs = getValidInts("Input Error: Enter 3 integers (e.g., \"103 105 100\").\nEnter the basePoint of the maze:");
        if (inputs.size() != 3) {
            std::cout << "Input Error: You must enter exactly 3 integers for the basePoint." << std::endl;
        }
    } while (inputs.size() != 3);
    
    mcpp::Coordinate basePoint = mcpp::Coordinate(inputs[0], inputs[1] + 1, inputs[2]);
    
    std::cout << "Enter the length and width of maze:" << std::endl;
    inputs.clear();

    do {
        inputs = getValidInts("Input Error: Enter an odd-numbered integer. ");
        bool isNotOddInput = false;

        if (inputs[0] % 2 == 0 || inputs[0] <= 0) {
            std::cout << "Input Error: Length must be a positive odd integer. ";
            isNotOddInput = true;
        }
        
        if (inputs[1] % 2 == 0 || inputs[1] <= 0) {
            std::cout << "Input Error: Width must be an odd-numbered integer. ";
            isNotOddInput = true;
        }

        if (isNotOddInput) {
            std::cout << "Please re-enter both length and width:" << std::endl;
        }

    } while (inputs[0] % 2 == 0 || inputs[1] % 2 == 0);
    
    int envLength = inputs[0], envWidth = inputs[1];
    
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

        if (validateMazeDimensions(rows, envLength, envWidth) && validateMazeCharacters(rows)) {
            validMaze = true;  // Update the control variable to exit the loop
        } else {
            std::cout << "Invalid maze structure. Please ensure that dimensions are correct";
            std::cout << " and only 'x' and '.' characters are used." << std::endl;
            std::cout << "'x' represents a wall, and '.' represents a walkable tile." << std::endl;
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
    AgentDirection dir = UP; // placeholder

    // Initialise variables for x and z coordinates
    int x = playerPos.x;
    int z = playerPos.z;

    // Condition for while-loop
    bool solvedMaze = false;

    // Initialise player
    InitialisePlayer(mc, playerPos, dir);

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
        solvedMaze = AllVisited(playerPos, dir, visitedTiles, mc);
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

            // Store the original direction just in-case
            AgentDirection originalDir = dir;
            
            // Force it to turn right 3 times (effectively left turn relative to current dir)
            for (int i = 0; i < 3; ++i) {
                dir = player->turn(dir);
            }

            // Now check if it can move in this new direction
            if (player->canMove(x, z, dir, playerPos, mc)) {
                moved = true;
            } else {
                // If it still can't move, revert to original direction and then turn around
                std::cout << "Turning around." << std::endl;
                dir = originalDir;
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
bool AllVisited(mcpp::Coordinate coord, const AgentDirection &dir, std::vector<CoordDir> &  
                visitedTiles, mcpp::MinecraftConnection* mc) {
    CoordDir currentTile {coord, dir};
    visitedTiles.push_back(currentTile);

    bool isSolved = false;

    isSolved = CheckIfSolved(coord, mc, dir);
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
            mc->postToChat("Detected loop in maze. Solving algorithm stopped.");
                std::cout << "Detected loop in maze. Solving algorithm stopped." << std::endl;
                isSolved = true; // Exit condition
        }
    }
    return isSolved;
}

bool CheckIfSolved(mcpp::Coordinate& coord, mcpp::MinecraftConnection* mc, AgentDirection dir) {
    bool solved = true; // Initially assume it's solved
    std::vector<mcpp::Coordinate> front_coords;

    if (dir == UP) {
        front_coords = {
            mcpp::Coordinate(coord.x + 1, coord.y, coord.z),
            mcpp::Coordinate(coord.x + 1, coord.y, coord.z + 1),
            mcpp::Coordinate(coord.x + 1, coord.y, coord.z - 1),
            mcpp::Coordinate(coord.x, coord.y, coord.z + 1),
            mcpp::Coordinate(coord.x, coord.y, coord.z - 1)
        };
    } else if (dir == RIGHT) {
        front_coords = {
            mcpp::Coordinate(coord.x, coord.y, coord.z + 1),
            mcpp::Coordinate(coord.x + 1, coord.y, coord.z + 1),
            mcpp::Coordinate(coord.x - 1, coord.y, coord.z + 1),
            mcpp::Coordinate(coord.x + 1, coord.y, coord.z),
            mcpp::Coordinate(coord.x - 1, coord.y, coord.z)
        };
    } else if (dir == DOWN) {
        front_coords = {
            mcpp::Coordinate(coord.x - 1, coord.y, coord.z),
            mcpp::Coordinate(coord.x - 1, coord.y, coord.z + 1),
            mcpp::Coordinate(coord.x - 1, coord.y, coord.z - 1),
            mcpp::Coordinate(coord.x, coord.y, coord.z + 1),
            mcpp::Coordinate(coord.x, coord.y, coord.z - 1)
        };
    } else if (dir == LEFT) {
        front_coords = {
            mcpp::Coordinate(coord.x, coord.y, coord.z - 1),
            mcpp::Coordinate(coord.x + 1, coord.y, coord.z - 1),
            mcpp::Coordinate(coord.x - 1, coord.y, coord.z - 1),
            mcpp::Coordinate(coord.x + 1, coord.y, coord.z),
            mcpp::Coordinate(coord.x - 1, coord.y, coord.z)
        };
    } else {
        std::cout << "Error: Unable to find the solution to the maze. Exiting ... " << std::endl;
    }

    for (const auto& front_coord : front_coords) {
        mcpp::BlockType block = mc->getBlock(front_coord);
        if (!(block == mcpp::Blocks::AIR)) {
            solved = false; // One of the blocks is not air, not solved
        }
    }

    if (solved) {
        std::cout << "Maze solved!" << std::endl;
        mc->postToChat("Maze solved!");
    }
    
    return solved;
}

void SolveManually(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, Agent*& player) {
    // Initialise random seed
    std::srand(std::time(0));

    // Get all walkable tiles from private member variable
    std::vector<mcpp::Coordinate> coords = terminalMaze->getWalkableCoords();

    // Set player position to a random walkable tile & allow them to solve manually
    if (!coords.empty()) {
        int randomIndex = std::rand() % coords.size();
        std::cout << "Random index: " << randomIndex << std::endl;
        mcpp::Coordinate walkableTile = coords[randomIndex];
        std::cout << "Teleporting to: " << walkableTile.x << ", " << walkableTile.y << ", " << walkableTile.z << std::endl;
        mc->setPlayerPosition(walkableTile);
        player = new Agent(walkableTile);
    } else {
        std::cout << "No walkable tile found! Please generate another maze." << std::endl;
    }
}

void InitialisePlayer(mcpp::MinecraftConnection* mc, mcpp::Coordinate& startLoc, AgentDirection& dir) {
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
