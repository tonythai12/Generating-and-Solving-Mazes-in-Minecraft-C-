#include "Maze.h"
#include <random>


Maze::Maze(mcpp::Coordinate basePoint, unsigned int xlen, 
                                    unsigned int zlen,
                                    bool mode)
{
    this->basePoint = basePoint;
    this->height = xlen;
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
    for (unsigned int i = 0; i < height; i++)
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
    for (unsigned int i = 0; i < height; i++)
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

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            std::cout << maze[i][j] << " ";
        }
        std::cout << std::endl;
    }
}