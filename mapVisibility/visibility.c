/*
* visibility.c - implementation for CS50 Nuggets 'visibility' module
*
* See visibility.h for detailed information about the module interface
* 
* Team 11, Winter 2025
* Mar 8, 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "visibility.h"
#include "map.h"

/**************** Global type ****************/
typedef struct memory {
    grid_t* grid;      // Copy of the grid that's in player's memory
    bool** visible;    // 2D array of visibility flags
} memory_t;

/**************** Public functions ****************/
/* For more details, see visibility.h */
memory_t* visibility_new(grid_t* grid);
bool visibility_delete(memory_t* memory);
void visibility_calculate(memory_t* memory, grid_t* grid, point_t* pos);
bool visibility_isVisible(grid_t* grid, point_t* from, point_t* to);
char* visibility_toString(memory_t* memory, grid_t* grid, point_t* pos);
void visibility_reset(memory_t* memory, grid_t* grid, point_t* pos);

/**************** Local functions ****************/
/* Check if the path from (x1, y1) to (x0, y0) is passing through two boundary points */
static bool hasLineOfSight(grid_t* grid, int x0, int y0, int x1, int y1);

/* Check if a point is a wall or boundary */
static bool isWall(grid_t* grid, int row, int col);

/**************** visibility_new() ****************/
memory_t* visibility_new(grid_t* grid)
{
    // Validate the inputs
    if (grid == NULL) {
        return NULL;
    }

    int nrows = grid_getRows(grid);
    int ncols = grid_getCols(grid);
    
    if (nrows <= 0 || ncols <= 0) {
        return NULL;
    }

    // Allocate memory for the memory_t struct
    memory_t* memory = malloc(sizeof(memory_t));
    if (memory == NULL) {
        return NULL; 
    }

    // Create a new grid for memory
    memory->grid = grid_new(nrows, ncols);
    if (memory->grid == NULL) {
        free(memory);
        return NULL; 
    }

    // Initialize grid with empty spots
    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            grid_set(memory->grid, row, col, ' ');
        }
    }

    // Allocate visibility array of boolean pointers
    memory->visible = malloc(nrows * sizeof(bool*));
    if (memory->visible == NULL) {
        grid_delete(memory->grid);
        free(memory);
        return NULL; 
    }

    // Allocate and initialize each row of visibility array
    for (int i = 0; i < nrows; i++) {
        memory->visible[i] = malloc(ncols * sizeof(bool));
        if (memory->visible[i] == NULL) {
            // Clean up if any error
            for (int j = 0; j < i; j++) {
                free(memory->visible[j]);
            }
            free(memory->visible);
            grid_delete(memory->grid);
            free(memory);
            return NULL;
        }
        
        // Initialize all spots as not visible
        for (int j = 0; j < ncols; j++) {
            memory->visible[i][j] = false;
        }
    }

    return memory;
}

/**************** visibility_delete() ****************/
bool visibility_delete(memory_t* memory)
{
    if (memory == NULL) {
        return false;
    }

    // Free the visibility array by going through all columns in each row
    if (memory->visible != NULL) {
        int nrows = grid_getRows(memory->grid);
        for (int i = 0; i < nrows; i++) {
            if (memory->visible[i] != NULL) {
                free(memory->visible[i]);
            }
        }
        free(memory->visible);
    }

    // Free memory grid and then the entire struct
    if (memory->grid != NULL) {
        grid_delete(memory->grid);
    }
    free(memory);
    return true;
}

/**************** visibility_calculate() ****************/
void visibility_calculate(memory_t* memory, grid_t* grid, point_t* pos)
{
    if (memory == NULL || grid == NULL || pos == NULL) {
        return;
    }

    int nrows = grid_getRows(grid);
    int ncols = grid_getCols(grid);

    // Set all spots to not visible
    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            memory->visible[row][col] = false;
        }
    }
    memory->visible[point_getRow(pos)][point_getCol(pos)] = true;

    // Check visibility for each spot in the grid by calling the isVisible() helper function
    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            if (row == point_getRow(pos) && col == point_getCol(pos)) {
                continue;
            }

            // Check if this spot is visible from player's position
            point_t* curr = point_new(row, col);
            if (curr != NULL) {
                bool visible = visibility_isVisible(grid, pos, curr);
                memory->visible[row][col] = visible;
                
                // If spot visible, update memory grid
                if (visible) {
                    char content = grid_get(grid, row, col);
                    grid_set(memory->grid, row, col, content);
                }
                point_delete(curr);
            }
        }
    }
}

/**************** visibility_isVisible() ****************/
bool visibility_isVisible(grid_t* grid, point_t* from, point_t* to)
{
    if (grid == NULL || from == NULL || to == NULL) {
        return false;
    }

    // Check for line of sight by calling the helper hasLineofSight function
    return hasLineOfSight(grid, point_getCol(from), point_getRow(from), point_getCol(to), point_getRow(to));
}

/**************** visibility_toString() ****************/
char* visibility_toString(memory_t* memory, grid_t* grid, point_t* pos)
{
    if (memory == NULL || grid == NULL || pos == NULL) {
        return NULL;
    }

    int nrows = grid_getRows(grid);
    int ncols = grid_getCols(grid);

    // Allocate memory for each row + newline and the null terminator at the end
    size_t size = nrows * (ncols + 1) + 1;
    char* str = malloc(size);
    if (str == NULL) {
        return NULL; 
    }

    // Start with empty string
    str[0] = '\0';
    char line[ncols + 1];  // +1 for null terminator

    // Build string row by row
    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            if (memory->visible[row][col]) {
                // Spot is currently visible, so direclty use grid content
                line[col] = grid_get(grid, row, col);
            } else {
                // Spot is not currently visible, so use memory if previously seen, or space if not
                char remembered = grid_get(memory->grid, row, col);
                if  (remembered != ' ') {
                    line[col] = remembered;
                }
                else {
                    line[col] = ' ';
                }
            }
        }
        line[ncols] = '\0';
        strcat(str, line);
        strcat(str, "\n");
    }

    return str;
}

/**************** visibility_reset() ****************/
void visibility_reset(memory_t* memory, grid_t* grid, point_t* pos)
{
    if (memory == NULL || grid == NULL || pos == NULL) {
        return;
    }

    // Recalculate visibility from current position
    visibility_calculate(memory, grid, pos);
}

/**************** hasLineOfSight() ****************/
static bool hasLineOfSight(grid_t* grid, int x0, int y0, int x1, int y1)
{
    // Implementation of Bresenham's line algorithm
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true) {
        // If we've reached the endpoint, and has not returned yet, there's line of sight
        if (x0 == x1 && y0 == y1) {
            return true;
        }

        // Skip the starting point when checking for walls
        if (!(x0 == x1 && y0 == y1) && isWall(grid, y0, x0)) {
            // Current point is a wall, so there's no line of sight
            return false;
        }

        // Calculate next point
        e2 = 2 * err;
        if (e2 >= dy) {  // step in x direction
            if (x0 == x1) {
                break;  // Reached target x
            }
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx) {  // step in y direction
            if (y0 == y1) {
                break;  // Reached target y
            }
            err += dx;
            y0 += sy;
        }
    }

    // We reached the target position
    return true;
}

/**************** isWall() ****************/
static bool isWall(grid_t* grid, int row, int col)
{
    // Get character at position and check if it's a wall character
    char ch = grid_get(grid, row, col);
    return (ch == GRID_HORIZONTAL_WALL || ch == GRID_VERTICAL_WALL || ch == GRID_CORNER_WALL);
}