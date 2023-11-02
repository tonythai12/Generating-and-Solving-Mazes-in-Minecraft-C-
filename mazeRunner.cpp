#include <iostream>
#include <mcpp/mcpp.h>
#include "menuUtils.h"
#include "Maze.h"
#include "Agent.h"
#include <thread>   
#include <chrono> 
#include <map>
#include <algorithm>
#include <queue>
#include <set>
#include <tuple>

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
 *  visited more than N times. Used as a failsafe to detect loops in the maze.
 */
struct CoordDir {
    mcpp::Coordinate coord;
    AgentDirection dir;
};

/**
 * @struct CoordDirComparator
 * @brief Custom comparator for CoordDir objects.
 *
 * This struct provides a custom comparison operator for objects of type CoordDir.
 * It is designed to provide a total ordering based on the x, y, and z coordinates, 
 * as well as the direction (dir) member of CoordDir objects.
 */
struct CoordDirComparator {
    /**
     * @brief Overloaded function call operator for comparing two CoordDir objects.
     *
     * @param a The first CoordDir object.
     * @param b The second CoordDir object.
     * @return True if 'a' should come before 'b', false otherwise.
     */
    bool operator()(const CoordDir& a, const CoordDir& b) const {
        if(a.coord.x != b.coord.x) return a.coord.x < b.coord.x;
        if(a.coord.y != b.coord.y) return a.coord.y < b.coord.y;
        if(a.coord.z != b.coord.z) return a.coord.z < b.coord.z;
        return a.dir < b.dir;
    }
};

/**
 * @struct CoordinateComparator
 * @brief Custom comparator for mcpp::Coordinate objects.
 *
 * This struct provides a custom comparison operator for objects of type mcpp::Coordinate.
 * It offers a total ordering based on the x, y, and z coordinates of the mcpp::Coordinate objects.
 */
