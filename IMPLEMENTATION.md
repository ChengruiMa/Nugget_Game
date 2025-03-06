# IMPLEMENTATION.md

## Overview

- Creating **test maps** and **test cases**  
- Implementing **unit tests** for both server and client  
- Creating and running **integration tests** for full system testing  
- Managing project documentation (*DESIGN.md*, *IMPLEMENTATION.md*)  
- Handling the **build system** (Makefiles) and ensuring a clean, consistent build  

Below, we describe how each of these responsibilities will be implemented, referencing existing modules and planned code structure. All references to data structures and modules (e.g., `grid_t`, `player_t`) come from our [DESIGN.md](DESIGN.md). 

---
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

#### `void grid_delete(grid_t* grid);`
- Frees memory associated with the grid.

- **Parameters:**
* `grid` - pointer to the grid to be deleted

**Returns:** None

#### `bool grid_load(grid_t* grid, FILE* fp);`
Loads map data from a file into the grid.

**Parameters:**
* `grid` - pointer to the grid structure to populate
* `fp` - file pointer to the map file

**Returns:**
* `true` if the map was loaded successfully
* `false` if any error occurred (e.g., file reading error, map format error)

#### `char grid_get(grid_t* grid, int row, int col);`
Gets the character at a specific position in the grid.

**Parameters:**
* `grid` - pointer to the grid structure
* `row` - row index
* `col` - column index

**Returns:**
* The character at the specified position, or '\0' if position is invalid

#### `bool grid_set(grid_t* grid, int row, int col, char ch);`
Sets the character at a specific position in the grid.

**Parameters:**
* `grid` - pointer to the grid structure
* `row` - row index
* `col` - column index
* `ch` - character to place at the position

**Returns:**
* `true` if successful
* `false` if the position is invalid

#### `bool grid_isRoom(grid_t* grid, int row, int col);`
Determines if a specific position is a room spot (can hold players or gold).

**Parameters:**
* `grid` - pointer to the grid structure
* `row` - row index
* `col` - column index

**Returns:**
* `true` if the position is a room spot
* `false` otherwise

#### `bool grid_isPassage(grid_t* grid, int row, int col);`
Determines if a specific position is a passage spot (can hold players).

**Parameters:**
* `grid` - pointer to the grid structure
* `row` - row index
* `col` - column index

**Returns:**
* `true` if the position is a passage spot
* `false` otherwise

#### `point_t* grid_findEmptySpot(grid_t* grid);`
Finds a random empty room spot in the grid.

**Parameters:**
* `grid` - pointer to the grid structure

**Returns:**
* A pointer to a new point_t with coordinates of an empty room spot, or NULL if no empty spots are found

#### `char* grid_toString(grid_t* grid);`
Converts the grid to a string representation.

**Parameters:**
* `grid` - pointer to the grid structure

**Returns:**
* A new string representation of the grid, caller must free this memory

#### `int grid_getRows(grid_t* grid);`
Gets the number of rows in the grid.

**Parameters:**
* `grid` - pointer to the grid structure

**Returns:**
* Number of rows, or 0 if grid is NULL

#### `int grid_getCols(grid_t* grid);`
Gets the number of columns in the grid.

**Parameters:**
* `grid` - pointer to the grid structure

**Returns:**
* Number of columns, or 0 if grid is NULL

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

### Functions

#### `memory_t* visibility_new(grid_t* grid);`
Creates a new memory/visibility tracker for a player.

**Parameters:**
* `grid` - pointer to the base grid structure

**Returns:**
* A pointer to a new memory_t structure, or NULL if error

#### `void visibility_delete(memory_t* memory);`
Frees memory associated with a memory structure.

**Parameters:**
* `memory` - pointer to the memory structure to delete

**Returns:** None

#### `void visibility_calculate(memory_t* memory, grid_t* grid, point_t* pos);`
Calculates visible spots from a given position and updates the memory grid.

**Parameters:**
* `memory` - pointer to the memory structure to update
* `grid` - pointer to the current game grid
* `pos` - position from which to calculate visibility

**Returns:** None

#### `bool visibility_isVisible(grid_t* grid, point_t* from, point_t* to);`
Determines if one point is visible from another.

**Parameters:**
* `grid` - pointer to the grid structure
* `from` - source position
* `to` - target position

**Returns:**
* `true` if target is visible from source
* `false` otherwise

#### `char* visibility_toString(memory_t* memory, grid_t* grid, point_t* pos);`
Creates a string representation of what's visible to a player.

**Parameters:**
* `memory` - pointer to the memory structure
* `grid` - pointer to the current game grid
* `pos` - current position of the player

