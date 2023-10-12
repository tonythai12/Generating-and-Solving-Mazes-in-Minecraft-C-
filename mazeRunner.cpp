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
// dummy maze, assume it's generated
char maze[5][6] = {
    {'x', 'x', 'x', 'x', 'x', 'x'},
    {'x', '.', '.', '.', '.', 'x'},
    {'x', 'x', 'x', '.', 'x', 'x'},
    {'x', '.', '.', '.', '.', 'x'},
    {'x', 'x', 'x', 'x', 'x', 'x'}
};

void ReadMazeFromTerminal(); // option 1 in generate maze menu
void GenerateRandomMaze(); // option 2 in generate maze menu
void SolveMaze();
AgentDirection turn(AgentDirection dir);
bool canMove(int x, int z, AgentDirection dir, mcpp::Coordinate& loc);

int main(void){

    // bool mode = NORMAL_MODE;
    //read Mode

    mcpp::MinecraftConnection mc;
    printStartText();
    printMainMenu();

    void SolveMaze();
    
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
    int x, y, z;
    std::cout << "Enter the basePoint of maze:" << std::endl;
    std::cin >> x >> y >> z;
    mcpp::Coordinate basePoint = mcpp::Coordinate(x, y + 1, z);

    // Length and Width
    int length, width;
    std::cin >> length;
    do {
        std::cin >> width;
        if (width % 2 == 0) {
            std::cout << "Width must be an odd number. Please re-enter:" << std::endl;
        }
    } while (width % 2 == 0);

    //TODO: Maze Structure

}
// Ravi
void GenerateRandomMaze() {
    //TODO: Generate maze and print in console

    // dummy maze for solving, assume generated
    
}

AgentDirection turn(AgentDirection dir) {
    return static_cast<AgentDirection>((dir + 1) % 4);
}

bool canMove(int x, int z, AgentDirection dir, mcpp::Coordinate& loc) {

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

// Keenan
void SolveMaze() {
    // TODO: Solve maze code 
    mcpp::MinecraftConnection mc;

    // Find player position
    mcpp::Coordinate playerPos = mc.getPlayerPosition();

    // Initialise variables for x and z coordinates
    int x = playerPos.x;
    int z = playerPos.z;

    // Condition for while-loop
    bool solved = false;
    AgentDirection dir = UP;

    while (!solved) {
        // Try turning right
        if (!canMove(x, z, dir, playerPos)) {
            do {
                // Turns right until there's a valid move
                dir = turn(dir);
            // Exits loop do-while loop when there's a valid move
            } while (!canMove(x, z, dir, playerPos));
        }

        // Move in directions
        if (dir == UP) {
            x--;
        } else if (dir == RIGHT) {
            z++;
        } else if (dir == DOWN) {
            x++;
        } else if (dir == LEFT) {
            z--;
        }
        // Updates player position with new coordinates
        playerPos.x = x;
        playerPos.z = z;

        // Highlights 'solved' tile
        mc.setBlock(playerPos, mcpp::Blocks::LIME_CARPET);
        
        // 3 second delay and then remove highlighted tile
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        mc.setBlock(playerPos, mcpp::Blocks::AIR);

        // Exit dummy" maze if reached this condition
        if (x == 4 && z == 5) {
            solved = true;
        }
    }
}