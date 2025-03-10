/**
 * testvisibility_short.c - Simple test program for visibility module
 * 
 * CS50 Nuggets - Team 11, Winter 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "map.h"
#include "visibility.h"

// Helper function to load a map from file
grid_t* load_map_from_file(const char* filename);

// Test visibility between points
void test_visibility_between_points(grid_t* grid);

// Interactive test mode
void interactive_test_mode(grid_t* grid);

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

    grid_delete(grid);
    return 0;
}

// Helper function to load a map from a file
grid_t* load_map_from_file(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("0");
        return NULL;
    }
    
    // Create a grid with dimensions suitable for the maps provided
    grid_t* grid = grid_new(30, 100);
    if (grid == NULL) {
        fclose(fp);
        printf("1");
        return NULL;
    }
    
    bool loaded = grid_load(grid, fp);
    fclose(fp);
    
    if (!loaded) {
        grid_delete(grid);
        printf("2");
        return NULL;
    }
    
    return grid;
}

// Test visibility between various points
void test_visibility_between_points(grid_t* grid)
{
    printf("\n===== Testing Visibility Between Points =====\n");
    
    // Define test cases with coordinates specific to the map
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
        {14, 25, 13, 23, "Adjacent diagonal"},
        
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
            continue;
        }
        
        bool visible = visibility_isVisible(grid, from, to);
        
        printf("Test %d (%s): (%d,%d) to (%d,%d) - %s\n", 
            i+1, testCases[i].description,
            testCases[i].fromRow, testCases[i].fromCol, 
            testCases[i].toRow, testCases[i].toCol,
            visible ? "VISIBLE" : "NOT VISIBLE");
        
        point_delete(from);
        point_delete(to);
    }
}