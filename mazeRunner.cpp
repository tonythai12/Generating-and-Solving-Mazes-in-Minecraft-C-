#include <iostream>

#include <mcpp/mcpp.h>

#include "menuUtils.h"
#include "Maze.h"
#include "Agent.h"

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
}

// Keenan
void SolveMaze() {
    // TODO: Solve maze code 
}