**Returns:**
* A new string showing what's visible to the player (with memory of previously seen spots), caller must free

## Implementation

The implementation will follow these key design decisions:

### Map Loading and Validation
- When loading a map, each line is read into a dynamically allocated 2D array.
- The map is validated to ensure it's rectangular and contains valid characters.
- Error checking ensures the map has spaces for players and room for gold.

### Visibility Calculation
- Visibility is calculated using a line-of-sight algorithm.
- For each grid point, we draw a straight line to the player's position.
- If the line doesn't pass through any walls, the point is considered visible.
- Bresenham's line algorithm will be used for line-of-sight checks.

### Memory Management
- All dynamically allocated memory must be properly managed with matching alloc/free pairs.
- Functions that return dynamically allocated structures clearly document the caller's responsibility to free.
- We use the 'initialized' flag to validate grid operations.

## Error Handling
- Functions return appropriate values (NULL, false) on errors.
- Invalid parameters are checked at the beginning of each function.
- Memory allocation failures are properly handled.

### Data structures

### Definition of function prototypes

### Detailed pseudo code

---

## 1. Test Maps and Custom Map Designs

### 1.1 Creating Test Maps
- **Purpose**: We need small, medium, and large map files to test various in-game scenarios.
- **Implementation Steps**:
  1. **Draft**: Manually create map files (`maps/test1.txt`, `maps/test2.txt`, etc.) which vary in:
     - Rooms and tunnels layout
     - Presence or absence of connecting passages
     - Edge cases like narrow corridors and minimal open space
  2. **Validation**: Use our internal `checkmap` tool (or the provided script `prof/checkmap`) to ensure each map is valid (rectangular, no forbidden characters, properly padded if needed).
  3. **Diverse Cases**:
     - *Tiny map*: e.g., 5x5 grid with minimal open area
     - *Moderate map*: e.g., ~20x10 with multiple rooms
     - *Large map*: e.g., ~50x20 or bigger to stress-test movement and performance
  4. **Custom designs**: Incorporate thematic or interesting geometry for final demonstration (`maps/themed_castle.txt`, `maps/maze.txt`, etc.).
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

#### `void grid_delete(grid_t* grid);`
- Frees memory associated with the grid.

- **Parameters:**
* `grid` - pointer to the grid to be deleted

**Returns:** None

#### `bool grid_load(grid_t* grid, FILE* fp);`
Loads map data from a file into the grid.

**Parameters:**
* `grid` - pointer to the grid structure to populate
* `fp` - file pointer to the map file

**Returns:**
* `true` if the map was loaded successfully
* `false` if any error occurred (e.g., file reading error, map format error)

#### `char grid_get(grid_t* grid, int row, int col);`
Gets the character at a specific position in the grid.

**Parameters:**
* `grid` - pointer to the grid structure
* `row` - row index
* `col` - column index

**Returns:**
* The character at the specified position, or '\0' if position is invalid

#### `bool grid_set(grid_t* grid, int row, int col, char ch);`
Sets the character at a specific position in the grid.

**Parameters:**
* `grid` - pointer to the grid structure
* `row` - row index
* `col` - column index
* `ch` - character to place at the position

**Returns:**
* `true` if successful
* `false` if the position is invalid

#### `bool grid_isRoom(grid_t* grid, int row, int col);`
Determines if a specific position is a room spot (can hold players or gold).

**Parameters:**
* `grid` - pointer to the grid structure
* `row` - row index
* `col` - column index

**Returns:**
* `true` if the position is a room spot
* `false` otherwise

#### `bool grid_isPassage(grid_t* grid, int row, int col);`
Determines if a specific position is a passage spot (can hold players).

**Parameters:**
* `grid` - pointer to the grid structure
* `row` - row index
* `col` - column index

**Returns:**
* `true` if the position is a passage spot
* `false` otherwise

#### `point_t* grid_findEmptySpot(grid_t* grid);`
Finds a random empty room spot in the grid.

**Parameters:**
* `grid` - pointer to the grid structure

**Returns:**
* A pointer to a new point_t with coordinates of an empty room spot, or NULL if no empty spots are found

#### `char* grid_toString(grid_t* grid);`
Converts the grid to a string representation.

**Parameters:**
* `grid` - pointer to the grid structure

**Returns:**
* A new string representation of the grid, caller must free this memory

#### `int grid_getRows(grid_t* grid);`
Gets the number of rows in the grid.

**Parameters:**
* `grid` - pointer to the grid structure

**Returns:**
* Number of rows, or 0 if grid is NULL

