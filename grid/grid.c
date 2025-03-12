/*
* grid.c - implementation for CS50 Nuggets 'grid' module with integrated visibility
*
* See grid.h for detailed information about the module interface
* 
* Team 11, Winter 2025
* Mar 10, 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "grid.h"

const char GRID_EMPTY_SPOT = ' ';
const char GRID_PASSAGE_SPOT = '#';
const char GRID_HORIZONTAL_WALL = '-';
const char GRID_VERTICAL_WALL = '|';
const char GRID_CORNER_WALL = '+';
const char GRID_GOLD_SPOT = '*';

/**************** Global types ****************/


/**************** Public functions ****************/
/* Operations on the grid structure */
/* For detailed descriptions, see grid.h */
grid_t* grid_new(int nrows, int ncols);
grid_t* grid_newWithMemory(int nrows, int ncols);
grid_t* grid_createPlayerGrid(grid_t* grid);
bool grid_delete(grid_t* grid);
grid_t* grid_load(FILE* map);
char grid_get(grid_t* grid, int row, int col);
bool grid_set(grid_t* grid, int row, int col, char ch);
bool grid_isRoom(grid_t* grid, int row, int col);
bool grid_isPassage(grid_t* grid, int row, int col);
bool grid_isGold(grid_t* grid, int row, int col);
point_t* grid_findEmptyRoomSpot(grid_t* grid);
char* grid_toString(grid_t* grid);
int grid_getRows(grid_t* grid);
int grid_getCols(grid_t* grid);

/* Operations on the visibility */
void grid_calculateVisibility(grid_t* grid, point_t* pos);
bool grid_isVisible(grid_t* grid, point_t* from, point_t* to);
bool grid_isPointVisible(grid_t* grid, int row, int col);
char* grid_visibilityToString(grid_t* grid, point_t* pos);
void grid_updateMemory(grid_t* grid);

/* Operations on the point structure */
point_t* point_new(int row, int col);
void point_delete(point_t* point);
int point_getRow(point_t* point);
int point_getCol(point_t* point);

/**************** local functions ****************/
/* Check if a point is within the grid boundaries */
static bool isValidPoint(grid_t* grid, int row, int col);

// Return true if the cell at (row,col) *blocks* line-of-sight
static bool isBlocker(grid_t* grid, int row, int col, bool isFinal);

/* Check if a point is a wall or boundary */
static bool isWall(grid_t* grid, int row, int col);

/* Check if the path from (x1, y1) to (x0, y0) is visible */
static bool hasLineOfSight(grid_t* grid, int x0, int y0, int x1, int y1);

/**************** grid_new() ****************/
grid_t* grid_new(int nrows, int ncols)
{
    // Validate input parameters
    if (nrows <= 0 || ncols <= 0) {
        return NULL;  
    }

    // Allocate memory for grid structure
    grid_t* grid = malloc(sizeof(grid_t));
    if (grid == NULL) {
        return NULL;  // Out of memory
    }

    // Initialize grid properties
    grid->nrows = nrows;
    grid->ncols = ncols;
    grid->initialized = false;
    grid->hasMemory = false;
    grid->memory = NULL;
    grid->visible = NULL;

    // Allocate memory for cells, return Null if failed
    grid->cells = malloc(nrows * sizeof(char*));
    if (grid->cells == NULL) {
        free(grid);
        return NULL; 
    }

    // Allocate each row and initialize with spaces
    for (int i = 0; i < nrows; i++) {
        grid->cells[i] = malloc((ncols + 1) * sizeof(char));  // +1 for null terminator
        if (grid->cells[i] == NULL) {
            // Failed to allocate, clean up
            for (int j = 0; j < i; j++) {
                free(grid->cells[j]);
            }
            free(grid->cells);
            free(grid);
            return NULL;
        }
        
        // Initialize row with spaces and null-terminate
        for (int j = 0; j < ncols; j++) {
            grid->cells[i][j] = GRID_EMPTY_SPOT;
        }
        grid->cells[i][ncols] = '\0';
    }

    grid->initialized = true;
    return grid;
}

