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
    for (unsigned int i = 0; i < length; i++)
    {
        std::vector<int> temp;
        for (unsigned int j = 0; j < width; j++)
        {
            temp.push_back(0);
        }
        maze.push_back(temp);
    }

    // 2. Generate the maze
    // 2.1 Generate the random number
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    // 2.2 Generate the maze structure
    for (unsigned int i = 0; i < length; i++)
    {
        for (unsigned int j = 0; j < width; j++)
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
                mc.setBlock(basePoint + mcpp::Coordinate(i, 1, j), mcpp::Blocks::BRICKS);
            }
            else
            {
                mc.setBlock(basePoint + mcpp::Coordinate(i, 0, j), mcpp::Blocks::AIR);
                mc.setBlock(basePoint + mcpp::Coordinate(i, 1, j), mcpp::Blocks::AIR);
                mc.setBlock(basePoint + mcpp::Coordinate(i, 1, j), mcpp::Blocks::AIR);
            }
        }
        std::cout << std::endl;
    }
}

mcpp::Coordinate Maze::selectRandomStartingPoint(){
    
    mcpp::Coordinate randomStart;
    
    // Array to store mccp::Coordinate of the starting point
    std::vector<mcpp::Coordinate> startingPoints;

    for (int i = 0; i < length; i++)
    {
        for(int j = 0; j < width; j++)
        {
            if (maze[i][j] == 0)
            {
                if (j < j / 2 && i < i / 2) {
                    startingPoints.push_back(mcpp::Coordinate(i - 1, 0, j - 1));
                }
                else if (j < j / 2 && i > i / 2)
                {
                    startingPoints.push_back(mcpp::Coordinate(i + 1, 0, j - 1));
                }
                else if (j > j / 2 && i < i / 2)
                {
                    startingPoints.push_back(mcpp::Coordinate(i - 1, 0, j + 1));
                }
                else if (j > j / 2 && i > i / 2)
                {
                    startingPoints.push_back(mcpp::Coordinate(i + 1, 0, j + 1));
                }
                
            }
        }

        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distribution(0, startingPoints.size() - 1);
        int random_index = distribution(generator);
        randomStart = startingPoints.at(random_index);
    }
    
    return randomStart;
}