#include "Maze.h"
#include <random>
#include <vector>
// For sleep and time duration
#include <thread>   
#include <chrono>

#define BUFFER 2

Maze::Maze(mcpp::Coordinate basePoint, int xlen, int zlen, std::vector<std::vector<char>> mazeStructure)
    : basePoint(basePoint), length(xlen), width(zlen), mazeStructure(mazeStructure) //mode(mode)
{
}

Maze::~Maze()
{
    std::cout << "Maze destructor successfully called." << std::endl;
}

void Maze::generateMazeStructure()
{
    // Generate the maze
    // 1. Initialize the maze
    for (int i = 0; i < length; i++)
    {
        std::vector<int> temp;
        std::vector<char> temp2;
        for (int j = 0; j < width; j++)
        {
            temp.push_back(0);
            temp2.push_back(' ');
        }
        maze.push_back(temp);
        mazeStructure.push_back(temp2);
    }

    // Generate the maze structure
    for (int i = 0; i < length; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // if the row number is even, then set the wall
            // if the column number is even, then set the wall
            if(i % 2 == 0)
            {
                maze[i][j] = 1;
            }
            else
            {
                if(j % 2 == 0)
                {
                    maze[i][j] = 1; // 1 means wall
                }
            }
        }
    }

}

void Maze::generateMaze(){
    Maze::generateMazeStructure();

    mcpp::MinecraftConnection mc;
    basePoint.y = mc.getHeight(basePoint.x, basePoint.z) + 1;
    for(int i = 0; i < length; i++){
        for(int j = 0; j < width; j++){
            if (maze[i][j] == 1){
                // Add the coordinate to cell vector and set isVisited to false
                cells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});

                if (i > 0 && i < length - 1 && j > 0 && j < width - 1) {
                    innerWallCells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});
                }
            }
            else
            {
                // Add the coordinate of the cell to the vector
                cells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});

                if (i > 0 && i < length - 1 && j > 0 && j < width - 1) {
                    innerWallCells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});
                }
            }
        }
    }

    // Select a random starting point
    mcpp::Coordinate randomStart = Maze::selectRandomStartingPoint();
    Cell start{randomStart, true};

    // Select a random outer wall cell
    Cell outerWallCell = Maze::OuterWallCell(randomStart);

    mcpp::Coordinate posOuter = outerWallCell.coordinate - basePoint;
    maze[posOuter.x][posOuter.z] = 0;
    
    // Make way in the maze
    removeWall(outerWallCell);

    // Recursively make way in the maze
    generateRecursiveMaze(innerWallCells, start);

}

// update Maze Structure
void Maze::updateMazeStructure() {

   for(int i = 0; i < length; i++){
        for(int j = 0; j < width; j++){
            if (maze[i][j] == 1){
                mazeStructure[i][j] = 'X';
            }
            else
            {
                mazeStructure[i][j] = '.';
            }
        }
    }
}

mcpp::Coordinate Maze::selectRandomStartingPoint(){
    
    mcpp::Coordinate randomStart;
    
    // Array to store mccp::Coordinate of the starting point
    std::vector<mcpp::Coordinate> startingPoints;

    // If there is 0 in the maze, then it is the starting point
    for (int i = 0; i < length; i++)
    {
        for(int j = 0; j < width; j++)
        {
            if (i == 1 || j == 1 || i == length - 2 || j == width - 2) {
                if (maze[i][j] == 0)
                {
                    startingPoints.push_back(basePoint + mcpp::Coordinate(i, 0, j));
                }
            }
        }
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, startingPoints.size() - 1);
    int random_index = distribution(generator);
    randomStart = startingPoints.at(random_index);

    return randomStart;
}

Maze::Cell Maze::OuterWallCell(mcpp::Coordinate cell){
    const mcpp::Coordinate offsets[] = {
        mcpp::Coordinate(2, 0, 0),
        mcpp::Coordinate(-2, 0, 0),
        mcpp::Coordinate(0, 0, 2),
        mcpp::Coordinate(0, 0, -2),
    };

    Cell outerWallCell;
    mcpp::Coordinate temp;

    for (int i = 0; i < 4; i++) {
        bool isOuterWall = true;
        outerWallCell.coordinate = cell + offsets[i];

        for (int j = 0; j < static_cast<int>(cells.size()); j++) {
            if (outerWallCell.coordinate == cells.at(j).coordinate) {
                isOuterWall = false;
            }
        }

        if(isOuterWall) {
            if(i == 0) {
                temp = cell + mcpp::Coordinate(1, 0, 0);
            }
            else if (i == 1) {
                temp = cell - mcpp::Coordinate(1, 0, 0);
            }
            else if (i == 2) {
                temp = cell + mcpp::Coordinate(0, 0, 1);
            }
            else if (i == 3) {
                temp = cell - mcpp::Coordinate(0, 0, 1);
            }
        }
    }

    outerWallCell.coordinate = temp;

    return outerWallCell;
}