/**************** grid_newWithMemory() ****************/
grid_t* grid_newWithMemory(int nrows, int ncols)
{
    // Create a regular grid first
    grid_t* grid = grid_new(nrows, ncols);
    if (grid == NULL) {
        return NULL;
    }

    // Add memory feature and allocate memory
    grid->hasMemory = true;
    grid->memory = malloc(nrows * sizeof(char*));
    if (grid->memory == NULL) {
        grid_delete(grid);
        return NULL; 
    }

    // Initialize memory grid with empty spots
    for (int i = 0; i < nrows; i++) {
        grid->memory[i] = malloc((ncols + 1) * sizeof(char));
        if (grid->memory[i] == NULL) {
            // Failed to allocate, clean up
            for (int j = 0; j < i; j++) {
                free(grid->memory[j]);
            }
            free(grid->memory);
            grid->memory = NULL;
            grid_delete(grid);
            return NULL;
        }
        
        for (int j = 0; j < ncols; j++) {
            grid->memory[i][j] = GRID_EMPTY_SPOT;
        }
        grid->memory[i][ncols] = '\0';
    }

    // Allocate visibility array
    grid->visible = malloc(nrows * sizeof(bool*));
    if (grid->visible == NULL) {
        // Clean up memory grid first
        for (int i = 0; i < nrows; i++) {
            free(grid->memory[i]);
        }
        free(grid->memory);
        grid->memory = NULL;
        grid_delete(grid);
        return NULL;
    }

    // Initialize visibility array
    for (int i = 0; i < nrows; i++) {
        grid->visible[i] = malloc(ncols * sizeof(bool));
        if (grid->visible[i] == NULL) {
            // Clean up
            for (int j = 0; j < i; j++) {
                free(grid->visible[j]);
            }
            free(grid->visible);
            grid->visible = NULL;
            for (int j = 0; j < nrows; j++) {
                free(grid->memory[j]);
            }
            free(grid->memory);
            grid->memory = NULL;
            
            grid_delete(grid);
            return NULL;
        }
        
        // Initialize all spots as not visible
        for (int j = 0; j < ncols; j++) {
            grid->visible[i][j] = false;
        }
    }

    return grid;
}

/**************** grid_createPlayerGrid() ****************/
grid_t* grid_createPlayerGrid(grid_t* grid)
{
    if (grid == NULL || !grid->initialized) {
        return NULL;
    }

    // Create a new grid with memory functionality
    grid_t* playerGrid = grid_newWithMemory(grid->nrows, grid->ncols);
    
    if (playerGrid == NULL) {
        return NULL;
    }

    // Copy the master grid cells to the player grid
    for (int row = 0; row < grid->nrows; row++) {
        for (int col = 0; col < grid->ncols; col++) {
            playerGrid->cells[row][col] = grid->cells[row][col];
        }
    }

    return playerGrid;
}

/**************** grid_delete() ****************/
bool grid_delete(grid_t* grid)
{
    if (grid == NULL) {
        return false;
    }

    // Free memory grid if it exists
    if (grid->hasMemory && grid->memory != NULL) {
        for (int i = 0; i < grid->nrows; i++) {
            if (grid->memory[i] != NULL) {
                free(grid->memory[i]);
            }
        }
        free(grid->memory);
    }

    // Free visibility array if it exists
    if (grid->hasMemory && grid->visible != NULL) {
        for (int i = 0; i < grid->nrows; i++) {
            if (grid->visible[i] != NULL) {
                free(grid->visible[i]);
            }
        }
        free(grid->visible);
    }

    // Free cells if they exist
    if (grid->cells != NULL) {
        for (int i = 0; i < grid->nrows; i++) {
            if (grid->cells[i] != NULL) {
                free(grid->cells[i]);
            }
        }
        free(grid->cells);
    }

    // Free the grid structure
    free(grid);
    return true;
}

