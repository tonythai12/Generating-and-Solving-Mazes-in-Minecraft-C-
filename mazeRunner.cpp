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
bool canMove(int x, int z, AgentDirection dir);

int main(void){

    // bool mode = NORMAL_MODE;
    //read Mode
    
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


    mcpp::MinecraftConnection mc; 
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

bool canMove(int x, int z, AgentDirection dir) {
    if (dir == UP) {
        return maze[x][z + 1] == '.';
    } else if (dir == RIGHT) {
        return maze[x + 1][z] == '.';
    } else if (dir == DOWN) {
        return maze[x][z - 1] == '.';
    } else if (dir == LEFT) {
        return maze[x - 1][z] == '.';
    }
}

// Keenan
void SolveMaze() {
    // TODO: Solve maze code 
    
    // Find player position
    mcpp::MinecraftConnection mc;
    
    mcpp::Coordinate playerPos = mc.getPlayerPosition();
    int x = playerPos.x;
    int z = playerPos.z;

    bool solved = false;
    AgentDirection dir = UP;

    while (!solved) {
        // Try turning right
        if (!canMove(x, z, dir)) {
            do {
                // Turns right until there's a valid move
                dir = turn(dir);
            } while (!canMove(x, z, dir));
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
        // Highlight solved tile
        void setBlock(mcpp::Coordinate playerPos, mcpp::BlockType LIME_CARPET);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 1s delay

        // Exit dummy maze if reached this condition
        if (x == 4 && z == 5) {
            solved = true;
        }
    }
}