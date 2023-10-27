#include "Maze.h"
#include <random>
#include <vector>
// For sleep and time duration
#include <thread>   
#include <chrono> 


Maze::Maze(mcpp::Coordinate basePoint, int xlen, int zlen, std::vector<std::vector<char>> mazeStructure)
    : basePoint(basePoint), length(xlen), width(zlen), mazeStructure(mazeStructure) //mode(mode)
{
}

Maze::~Maze()
{

}

void Maze::generateMazeStructure()
{
    // Generate the maze
    // 1. Initialize the maze
    for (int i = 0; i < length; i++)
    {
        std::vector<int> temp;
        for (int j = 0; j < width; j++)
        {
            temp.push_back(0);
        }
        maze.push_back(temp);
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
                mc.setBlock(basePoint + mcpp::Coordinate(i, 0, j), mcpp::Blocks::BRICKS);
                mc.setBlock(basePoint + mcpp::Coordinate(i, 1, j), mcpp::Blocks::BRICKS);
                mc.setBlock(basePoint + mcpp::Coordinate(i, 2, j), mcpp::Blocks::BRICKS);

                // Add the coordinate to cell vector and set isVisited to false
                cells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});

                if (i > 0 && i < length - 1 && j > 0 && j < width - 1) {
                    innerWallCells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});
                }
            }
            else
            {
                mc.setBlock(basePoint + mcpp::Coordinate(i, 0, j), mcpp::Blocks::AIR);
                mc.setBlock(basePoint + mcpp::Coordinate(i, 1, j), mcpp::Blocks::AIR);
                mc.setBlock(basePoint + mcpp::Coordinate(i, 2, j), mcpp::Blocks::AIR);

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

    // print the starting point
    mc.setBlock(randomStart, mcpp::Blocks::DIAMOND_BLOCK);

    // Select a random outer wall cell
    Cell outerWallCell = Maze::OuterWallCell(randomStart);

    // print the outer wall cell
    std::cout << "Outer wall cell: " << outerWallCell.coordinate.x << ", " << outerWallCell.coordinate.y << ", " << outerWallCell.coordinate.z << std::endl;

    visitedCells.push_back(outerWallCell);
    
    // Make way in the maze
    removeWall(outerWallCell);

    // Recursively make way in the maze
    generateRecursiveMaze(innerWallCells, start);

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
    Cell outerWallCell;
    bool isOuterWall = false;
    
    outerWallCell.coordinate = cell - mcpp::Coordinate(2, 0, 0);
    for (int i = 0; i < static_cast<int>(cells.size()); i++)
    {
        if (outerWallCell.coordinate == cells.at(i).coordinate)
        {
            isOuterWall = false;
            break;
        }
        else
        {
            isOuterWall = true;
        }
    }

    if (isOuterWall)
    {
        outerWallCell.coordinate = cell - mcpp::Coordinate(1, 0, 0);
        return outerWallCell;
    }

    outerWallCell.coordinate = cell + mcpp::Coordinate(2, 0, 0);
    for (int i = 0; i < static_cast<int>(cells.size()); i++)
    {
        if (outerWallCell.coordinate == cells.at(i).coordinate)
        {
            isOuterWall = false;
            break;
        }
        else
        {
            isOuterWall = true;
        }
    }

    if (isOuterWall)
    {
        outerWallCell.coordinate = cell + mcpp::Coordinate(1, 0, 0);
        return outerWallCell;
    }

    outerWallCell.coordinate = cell - mcpp::Coordinate(0, 0, 2);
    for (int i = 0; i < (int)static_cast<int>(cells.size()); i++)
    {
        if (outerWallCell.coordinate == cells.at(i).coordinate)
        {
            isOuterWall = false;
            break;
        }
        else
        {
            isOuterWall = true;
        }
    }

    if (isOuterWall)
    {
        outerWallCell.coordinate = cell - mcpp::Coordinate(0, 0, 1);
        return outerWallCell;
    }

    outerWallCell.coordinate = cell + mcpp::Coordinate(0, 0, 2);
    for (int i = 0; i < static_cast<int>(cells.size()); i++)
    {
        if (outerWallCell.coordinate == cells.at(i).coordinate)
        {
            isOuterWall = false;
            break;
        }
        else
        {
            isOuterWall = true;
        }
    }

    if (isOuterWall)
    {
        outerWallCell.coordinate = cell + mcpp::Coordinate(0, 0, 1);
        return outerWallCell;
    }

    return outerWallCell;

}

void Maze::removeWall(Maze::Cell cell){
    mcpp::MinecraftConnection mc;
    mc.setBlock(cell.coordinate, mcpp::Blocks::AIR);
    mc.setBlock(cell.coordinate + mcpp::Coordinate(0, 1, 0), mcpp::Blocks::AIR);
    mc.setBlock(cell.coordinate + mcpp::Coordinate(0, 2, 0), mcpp::Blocks::AIR);

    // Set the cell to visited
    for (int i = 0; i < static_cast<int>(innerWallCells.size()); i++)
    {
        if (innerWallCells.at(i).coordinate == cell.coordinate)
        {
            innerWallCells.at(i).isVisited = true;
            break;
        }
    }

    
}

int Maze::getRandomDirection() {
    return rand() % 4;
}

bool Maze::isInsideMaze(mcpp::Coordinate cell){
    for(int i = 0; i < static_cast<int>(innerWallCells.size()); i++){
        if (innerWallCells.at(i).coordinate == cell)
        {
            return true;
        }
    }

    return false;
}