/**************** grid_load() ****************/
grid_t* grid_load(FILE* map)
{
    if (map == NULL) {
        return NULL;
    }

    // Determine dimensions (rows and columns) within the first pass
    int nrows = 0;
    int ncols = 0;
    char line[1024]; // Assuming no line is extremely long (i.e. longer than 1024 characters)

    while (fgets(line, sizeof(line), map) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }

        // Update maximum line length
        if ((int)len > ncols) {
            ncols = len;
        }

        nrows++;
    }

    // Reset file position to the beginning
    rewind(map);

    // Create a new grid with the known dimensions
    grid_t* grid = NULL;
    // if (withMemory) {
    //     grid = grid_newWithMemory(nrows, ncols);
    // } else {
    //     grid = grid_new(nrows, ncols);
    // }
    grid = grid_new(nrows, ncols);
    if (grid == NULL) {
        fprintf(stderr, "Error: Something went wrong while creating grid\n");
        return NULL;
    }

    // Fill the grid with map data
    int row = 0;
    while (fgets(line, sizeof(line), map) != NULL && row < nrows) {
        // Remove newline character
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }

        // Copy line to grid
        for (size_t col = 0; col < len; col++) {
            grid->cells[row][col] = line[col];
        }

        // If line is shorter than grid width, fill with spaces
        for (int col = len; col < ncols; col++) {
            grid->cells[row][col] = GRID_EMPTY_SPOT;
        }

        row++;
    }

    return grid;
}

/**************** grid_get() ****************/
char grid_get(grid_t* grid, int row, int col)
{
    if (grid == NULL || !grid->initialized || !isValidPoint(grid, row, col)) {
        return '\0';  // Invalid position or grid
    }
    return grid->cells[row][col];
}

/**************** grid_set() ****************/
bool grid_set(grid_t* grid, int row, int col, char ch)
{
    if (grid == NULL || !grid->initialized || !isValidPoint(grid, row, col)) {
        return false;  // Invalid position or grid
    }
    grid->cells[row][col] = ch;
    return true;
}

/**************** grid_isRoom() ****************/
bool grid_isRoom(grid_t* grid, int row, int col)
{
    char ch = grid_get(grid, row, col);
    return (ch == '.' || ch == GRID_GOLD_SPOT);
}

/**************** grid_isPassage() ****************/
bool grid_isPassage(grid_t* grid, int row, int col)
{
    return (grid_get(grid, row, col) == GRID_PASSAGE_SPOT);
}

/**************** grid_isGold() ****************/
bool grid_isGold(grid_t* grid, int row, int col)
{
    return (grid_get(grid, row, col) == GRID_GOLD_SPOT);
}

/**************** grid_findEmptyRoomSpot() ****************/
point_t* grid_findEmptyRoomSpot(grid_t* grid)
{
    if (grid == NULL || !grid->initialized) {
        return NULL;
    }

    // Seed random number generator if not already seeded
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    // Count the total number of empty spots
    int emptyRoomSpot = 0;
    for (int row = 0; row < grid->nrows; row++) {
        for (int col = 0; col < grid->ncols; col++) {
            if (grid->cells[row][col] == '.') {
                emptyRoomSpot++;
            }
        }
    }

    if (emptyRoomSpot == 0) {
        return NULL; 
    }

    // Choose a random empty spot
    int targetSpot = rand() % emptyRoomSpot;
    int currentSpot = 0;

    for (int row = 0; row < grid->nrows; row++) {
        for (int col = 0; col < grid->ncols; col++) {
            if (grid->cells[row][col] == '.') {
                if (currentSpot == targetSpot) {
                    return point_new(row, col);
                }
                currentSpot++;
            }
        }
    }

    // If for some error can't find a random Empty spot
    return NULL;
}

