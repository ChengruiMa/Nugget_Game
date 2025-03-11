#include <stdio.h>
#include <assert.h>
#include "map.h"  // Changed from "grid.h" to "map.h"

void test_grid_new_and_delete() {
    int nrows = 5, ncols = 10;
    grid_t* grid = grid_new(nrows, ncols);
    assert(grid != NULL);
    assert(grid_getRows(grid) == nrows);
    assert(grid_getCols(grid) == ncols);
    for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
            // All cells should be initialized to GRID_EMPTY_SPOT
            assert(grid_get(grid, r, c) == GRID_EMPTY_SPOT);
        }
    }
    bool deleted = grid_delete(grid);
    assert(deleted == true);
    printf("test_grid_new_and_delete passed\n");
}

void test_grid_load() {
    FILE* fp = fopen("maps/test_simple.txt", "r");
    assert(fp != NULL);
    grid_t* grid = grid_new(5, 7); // Adjust dimensions based on your test map file
    bool loaded = grid_load(grid, fp);
    fclose(fp);
    assert(loaded == true);
    // We assume that maps/test_simple.txt has '+' at (0,0) and '.' at (1,1)
    assert(grid_get(grid, 0, 0) == '+');
    assert(grid_get(grid, 1, 1) == '.');
    grid_delete(grid);
    printf("test_grid_load passed\n");
}

int main() {
    test_grid_new_and_delete();
    test_grid_load();
    printf("All grid tests passed!\n");
    return 0;
}