void Maze::removeWall(Maze::Cell cell){
    mcpp::Coordinate pos = cell.coordinate - basePoint;
    maze[pos.x][pos.z] = 0;

    // Set the cell to visited
    for (int i = 0; i < static_cast<int>(innerWallCells.size()); i++)
    {
        if (innerWallCells.at(i).coordinate == cell.coordinate)
        {
            innerWallCells.at(i).isVisited = true;
        }
    }
    
}

int Maze::getRandomDirection() {
    return rand() % 4;
}

bool Maze::isInsideMaze(mcpp::Coordinate cell){

    bool isInside = false;
    for(int i = 0; i < static_cast<int>(innerWallCells.size()); i++){
        if (innerWallCells.at(i).coordinate == cell)
        {
            isInside = true;
        }
    }

    return isInside;
}

bool Maze::hasUnvisitedNeighbour(Cell cell) {
    static int dx[] = {0, 2, 0, -2};
    static int dz[] = {-2, 0, 2, 0};

    bool hasUnvisitedNeighbour = false;

    for (int i = 0; i < 4; i++)
    {
        Cell neighbour;
        neighbour.coordinate = cell.coordinate + mcpp::Coordinate(dx[i], 0, dz[i]);
        if (isInsideMaze(neighbour.coordinate) && !visited(neighbour))
        {
            hasUnvisitedNeighbour = true;
        }
    }

    return hasUnvisitedNeighbour;
}

void Maze::generateRecursiveMaze(std::vector<Cell> &cells, Cell cell) {

    cell.isVisited = true;
    visitedCells.push_back(cell);

    while (hasUnvisitedNeighbour(cell))
    {
        int direction = getRandomDirection();

        // Create a new cell
        Cell newCell;
        Cell tempCell;

        if (direction == 0) {
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(2, 0, 0);
            tempCell.coordinate = cell.coordinate + mcpp::Coordinate(1, 0, 0);
        }
        else if (direction == 1) {
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, 2);
            tempCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, 1);
        }
        else if (direction == 2) {
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(-2, 0, 0);
            tempCell.coordinate = cell.coordinate + mcpp::Coordinate(-1, 0, 0);
        }
        else if (direction == 3) {
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, -2);
            tempCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, -1);
        }

        if(isInsideMaze(newCell.coordinate) && !visited(newCell)){
            removeWall(tempCell);
            generateRecursiveMaze(cells, newCell);
        }
    }
}

bool Maze::visited(Cell cell) {
    bool visited = false;
    for (int i = 0; i < static_cast<int>(visitedCells.size()); i++)
    {
        if (visitedCells.at(i).coordinate == cell.coordinate)
        {
            visited = true;
        }
    }

    return visited;
}

/**
 * Prints the maze structure to the console.
 */
void Maze::PrintMaze() {
    int zLen = mazeStructure.size();
    int xLen = mazeStructure[0].size();

    std::cout << "**Printing Maze**" << std::endl;
    std::cout << "Basepoint: (" << basePoint.x << ", " << basePoint.y << ", " << basePoint.z << ")" << std::endl;
    std::cout << "Structure:" << std::endl;

    for (int i = 0; i < zLen; i++) {
        for (int j = 0; j < xLen; j++) {
            std::cout << mazeStructure[i][j];
        }
        std::cout << std::endl;
    }

    std::cout << "**End Printing Maze**" << std::endl;
}
/**
 * Builds the maze in MineCraft. Accesses the mazeStructure vector to determine where to place blocks.
 * Also stores the coordinates of the walkable blocks in a vector, which will be used to solve the maze.
 * @param mc The MinecraftConnection object.
*/
void Maze::GenerateMazeInMC(mcpp::MinecraftConnection* mc) {
    int zLen = mazeStructure.size();
    int xLen = mazeStructure[0].size();
    int yLen = 3;  // Height of the maze

    for (int y = 0; y < yLen; y++) { 
        for (int i = 0; i < zLen; i++) {  
            for (int j = 0; j < xLen; j++) {  
                if (mazeStructure[i][j] == 'x') {
                    mc->setBlock(basePoint + mcpp::Coordinate(i, y + 1, j), mcpp::Blocks::ACACIA_WOOD_PLANK);
                    //StoreOldBlock(mc, basePoint + mcpp::Coordinate(i, y + 1, j));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                } else {
                    if (!(mc->getBlock(basePoint + mcpp::Coordinate(i, y + 1, j)) == mcpp::Blocks::AIR)) {
                        mc->setBlock(basePoint + mcpp::Coordinate(i, y + 1, j), mcpp::Blocks::AIR);
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    }
                    walkableCoords.push_back(basePoint + mcpp::Coordinate(i, 1, j));
                }
            }
        }
    }
}

