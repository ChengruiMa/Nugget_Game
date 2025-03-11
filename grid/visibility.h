/*
* visibility.h - header file for CS50 Nuggets 'visibility' module
*
* A module for calculating what's visible to players on the game grid
* Includes functions for line-of-sight calculations and memory tracking
* 
* Team 11
* Mar 8, 2025
*/

#ifndef __VISIBILITY_H
#define __VISIBILITY_H

#include <stdio.h>
#include <stdbool.h>
#include "grid.h"

/**************** Global type ****************/
/* Memory grid structure to track what a player has seen */
typedef struct memory memory_t;

/**************** Public functions ****************/

/*
 * Create a new memory structure for a game grid
 * 
 * Caller provides:
 *   Valid pointer to a grid
 * We return:
 *   Pointer to a new memory structure, or NULL if error
 * Caller is responsible for later calling visibility_delete
 */
memory_t* visibility_new(grid_t* grid);

/*
 * Delete a memory structure and free all associated memory
 * 
 * Caller provides:
 *   Valid pointer to a memory structure
 * We return:
 *   true if successful, false otherwise
 * We do nothing if memory is NULL
 */
bool visibility_delete(memory_t* memory);

/*
 * Calculate visibility from a given position
 * 
 * Caller provides:
 *   Valid pointer to a memory structure
 *   Valid pointer to a grid
 *   Valid pointer to a position point
 * We do:
 *   Update the memory's visibility information based on the given position
 */
void visibility_calculate(memory_t* memory, grid_t* grid, point_t* pos);

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
bool visibility_isVisible(grid_t* grid, point_t* from, point_t* to);

/*
 * Generate a string representation of the current view
 * 
 * Caller provides:
 *   Valid pointer to a memory structure
 *   Valid pointer to a grid
 *   Valid pointer to current position point
 * We return:
 *   String containing the current view, or NULL if error
 * Notes:
 *   String includes currently visible spots and remembered spots
 *   Caller is responsible for freeing the returned string
 */
char* visibility_toString(memory_t* memory, grid_t* grid, point_t* pos);

/*
 * Reset visibility calculation for a new position
 * 
 * Caller provides:
 *   Valid pointer to a memory structure
 *   Valid pointer to a grid
 *   Valid pointer to new position point
 * We do:
 *   Recalculate visibility from the new position
 */
void visibility_reset(memory_t* memory, grid_t* grid, point_t* pos);


#endif // __VISIBILITY_H