bool Maze::hasUnvisitedNeighbour(Cell cell) {
    static int dx[] = {0, 2, 0, -2};
    static int dz[] = {-2, 0, 2, 0};

    for (int i = 0; i < 4; i++)
    {
        Cell neighbour;
        neighbour.coordinate = cell.coordinate + mcpp::Coordinate(dx[i], 0, dz[i]);
        if (isInsideMaze(neighbour.coordinate) && !visited(neighbour))
        {
            return true;
        }
    }

    return false;
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

        switch (direction)
        {
        case 0:
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(2, 0, 0);
            tempCell.coordinate = cell.coordinate + mcpp::Coordinate(1, 0, 0);
            break;

        case 1:
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, 2);
            tempCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, 1);
            break;

        case 2:
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(-2, 0, 0);
            tempCell.coordinate = cell.coordinate + mcpp::Coordinate(-1, 0, 0);
            break;

        case 3:
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, -2);
            tempCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, -1);
            break;

        default:
            break;
        }

        if(isInsideMaze(newCell.coordinate) && !visited(newCell)){
            removeWall(tempCell);
            generateRecursiveMaze(cells, newCell);
        }
    }
}

bool Maze::visited(Cell cell) {
    for (int i = 0; i < static_cast<int>(visitedCells.size()); i++)
    {
        if (visitedCells.at(i).coordinate == cell.coordinate)
        {
            return true;
        }
    }

    return false;
}

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

void Maze::GenerateMazeInMC(mcpp::MinecraftConnection* mc) {
    int zLen = mazeStructure.size();
    int xLen = mazeStructure[0].size();
    int yLen = 3;  // Height of the maze

    for (int y = 0; y < yLen; y++) { 
        for (int i = 0; i < zLen; i++) {  
            for (int j = 0; j < xLen; j++) {  
                if (mazeStructure[i][j] == 'x') {
                    mc->setBlock(basePoint + mcpp::Coordinate(i, y + 1, j), mcpp::Blocks::BRICKS);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                } else {
                    mc->setBlock(basePoint + mcpp::Coordinate(i, y + 1, j), mcpp::Blocks::AIR);
                    walkableCoords.push_back(basePoint + mcpp::Coordinate(i, y + 1, j));
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
        }
    }
}

void Maze::UndoMaze(mcpp::MinecraftConnection* mc) {
    int zLen = mazeStructure.size();
    int xLen = mazeStructure[0].size();
    int yLen = 3;  // Height of the maze

    for (int y = 0; y < yLen; y++) { 
        for (int i = 0; i < zLen; i++) {  
            for (int j = 0; j < xLen; j++) {  
                if (mazeStructure[i][j] == 'x' || mazeStructure[i][j] == '.') {
                    mc->setBlock(basePoint + mcpp::Coordinate(i, y, j), mcpp::Blocks::AIR);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
        }
    }
}


/*
 *  Retrieves the blockTypes of the environment within the specified bounds.
 *  Stored in a 3D vector, which will be used to restore the environment after solving the maze.
 */
std::vector<std::vector<std::vector<mcpp::BlockType>>> Maze::getEnvironment(mcpp::MinecraftConnection* mc) {
    
    // Calculate corners and then get heights
    mcpp::Coordinate corner1(basePoint.x, basePoint.y, basePoint.z);
    mcpp::Coordinate corner2(basePoint.x + length, basePoint.y, basePoint.z + width);
    auto envHeights = mc->getHeights(corner1, corner2);

    // Find min and max y-values
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();
    for (const auto& row : envHeights) {
        for (int height : row) {
            if (height < minY) minY = height;
            if (height > maxY) maxY = height;
        }
    }

    // Get all blocks using the min/max y-values
    mcpp::Coordinate loc1 = mcpp::Coordinate(basePoint.x, minY, basePoint.z);
    mcpp::Coordinate loc2 = mcpp::Coordinate(basePoint.x + length, maxY, basePoint.z + width);
    auto blocks = mc->getBlocks(loc1, loc2);

    return blocks;
}

/*
    Flattens the environment within the specified bounds. 
*/
void Maze::flattenEnvironment(mcpp::MinecraftConnection* mc) {
    // Calculate corners and then get heights
    mcpp::Coordinate corner1(basePoint.x, basePoint.y, basePoint.z);
    mcpp::Coordinate corner2(basePoint.x + length, basePoint.y, basePoint.z + width);

    // Heights of the environment at (x, z) (as y-coords are different for each pair)
    auto heights = mc->getHeights(corner1, corner2);
    int floorLevel = corner1.y;
    
    // Assume [x][z] for testing
    for (int x = 0; x < static_cast<int>(heights.size()); x++) {
        for (int z = 0; z < static_cast<int>(heights[x].size()); z++) {
            int highestBlockY = heights[x][z];

            // Set blocks above the floorLevel and up to the highest block to AIR
            for (int y = floorLevel + 1; y <= highestBlockY; y++) {
                mcpp::Coordinate coord(corner1.x + x, y, corner1.z + z);
                mc->setBlock(coord, mcpp::Blocks::AIR);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            // Set the block at floorLevel to GRASS
            mcpp::Coordinate coord(corner1.x + x, floorLevel, corner1.z + z);
            mc->setBlock(coord, mcpp::Blocks::GRASS);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // Set blocks below floorLevel to GRASS
            for (int y = highestBlockY; y < floorLevel; y++) {
                mcpp::Coordinate coord(corner1.x + x, y, corner1.z + z);
                mc->setBlock(coord, mcpp::Blocks::GRASS);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    }
}

/*
    Rebuilds the environment after the user exits the program. Uses the 3D vector returned from getEnvironment().
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

void Maze::FlattenAndBuild(mcpp::MinecraftConnection* mc) {
    flattenEnvironment(mc);
    GenerateMazeInMC(mc);
}