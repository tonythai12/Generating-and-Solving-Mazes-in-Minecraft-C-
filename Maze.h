#ifndef ASSIGN_MAZE_H
#define ASSIGN_MAZE_H

#include <vector>
#include <mcpp/mcpp.h>

class Maze
{

public:
    Maze(mcpp::Coordinate basePoint, unsigned int xlen, 
                                        unsigned int zlen,
                                        bool mode);
    ~Maze();

    // Generate the maze structure
    void generateMazeStructure();

    // Generate the maze
    void generateMaze();

    // Print the maze
    void printMaze();

    // Selecting random starting point in the maze
    mcpp::Coordinate selectRandomStartingPoint();


private:
    /* data */
    // Random length and width of the maze
    unsigned int length;
    unsigned int width;
    unsigned int ylen;

    // The base point of the maze
    mcpp::Coordinate basePoint;

    // Mode
    bool mode = false;

    // The maze
    std::vector<std::vector<int>> maze;

    // BlockType
    mcpp::BlockType blockType = mcpp::Blocks::BRICKS;
};




#endif //ASSIGN_MAZE_H
