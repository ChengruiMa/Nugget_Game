# CS50 Nuggets
## Implementation Spec
### Team 11, Winter, 2025

> This **template** includes some gray text meant to explain how to use the template; delete all of them in your document!

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

> Update your plan for distributing the project work among your 3(4) team members.
> Who writes the client program, the server program, each module?
> Who is responsible for various aspects of testing, for documentation, etc?

## Player

> Teams of 3 students should delete this section.

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	initialize message module
	print assigned port number
	decide whether spectator or player

---

## Server

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()

---

## Map Module

### Data Structures

The Map module uses the following data structure to represent the game grid:

```c
typedef struct grid {
    int nrows;           // number of rows
    int ncols;           // number of columns
    char** cells;        // 2D array of characters representing the map
    bool initialized;    // flag to check if grid is properly initialized
} grid_t;
```

### Definition of constants and function prototypes

For convenient, we will define these constants in the map module to indicate what type of spots each position is:

```c
#define GRID_EMPTY_SPOT ' '    // Empty room spot
#define GRID_PASSAGE_SPOT '#'   // Passage spot
#define GRID_HORIZONTAL_WALL '-' // Horizontal wall
#define GRID_VERTICAL_WALL '|'   // Vertical wall
#define GRID_CORNER_WALL '+'     // Corner wall
#define GRID_GOLD_SPOT '*'       // Gold pile
```

Then, we will have the following functions:

#### `grid_t* grid_new(int nrows, int ncols);`
- Creates a new grid data structure with the given dimensions.
- **Parameters:**
	* `nrows` - number of rows in the grid
	* `ncols` - number of columns in the grid

- **Returns:**
	* A pointer to a new grid_t, or NULL if any error (e.g., memory allocation failure)

#### `bool grid_delete(grid_t* grid);`
- Frees memory associated with the grid.

- **Parameters:**
	* `grid` - pointer to the grid to be deleted
	**Returns:** True if success, false if any error

#### `bool grid_load(grid_t* grid, FILE* fp);`
- Loads map data from a file into the grid.

- **Parameters:**
	* `grid` - pointer to the grid structure to populate
	* `fp` - file pointer to the map file

- **Returns:**
	* `true` if the map was loaded successfully
	* `false` if any error occurred (e.g., file reading error, map format error)

#### `char grid_get(grid_t* grid, int row, int col);`
- Gets the character at a specific position in the grid.

- **Parameters:**
	* `grid` - pointer to the grid structure
	* `row` - row index
	* `col` - column index

- **Returns:**
	* The character at the specified position, or '\0' if position is invalid

#### `bool grid_set(grid_t* grid, int row, int col, char ch);`
- Sets the character at a specific position in the grid.

- **Parameters:**
	* `grid` - pointer to the grid structure
	* `row` - row index
	* `col` - column index
	* `ch` - character to place at the position

- **Returns:**
	* `true` if successful
	* `false` if the position is invalid

#### `bool grid_isRoom(grid_t* grid, int row, int col);`
- Determines if a specific position is a valid room spot that can hold both golds and players.

- **Parameters:**
	* `grid` - pointer to the grid structure
	* `row` - row index
	* `col` - column index

- **Returns:**
	* `true` if the position is a room spot
	* `false` otherwise

#### `bool grid_isPassage(grid_t* grid, int row, int col);`
- Determines if a specific position is a passage spot that can hold players (not golds).

- **Parameters:**
	* `grid` - pointer to the grid structure
	* `row` - row index
	* `col` - column index

- **Returns:**
	* `true` if the position is a passage spot
	* `false` otherwise

#### `point_t* grid_findEmptySpot(grid_t* grid);`
- Finds a random empty room spot in the grid.

- **Parameters:**
	* `grid` - pointer to the grid structure

- **Returns:**
	* A pointer to a new point_t with coordinates of an empty room spot, or NULL if no empty spots are found

#### `char* grid_toString(grid_t* grid);`
- Converts the grid to a string representation.

- **Parameters:**
	* `grid` - pointer to the grid structure

- **Returns:**
	* A new string representation of the grid, caller must free this memory

#### `int grid_getRows(grid_t* grid);`
Gets the number of rows in the grid.

- **Parameters:**
	* `grid` - pointer to the grid structure

- **Returns:**
	* Number of rows, or 0 if grid is NULL

#### `int grid_getCols(grid_t* grid);`
- Gets the number of columns in the grid.

- **Parameters:**
	* `grid` - pointer to the grid structure

- **Returns:**
	* Number of columns, or 0 if grid is NULL

### Detailed pseudo code

## Visibility Module

### Data Structures

We define the following structure to represent a point on the grid:

```c
typedef struct point {
    int row;  // row coordinate
    int col;  // column coordinate
} point_t;
```

We also use a memory grid to track what each player has seen:

```c
typedef struct memory {
    grid_t* grid;       // copy of the grid for player's memory
    bool** visible;     // 2D array tracking currently visible spots
} memory_t;
```

### Function Prototypes

#### `memory_t* visibility_new(grid_t* grid);`
- Creates a new memory/visibility tracker for a player.

- **Parameters:**
	* `grid` - pointer to the base grid structure

- **Returns:**
	* A pointer to a new memory_t structure, or NULL if error

#### `bool visibility_delete(memory_t* memory);`
- Frees memory associated with a memory structure.

- **Parameters:**
	* `memory` - pointer to the memory structure to delete

- **Returns:** True if delete successfully, false otherwise.

#### `void visibility_calculate(memory_t* memory, grid_t* grid, point_t* pos);`
- Calculates visible spots from a given position and updates the memory grid.

- **Parameters:**
	* `memory` - pointer to the memory structure to update
	* `grid` - pointer to the current game grid
	* `pos` - position from which to calculate visibility

- **Returns:** None

#### `bool visibility_isVisible(grid_t* grid, point_t* from, point_t* to);`
- Determines if one point is visible from another.

- **Parameters:**
	* `grid` - pointer to the grid structure
	* `from` - source position
	* `to` - target position

- **Returns:**
	* `true` if target is visible from source
	* `false` otherwise

#### `char* visibility_toString(memory_t* memory, grid_t* grid, point_t* pos);`
- Creates a string representation of what's visible to a player.

- **Parameters:**
	* `memory` - pointer to the memory structure
	* `grid` - pointer to the current game grid
	* `pos` - current position of the player
- **Returns:**
	* A new string showing what's visible to the player (with memory of previously seen spots), caller must free later.

### Detailed pseudo code

---

## Testing plan

### unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

### integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### system testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.