/**
 * Gets and stores the environment within the specified bounds. Used to rebuild the environment after 
 * the user either exits the program, or builds a new maze.
 * @param mc The MinecraftConnection object.
 * @return A 3D vector containing the environment within the specified bounds.
*/
std::vector<std::vector<std::vector<mcpp::BlockType>>> Maze::getEnvironment(mcpp::MinecraftConnection* mc) {

    // Get all blocks using the min/max y-values
    mcpp::Coordinate loc1 = mcpp::Coordinate(basePoint.x - BUFFER, basePoint.y, basePoint.z - BUFFER);
    mcpp::Coordinate loc2 = mcpp::Coordinate(basePoint.x + length + BUFFER, basePoint.y + 6, basePoint.z + width + BUFFER);
    auto blocks = mc->getBlocks(loc1, loc2);

    return blocks;
}

/**
 * Flattens the environment within the specified bounds. Used to build the maze on a level-field.
 * @param mc The MinecraftConnection object.
*/
void Maze::flattenEnvironment(mcpp::MinecraftConnection* mc) {
    // Calculate corners and then get heights (2 block buffer around all sides)
    mcpp::Coordinate corner1(basePoint.x - BUFFER, basePoint.y, basePoint.z - BUFFER);
    mcpp::Coordinate corner2(basePoint.x + length + BUFFER, basePoint.y, basePoint.z + width + BUFFER); 

    mc->setPlayerPosition(basePoint + mcpp::Coordinate(0, 10, 0));

    // Heights of the environment at (x, z) (as y-coords are different for each pair)
    auto heights = mc->getHeights(corner1, corner2);
    int floorLevel = corner1.y;
    mcpp::Coordinate blockCoordToGet(corner1.x, mc->getHeight(corner1.x, corner1.z), corner1.z);
    mcpp::BlockType basePointBlock = mc->getBlock(blockCoordToGet);
    
    // Assume [x][z] format
    for (int x = 0; x < static_cast<int>(heights.size()); x++) {
        for (int z = 0; z < static_cast<int>(heights[x].size()); z++) {
            int highestBlockY = heights[x][z];

            // Set blocks above the floorLevel and up to the highest block to AIR
            for (int y = floorLevel + 1; y <= highestBlockY; y++) {
                mcpp::Coordinate coord(corner1.x + x, y, corner1.z + z);
                mc->setBlock(coord, mcpp::Blocks::AIR);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            // Set the block at floorLevel to BlockType at basePoint
            mcpp::Coordinate coord(corner1.x + x, floorLevel, corner1.z + z);
            if (!(basePointBlock == mc->getBlock(coord))) {
                StoreOldBlock(mc, coord);
                mc->setBlock(coord, basePointBlock);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            // Set blocks below floorLevel to BlockType at basePoint
            for (int y = highestBlockY; y < floorLevel; y++) {
                mcpp::Coordinate coord(corner1.x + x, y, corner1.z + z);
                StoreOldBlock(mc, coord);
                mc->setBlock(coord, basePointBlock);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }
}


/**
 * Rebuilds the environment within the specified bounds. Used to rebuild the environment after
 * the user either exits the program, or builds a new maze.
 * @param mc The MinecraftConnection object.
 * @param savedEnvironment A 3D vector containing the environment within the specified bounds.
*/
void Maze::rebuildEnvironment(mcpp::MinecraftConnection* mc,
                        const std::vector<std::vector<std::vector<mcpp::BlockType>>>& savedEnvironment) {

    // Format is [y][x][z]
    int yLen = savedEnvironment.size();
    int xLen = savedEnvironment[0].size();
    int zLen = savedEnvironment[0][0].size(); 

    for (int y = 0; y < yLen; y++) {
        for (int x = 0; x < xLen; x++) {
            for (int z = 0; z < zLen; z++) {
                mcpp::Coordinate newCoord(basePoint.x + x, basePoint.y + y, basePoint.z + z);
                mc->setBlock(newCoord, savedEnvironment[y][x][z]);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }
}

/**
 * Flattens the environment within the specified bounds, and then builds the maze.
 * @param mc The MinecraftConnection object.
*/
void Maze::FlattenAndBuild(mcpp::MinecraftConnection* mc) {
    flattenEnvironment(mc);
    GenerateMazeInMC(mc);
}

/**
 * If program is generating blocks outside of the bounds of getEnvironment, this function can be used
 * to store the old block type and coordinate, so that it can be restored later.
 * @param mc The MinecraftConnection object.
 * @param coord The coordinate of the block to be stored.
*/
void Maze::StoreOldBlock(mcpp::MinecraftConnection* mc, mcpp::Coordinate& coord) {
    GeneratedBlock oldBlock;
    oldBlock.originalType = mc->getBlock(coord);
    oldBlock.coordinate = coord;
    generatedBlocks.push_back(oldBlock);
}

/**
 * Restores the old blocks to their original type. Used to restore the environment after the user
 * either exits the program, or builds a new maze.
 * @param mc The MinecraftConnection object.
*/
void Maze::RestoreOldBlocksFirst(mcpp::MinecraftConnection* mc) {
    for (const auto& block : generatedBlocks) {
        // Set block.coordinate to block.originalType
        mc->setBlock(block.coordinate, block.originalType);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
