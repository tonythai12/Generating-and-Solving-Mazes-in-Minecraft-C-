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

     // struct to store the coordinates of the maze cells
    struct Cell
    {
        mcpp::Coordinate coordinate;
        bool isVisited = false;
    };

    // Directions
    const int UP = 0;
    const int RIGHT = 1;
    const int DOWN = 2;
    const int LEFT = 3;

    // Generate the maze structure
    void generateMazeStructure();

    // Generate the maze
    void generateMaze();

    // Print the maze
    void printMaze();

    // Selecting random starting point in the maze
    mcpp::Coordinate selectRandomStartingPoint();

    // return if the cell is at outer wall
    Cell OuterWallCell(mcpp::Coordinate cell); 

    // make way in the maze
    void removeWall(Cell cell);

    // return if the cell has unvisited neighbour
    bool hasUnvisitedNeighbour(Cell cell);

    // check if the cell is inside the maze
    bool isInsideMaze(mcpp::Coordinate cell);

    // get random direction
    int getRandomDirection();

    void generateRecursiveMaze(std::vector<Cell> &cells, Cell cell);

    // check if the cell is visited
    bool visited(Cell cell);

    mcpp::Coordinate getBasePoint() const { return basePoint; }
    int getLength() const { return length; }
    int getWidth() const { return width; }

private:
    /* data */
    // The base point of the maze
    mcpp::Coordinate basePoint;

    // Random length and width of the maze
    unsigned int length;
    unsigned int width;

    // Mode
    bool mode = false;

    // The maze
    std::vector<std::vector<int>> maze;

    // BlockType
    mcpp::BlockType blockType = mcpp::Blocks::BRICKS;

    // vector to store the coordinates of the maze cells
    std::vector<Cell> cells;

    // vector to store inner wall cells
    std::vector<Cell> innerWallCells;

    //  visited cells in the maze
    std::vector<Cell> visitedCells;
};




#endif //ASSIGN_MAZE_H
