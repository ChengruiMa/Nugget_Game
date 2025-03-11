#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "visibility.h"

// Helper to create a point and check it is non-null.
point_t* create_point(int row, int col) {
    point_t* pt = point_new(row, col);
    assert(pt != NULL);
    return pt;
}

void test_visibility_line_of_sight() {
    // Create a 5x5 grid and insert a wall at (2,2)
    int nrows = 5, ncols = 5;
    grid_t* grid = grid_new(nrows, ncols);
    assert(grid != NULL);
    
    // Place a wall using the wall constant from map.h (e.g., GRID_HORIZONTAL_WALL)
    grid_set(grid, 2, 2, GRID_HORIZONTAL_WALL);
    
    // Test line-of-sight directly using visibility_isVisible()
    point_t* from = create_point(0, 0);
    point_t* to_wall = create_point(2, 2);
    // Expect false because a wall is on the line from (0,0) to (2,2)
    assert(visibility_isVisible(grid, from, to_wall) == false);

    // Test a clear line-of-sight (e.g., (0,0) to (1,1) should be visible)
    point_t* to_clear = create_point(1, 1);
    assert(visibility_isVisible(grid, from, to_clear) == true);

    point_delete(from);
    point_delete(to_wall);
    point_delete(to_clear);
    grid_delete(grid);
    printf("test_visibility_line_of_sight passed\n");
}

void test_visibility_calculate_and_toString() {
    // Create a simple 3x3 grid with distinct characters
    int nrows = 3, ncols = 3;
    grid_t* grid = grid_new(nrows, ncols);
    assert(grid != NULL);
    
    // Set unique content so we can verify the output string.
    char contents[3][4] = { "ABC", "DEF", "GHI" };
    for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
            grid_set(grid, r, c, contents[r][c]);
        }
    }
    
    // Create the memory structure for visibility.
    memory_t* mem = visibility_new(grid);
    assert(mem != NULL);
    
    // Choose a center position (1,1) for testing.
    point_t* pos = create_point(1, 1);
    visibility_calculate(mem, grid, pos);

    // In an open grid with no walls, every cell should be visible.
    char* view = visibility_toString(mem, grid, pos);
    assert(view != NULL);
    // The expected view should match the grid’s content, with newlines at the end of each row.
    char expected[50];
    snprintf(expected, sizeof(expected), "ABC\nDEF\nGHI\n");
    assert(strcmp(view, expected) == 0);

    free(view);
    point_delete(pos);
    visibility_delete(mem);
    grid_delete(grid);
    printf("test_visibility_calculate_and_toString passed\n");
}

int main() {
    test_visibility_line_of_sight();
    test_visibility_calculate_and_toString();
    printf("All visibility tests passed!\n");
    return 0;
}
