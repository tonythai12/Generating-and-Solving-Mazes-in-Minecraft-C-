[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=12196410&assignment_repo_type=AssignmentRepo)

# Assignment 3: Generating and solving mazes in Minecraft via C++

This program uses C++ and the external [```mcpp```](https://github.com/rozukke/mcpp) library to build and generate mazes in Minecraft.

**Table of Contents**

- [Assignment 3: Generating and solving mazes in Minecraft via C++](#assignment-3-generating-and-solving-mazes-in-minecraft-via-c)
  - [Key Features](#key-features)
  - [Technical aspects](#technical-aspects)
  - [How to Use](#how-to-use)
    - [Test Mode](#test-mode)
    - [Breadth-First Search](#breadth-first-search)
  - [Assumptions](#assumptions)
    - [General Assumptions](#general-assumptions)
    - [Maze Assumptions](#maze-assumptions)
    - [Solving Algorithm Assumptions](#solving-algorithm-assumptions)
    - [Agent (player) Assumptions](#agent-player-assumptions)
    - [User Interaction Assumptions](#user-interaction-assumptions)
  - [Need to Knows](#need-to-knows)
  - [Data Structures](#data-structures)
    - [Custom structs](#custom-structs)
    - [Vectors vs other Data Structures (DS)](#vectors-vs-other-data-structures-ds)
    - [Maps, Sets, and Tuples](#maps-sets-and-tuples)
    - [Private member variables of the Agent class](#private-member-variables-of-the-agent-class)
  - [Contributors and Workload](#contributors-and-workload)

## Key Features

- **Build Mazes:** Users can create their own 'perfect' or 'looped' mazes by specifying the maze structure.

- **Generate Mazes:** Automatically generate a 'perfect' maze without loops using the recursive backtracking algorithm.

- **Solve Mazes:** Optionally, users can receive help to solve the maze using either the Right-Hand Wall-Follower (RHWF) algorithm or the Breadth-First Search (BFS) algorithm.

- **Environment Management:** After a maze is built or the program exits, the Minecraft environment is restored to its prior state.

## Technical aspects

- **Error Handling:** All incorrect user inputs are handled gracefully.

- **Algorithm Robustness:** Special conditions for RHWF and BFS to avoid infinite loops.

- **Efficiency:** Most variables are passed by reference to enhance performance.

## How to Use

### Test Mode

- Run ```./mazeRunner -testmode``` to enable test mode. This initialises the ```Agent``` using ```InitialisePlayer()``` such that their right hand is facing the positive ```x``` direction. Then, the algorithm turns the agent clockwise until its right hand faces a wall. Additionally, they are always be placed in the empty cell that is closest to the lower-right edge (furthest from the ```basePoint```) of the maze.

### Breadth-First Search

- Run ```./mazeRunner -e2``` to enable BFS instead of RHWF. This significantly speeds up the solving algorithm.

## Assumptions

### General Assumptions

- **Language and Libraries:** The project is written in C++ and uses the [```mcpp```](https://github.com/rozukke/mcpp) library for Minecraft interactions.

- **Operating System:** Assumes a Unix-like (or compatible) operating system for compilation and execution.

- **Minecraft Server:** Assumes that a Minecraft server is running and accessible, with the [```mcpp```](https://github.com/rozukke/mcpp) library compatible with the server version.

### Maze Assumptions

- **Maze Construction:** Assumes that the maze can only be built in a predefined location in the Minecraft world. This predefined location is known as the ```basePoint```, consisting of ```(x, y, z)``` coordinates.

- **Maze Structure:** Assumes that the maze uses ```mcpp::Blocks::ACACIA_WOOD_PLANK``` for walls, and ```mcpp::Blocks::AIR``` for empty spaces.

- **Maze Pathways:** Assumes that a path that a user can walk has a ```width``` of ```1```.

- **Maze Generation:** Assumes the maze has a single starting and ending point. Also assumes that if reading a maze from the terminal, it will be a 'perfect' maze.

- **Maze Exit:** Assumes the maze has only one exit point. Assumes that the maze exit is a total of 5 air blocks in a 180-degree fashion in front of the player, relative to their current direction. The ```basePoint.y``` coordinate remains unchanged.

- **Base Point:** Assumes that the user chooses a ```basePoint``` that contains a solid block (e.g., ```mcpp::Blocks::GRASS```, ```mcpp::Blocks::STONE```) and NOT blocks that they can walk through or damage them (e.g., ```mcpp::Blocks::STILL_WATER```, ```mcpp::Blocks::STILL_LAVA```). This is because the building algorithm gets the block (using ```getBlock(x, z)```) at the ```basePoint``` and makes that the foundation for the ground of the maze.

### Solving Algorithm Assumptions

- **BFS:** Assumes that the maze is 'perfect' with only one exit point. Multiple exit points or a loop will lead to undefined behaviour, including infinite loops. See [User Interaction](#user-interaction-assumptions) for more information.

- **RHWF:** Assumes that the maze is 'perfect' with only one exit point. Multiple exit points may lead to undefined behaviour. Loops are designed to be detected after the agent has visited a tile with the same direction faced ```N``` times, but are not guaranteed to caught. A looped maze may cause an infinite loop.

### Agent (player) Assumptions

- **Initial Position:** Assumes that the agent starts at a specified location (through the "Solve Manually" option in the "Solve Maze" menu). This initial position is obtained through ```Agent::getStartLoc()```.

- **Direction:** Assumes the agent initially faces the ```"UP"/"NORTH"/"MOVE_ZMINUS"``` direction. In `-testmode`, this varies based on the presence of a wall.

- **Movement:** Assumes that the agent can only move in four directions - ```"UP"/"NORTH"/"MOVE_ZMINUS"```, ```"DOWN"/"SOUTH"/"MOVE_ZPLUS"```, ```"LEFT"/"WEST"/"MOVE_XMINUS"```, and ```"RIGHT"/"EAST/"MOVE_XPLUS"```. Assumes the agent cannot move in the ```y``` direction.

### User Interaction Assumptions

- **Command Line Arguments:** Assumes specific command-line arguments for different program behaviour, like ```-testmode``` for testing and/or ```-e2``` for enabling BFS. Both can be used together. Valid usage:

  - ```./mazeRunner```
  - ```./mazeRunner -testmode```
  - ```./mazeRunner -e2```
  - ```./mazeRunner -testmode -e2```

- **Input:** Assumes that the user will provide maze configurations in a specific format - ```x``` for walls, and ```.``` for empty spaces. These will be of data type ```char```. Assumes the ```length``` and ```width``` will be of data type positive ```int```. The program is designed to catch invalid inputs and re-prompt the user.

- **Environment:** Assumes the user will not destroy any walls at any time after the maze has been built. This may lead to RHWF and BFS providing incorrect solutions and being stuck in infinite loops. Similarly, assumes the user will not place any blocks (walls) at any time after the maze has been built. This may lead to RHWF and BFS providing incorrect solutions.

- **Prior to Maze Building:** Assumes that the user presses the space-bar twice (to initiate flying mode) before building the maze in Minecraft. This will ensure that they stay in the air when the building algorithm teleports them to ```(basePoint.x, basePoint.y + 10, basePoint.z)```.

## Need to Knows

- The program dynamically allocates memory for the ```mcpp::MinecraftConnection``` object, the ```Agent``` object, and the ```Maze``` object, using the ```new``` keyword. They have appropriate destructors that will be called when either building a new maze or exiting the program.

- Restoring the environment to its previous state may take a long time, depending on the existing structure. If you believe that the program is stuck in a loop or unresponsive, uncomment the ```std::cout``` statements found in ```Maze::RestoreOldBlocksFirst()``` and ```Maze::rebuildEnvironment()``` and run the program again - it will print the blocks that are being restored in the console. If the program hasn't yet called the destructors and nothing is printing in the console after waiting for a period of time, you can assume that the program is experiencing undefined behaviour. This, however, has been programmed against and shouldn't occur.

- You will know when a ```~Destructor``` has been called when you see ```<object> destructor successfully called.``` printed in the console.

- Debug messages will be printed while running RHWF and BFS, in addition to the ```Step[i]: (x, y, z)``` message. This is to provide greater transparency to the user on how the algorithm is working.

## Data Structures

### Custom structs

- The custom structs like ```CoordDirComparator``` and ```CoordinateComparator``` offer efficient and logical ways to compare coordinates and directions. These structs are crucial for storing unique sets of coordinates and ensuring that the program logic proceeds as expected.

### Vectors vs other Data Structures (DS)

- ```std::vector``` objects are chosen over ```LinkedLists``` (or any other DS) because they are generally faster for our needs. In a ```std::vector```, all elements are stored next to each other in memory, making it quicker for the computer to read or write data. This is especially useful in our maze algorithms, where quick access to elements can make a big difference in performance.

### Maps, Sets, and Tuples

- ```std::map``` and ```std::set``` provide quick insertions, deletions, and lookups which are essential for BFS. ```std::tuple``` are used to bundle multiple types of data together, offering a straightforward way to manage complex data.

### Private member variables of the Agent class

- Certain ```private``` member variables (PMV) of the ```Agent``` class are either not used, or significantly underutilised. We acknowledge that utilising PMV may be slightly more efficient than just passing in all variables by reference in ```mazeRunner.cpp```. For example, ```AgentDirection``` and ```mcpp::Coordinate startLoc``` are both private members of the ```Agent``` class. However, when function signatures and definitions were changed to take in these private members (and scope issues were fixed), erratic behaviour, such as RHWF breaking through walls occurred. As such, this is one of the efficiency/memory shortcomings of this iteration of the program.

## Contributors and Workload

**@ravisidhu007 - Ravikaran Singh:**

- Fully created the recursive backtracking algorithm for automated generating of 'perfect' mazes
- Partially contributed towards building maze
- Created custom structs to support recursive backtracking algorithm
- Basic implementation of ```Maze::Maze()``` constructor
- Implementation of ```-testmode``` for recursive backtracking algorithm

**@tonythai12 - Tony Thai:**

- Created test-cases/edgecases for testing
- Full Implementation of reading maze from terminal
- Creation of the ReadMazeFromTerminal function
- Assisted @pdotkeenan in implementing the flattening of terrain, and restoring environment
- Documentation for ReadMazeFromTerminal function
- Documentation for GenerateRandomMaze function 

**@pdotkeenan - Keenan Phillips:**

- Main Menu and its navigation logic
- Created the RHWF and Extension 2 (BFS)
- Created all functions for Agent class
- Implement maze-building algorithm
- Validating all user input, re-prompting for input if incorrect
- Implementing flattening of terrain, getting existing environment structure, and restoring environment
- Storing all generated mazes (terminal and randomly generated) in a ```Maze*``` object
- Creation of custom structs to assist with comparisons and visited tile tracking
- Assisted @tonythai12 in full and robust implementation of reading maze from terminal
- Assisted @ravisidhu007 in creating robust ```Maze::Maze()``` constructor
- Added documentation for all functions
- Creation of all test cases

See commit history for more information. It should be noted that @tonythai12 had trouble with VSCode and as a result was unable to push their share of the workload (as listed above). Other contributors have assumed @tonythai12's workload. VSCode would not allow @tonythai12 to commit for a while so other group members committed on @tonythai12's behalf. Forced to delete the majority of their commits in order to solve VSCode issues. (This is evident and highlighted in the checkpoints through class). 

**Ruwan has recommended that this group be individually marked for this assignment.**
