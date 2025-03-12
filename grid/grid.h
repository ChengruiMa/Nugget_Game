/*
* grid.h - header file for CS50 Nuggets 'grid' module
*
 * A grid is a grid of characters representing a game board.
 * It provides functionality to create, load, manipulate and
 * query a 2D grid for the game.
 * A point struct with its corresponding getter, deleter functions is also provided.
* 
* Team 11, Winter 2025
* Mar 8, 2025
*/

#ifndef __GRID_H
#define __GRID_H

#include <stdio.h>
#include <stdbool.h>

/**************** Global constants ****************/
extern const char GRID_EMPTY_SPOT;     
extern const char GRID_PASSAGE_SPOT;    
extern const char GRID_HORIZONTAL_WALL; 
extern const char GRID_VERTICAL_WALL;  
extern const char GRID_CORNER_WALL;     
extern const char GRID_GOLD_SPOT;      

/**************** Global types ****************/
// /* Point structure to represent a position on the grid */
// typedef struct point point_t;

// /* Grid structure to represent the game grid*/
// typedef struct grid grid_t;

typedef struct point {
    int row;  // row coordinate
    int col;  // column coordinate
} point_t;

typedef struct grid {
    int nrows;              // number of rows
    int ncols;              // number of columns
    char** cells;           // 2D array of characters for the entire grid
    char** memory;          // 2D array of characters for remembered state
    bool** visible;         // 2D array of visibility flags
    bool initialized;       // flag to indicate if grid is initialized successfully
    bool hasMemory;         // flag to indicate if this grid has memory functionality
} grid_t;


/**************** Public Functions ****************/

/**************** Grid operations ****************/

/*
* Create a new grid with the given dimensions
* 
* Caller provides:
*   Number of rows and columns (both positive)
* We return:
*   Pointer to a new grid, or NULL if error
* Caller is responsible for:
*   Later calling grid_delete
*/
grid_t* grid_new(int nrows, int ncols);

/* Create a new grid with memory tracking functionality.
 * Memory tracking allows the grid to remember previously seen cells.
 *
 * Caller provides:
 *   Number of rows and columns (both > 0)
 * We return:
 *   Pointer to a new grid with memory, or NULL if error
 * Caller is responsible for later calling grid_delete
 */
grid_t* grid_newWithMemory(int nrows, int ncols);


/* Create a new player grid based on a master grid.
 * The new grid includes memory functionality for tracking visibility.
 *
 * Caller provides:
 *   Pointer to an existing grid
 * We return:
 *   Pointer to a new grid with memory, or NULL if error
 * Caller is responsible for later calling grid_delete
 */
grid_t* grid_createPlayerGrid(grid_t* grid);

/*
* Delete a grid and free all associated memory
* 
* Caller provides:
*   Valid pointer to a grid
*   If grid is Null, we do nothing
* We return:
*   true if successful, false otherwise
*/
bool grid_delete(grid_t* grid);

/*
* Load grid data from a file
* 
* Caller provides:
*   Valid file pointer to a grid file
*   Valid boolean of whether want the grid to have memory feature or not
* We return:
*   A grid_t struct if successful, or NULL otherwise
*/
grid_t* grid_load(FILE* map);

/*
* Get the character at a specific grid position
* 
* Caller provides:
*   Valid pointer to a grid
*   Row and column indices
* We return:
*   Character at the specified position, or '\0' if invalid
*/
char grid_get(grid_t* grid, int row, int col);

/*
* Set a character at a specific grid position
* 
* Caller provides:
*   Valid pointer to a grid
*   Row and column indices
*   Character to set
* We return:
*   true if successful, false if invalid position or grid
*/
bool grid_set(grid_t* grid, int row, int col, char ch);

/*
* Check if a position is within a room
* 
* Caller provides:
*   Valid pointer to a grid
*   Row and column indices
* We return:
*   true if position is in a room (empty or has gold), false otherwise
*/
bool grid_isRoom(grid_t* grid, int row, int col);