#### `int grid_getCols(grid_t* grid);`
Gets the number of columns in the grid.

**Parameters:**
* `grid` - pointer to the grid structure

**Returns:**
* Number of columns, or 0 if grid is NULL

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

### Functions

#### `memory_t* visibility_new(grid_t* grid);`
Creates a new memory/visibility tracker for a player.

**Parameters:**
* `grid` - pointer to the base grid structure

**Returns:**
* A pointer to a new memory_t structure, or NULL if error

#### `void visibility_delete(memory_t* memory);`
Frees memory associated with a memory structure.

**Parameters:**
* `memory` - pointer to the memory structure to delete

**Returns:** None

#### `void visibility_calculate(memory_t* memory, grid_t* grid, point_t* pos);`
Calculates visible spots from a given position and updates the memory grid.

**Parameters:**
* `memory` - pointer to the memory structure to update
* `grid` - pointer to the current game grid
* `pos` - position from which to calculate visibility

**Returns:** None

#### `bool visibility_isVisible(grid_t* grid, point_t* from, point_t* to);`
Determines if one point is visible from another.

**Parameters:**
* `grid` - pointer to the grid structure
* `from` - source position
* `to` - target position

**Returns:**
* `true` if target is visible from source
* `false` otherwise

#### `char* visibility_toString(memory_t* memory, grid_t* grid, point_t* pos);`
Creates a string representation of what's visible to a player.

**Parameters:**
* `memory` - pointer to the memory structure
* `grid` - pointer to the current game grid
* `pos` - current position of the player

**Returns:**
* A new string showing what's visible to the player (with memory of previously seen spots), caller must free

## Implementation

The implementation will follow these key design decisions:

### Map Loading and Validation
- When loading a map, each line is read into a dynamically allocated 2D array.
- The map is validated to ensure it's rectangular and contains valid characters.
- Error checking ensures the map has spaces for players and room for gold.

### Visibility Calculation
- Visibility is calculated using a line-of-sight algorithm.
- For each grid point, we draw a straight line to the player's position.
- If the line doesn't pass through any walls, the point is considered visible.
- Bresenham's line algorithm will be used for line-of-sight checks.

### Memory Management
- All dynamically allocated memory must be properly managed with matching alloc/free pairs.
- Functions that return dynamically allocated structures clearly document the caller's responsibility to free.
- We use the 'initialized' flag to validate grid operations.

## Error Handling
- Functions return appropriate values (NULL, false) on errors.
- Invalid parameters are checked at the beginning of each function.
- Memory allocation failures are properly handled.

---

## 2. Unit Testing

### 2.1 Overview
Comprised of standalone C programs (or multiple small programs), each compiled separately to validate the correctness of a specific module’s functions in isolation.

### 2.2 Unit Test Modules

#### 2.2.1 `grid_test.c`
- **Purpose**: Validate functionality of `grid_load()`, `grid_new()`, `grid_delete()`, `grid_get()`, `grid_set()`, and `grid_isVisible()`.
- **Implementation**:
  1. **Arrange**: Create or load a test map into a `grid_t` instance.
  2. **Act**: Call each grid function with both valid and invalid inputs (e.g., out-of-bounds coordinates).
  3. **Assert**: Confirm the function outputs match expectations, such as correct return values, correct changes to `grid->cells`, and safe behavior on invalid indices.
- **Example**:
<pre>
  // Pseudocode for grid_test.c
  int main(void) {
      grid_t* testGrid = grid_load("maps/test1.txt");
      assert(testGrid != NULL);

      // Check the map boundary character
      char c = grid_get(testGrid, 0, 0);
      assert(c == '+');

      // Change a cell
      grid_set(testGrid, 2, 2, '*');
      assert(grid_get(testGrid, 2, 2) == '*');

      // ... additional tests ...

      grid_delete(testGrid);
      return 0; // success
  }
  </pre>

## 2.2.2 `player_test.c`

**Purpose**: Validate creation, position updates, gold tally, and other `player_t` attributes.

**Implementation**:
- Create a `player_t` with mock data (e.g., `'A'`, row=0, col=0, gold=0).
- Verify `player_setPosition()` updates position fields correctly.
- Verify `player_addGold()` properly increments the gold count.
- Confirm that the player’s name and letter fields match expected values.

**Example**:

<pre>
int main(void) {
    player_t* p = player_new('A', "TestPlayer", 1, 1, NULL); 
    assert(p != NULL);
    assert(p->letter == 'A');
    assert(strcmp(player_getName(p), "TestPlayer") == 0);

    player_setPosition(p, 5, 5);
    assert(p->row == 5 && p->col == 5);

    player_addGold(p, 10);
    assert(player_getGold(p) == 10);

    player_delete(p);
    return 0;
}
</pre>

