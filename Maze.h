#ifndef ASSIGN_MAZE_H
#define ASSIGN_MAZE_H

#include <vector>
#include <mcpp/mcpp.h>

#define NORMAL_MODE 0
#define TESTING_MODE 1

class Maze
{

public:
    Maze(mcpp::Coordinate basePoint, int xlen, int zlen,
        std::vector<std::vector<char>> mazeStructure);
    ~Maze();

     // struct to store the coordinates of the maze cells
    struct Cell
    {
        mcpp::Coordinate coordinate;
        bool isVisited = false;
    };
        struct GeneratedBlock {
        mcpp::BlockType originalType = mcpp::Blocks::AIR;
        mcpp::Coordinate coordinate;
    };

    // Generate the maze structure
    void generateMazeStructure();

    // Generate the maze
    void generateMaze(bool mode);

    // Print the maze
    void PrintMaze();

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

    void generateRecursiveMaze(std::vector<Cell> &cells, Cell cell, bool mode);

    // get the mazeStructure
    void updateMazeStructure();

    // check if the cell is visited
    bool visited(Cell cell);

    mcpp::Coordinate getBasePoint() const { return basePoint; }
    int getLength() const { return length; }
    int getWidth() const { return width; }
    void setBasePoint(mcpp::Coordinate basePoint) { this->basePoint = basePoint; }
    void setLength(int length) { this->length = length; }
    void setWidth(int width) { this->width = width; }
    void setMazeBuiltStatus(bool mazeBuiltAlready) { this->mazeBuiltAlready = mazeBuiltAlready; }
    bool getMazeBuiltStatus() const { return mazeBuiltAlready; }
    std::vector<mcpp::Coordinate> getWalkableCoords() const { return walkableCoords; }
    void GenerateMazeInMC(mcpp::MinecraftConnection* mc);
    std::vector<std::vector<std::vector<mcpp::BlockType>>> getEnvironment(mcpp::MinecraftConnection* mc);
    void flattenEnvironment(mcpp::MinecraftConnection* mc);
    void rebuildEnvironment(mcpp::MinecraftConnection* mc,
                        const std::vector<std::vector<std::vector<mcpp::BlockType>>>& savedEnvironment);
    void FlattenAndBuild(mcpp::MinecraftConnection* mc);
    void StoreOldBlock(mcpp::MinecraftConnection* mc, mcpp::Coordinate& coord);
    void RestoreOldBlocksFirst(mcpp::MinecraftConnection* mc);

private:
    /* data */
    // The base point of the maze
    mcpp::Coordinate basePoint;

    // Random length and width of the maze
    int length;
    int width;

    // Mode
    //bool mode = false;

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

    // Maze structure
    std::vector<std::vector<char>> mazeStructure;

    // Walkable coords for player to be teleported to
    std::vector<mcpp::Coordinate> walkableCoords;

    std::vector<GeneratedBlock> generatedBlocks;

    bool mazeBuiltAlready = false;
    int counter = 0;
};




#endif //ASSIGN_MAZE_H
