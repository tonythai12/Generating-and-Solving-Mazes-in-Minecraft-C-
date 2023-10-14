#include <iostream>

#include <mcpp/mcpp.h>

#include "menuUtils.h"
#include "Maze.h"
#include "Agent.h"

// For sleep and time duration
#include <thread>   
#include <chrono>   

#define NORMAL_MODE 0
#define TESTING_MODE 1

enum States{
    ST_Main,
    ST_GetMaze,
    ST_SolveMaze,
    ST_Creators,
    ST_Exit
};

void ReadMazeFromTerminal(); // option 1 in generate maze menu
void GenerateRandomMaze(); // option 2 in generate maze menu
void SolveMaze();
void SolveTile(Agent *player, AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos);
void HighlightSolvedBlock(const mcpp::Coordinate &playerPos, mcpp::MinecraftConnection &mc);
void PrintSteps(int &counter, const mcpp::Coordinate &playerPos);
void UpdateCoordsAfterSolving(const AgentDirection &dir, int &x, int &z, mcpp::Coordinate &playerPos);

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
                GenerateRandomMaze();
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

// Keenan
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
    }

    delete player;
}

void PrintSteps(int &counter, const mcpp::Coordinate &playerPos) {
    std::cout << "Step [" << counter << "]: (" << playerPos.x << ", " << playerPos.y << 
        ", " << playerPos.z << ")" << std::endl;
    counter++;
}

/* 
    The function below updates the coordinates of the agent that's solving the maze.
    It either increments or decrements the x or z coordinate depending on the direction.
    After that, it updates the playerPos with the new coordinates.
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

void HighlightSolvedBlock(const mcpp::Coordinate &playerPos, mcpp::MinecraftConnection &mc) {
    mc.setBlock(playerPos, mcpp::Blocks::LIME_CARPET);

    // 1 second sleep delay
    std::this_thread::sleep_for(std::chrono::seconds(1)); 

    // Remove highlighted block
    mc.setBlock(playerPos, mcpp::Blocks::AIR);
    std::this_thread::sleep_for(std::chrono::seconds(1)); 
}

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