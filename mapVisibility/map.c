/*
* map.c - implementation for CS50 Nuggets 'map' module
*
* See map.h for detailed information about the module interface
* 
* Team 11, Winter 2025
* Mar 8, 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "map.h"

const char GRID_EMPTY_SPOT = ' ';
const char GRID_PASSAGE_SPOT = '#';
const char GRID_HORIZONTAL_WALL = '-';
const char GRID_VERTICAL_WALL = '|';
const char GRID_CORNER_WALL = '+';
const char GRID_GOLD_SPOT = '*';

/**************** Global types ****************/
typedef struct point {
    int row;  // row coordinate
    int col;  // column coordinate
} point_t;

typedef struct grid {
    int nrows;          // number of rows
    int ncols;          // number of columns
    char** cells;       // 2D array of characters
    bool initialized;   // flag to indicate if grid is initialized successfully
} grid_t;

/**************** Public functions ****************/
/* Opeartions on the grid structure */
/* For detailed descriptions, see map.h */
grid_t* grid_new(int nrows, int ncols);
bool grid_delete(grid_t* grid);
bool grid_load(grid_t* grid, FILE* fp);
char grid_get(grid_t* grid, int row, int col);
bool grid_set(grid_t* grid, int row, int col, char ch);
bool grid_isRoom(grid_t* grid, int row, int col);
bool grid_isPassage(grid_t* grid, int row, int col);
point_t* grid_findEmptyRoomSpot(grid_t* grid);
char* grid_toString(grid_t* grid);
int grid_getRows(grid_t* grid);
int grid_getCols(grid_t* grid);

/* Opeartions on the point structure */
/* For detailed descriptions, see map.h */
point_t* point_new(int row, int col);
void point_delete(point_t* point);

/**************** local functions ****************/
/* Check if a point is within the grid boundaries */
static bool isValidPoint(grid_t* grid, int row, int col);

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

/**************** grid_delete() ****************/
bool grid_delete(grid_t* grid)
{
    if (grid == NULL) {
        return false;
    }

    if (grid->cells != NULL) {
        // Free each row
        for (int i = 0; i < grid->nrows; i++) {
            if (grid->cells[i] != NULL) {
                free(grid->cells[i]);
            }
        }
        // Free the array of row pointers
        free(grid->cells);
    }

    // Free the grid structure
    free(grid);
    return true;
}

/**************** grid_load() ****************/
bool grid_load(grid_t* grid, FILE* fp)
{
    if (grid == NULL || fp == NULL || !grid->initialized) {
        return false;
    }

    // Allocate memory for each line, with newline and null terminator
    char line[grid->ncols + 2]; 
    int row = 0;

    // Read line by line from the file
    while (fgets(line, sizeof(line), fp) != NULL && row < grid->nrows) {
        // Remove newline character if present
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }

        // Validate line length
        if (len > (size_t)grid->ncols) {
            return false; 
        }

        // Copy line to grid
        for (size_t col = 0; col < len; col++) {
            grid->cells[row][col] = line[col];
        }

        // If line is shorter than grid width, fill with spaces
        for (int col = len; col < grid->ncols; col++) {
            grid->cells[row][col] = GRID_EMPTY_SPOT;
        }

        row++;
    }

    // If we didn't read enough rows
    if (row < grid->nrows) {
        // Fill remaining rows with spaces
        for (int r = row; r < grid->nrows; r++) {
            for (int c = 0; c < grid->ncols; c++) {
                grid->cells[r][c] = GRID_EMPTY_SPOT;
            }
        }
    }

    return true;
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