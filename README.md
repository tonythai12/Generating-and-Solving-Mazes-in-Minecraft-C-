[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=12196410&assignment_repo_type=AssignmentRepo)

# Assignment 3: Generating and solving mazes in Minecraft via C++

This program uses C++ and the external 'mcpp' library to build and generate mazes in Minecraft.

## Key Features

- Build Mazes: Users can create their own 'perfect' or 'looped' mazes by specifying the maze structure.
- Generate Mazes: Automatically generate a 'perfect' maze without loops using the recursive backtracking algorithm.
- Solve Mazes: Optionally, users can receive help to solve the maze using either the Right-Hand Wall-Follower (RHWF) algorithm or the Breadth-First-Search (BFS) algorithm.
- Environment Management: After a maze is built or the program exits, the Minecraft environment is restored to its prior state.

## Technical aspects

- Error Handling: All incorrect user inputs are handled gracefully.
- Algorithm Robustness: Special conditions for RHWF and BFS to avoid infinite loops.
- Efficiency: Most variables are passed by reference to enhance performance.

## How to Use

### Test Mode

- Run './mazeRunner -testmode' to enable test mode. More details on how this affects gameplay can be found here.

### Breadth-First Search

- Run './mazeRunner -e2' to enable BFS instead of RHWF. This significantly speeds up the solving algorithm.

## Assumptions

### General Assumptions

- Language and Libraries: The project is written in C++ and uses the mcpp library for Minecraft interactions.
- Operating System: Assumes a Unix-like (or compatible) operating system for compilation and execution.
- Minecraft Server: Assumes that a Minecraft server is running and accessible, with the 'mcpp' library compatible with the server version.

### Maze Assumptions

- Maze Construction: Assumes that the maze can only be built in a predefined location in the Minecraft world. This predefined location is known as the "basePoint", consisting of (x, y, z) coordinates.
- Maze Structure: Assumes that the maze uses Acacia Wood Planks for walls, and Air blocks for empty spaces.
- Maze Pathways: Assumes that a path that a user can walk has a width of 1.
- Maze Generation: Assumes the maze has a single starting and ending point. Also assumes that if reading a maze from the terminal, it will be a 'perfect' maze.
- Maze Exit: Assumes the maze has only one exit point. Assumes that the maze exit is a total of 5 air blocks in a 180-degree fashion in front of the player, relative to their current direction. The y-coordinate remains unchanged.
- Base Point: Assumes that the user chooses a Base Point that contains a solid block (e.g., Grass, Stone) and NOT blocks that they can walk through or damage them (e.g., Water, Lava). This is because the building algorithm gets the block at the basePoint and makes that the foundation for the ground of the maze.

### Solving Algorithm Assumptions

- BFS: Assumes that the maze is 'perfect' with only one exit point. Multiple exit points or a loop will lead to undefined behaviour, including infinite loops.
- RHWF: Assumes that the maze is 'perfect' with only one exit point. Multiple exit points may lead to undefined behaviour. Loops are designed to be detected after the agent has visited a tile with the same direction faced N times, but are not guaranteed to caught. A looped maze may cause an infinite loop.

### Agent (player)

- Initial Position: Assumes that the agent starts at a specified location (through the "Solve Manually" option in the "Solve Maze" menu)
- Direction: Assumes the agent initially faces the "UP"/"NORTH"/"MOVE_ZMINUS" direction. In testing mode, this varies based on the presence of a wall.
- Movement: Assumes that the agent can only move in four directions - "UP"/"NORTH"/"MOVE_ZMINUS", "DOWN"/"SOUTH"/"MOVE_ZPLUS", "LEFT"/"WEST"/"MOVE_XMINUS", and "RIGHT"/"EAST/"MOVE_XPLUS". Assumes the agent cannot move in the y-direction.

### User Interaction

- Command Line Arguments: Assumes specific command-line arguments for different program behaviour, like '-testmode' for testing and/or '-e2' for enabling BFS. Both can be used together.
- Input: Assumes that the user will provide maze configurations in a specific format - 'x' for walls, and '.' for empty spaces. Assumes the length and width will be positive integers. The program is designed to catch invalid inputs and re-prompt the user.
- Environment: Assumes the user will not destroy any walls at any time after the maze has been built. This may lead to RHWF and BFS providing incorrect solutions and being stuck in infinite loops. Similarly, assumes the user will not place any blocks (walls) at any time after the maze has been built. This may lead to RHWF and BFS providing incorrect solutions.
- Assumes that the user presses the space-bar twice (to initiate flying mode) before building the maze in Minecraft. This will ensure that they stay in the air when the building algorithm teleports them to (basePoint.x, basePoint.y + 10, basePoint.z).

## Need-to-knows

- The program dynamically allocates memory for the Minecraft connection object, the Agent object, and the Maze object. They have appropriate destructors that will be called when either building a new maze or exiting the program.
- Restoring the environment to its previous state may take a long time, depending on the existing structure. If you believe that the program is stuck in a loop or unresponsive, uncomment the debug messages found in Maze::RestoreOldBlocksFirst() and Maze::rebuildEnvironment() and run the program again - it will print the blocks that are being restored in the console. If the program hasn't yet called the destructors and nothing is printing in the console after waiting for a period of time, you can assume that the program is experiencing undefined behaviour. This, however, has been programmed against and shouldn't occur.
- Debug messages will be printed while running RHWF and BFS, in addition to the "Step[i]: (x, y, z)" message. This is to provide greater transparency to the user on how the algorithm is working.

## Data Structures

### Custom structs

- The custom structs like 'CoordDirComparator' and 'CoordinateComparator' offer efficient and logical ways to compare coordinates and directions. These structs are crucial for storing unique sets of coordinates and ensuring that the program logic proceeds as expected.

### Vectors vs other Data Structures (DS)

- Vectors are chosen over linked lists (or any other DS) because they are generally faster for our needs. In a vector, all elements are stored next to each other in memory, making it quicker for the computer to read or write data. This is especially useful in our maze algorithms, where quick access to elements can make a big difference in performance.

### Maps, Sets, and Tuples

- Maps and sets provide quick insertions, deletions, and lookups which are essential for BFS. Tuples are used to bundle multiple types of data together, offering a straightforward way to manage complex data.

## Contributors

@pdotkeenan - Keenan Phillips

@ravisidhu007 - Ravikumar Singh

@tonythai12 - Tony Thai

### Workload

@ravisidhu007 - Ravikumar Singh:

- Fully create the recursive backtracking algorithm for automated generating of 'perfect' mazes
- Partially contributed towards building maze
- Create custom structs to support recursive backtracking algorithm
- Basic implementation of Maze::Maze() constructor

@tonythai12 - Tony Thai:

- Created basic test-cases
- Skeleton implementation of reading maze from terminal

@pdotkeenan - Keenan Phillips:

- Main Menu and its navigation logic
- Create the RHWF and Extension 2 (BFS)
- Create all functions for Agent class
- Implement maze-building algorithm
- Validating user input
- Implementing flattening of terrain, getting existing environment structure, and restoring environment
- Storing all generated mazes (terminal and randomly generated) in a Maze* object
- Assisted @tonythai12 in full and robust implementation of reading maze from terminal
- Assisted @ravisidhu007 in creating robust Maze::Maze() constructor

See commit history for more information. It should be noted that @tonythai12 had trouble with VSCode and as a result was unable to push their share of the workload (as listed above). Other contributors needed to take on their workload in some situations.

## Ruwan has recommended that everyone in this group be individually marked for this assignment