## Running Unit Tests
test: grid_test player_test game_test
	@echo "Running unit tests..."
	@./grid_test
	@./player_test
	@./game_test
	@echo "All tests passed."



## 3. Create Integration Tests for Full System Testing

Once the server and client modules pass their individual unit tests, we will conduct **integration tests** to confirm that all components interoperate correctly:

1. **Server-Client Interactions**  
   - **Multiple Players**: Launch one server and multiple clients. Verify that each client’s moves are reflected correctly on the server, and that visible areas, player icons, and gold counts update properly.  
   - **Spectator**: Connect a spectator during active gameplay. Confirm that it sees the *entire* map with real-time updates.

2. **Automated Keystrokes**  
   - Use a small script or the `miniclient` utility (if provided) to send commands and simulate keystrokes (`KEY <char>`, `QUIT`) automatically.  
   - Validate that the server responds correctly with `DISPLAY`, `GOLD`, and `QUIT` messages, and that each client handles them without errors.

3. **Gold Collection and Game End**  
   - Test scenarios in which all gold is collected. Observe whether the server broadcasts the `QUIT` message with final summary.  
   - Check that the server exits cleanly, and clients display the summary before terminating.

4. **Edge Cases**  
   - **Invalid Messages**: Attempt sending malformed commands (like `KEY ZZZ`) to ensure the server or client logs errors or safely ignores them.  
   - **Connectivity**: Launch the client with an incorrect port or hostname to confirm it handles connection failures gracefully.

By running these **integration tests** across various network and gameplay conditions, we confirm that all components behave as specified when combined into a complete Nuggets system.

### 3.2 Approach
1. **Launch Server**  
   ```bash
   ./server maps/test2.txt 1234

## 4. Documentation Management

Ensure the project’s **Markdown documentation** remains accurate and up-to-date throughout development:

- **DESIGN.md** – Contains the current design details, which are refined as necessary.
- **IMPLEMENTATION.md** – This file, expanded incrementally as implementation progresses.
- **README.md** – Top-level overview of subdirectories, usage instructions, and build information.

All documentation resides in our GitHub repository, evolving within feature branches that merge back to `main` following the Git Flow process. As the final deadline approaches, we will:

1. Create a `submit-final` branch.  
2. Finalize all documentation.  
3. Tag the release with our final version.  
 ## 5. Build System (Makefiles)

### 5.1 Directory Layout
We have the following example structure:

<pre>
.
├── client/
│   ├── client.c
│   ├── display.c
│   ├── client_state.c
│   ├── Makefile
│   └── ...
├── server/
│   ├── server.c
│   ├── game.c
│   ├── gold.c
│   ├── Makefile
│   └── ...
├── tests/
│   ├── grid_test.c
│   ├── player_test.c
│   ├── ...
│   └── Makefile
├── maps/
│   ├── test1.txt
│   ├── test2.txt
│   └── ...
├── Makefile
├── DESIGN.md
├── IMPLEMENTATION.md
└── README.md
</pre>
## 5.2 Top-Level Makefile

**Responsibilities**:
- Recursively build client and server using `make -C client` and `make -C server`.
- Provide `make all`, `make clean`, and `make test` targets.

**Key Points**:
- Variables for compiler flags (`CFLAGS`, `LDFLAGS`).
- Link any libraries, if used.

**Example**:
```makefile
# Top-level Makefile

.PHONY: all clean test

all:
	@echo "Building server..."
	$(MAKE) -C server
	@echo "Building client..."
	$(MAKE) -C client

clean:
	$(MAKE) -C server clean
	$(MAKE) -C client clean
	$(MAKE) -C tests clean

test: all
	$(MAKE) -C tests test

```
## 5.3 Individual Subdirectory Makefiles

Each subdirectory has its own `Makefile` that compiles object files (`.o`) and links them into an executable (e.g., `server`, `client`, or test harnesses).

**Example** (in `tests/Makefile`):
```makefile
CFLAGS = -Wall -pedantic -std=c11 -ggdb

TESTS = grid_test player_test game_test

all: $(TESTS)

grid_test: grid_test.o ../server/grid.o
	$(CC) $(CFLAGS) -o $@ $^

player_test: player_test.o ../server/player.o
	$(CC) $(CFLAGS) -o $@ $^

# ... more tests ...

clean:
	rm -f *.o $(TESTS)
```
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
