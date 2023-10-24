#include "Maze.h"
#include <random>


Maze::Maze(mcpp::Coordinate basePoint, unsigned int xlen, 
                                    unsigned int zlen,
                                    bool mode)
{
    this->basePoint = basePoint;
    this->length = xlen;
    this->width = zlen;
    this->mode = mode;
}

Maze::~Maze()
{

}

void Maze::generateMazeStructure()
{
    // Generate the maze
    // 1. Initialize the maze
    for (int i = 0; i < height; i++)
    {
        std::vector<int> temp;
        for (int j = 0; j < width; j++)
        {
            temp.push_back(0);
        }
        maze.push_back(temp);
    }

    // Generate the maze structure
    for (int i = 0; i < height; i++)
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

                if (i > 0 && i < height - 1 && j > 0 && j < width - 1) {
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

                if (i > 0 && i < height - 1 && j > 0 && j < width - 1) {
                    innerWallCells.push_back(Cell{basePoint + mcpp::Coordinate(i, 0, j), false});
                }
            }
        }
        std::cout << std::endl;
    }


    // Select a random starting point
    mcpp::Coordinate randomStart = Maze::selectRandomStartingPoint();
    Cell start{randomStart, true};

    // Select a random outer wall cell
    Cell outerWallCell = Maze::OuterWallCell(randomStart);

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
    for (int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            if (maze[i][j] == 0)
            {
                startingPoints.push_back(basePoint + mcpp::Coordinate(i, 0, j));
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
    for (int i = 0; i < cells.size(); i++)
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
    for (int i = 0; i < cells.size(); i++)
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
    for (int i = 0; i < cells.size(); i++)
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
    for (int i = 0; i < cells.size(); i++)
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
    for (int i = 0; i < innerWallCells.size(); i++)
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
    for(int i = 0; i < innerWallCells.size(); i++){
        if (innerWallCells.at(i).coordinate == cell)
        {
            return true;
        }
    }

    return false;
}

bool Maze::hasUnvisitedNeighbour(Cell cell) {
    static int dx[] = {0, 1, 0, -1};
    static int dz[] = {-1, 0, 1, 0};

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
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(1, 0, 0);
            break;

        case 1:
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, 1);
            break;

        case 2:
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(1, 0, 0);
            break;

        case 3:
            newCell.coordinate = cell.coordinate + mcpp::Coordinate(0, 0, -1);
            break;

        default:
            break;
        }

        if(isInsideMaze(newCell.coordinate) && !visited(newCell)){
            if(newCell.coordinate.x > cell.coordinate.x) {
                
            }
            removeWall(newCell);
            generateRecursiveMaze(cells, newCell);
        }
    }
}

bool Maze::visited(Cell cell) {
    for (int i = 0; i < visitedCells.size(); i++)
    {
        if (visitedCells.at(i).coordinate == cell.coordinate)
        {
            return true;
        }
    }

    return false;
}