/**************** grid_toString() ****************/
char* grid_toString(grid_t* grid)
{
    if (grid == NULL || !grid->initialized) {
        return NULL;
    }

    // Calculate required size: each row + newline + null terminator
    size_t size = grid->nrows * (grid->ncols + 1) + 1;
    char* str = malloc(size);
    if (str == NULL) {
        return NULL;
    }

    str[0] = '\0';

    // Concatenate each row with a newline
    for (int row = 0; row < grid->nrows; row++) {
        strcat(str, grid->cells[row]);
        strcat(str, "\n");
    }

    return str;
}

/**************** grid_visibilityToString() ****************/
char* grid_visibilityToString(grid_t* grid, point_t* pos)
{
    if (grid == NULL || !grid->initialized || !grid->hasMemory || pos == NULL) {
        return NULL;
    }

    // Update visibility 
    grid_calculateVisibility(grid, pos);
    
    // Calculate required size: each row + newline + null terminator
    size_t size = grid->nrows * (grid->ncols + 1) + 1;
    char* str = malloc(size);
    if (str == NULL) {
        return NULL;
    }

    str[0] = '\0';
    char line[grid->ncols + 1];  // +1 for null terminator

    // Build string row by row
    for (int row = 0; row < grid->nrows; row++) {
        for (int col = 0; col < grid->ncols; col++) {
            if (grid->visible[row][col]) {
                // Spot is currently visible, use current grid content
                line[col] = grid->cells[row][col];
            } else {
                // Spot is not visible, use memory if previously seen
                char remembered = grid->memory[row][col];
                if (remembered == '*') {
                    line[col] = '.';
                }
                else line[col] = remembered;
            }
        }
        line[grid->ncols] = '\0';
        strcat(str, line);
        strcat(str, "\n");
    }

    return str;
}

/**************** grid_getRows() ****************/
int grid_getRows(grid_t* grid)
{
    if (grid != NULL && grid->initialized) {
        return grid->nrows;
    }
    else {
        return -1;
    }
}

/**************** grid_getCols() ****************/
int grid_getCols(grid_t* grid)
{
    if (grid != NULL && grid->initialized) {
        return grid->ncols;
    }
    else {
        return -1;
    }
}
/**************** grid_calculateVisibility() ****************/
void grid_calculateVisibility(grid_t* grid, point_t* pos)
{
    if (grid == NULL || pos == NULL) return;
    int pr = point_getRow(pos);
    int pc = point_getCol(pos);

    // Reset all visibility flags to false
    for (int row = 0; row < grid->nrows; row++) {
        for (int col = 0; col < grid->ncols; col++) {
            grid->visible[row][col] = false;
        }
    }

    // For each cell in the grid, check line-of-sight from (pr, pc)
    for (int row = 0; row < grid->nrows; row++) {
        for (int col = 0; col < grid->ncols; col++) {
            if (hasLineOfSight(grid, pc, pr, col, row)) {
                grid->visible[row][col] = true;
            }
        }
    }

    // Once visibility is updated, remember newly seen cells
    grid_updateMemory(grid);
}



/**************** grid_updateMemory() ****************/
void grid_updateMemory(grid_t* grid)
{
    if (grid == NULL || !grid->initialized || !grid->hasMemory) {
        return;
    }

    // Copy any newly visible cells into memory
    for (int row = 0; row < grid->nrows; row++) {
        for (int col = 0; col < grid->ncols; col++) {
            if (grid->visible[row][col]) {
                grid->memory[row][col] = grid->cells[row][col];
            }
        }
    }
}

/**************** isBlocker() ****************/
static bool isBlocker(grid_t* grid, int row, int col, bool isFinal)
{
    // If it's out of bounds, treat as blocking
    if (row < 0 || row >= grid->nrows || col < 0 || col >= grid->ncols) {
        return true;
    }

    // Room cells never block
    if (grid_isRoom(grid, row, col)) {
        return false;
    }

    // If this is the final cell, then we allow it to be seen
    if (isFinal) {
        if (grid_isPassage(grid, row, col) || isWall(grid, row, col)) {
            return false;
        }
    }

    return true;
}