struct CoordinateComparator {
    /**
     * @brief Overloaded function call operator for comparing two mcpp::Coordinate objects.
     *
     * @param a The first mcpp::Coordinate object.
     * @param b The second mcpp::Coordinate object.
     * @return True if 'a' should come before 'b', false otherwise.
     */
    bool operator()(const mcpp::Coordinate& a, const mcpp::Coordinate& b) const {
        if(a.x != b.x) return a.x < b.x;
        if(a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    }
};


void ReadMazeFromTerminal(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, std::vector<Maze*>& generatedMazes);
void GenerateRandomMaze(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, 
                        std::vector<Maze*>& generatedMazes, const bool mode);
void SolveMaze(mcpp::MinecraftConnection* mc, Agent*& player, const bool mode);
void SolveTile(Agent*& player, AgentDirection &dir, int &x, int &z, mcpp::Coordinate& playerPos,
                mcpp::MinecraftConnection* mc);
void HighlightSolvedBlock(const mcpp::Coordinate& playerPos, mcpp::MinecraftConnection* mc);
void PrintSteps(int &counter, const mcpp::Coordinate& playerPos);
void UpdateCoordsAfterSolving(const AgentDirection& dir, int& x, int& z, mcpp::Coordinate& playerPos);
std::string coordDirToKey(const CoordDir& cd);
bool AllVisited(const mcpp::Coordinate cd, const AgentDirection& dir, std::vector<CoordDir>&        
                visitedTiles, mcpp::MinecraftConnection* mc);
void SolveManually(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, Agent*& player, const bool mode);
std::vector<int> getValidInts(const std::string& errorMsg);
bool validateMazeDimensions(const std::vector<std::string>& rows, int& envLength, int& envWidth);
void InitialisePlayer(mcpp::MinecraftConnection* mc, mcpp::Coordinate& startLoc, AgentDirection& dir, 
                      const bool mode);
bool CheckIfSolved(mcpp::Coordinate& coord, mcpp::MinecraftConnection* mc, AgentDirection dir);
bool validateMazeCharacters(const std::vector<std::string>& rows);
void CleanUp(std::vector<std::vector<std::vector<mcpp::BlockType>>>& environment,
             Maze*& maze, mcpp::MinecraftConnection* mc);
void BuildMazeInMC(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, std::vector<Maze*>& generatedMazes,
                   std::vector<std::vector<std::vector<mcpp::BlockType>>>& environment);
void ShowShortestPath(mcpp::MinecraftConnection* mc, std::vector<mcpp::Coordinate> path);
std::vector<mcpp::Coordinate> FindShortestPath(mcpp::MinecraftConnection* mc, Agent*& player);
void GetMazeInputs(mcpp::Coordinate& basePoint, int& length, int& width, mcpp::MinecraftConnection* mc);
std::vector<mcpp::Coordinate> BacktrackPath(CoordDir& backtrack, const mcpp::Coordinate& start, 
                                            std::map<CoordDir, CoordDir, CoordDirComparator>& parent);

int main(int argc, char* argv[]) {

    bool mode = NORMAL_MODE;
    solveAlgorithm solveAlgorithm = RIGHT_HAND_FOLLOW;
    std::cout << std::endl;
    
    // Check for testing mode and BFS flag
    for (int i = 1; i < argc; i++) {  
        std::string arg = argv[i];
        if (arg == "-testmode") {
            mode = TESTING_MODE;
            std::cout << "You are running the program in test mode." << std::endl;
        } else if (arg == "-e2") {
            solveAlgorithm = BREATH_FIRST_SEARCH;
            std::cout << "The maze-solving algorithm will be changed to a Breadth-First-Search." << std::endl;
        }
    }

    mcpp::MinecraftConnection* mc = new mcpp::MinecraftConnection();
    Maze* terminalMaze = nullptr;
    Agent* player = nullptr;
    std::vector<std::vector<std::vector<mcpp::BlockType>>> environment;
    std::vector<Maze*> generatedMazes;

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
                BuildMazeInMC(mc, terminalMaze, generatedMazes, environment);
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
                ReadMazeFromTerminal(mc, terminalMaze, generatedMazes);
                curState = ST_Main;
            } else if (input == 2) {
                GenerateRandomMaze(mc, terminalMaze, generatedMazes, mode);
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
                    SolveManually(mc, terminalMaze, player, mode);
                } else {
                    std::cout << "Please generate a maze first." << std::endl;
                }
                curState = ST_Main;
            } else if (input == 2) {
                if (player) {
                    if (solveAlgorithm == RIGHT_HAND_FOLLOW) {
                        SolveMaze(mc, player, mode);
                    } else {
                        std::vector<mcpp::Coordinate> shortestPath = FindShortestPath(mc, player);
                        ShowShortestPath(mc, shortestPath);
                    }
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

    CleanUp(environment, terminalMaze, mc);
    
    if (player) {
        delete player;
    }
    delete terminalMaze;
    delete mc;
    mc = nullptr;
    terminalMaze = nullptr;
    player = nullptr;
    return EXIT_SUCCESS;
}

/**
 * Builds the generated maze in the MineCraft world. First checks if there are any existing mazes built.
 * If so, it cleans up the environment and restores the old blocks. It then builds the new maze.
 * If the user is generating the same maze, it prints an error message and does not generate a new maze.
 * @param mc: The MineCraft connection
 * @param terminalMaze: The maze that is being built
 * @param generatedMazes: A vector of mazes that have been generated
 * @param environment: A vector of vectors of vectors of BlockTypes that stores the old blocks
 *                    in the environment.
*/
void BuildMazeInMC(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, std::vector<Maze*>& generatedMazes,
                   std::vector<std::vector<std::vector<mcpp::BlockType>>>& environment) {
    if (terminalMaze) { 
        Maze* mazeToBuild = generatedMazes.back();  // The last maze generated
        for (Maze* oldMaze : generatedMazes) { // Iterate through the previous mazes
            if (oldMaze != mazeToBuild) {
                std::cout << "Cleaning up and restoring existing environment." << std::endl;
                CleanUp(environment, oldMaze, mc); // Clean and restore environment
                delete oldMaze;
                std::cout << "Cleanup and restore successful. Now building new maze." << std::endl;
            } 
        }
        if (mazeToBuild->getMazeBuiltStatus() == false) { // Check if maze has not been made yet
            generatedMazes.clear(); // Clears list of generated mazes 
            generatedMazes.push_back(mazeToBuild);
            environment = mazeToBuild->getEnvironment(mc);
            mazeToBuild->FlattenAndBuild(mc);
            mazeToBuild->setMazeBuiltStatus(true);
        } else {
            std::cout << "Error: You are trying to build a maze that's already been built." << std::endl;
        }

    } else {
        std::cout << "Please generate a maze first." << std::endl;
    }
}

/** 
 * Cleans up the existing MineCraft environment by restoring the old blocks and then rebuilding the
 * environment from the environment vector. This is used when the user wants to generate a new maze
 * without restarting the program OR when they are exiting the program.
 * @param environment: The vector that stores the old blocks.
 * @param maze: The maze that is being cleaned up.
 * @param mc: The MineCraft connection.
 */
void CleanUp(std::vector<std::vector<std::vector<mcpp::BlockType>>>& environment,
             Maze*& maze, mcpp::MinecraftConnection* mc) {
    if (maze && !environment.empty()) {
        maze->RestoreOldBlocksFirst(mc);
        maze->rebuildEnvironment(mc, environment);
        environment.clear();
    } else {
        std::cout << "The environment vector is empty, so unable to restore environment." << 
        " Ignore this message if you didn't generate a maze" << std::endl;
    }
}

/** 
 * Checks if any given input is a valid integer. If not, it will print an error message and ask the
 * user to re-enter the input.
 * @param errorMsg: the error message to be printed if the input is invalid.
 * @return A vector of integers that the user has entered.
 */
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

/**
 * Validates the dimensions of the maze. It checks if the number of rows is equal to the length of
 * the maze and if the number of columns is equal to the width of the maze.
 * @param rows The rows of the maze
 * @param envLength The length of the maze
 * @param envWidth The width of the maze
 * @return True if the dimensions are valid, false otherwise.
*/
bool validateMazeDimensions(const std::vector<std::string>& rows, int& envLength, int& envWidth) {
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

/**
 * Validates the characters of the maze. It checks if the characters are either 'x' or '.'.
 * @param rows A vector of rows of the maze
 * @return True if the characters are valid, false otherwise.
*/
bool validateMazeCharacters(const std::vector<std::string>& rows) {
    bool valid = true;  // Initially assume that it's valid, then check for invalid characters
    size_t i = 0;
    size_t numRows = rows.size();

    while (valid && i < numRows) {
        size_t j = 0;
        size_t numCols = rows[i].size();
        
        while (valid && j < numCols) {
            char c = rows[i][j];
            if (c != 'x' && c != '.') {
                valid = false;
            }
            j++;
        }
        i++;
    }

    return valid;
}

/**
 * Allows the user to read the maze from the terminal. It will ask the user to enter the basePoint,
 * length, width and the structure of the maze. It will then store the maze in the terminalMaze
 * variable and add it to the generatedMazes vector.
 * @param mc: The MineCraft connection
 * @param terminalMaze: The maze that is being read from the terminal
 * @param generatedMazes: A vector of mazes that have been generated
*/
void ReadMazeFromTerminal(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, std::vector<Maze*>& generatedMazes) {
    mcpp::Coordinate basePoint(0, 0, 0);
    int envLength = 0;
    int envWidth = 0;

    GetMazeInputs(basePoint, envLength, envWidth, mc);

    std::vector<std::vector<char>> mazeStructure; // Store maze structure
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
        // Check if user entered dimensions are valid 
        if (validateMazeDimensions(rows, envLength, envWidth) && validateMazeCharacters(rows)) {
            validMaze = true;  // Update the control variable to exit the loop
        } else {
            std::cout << "Invalid maze structure. Please ensure that dimensions are correct";
            std::cout << " and only 'x' and '.' characters are used." << std::endl;
            std::cout << "'x' represents a wall, and '.' an empty space." << std::endl;
        }
    }
    // Resize 2d vector into correct dimensions
    mazeStructure.resize(envLength, std::vector<char>(envWidth)); 

    for (int i = 0; i < envLength; i++) {
        for (int j = 0; j < envWidth; j++) {
            mazeStructure[i][j] = rows[i][j];
        }
    }
    Maze* newMaze = new Maze(basePoint, envLength, envWidth, mazeStructure);
    generatedMazes.push_back(newMaze); // Add maze to list of generated mazes
    std::cout << "Maze read successfully" << std::endl; // Update temrinal maze with new maze and print
    terminalMaze = newMaze;
    newMaze->PrintMaze();
}

/**
 * Gets all required inputs from the user for generating a maze. Includes input validation.
 * @param basePoint: The basePoint of the maze
 * @param length: The length of the maze
 * @param width: The width of the maze
 * @param mc: The MineCraft connection
*/
void GetMazeInputs(mcpp::Coordinate& basePoint, int& length, int& width, mcpp::MinecraftConnection* mc) {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<int> inputs;

    do {
        std::cout << "Enter the basePoint of maze:" << std::endl;
        inputs = getValidInts("Input Error: Enter 3 integers (e.g., \"103 105 100\").\nEnter the basePoint of the maze:");
        if (static_cast<int>(inputs.size()) != 3) {
            std::cout << "Input Error: You must enter exactly 3 integers for the basePoint." << std::endl;
        }
    } while (static_cast<int>(inputs.size()) != 3);
    
    basePoint = mcpp::Coordinate(inputs[0], inputs[1], inputs[2]);
    basePoint.y = mc->getHeight(basePoint.x, basePoint.z) + 1;
    
    std::cout << "Enter the length and width of maze:" << std::endl;
    inputs.clear();

    bool shouldContinue = true;
    do {
        shouldContinue = false; 
        inputs = getValidInts("Input Error: Enter an odd-numbered integer. ");
        bool isNotOddInput = false;

        if (static_cast<int>(inputs.size()) < 2) {
            std::cout << "Please enter both length and width." << std::endl;
            shouldContinue = true;
        }
        else {
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
                shouldContinue = true;
            }
        }

    } while (shouldContinue);

    
     length = inputs[0], width = inputs[1];
}

