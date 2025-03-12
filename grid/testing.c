/**
 * testvisibility_short.c - Simple test program for visibility module
 * 
 * CS50 Nuggets - Team 11, Winter 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "grid.h"
// #include "visibility.h"

grid_t* load_map_from_file(const char* filename);

// Test visibility between points
void test_visibility_between_points(grid_t* grid);

// Test player memory and visibility
void test_player_memory(grid_t* grid);

// Test toString functions
void test_string_functions(grid_t* grid);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mapfile>\n", argv[0]);
        return 1;
    }

    // Load the map
    grid_t* grid = load_map_from_file(argv[1]);
    if (grid == NULL) {
        fprintf(stderr, "Failed to load map from %s\n", argv[1]);
        return 1;
    }

    printf("Map loaded from %s (%d rows, %d columns)\n", 
        argv[1], grid_getRows(grid), grid_getCols(grid));

    // Run automated tests
    test_visibility_between_points(grid);
    
    // Test player memory
    test_player_memory(grid);
    
    // Test string functions
    test_string_functions(grid);

    grid_delete(grid);
    return 0;
}

// Helper function to load a map from a file
grid_t* load_map_from_file(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }
    
    grid_t* grid = grid_load(fp);
    if (grid == NULL) {
        fclose(fp);
        printf("Error: Could not create grid\n");
        return NULL;
    }
    fclose(fp);
    
    return grid;
}

// Test visibility between various points
void test_visibility_between_points(grid_t* grid)
{
    printf("\n===== Testing Visibility Between Points =====\n");
    
    // Define test cases
    struct {
        int fromRow, fromCol;
        int toRow, toCol;
        const char* description;
    } testCases[] = {
        // Same position (always visible)
        {14, 25, 14, 25, "Same position"},
        
        // Adjacent positions (should be visible)
        {14, 25, 14, 26, "Adjacent horizontal"},
        {14, 25, 13, 25, "Adjacent vertical"},
        {14, 25, 13, 24, "Adjacent diagonal"},
        
        // Various test cases for the specific map
        {14, 25, 13, 35, "Test A"},
        {14, 25, 14, 35, "Test B"},
        {14, 25, 15, 35, "Test C"},
        {14, 25, 16, 35, "Test D"},
        {14, 25, 17, 35, "Test E"},
        {14, 25, 18, 35, "Test F"},
    };
    
    int numCases = sizeof(testCases) / sizeof(testCases[0]);
    
    for (int i = 0; i < numCases; i++) {
        point_t* from = point_new(testCases[i].fromRow, testCases[i].fromCol);
        point_t* to = point_new(testCases[i].toRow, testCases[i].toCol);
        
        if (from == NULL || to == NULL) {
            printf("Error: Could not create points\n");
            if (from != NULL) point_delete(from);
            if (to != NULL) point_delete(to);
            continue;
        }
        
        bool visible = grid_isVisible(grid, from, to);
        
        printf("Test %d (%s): (%d,%d) to (%d,%d) - %s\n", 
            i+1, testCases[i].description,
            testCases[i].fromRow, testCases[i].fromCol, 
            testCases[i].toRow, testCases[i].toCol,
            visible ? "VISIBLE" : "NOT VISIBLE");
        
        point_delete(from);
        point_delete(to);
    }
}

// Test player memory and visibility functionality
void test_player_memory(grid_t* grid)
{
    printf("\n===== Testing Player Memory and Visibility =====\n");
    
    // Create a player view from the master grid
    grid_t* playerGrid = grid_createPlayerGrid(grid);
    if (playerGrid == NULL) {
        printf("Error: Could not create player grid\n");
        return;
    }
    
    // Test position in the middle of the map
    point_t* playerPos = point_new(14, 25);
    if (playerPos == NULL) {
        printf("Error: Could not create player position\n");
        grid_delete(playerGrid);
        return;
    }
    
    // Calculate visibility from player position
    printf("Calculating visibility from position (%d,%d)\n", 
           point_getRow(playerPos), point_getCol(playerPos));
    grid_calculateVisibility(playerGrid, playerPos);
    
    // Test some positions for visibility
    int testPositions[][2] = {
        {14, 25},  // Player position (should be visible)
        {14, 26},  // Adjacent (should be visible)
        {16, 39},  // Further away 
        {16, 38},  // Further away 
    };
    
    int numPositions = sizeof(testPositions) / sizeof(testPositions[0]);
    
    for (int i = 0; i < numPositions; i++) {
        int row = testPositions[i][0];
        int col = testPositions[i][1];
        
        bool isVisible = grid_isPointVisible(playerGrid, row, col);
        char cellContent = grid_get(playerGrid, row, col);
        
        printf("Position (%d,%d): %s, Content: '%c'\n", 
               row, col, 
               isVisible ? "VISIBLE" : "NOT VISIBLE",
               cellContent);
    }
    
    // Cleanup
    point_delete(playerPos);
    grid_delete(playerGrid);
}

// Test string representation functions
void test_string_functions(grid_t* grid)
{
    printf("\n===== Testing String Functions =====\n");
    
    // Create a player view from the master grid
    grid_t* playerGrid = grid_createPlayerGrid(grid);
    if (playerGrid == NULL) {
        printf("Error: Could not create player grid\n");
        return;
    }
    
    // Test position in the middle of the map
    point_t* playerPos = point_new(2, 9);
    if (playerPos == NULL) {
        printf("Error: Could not create player position\n");
        grid_delete(playerGrid);
        return;
    }
    
    // Calculate visibility from player position
    grid_calculateVisibility(playerGrid, playerPos);
    
    // Get string representation of regular grid
    char* regularStr = grid_toString(grid);
    if (regularStr != NULL) {
        printf("Regular grid:\n%s\n", regularStr);
        free(regularStr);
    } else {
        printf("Error: Could not get string representation of grid\n");
    }
    
    // Get string representation of visibility
    char* visibilityStr = grid_visibilityToString(playerGrid, playerPos);
    if (visibilityStr != NULL) {
        printf("\nVisibility grid:\n%s\n", visibilityStr);
        free(visibilityStr);
    } else {
        printf("Error: Could not get visibility string representation\n");
    }
    
    // Change player position and test again
    point_delete(playerPos);
    playerPos = point_new(13, 24);
    if (playerPos != NULL) {
        printf("\nMoving player to position (%d,%d) and recalculating visibility\n", 
               point_getRow(playerPos), point_getCol(playerPos));
        
        grid_calculateVisibility(playerGrid, playerPos);
        
        char* newVisibilityStr = grid_visibilityToString(playerGrid, playerPos);
        if (newVisibilityStr != NULL) {
            printf("New visibility grid:\n%s\n", newVisibilityStr);
            free(newVisibilityStr);
        }
    }
    
    // Cleanup
    point_delete(playerPos);
    grid_delete(playerGrid);
}
