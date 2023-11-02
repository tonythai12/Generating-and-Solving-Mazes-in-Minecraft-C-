#include "Maze.h"
#include <random>
#include <vector>
// For sleep and time duration
#include <thread>   
#include <chrono>

#define BUFFER 2

Maze::Maze(mcpp::Coordinate basePoint, int xlen, int zlen, std::vector<std::vector<char>> mazeStructure)
: basePoint(basePoint), length(xlen), width(zlen), mazeStructure(mazeStructure)
{
}

Maze::~Maze()
{
    std::cout << "Maze destructor successfully called." << std::endl;
}

/**
 * @brief Generates the initial structure of the maze.
 * 
 * This function initializes the 'maze' and 'mazeStructure' member variables
 * to represent the layout of the maze. Walls are represented by 1s and open
 * cells by 0s in 'maze'. In 'mazeStructure', walls are represented by 'x'
 * and open cells by '.'.
 * 
 * The maze is generated such that every other row and every other column are walls.
 * Specifically, all even-indexed rows are walls, and within odd-indexed rows, even-indexed
 * columns are walls.
 */
void Maze::generateMazeStructure() {   
    // Clear any previous data in maze and mazeStructure vectors
    maze.clear();
    mazeStructure.clear();

    // Clear any previous data in maze and mazeStructure vectors
    for (int i = 0; i < length; i++) {
        std::vector<int> temp;
        std::vector<char> temp2;
        for (int j = 0; j < width; j++) {
            temp.push_back(0);
            temp2.push_back('.');
        }
        maze.push_back(temp);
        mazeStructure.push_back(temp2);
    }

    // Set wall positions in the maze vector
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {

            // If the row index is even, mark the whole row as a wall
            if (i % 2 == 0) {
                maze[i][j] = 1;  // Set wall
            } else {
                if (j % 2 == 0) {
                    maze[i][j] = 1; // Set wall
                }
            }
        }
    }

    // Update mazeStructure based on the value in maze
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            mazeStructure[i][j] = (maze[i][j] == 1) ? 'x' : '.';
        }
    }
}

/**
 * @brief Generates a maze based on the selected mode.
 * 
 * This function first calls generateMazeStructure() to initialize the maze. Then, it populates
 * the 'cells' and 'innerWallCells' vectors based on the structure. Finally, it chooses a
 * starting point and an outer wall to remove, beginning the maze generation process.
 * 
 * @param mode Boolean value that signifies the mode (NORMAL_MODE or TESTING_MODE).
 */
void Maze::generateMaze(bool mode) {
    
    Maze::generateMazeStructure();

    for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {

            if (maze[i][j] == 1) {
                // Add the coordinate to cell vector and set isVisited to false
                cells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});

                if (i > 0 && i < length - 1 && j > 0 && j < width - 1) {
                    innerWallCells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});
                }

            } else {
                // Add the coordinate of the cell to the vector
                cells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});

                if (i > 0 && i < length - 1 && j > 0 && j < width - 1) {
                    innerWallCells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});
                }
            }
        }
    }

    mcpp::Coordinate randomStart;
    Cell outerWallCell;
    Cell start;
    mcpp::Coordinate posOuter;

    if (mode == NORMAL_MODE) {
        // Select a random starting point
        randomStart = Maze::selectRandomStartingPoint();
        start = Cell{randomStart, true};

        // Select a random outer wall cell
        outerWallCell = Maze::OuterWallCell(randomStart);

        posOuter = outerWallCell.coordinate - basePoint;
        maze[posOuter.x][posOuter.z] = 0;

    } else if (mode == TESTING_MODE) {
        randomStart = basePoint + mcpp::Coordinate(1, 0, 1);
        start = Cell{randomStart, true};

        outerWallCell = Cell{basePoint + mcpp::Coordinate(1, 0, 0), true};

        posOuter = outerWallCell.coordinate - basePoint;
        maze[posOuter.x][posOuter.z] = 0;
        
    }
    // Make way in the maze
    removeWall(outerWallCell);

    // Recursively make way in the maze
    generateRecursiveMaze(innerWallCells, start, mode);

}

/**
 * @brief Updates the 'mazeStructure' vector based on the 'maze' vector.
 * 
 * This function iterates over each cell in the 'maze' 2D vector. It then sets the corresponding
 * cell in the 'mazeStructure' 2D vector to 'x' if the cell in 'maze' is a wall (has a value of 1),
 * and sets it to '.' otherwise (open cell).
 */
void Maze::updateMazeStructure() {

   for (int i = 0; i < length; i++) {
        for (int j = 0; j < width; j++) {
            if (maze[i][j] == 1) {
                mazeStructure[i][j] = 'x';
            } else
            {
                mazeStructure[i][j] = '.';
            }
        }
    }
}

