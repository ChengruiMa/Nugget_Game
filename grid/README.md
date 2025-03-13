# Grid Module - Nuggets

This directory contains the implementation for the grid in the Nuggets game.

## Overview

The grid module provides a 2D representation of the game map, handling room layout, passages, walls, and visibility calculations. It includes player memory and line-of-sight algorithms to determine what each player can see.

## Files

* `grid.c` - Implementation of the grid module
* `grid.h` - Header file with function prototypes and type definitions

## Constants

* `GRID_EMPTY_SPOT` - Space character for empty spots
* `GRID_PASSAGE_SPOT` - '#' character for passages
* `GRID_HORIZONTAL_WALL` - '-' character for horizontal walls
* `GRID_VERTICAL_WALL` - '|' character for vertical walls
* `GRID_CORNER_WALL` - '+' character for wall corners
* `GRID_GOLD_SPOT` - '*' character for gold nuggets

## Data Structures

* `grid_t` - The main grid structure holding the cells, memory, and visibility information
```
typedef struct grid {
    int nrows;              // number of rows
    int ncols;              // number of columns
    char** cells;           // 2D array of characters for the entire grid
    char** memory;          // 2D array of characters for remembered state
    bool** visible;         // 2D array of visibility flags
    bool initialized;       // flag to indicate if grid is initialized successfully
    bool hasMemory;         // flag to indicate if this grid has memory functionality
} grid_t;
```

* `point_t` - A simple structure for storing (row, col) coordinates
```
typedef struct point {
    int row;  // row coordinate
    int col;  // column coordinate
} point_t;

```


## Usage

The grid module provides functions for:
* Creating and managing grids (`grid_new`, `grid_newWithMemory`, `grid_delete`)
* Loading maps from files (`grid_load`)
* Accessing and modifying grid cells (`grid_get`, `grid_set`)
* Testing cell properties (`grid_isRoom`, `grid_isPassage`, `grid_isGold`)
* Finding random empty spots for player/gold placement
* Calculating visibility from a given position
* Converting grid state to string representation

## Visibility Algorithm

The module implements a line-of-sight algorithm that:
* Determines what cells are visible from a player's position
* Updates player memory to remember previously seen cells
* Handles special cases for passages, rooms, and walls

## Build
To build, simply 
```
make
```