/*
* Check if a position is a passage
* 
* Caller provides:
*   Valid pointer to a grid
*   Row and column indices
* We return:
*   true if position is a passage, false otherwise
*/
bool grid_isPassage(grid_t* grid, int row, int col);

/*
* Check if a position is a gold pile
* 
* Caller provides:
*   Valid pointer to a grid
*   Row and column indices
* We return:
*   true if position is a gold pile, false otherwise
*/
bool grid_isGold(grid_t* grid, int row, int col);

/*
* Find a random empty spot in the grid
* 
* Caller provides:
*   Valid pointer to a grid
* We return:
*   Pointer to a new point structure with coordinates of a random empty spot
*   NULL if no empty spots or error
* Caller is responsible for later calling point_delete
*/
point_t* grid_findEmptyRoomSpot(grid_t* grid);

/*
* Convert grid to a string representation
* 
* Caller provides:
*   Valid pointer to a grid
* We return:
*   Malloc'd string containing the grid layout with newlines
*   NULL if error
* Caller is responsible for later calling free on the returned string
*/
char* grid_toString(grid_t* grid);

/*
* Get the number of rows in the grid
* 
* Caller provides:
*   Valid pointer to a grid
* We return:
*   Number of rows, or -1 if error
*/
int grid_getRows(grid_t* grid);

/*
* Get the number of columns in the grid
* 
* Caller provides:
*   Valid pointer to a grid
* We return:
*   Number of columns, or -1 if error
*/
int grid_getCols(grid_t* grid);

/**************** Point operations ****************/

/*
* Create a new point with given coordinates
* 
* Caller provides:
*   Row and column indices
* We return:
*   Pointer to a new point, or NULL if error
* Caller is responsible for later calling point_delete
*/
point_t* point_new(int row, int col);

/*
* Delete a point and free associated memory
* 
* Caller provides:
*   Valid pointer to a point
* The function does nothing if point is NULL
*/
void point_delete(point_t* point);

/*
* Get the row coordinate of a point
* 
* Caller provides:
*   Valid pointer to a point
* We return:
*   Row coordinate, or -1 if error
*/
int point_getRow(point_t* point);

/*
* Get the column coordinate of a point
* 
* Caller provides:
*   Valid pointer to a point
* We return:
*   Column coordinate, or -1 if error
*/
int point_getCol(point_t* point);

/**************** Visibility operations ****************/

/*
 * Calculate visibility from a given position
 * 
 * Caller provides:
 *   Valid pointer to a grid struct that has memory field
 *   Valid pointer to a position point
 * We do:
 *   Update the memory's visibility information based on the given position
 */
void grid_calculateVisibility(grid_t* grid, point_t* pos);

/*
 * Check if one point is visible from another point
 * 
 * Caller provides:
 *   Valid pointer to a grid
 *   Valid pointer to source point (from)
 *   Valid pointer to target point (to)
 * We return:
 *   true if target point is visible from source point, false otherwise
 */
bool grid_isVisible(grid_t* grid, point_t* from, point_t* to);

/* Check if a specific point is currently visible.
 *
 * Caller provides:
 *   Pointer to a grid structure, and row and column coordinates
 * We return:
 *   true if the point is currently visible, false otherwise
 */
bool grid_isPointVisible(grid_t* grid, int row, int col);

/* Convert the grid with visibility information to a string
 *
 * Caller provides:
 *   Pointer to a grid structure and pointer to a point struct that represents the player's position
 * We return:
 *   Pointer to a newly allocated string showing visible areas, or NULL on error
 * Notes:
 *   Returns a string with the current visible grid plus remembered areas (where spot with golds will simply be treated as normal room spot)
 *   Caller must free the returned string when done
 */
char* grid_visibilityToString(grid_t* grid, point_t* pos);

/* Update the grid's memory based on currently visible cells
 *
 * Caller provides:
 *   Pointer to a grid structure
 * We do:
 *   Update the memory to remember all currently visible cells
 */
void grid_updateMemory(grid_t* grid);


#endif // __GRID_H