/**
 * Generates a random maze in Minecraft using the recursive backtracking algorithm.
 * @param mc: The MineCraft connection
 * @param terminalMaze: The maze that is being generated
 * @param generatedMazes: A vector of mazes that have been generated
 * @param mode: Flag indicating the mode of maze generation (NORMAL_MODE/TESTING_MODE).
*/
void GenerateRandomMaze(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, 
                        std::vector<Maze*>& generatedMazes, const bool mode) {

    std::vector<std::vector<char>> mazeStructure;
    mcpp::Coordinate basePoint(0, 0, 0); // Define base point and initialize dimensions
    int envLength = 0;
    int envWidth = 0;
    // Call function to get user inputs for basePoint, envLength, envWidth
    GetMazeInputs(basePoint, envLength, envWidth, mc);
    // Resize mazeStructure to match dimensions
    mazeStructure.resize(envLength, std::vector<char>(envWidth)); 
    // Make new maze object with parameters basePoint, envLength, envWidth, mazeStructure
    Maze* newMaze = new Maze(basePoint, envLength, envWidth, mazeStructure);
    // Generate random maze in maze object
    newMaze->generateMaze(mode);
    // Update maze structure
    newMaze->updateMazeStructure();
    // Add new maze to generated mazes
    generatedMazes.push_back(newMaze);
    // Tells user that the maze was read successfully
    std::cout << "Maze read successfully" << std::endl;
    terminalMaze = newMaze; // Update terminalMaze with new maze and print
    terminalMaze->PrintMaze();
}