/**************** hasLineOfSight() ****************/
static bool hasLineOfSight(grid_t* grid, int x0, int y0, int x1, int y1)
{
    // If the start and end are the same, trivial visibility
    if (x0 == x1 && y0 == y1) {
        return true;
    }

    double dx = x1 - x0;
    double dy = y1 - y0;
    double steps = fmax(fabs(dx), fabs(dy));
    if (steps < 1) steps = 1; 
    double xInc = dx / steps;
    double yInc = dy / steps;

    double x = x0;
    double y = y0;

    // We'll step from i=0 up to i=steps
    for (int i = 0; i <= (int)steps; i++) {
        // Identify which cell we’re “in” by rounding
        int cx = (int)floor(x + 0.5);
        int cy = (int)floor(y + 0.5);

        bool isFinal = (cx == x1 && cy == y1);

        double xDiff = fabs(x - round(x));
        double yDiff = fabs(y - round(y));
        const double epsilon = 1e-6;

        if (xDiff < epsilon && yDiff < epsilon) {
            // If the single cell is a blocker (unless final cell rules), block
            if (isBlocker(grid, cy, cx, isFinal)) {
                return false;
            }
        }
        else if (xDiff < epsilon) {
            // We pass between two cells vertically
            int row1 = (int)floor(y);
            int row2 = row1 + 1;

            // If both cells are blockers, line is blocked
            bool block1 = isBlocker(grid, row1, cx, isFinal && (row1 == y1));
            bool block2 = isBlocker(grid, row2, cx, isFinal && (row2 == y1));
            if (block1 && block2) {
                return false;
            }
        }
        else if (yDiff < epsilon) {
            // We pass between two cells horizontally
            int col1 = (int)floor(x);
            int col2 = col1 + 1;

            bool block1 = isBlocker(grid, cy, col1, isFinal && (col1 == x1));
            bool block2 = isBlocker(grid, cy, col2, isFinal && (col2 == x1));
            if (block1 && block2) {
                return false;
            }
        }
        else {
            // If that single cell blocks, we’re done
            if (isBlocker(grid, cy, cx, isFinal)) {
                return false;
            }
        }

        // Step to the next point
        x += xInc;
        y += yInc;
    }

    return true;
}




/**************** grid_isVisible() ****************/
bool grid_isVisible(grid_t* grid, point_t* from, point_t* to)
{
    if (grid == NULL || !grid->initialized || from == NULL || to == NULL) {
        return false;
    }

    return hasLineOfSight(grid, point_getCol(from), point_getRow(from), point_getCol(to), point_getRow(to));
}

/**************** grid_isPointVisible() ****************/
bool grid_isPointVisible(grid_t* grid, int row, int col)
{
    if (grid == NULL || !grid->initialized || !grid->hasMemory || !isValidPoint(grid, row, col)) {
        return false;
    }

    return grid->visible[row][col];
}


/**************** point_new() ****************/
point_t* point_new(int row, int col)
{
    point_t* point = malloc(sizeof(point_t));
    if (point == NULL) {
        return NULL;  
    }
    point->row = row;
    point->col = col;
    return point;
}

/**************** point_delete() ****************/
void point_delete(point_t* point)
{
    if (point != NULL) {
        free(point);
    }
}


/**************** isValidPoint() ****************/
static bool isValidPoint(grid_t* grid, int row, int col)
{
    return (row >= 0 && row < grid->nrows && col >= 0 && col < grid->ncols);
}

/**************** point_getRow() ****************/
int point_getRow(point_t* point)
{
    if (point != NULL) {
        return point->row;
    }
    return -1;
}

/**************** point_getCol() ****************/
int point_getCol(point_t* point)
{
    if (point != NULL) {
        return point->col;
    }
    return -1;
}

/**************** isWall() ****************/
static bool isWall(grid_t* grid, int row, int col)
{
    char ch = grid_get(grid, row, col);
    return (ch == GRID_HORIZONTAL_WALL || ch == GRID_VERTICAL_WALL || ch == GRID_CORNER_WALL);
}