/**
 * @brief Selects a random starting point from the available options in the maze.
 * 
 * This function searches the perimeter of the maze grid for cells that are open (having a value of 0).
 * It then selects one of these open cells at random and returns its coordinate.
 * 
 * @return mcpp::Coordinate representing the randomly chosen starting point in the maze.
 */
mcpp::Coordinate Maze::selectRandomStartingPoint() {
    
    mcpp::Coordinate randomStart;
    
    // Vector to store mccp::Coordinate of the starting point
    std::vector<mcpp::Coordinate> startingPoints;

    // If there is 0 in the maze, then it is the starting point
    for (int i = 0; i < length; i++) {
        for(int j = 0; j < width; j++) {
            if (i == 1 || j == 1 || i == length - 2 || j == width - 2) {
                if (maze[i][j] == 0) {
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

Maze::Cell Maze::OuterWallCell(mcpp::Coordinate cell) {
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

        if (isOuterWall) {
            if (i == 0) {
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

/**
 * @brief Finds a cell that can serve as an "outer wall" relative to a given cell.
 * 
 * This function explores potential cells at a distance of 2 units away from the given cell 
 * in all four cardinal directions (North, South, East, West). 
 * It then checks whether these potential cells already exist in the 'cells' vector.
 * If a potential cell does not exist in 'cells', it is considered as an "outer wall" cell.
 *
 * @param cell The given cell's coordinate relative to which an "outer wall" cell is to be found.
 * @return Cell representing the coordinate of the identified "outer wall" cell.
 */
void Maze::removeWall(Maze::Cell cell) {
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

/**
 * @brief Chooses a random direction.
*/
int Maze::getRandomDirection() {
    return rand() % 4;
}

bool Maze::isInsideMaze(mcpp::Coordinate cell) {

    bool isInside = false;
    for (int i = 0; i < static_cast<int>(innerWallCells.size()); i++) {
        if (innerWallCells.at(i).coordinate == cell) {
            isInside = true;
        }
    }
    return isInside;
}

/**
 * @brief Checks if a given cell has any unvisited neighbors.
 * 
 * This function uses the given cell's coordinates to look for neighbors in all four cardinal 
 * directions (North, South, East, West), specifically 2 units away from the current cell. 
 * Then, it checks if each neighbor is both inside the maze boundaries and has not been visited yet.
 *
 * @param cell The cell for which to check for unvisited neighbors.
 * @return A boolean value indicating whether the cell has at least one unvisited neighbor.
 */
bool Maze::hasUnvisitedNeighbour(Cell cell) {
    static int dx[] = {0, 2, 0, -2};
    static int dz[] = {-2, 0, 2, 0};

    bool hasUnvisitedNeighbour = false;

    for (int i = 0; i < 4; i++) {
        Cell neighbour;
        neighbour.coordinate = cell.coordinate + mcpp::Coordinate(dx[i], 0, dz[i]);

        if (isInsideMaze(neighbour.coordinate) && !visited(neighbour)) {
            hasUnvisitedNeighbour = true;
        }
    }

    return hasUnvisitedNeighbour;
}

/**
 * @brief Recursively generates a maze structure.
 *
 * This function modifies the maze by carving out paths. It uses recursion to
 * traverse through the maze, removing walls to form paths. It has two modes:
 * NORMAL_MODE and TESTING_MODE, which affect how the maze is generated.
 *
 * @param cells Reference to a vector containing all cells of the maze.
 * @param cell Current cell being processed.
 * @param mode Flag indicating the mode of maze generation (NORMAL_MODE/TESTING_MODE).
 */
void Maze::generateRecursiveMaze(std::vector<Cell> &cells, Cell cell, bool mode) {

    cell.isVisited = true;
    visitedCells.push_back(cell);
    int direction;

    while (hasUnvisitedNeighbour(cell)) {
        // Create a new cell
        Cell newCell;
        Cell tempCell;

        if (mode == NORMAL_MODE) {
            direction = getRandomDirection();
       
            if (direction == 0) {
                newCell.coordinate = cell.coordinate + mcpp::Coordinate(-2, 0, 0);
                tempCell.coordinate = cell.coordinate + mcpp::Coordinate(-1, 0, 0);
            }
            else if (direction == 1) {
                newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, 2);
                tempCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, 1);
            }
            else if (direction == 2) {
                newCell.coordinate = cell.coordinate + mcpp::Coordinate(2, 0, 0);
                tempCell.coordinate = cell.coordinate + mcpp::Coordinate(1, 0, 0);
            }
            else if (direction == 3) {
                newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, -2);
                tempCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, -1);
            }

            if (isInsideMaze(newCell.coordinate) && !visited(newCell)) {
                removeWall(tempCell);
                generateRecursiveMaze(cells, newCell, mode);
            }

        } else if (mode == TESTING_MODE) {
            direction = counter;
            if (counter == 4) {
                this->counter = 0;
            } else {
                this->counter++;
            }
            std::cout << "Direction: " << direction << std::endl;

            if (direction == 0) {
                newCell.coordinate = cell.coordinate + mcpp::Coordinate(-2, 0, 0);
                tempCell.coordinate = cell.coordinate + mcpp::Coordinate(-1, 0, 0);
            }
            else if (direction == 1) {
                newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, 2);
                tempCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, 1);
            }
            else if (direction == 2) {
                newCell.coordinate = cell.coordinate + mcpp::Coordinate(2, 0, 0);
                tempCell.coordinate = cell.coordinate + mcpp::Coordinate(1, 0, 0);
            }
            else if (direction == 3) {
                newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, -2);
                tempCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, -1);
            }

            if (isInsideMaze(newCell.coordinate) && !visited(newCell)) {
                removeWall(tempCell);
                generateRecursiveMaze(cells, newCell, mode);
            }
        }
    } 
}

bool Maze::visited(Cell cell) {
    bool visited = false;
    for (int i = 0; i < static_cast<int>(visitedCells.size()); i++) {
        if (visitedCells.at(i).coordinate == cell.coordinate) {
            visited = true;
        }
    }
    return visited;
}

/**
 * Prints the maze structure to the console.
 */
void Maze::PrintMaze() {
    int zLen = static_cast<int>(mazeStructure.size());
    int xLen = static_cast<int>(mazeStructure[0].size());

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
    int zLen = static_cast<int>(mazeStructure.size());
    int xLen = static_cast<int>(mazeStructure[0].size());
    int yLen = 3;  // Height of the maze

    for (int y = 0; y < yLen; y++) { 
        for (int i = 0; i < zLen; i++) {  
            for (int j = 0; j < xLen; j++) {  
                if (mazeStructure[i][j] == 'x') {
                    mc->setBlock(basePoint + mcpp::Coordinate(i, y + 1, j), mcpp::Blocks::ACACIA_WOOD_PLANK);
                    std::this_thread::sleep_for(std::chrono::milliseconds(40));
                } else {
                    // if (!(mc->getBlock(basePoint + mcpp::Coordinate(i, y + 1, j)) == mcpp::Blocks::AIR)) {
                    //     mc->setBlock(basePoint + mcpp::Coordinate(i, y + 1, j), mcpp::Blocks::AIR);
                    // }
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
    mcpp::Coordinate loc2 = mcpp::Coordinate(basePoint.x + length + BUFFER, basePoint.y + 4, basePoint.z + width + BUFFER);
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
                mcpp::Coordinate coord1(corner1.x + x, y, corner1.z + z);
                if (!(basePointBlock == mcpp::Blocks::AIR)) {
                    StoreOldBlock(mc, coord1);
                    mc->setBlock(coord1, mcpp::Blocks::AIR);
                    std::this_thread::sleep_for(std::chrono::milliseconds(40));
                }
            }

            // Set the block at floorLevel to BlockType at basePoint
            mcpp::Coordinate coord(corner1.x + x, floorLevel, corner1.z + z);
            StoreOldBlock(mc, coord);
            mc->setBlock(coord, basePointBlock);

            // Set blocks below floorLevel to BlockType at basePoint
            for (int y = highestBlockY; y < floorLevel; y++) {
                mcpp::Coordinate coord2(corner1.x + x, y, corner1.z + z);
                StoreOldBlock(mc, coord2);
                mc->setBlock(coord2, basePointBlock);
                std::this_thread::sleep_for(std::chrono::milliseconds(40));
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
    int yLen = static_cast<int>(savedEnvironment.size());
    int xLen = static_cast<int>(savedEnvironment[0].size());
    int zLen = static_cast<int>(savedEnvironment[0][0].size()); 

    for (int y = 0; y < yLen; y++) {
        for (int x = 0; x < xLen; x++) {
            for (int z = 0; z < zLen; z++) {
                mcpp::Coordinate newCoord(basePoint.x + x - BUFFER, basePoint.y + y, basePoint.z + z - BUFFER);
                mc->setBlock(newCoord, savedEnvironment[y][x][z]);
                std::this_thread::sleep_for(std::chrono::milliseconds(40));
                //std::cout << "Rebuilding " << savedEnvironment[y][x][z] << " at (" 
                //<< newCoord.x << ", " << newCoord.y << ", " << newCoord.z << ")" << std::endl;
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
void Maze::StoreOldBlock(mcpp::MinecraftConnection* mc, const mcpp::Coordinate& coord) {
    GeneratedBlock oldBlock(mc->getBlock(coord), coord);
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
        //std::cout << "Restoring " << block.originalType << " at (" << block.coordinate.x << ", " << 
        //block.coordinate.y << ", " << block.coordinate.z << ")" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