/**
 * Initialises and sets the player's starting location and direction based on the maze's outer walls. 
 * Then uses helper functions to show the escape route.
 * @param mc: The MineCraft connection
 * @param player: The player that is being initialised
 * @param mode: Flag indicating the mode of maze generation (NORMAL_MODE/TESTING_MODE)
*/
void SolveMaze(mcpp::MinecraftConnection* mc, Agent*& player, bool mode) {
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
    InitialisePlayer(mc, playerPos, dir, mode);
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

/**
 * Prints the coordinates of the solved tile in the console. Part of the SolveMaze() function.
 * @param counter: The number of steps taken to solve the maze
 * @param playerPos: The player's current position
*/
void PrintSteps(int &counter, const mcpp::Coordinate &playerPos) {
    std::cout << "Step [" << counter << "]: (" << playerPos.x << ", " << playerPos.y << 
        ", " << playerPos.z << ")" << std::endl;
    counter++;
}

/**
 * After a tile has been solved, this function increments or decrements the 'x' or 'z' coordinate, 
 * depending on the direction that the agent is facing. Through this, the agent is able to move.
 * Part of the SolveMaze function.
 * @param dir: The direction that the agent is facing
 * @param x: The x coordinate of the agent
 * @param z: The z coordinate of the agent
 * @param playerPos: The player's current position
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

/**
 * Highlights the solved tile with a lime carpet block for 1 second.
 * It then removes the lime carpet block by generating an air block in place of it.
 * Part of the SolveMaze() function.
 * @param playerPos: The player's current position
 * @param mc: The MineCraft connection
*/
void HighlightSolvedBlock(const mcpp::Coordinate &playerPos, mcpp::MinecraftConnection* mc) {
    mc->setBlock(playerPos, mcpp::Blocks::LIME_CARPET);

    // 1 second sleep delay
    std::this_thread::sleep_for(std::chrono::seconds(1)); 

    // Remove highlighted block
    mc->setBlock(playerPos, mcpp::Blocks::AIR);
    std::this_thread::sleep_for(std::chrono::seconds(1)); 
}

/**
 * Solves the current tile by turning right, straight, left, or turning around. This is achieved
 * through the right-hand wall-follower algorithm.
 * If it can turn right it will. If not, it moves straight. If it can't move straight, 
 * it will try to turn left. If it can't turn left, it will turn around. 
 * This function will continue to execute until AllVisited returns true.
 * Part of the SolveMaze function.
 * @param player: The player that is being initialised
 * @param dir: The direction that the agent is facing
 * @param x: The x coordinate of the agent
 * @param z: The z coordinate of the agent
 * @param playerPos: The player's current position
 * @param mc: The MineCraft connection
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

/**
 * Converts a CoordDir struct to a string, which is used as a key in a map.
 * This is used in the AllVisited function.
 * @param cd: The CoordDir struct that is being converted to a string
 * @return A string that is used as a key in a map
 */
std::string coordDirToKey(const CoordDir& cd) {
    return std::to_string(cd.coord.x) + "_" + std::to_string(cd.coord.y) + "_" 
    + std::to_string(cd.coord.z) + "_" + std::to_string(cd.dir);
}

/**
 * Checks if a tile has been visited more than N times with the same agent direction.
 * If so, it returns true. Otherwise, it returns false. 
 * Used as an exit condition for SolveMaze.
 * @param coord: The coordinate of the tile that is being checked
 * @param dir: The direction that the agent is facing
 * @param visitedTiles: A vector of CoordDir structs that stores the coordinates and directions of
 *                     the tiles that have been visited
 * @param mc: The MineCraft connection
 * @return True if the tile has been visited more than N times OR if CheckIfSolved returns true, false otherwise.
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

/**
 * Checks if the maze has been solved. It does this by checking if the blocks in front of the agent
 * are all air blocks.
 * @param coord: The coordinate of the tile that is being checked
 * @param mc: The MineCraft connection
 * @param dir: The direction that the agent is facing
 * @return True if the maze has been solved, false otherwise.
*/
bool CheckIfSolved(mcpp::Coordinate& coord, mcpp::MinecraftConnection* mc, AgentDirection dir) {
    bool solved = true; // Initially assume it's solved
    std::vector<mcpp::Coordinate> front_coords;

    if (dir == UP) {
        front_coords = {
            coord + MOVE_XPLUS,
            coord + MOVE_XPLUS + MOVE_ZPLUS,
            coord + MOVE_XPLUS + MOVE_ZMINUS,
            coord + MOVE_ZPLUS,
            coord + MOVE_ZMINUS
        };
    } else if (dir == RIGHT) {
        front_coords = {
            coord + MOVE_ZPLUS,
            coord + MOVE_ZPLUS + MOVE_XPLUS,
            coord + MOVE_ZPLUS + MOVE_XMINUS,
            coord + MOVE_XPLUS,
            coord + MOVE_XMINUS
        };
    } else if (dir == DOWN) {
        front_coords = {
            coord + MOVE_XMINUS,
            coord + MOVE_XMINUS + MOVE_ZPLUS,
            coord + MOVE_XMINUS + MOVE_ZMINUS,
            coord + MOVE_ZPLUS,
            coord + MOVE_ZMINUS
        };
    } else if (dir == LEFT) {
        front_coords = {
            coord + MOVE_ZMINUS,
            coord + MOVE_ZMINUS + MOVE_XPLUS,
            coord + MOVE_ZMINUS + MOVE_XMINUS,
            coord + MOVE_XPLUS,
            coord + MOVE_XMINUS
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

/**
 * Teleports the user to a random walkable tile in the maze. Through this, a new Agent is created.
 * @param mc: The MineCraft connection
 * @param terminalMaze: The maze that is being solved
 * @param player: The player that is being initialised
 * @param mode: The mode that the program is running in, as this affects the algorithm
 */
void SolveManually(mcpp::MinecraftConnection* mc, Maze*& terminalMaze, Agent*& player, const bool mode) {
    // Initialise random seed for normal mode
    if (mode == NORMAL_MODE) {
        std::srand(std::time(0));
    }

    // Get all walkable tiles from private member variable
    std::vector<mcpp::Coordinate> coords = terminalMaze->getWalkableCoords();
    mcpp::Coordinate walkableTile;

    // Set player position & allow them to solve manually
    if (!coords.empty()) {
        if (mode == NORMAL_MODE) {
            int randomIndex = std::rand() % static_cast<int>(coords.size());
            std::cout << "Random index: " << randomIndex << std::endl;
            walkableTile = coords[randomIndex];
        } else {  // TESTING_MODE
            walkableTile = coords[0];  // Initialise to the first coordinate

            // Loop through to find the coordinate closest to the lower-right edge
            for (const auto& coord : coords) {
                if (coord.x >= walkableTile.x && coord.z >= walkableTile.z) {
                    walkableTile = coord;
                }
            }
        }
        std::cout << "Teleporting to: " << walkableTile.x << ", " << walkableTile.y << ", " << walkableTile.z << std::endl;
        mc->setPlayerPosition(walkableTile);
        player = new Agent(walkableTile);
    } else {
        std::cout << "No walkable tile found! Please generate another maze." << std::endl;
    }
}

/**
 * Initialises the player's starting location and direction based on the maze's outer walls. Will
 * orient the Agent with its right hand touching a wall. If used in TESTING_MODE, it will orient the
 * player in the positive x-direction, with the right hand touching a wall.
 * @param mc: The MineCraft connection
 * @param startLoc: The player's starting location
 * @param dir: The direction that the agent is facing
 * @param mode: The mode that the program is running in, as this affects the algorithm
 */
void InitialisePlayer(mcpp::MinecraftConnection* mc, mcpp::Coordinate& startLoc, AgentDirection& dir,
                      const bool mode) {
    if (mode == NORMAL_MODE) {
        mcpp::BlockType blockTypeEast = mc->getBlock(startLoc + MOVE_XPLUS);
        mcpp::BlockType blockTypeWest = mc->getBlock(startLoc + MOVE_XMINUS);
        mcpp::BlockType blockTypeNorth = mc->getBlock(startLoc + MOVE_ZMINUS);
        mcpp::BlockType blockTypeSouth = mc->getBlock(startLoc + MOVE_ZPLUS);

        if (blockTypeEast == mcpp::Blocks::ACACIA_WOOD_PLANK) {
            // Wall is to the Right, so face North (up)
            dir = UP;
        } else if (blockTypeWest == mcpp::Blocks::ACACIA_WOOD_PLANK) {
            // Wall is to the Left, so face South (down)
            dir = DOWN;
        } else if (blockTypeNorth == mcpp::Blocks::ACACIA_WOOD_PLANK) {
            // Wall is to the North, so face West (left)
            dir = LEFT;
        } else if (blockTypeSouth == mcpp::Blocks::ACACIA_WOOD_PLANK) {
            // Wall is to the South, so face East (right)
            dir = RIGHT;
        }
    } else {
        dir = RIGHT; // Start by facing East (right)
        mcpp::BlockType blockTypes[] = {
            mc->getBlock(startLoc + MOVE_XPLUS),   // East
            mc->getBlock(startLoc + MOVE_ZPLUS),   // South
            mc->getBlock(startLoc + MOVE_XMINUS),  // West
            mc->getBlock(startLoc + MOVE_ZMINUS)   // North
        };
        AgentDirection directions[] = { RIGHT, DOWN, LEFT, UP };

        bool foundWall = false;
        for (int i = 0; i < 4 && !foundWall; ++i) {
            if (blockTypes[i] == mcpp::Blocks::ACACIA_WOOD_PLANK) {
                dir = directions[i];
                foundWall = true;
            }
        }
    }
}

/**
 * Finds the shortest path from the start to the end of the maze. It does this by using a
 * Breadth-First Search algorithm. It will then backtrack from the end to the start to find the
 * shortest path.
 * @param mc: The MineCraft connection
 * @param player: The player that is being initialised
 * @return A vector of coordinates that represent the shortest path from the start to the end of the maze.
*/
std::vector<mcpp::Coordinate> FindShortestPath(mcpp::MinecraftConnection* mc, Agent*& player) {
    std::queue<std::tuple<mcpp::Coordinate, mcpp::Coordinate, AgentDirection>> bfsQueue;
    std::set<CoordDir, CoordDirComparator> visited;
    std::map<CoordDir, CoordDir, CoordDirComparator> parent;
    std::vector<mcpp::Coordinate> path;

    mcpp::Coordinate start = player->getStartLoc();
    CoordDir startState = {.coord = start, .dir = UP};
    
    bfsQueue.push({start, start, UP});
    visited.insert(startState);
    parent[startState] = startState;

    bool pathFound = false;

    std::cout << "Starting BFS from: (" << start.x << ", " << start.y << ", " << start.z << ")" << std::endl;

    // While the queue is not empty and the path has not been found
    while (!bfsQueue.empty() && !pathFound) {
        auto [currentPosition, previousPosition, currentDirection] = bfsQueue.front();
        bfsQueue.pop();

        CoordDir currentState = {.coord = currentPosition, .dir = currentDirection};
        std::cout << "Current Position: (" << currentPosition.x << ", " << currentPosition.y << ", " << currentPosition.z << ")" << std::endl;

        if (CheckIfSolved(currentPosition, mc, currentDirection)) {
            std::cout << "Maze Solved! Backtracking to find the path." << std::endl;
            CoordDir backtrack = {.coord = currentPosition, .dir = currentDirection};
            path = BacktrackPath(backtrack, start, parent);
            pathFound = true;
        }

        // Process all neighbours
        if (!pathFound) {
            AgentDirection tempDirection = currentDirection;
            for (int i = 0; i < 4; i++) {
                if (player->canMove(currentPosition.x, currentPosition.z, tempDirection, currentPosition, mc)) {
                    mcpp::Coordinate neighbour = player->findNeighbour(tempDirection, currentPosition, mc);
                    CoordDir neighbourState = {.coord = neighbour, .dir = tempDirection};
                    std::cout << "Found Neighbour: (" << neighbour.x << ", " << neighbour.y 
                    << ", " << neighbour.z << ")" << std::endl;

                    if (!visited.count(neighbourState)) {
                        visited.insert(neighbourState);  // Mark as visited here
                        bfsQueue.push({neighbour, currentPosition, tempDirection});
                        parent[neighbourState] = currentState;
                        std::cout << "Enqueueing neighbour and marking as visited." << std::endl;
                    }
                }
                tempDirection = player->turn(tempDirection);
            }
        }
    }
    return path;
}

/**
 * Backtracks from the end of the maze to the start to find the shortest path.
 * @param backtrack: The current position of the agent
 * @param start: The starting position of the agent
 * @param parent: A map that stores the parent of each tile
 * @return A vector of coordinates that represent the shortest path from the start to the end of the maze.
*/
std::vector<mcpp::Coordinate> BacktrackPath(CoordDir& backtrack, const mcpp::Coordinate& start, 
                                            std::map<CoordDir, CoordDir, CoordDirComparator>& parent) {
    std::vector<mcpp::Coordinate> path;
    while (!(backtrack.coord == start)) {
        path.push_back(backtrack.coord);
        backtrack = parent[backtrack];  // Update backtrack for the next iteration
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

/**
 * Shows the shortest path in MineCraft by highlighting the blocks in the path with a lime carpet
 * block for 1 second. It then removes the lime carpet block by generating an air block in place of it.
 * @param mc: The MineCraft connection
 * @param path: A vector of coordinates that represent the shortest path from the start to the end of the maze.
*/
void ShowShortestPath(mcpp::MinecraftConnection* mc, std::vector<mcpp::Coordinate> path) {
    int counter = 1;
    if (!path.empty()) {
        for (const auto& coord : path) {
            HighlightSolvedBlock(coord, mc);
            PrintSteps(counter, coord);
        }
    } else {
        std::cout << "Error: No solution exists for this maze." << std::endl;
    }
    mc->postToChat("All done!");
}