/*
 * display.c - implementation of ncurses display module
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ncurses.h>
#include "../support/log.h"
#include "../grid/grid.h"
#include "display.h"

//const
#define STATUS_HEIGHT 1 //height of status line

/*
 * Initialize the ncurses display
 * @param client client state to initialize
 * @return true if successful, false if error
 */
bool display_initialize(client_t* client)
{
    if (client == NULL || client->grid == NULL) {
        log_v("Cannot initialize display: client or grid is NULL");
        return false;
    }
    
    initscr(); //initialize curses screen
    cbreak(); //disable line buffering
    noecho(); //don't echo keystrokes
    curs_set(0); //hide the cursor
    
    if (!display_check_size(client)) { //check if terminal window is large enough
        return false;
    }
    
    int rows = grid_getRows(client->grid); //create game window
    int cols = grid_getCols(client->grid);
    client->gameWindow = newwin(rows, cols, STATUS_HEIGHT, 0);
    
    keypad(stdscr, TRUE); //enable keypad input for function keys, arrow keys, etc.
    
    //intalize colors if terminal supports them
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);  //gold
        init_pair(2, COLOR_GREEN, COLOR_BLACK);   //player
        init_pair(3, COLOR_BLUE, COLOR_BLACK);    //other player
        init_pair(4, COLOR_WHITE, COLOR_BLACK);   //walls and boundaries
    }
    
    clear(); //clear screen
    refresh(); //refresh screen
    
    return true;
}

/*
 * Clean up ncurses display resources
 * @param client client state to cleanup
 */
void display_cleanup(client_t* client)
{
    endwin(); //end ncurses mode
}

/*
 * Check if the terminal window is large enough for the game grid
 * @param client client state to check
 * @return true if window is big enough, false if not
 */
bool display_check_size(client_t* client)
{
    if (client == NULL || client->grid == NULL) {
        return false;
    }
    
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    
    int requiredRows = grid_getRows(client->grid) + STATUS_HEIGHT;
    int requiredCols = grid_getCols(client->grid);
    
    if (rows < requiredRows || cols < requiredCols) {
        clear();
        mvprintw(0, 0, "Terminal window too small, you need at least %d rows x %d cols",
                 requiredRows, requiredCols);
        mvprintw(1, 0, "Current window size: %d rows x %d cols", rows, cols);
        mvprintw(2, 0, "Please resize your terminal and press any key to continue...");
        refresh();
        getch(); //wait for a keystroke
        
        // After resize, reinitialize the window
        endwin();
        refresh();
        clear();
        
        // Get new dimensions
        getmaxyx(stdscr, rows, cols);
        if (rows < requiredRows || cols < requiredCols) {
            return false; // Still too small
        }
    }
    
    // Delete old window if it exists
    if (client->gameWindow != NULL) {
        delwin(client->gameWindow);
    }
    
    // Create new game window with current dimensions
    int gridRows = grid_getRows(client->grid);
    int gridCols = grid_getCols(client->grid);
    client->gameWindow = newwin(gridRows, gridCols, STATUS_HEIGHT, 0);
    
    if (client->gameWindow == NULL) {
        return false;
    }
    
    refresh();
    wrefresh(client->gameWindow);
    
    return true;
}

/*
 * Update the status line with information from the client state
 * @param client client state to update
 * @param message message to display
 */
void display_status(client_t* client, const char* message)
{
    if (client == NULL) {
        return;
    }

    int rows, cols;
    getmaxyx(stdscr, rows, cols); // Get screen dimensions correctly
    
    // Print the left-side status
    move(0, 0);
    if (client->isSpectator) {
        printw("Spectator: %d nuggets unclaimed.", client->remainingGold);
    } else {
        printw("Player %c has %d nuggets (%d nuggets unclaimed).",
               client->playerLetter, client->purse, client->remainingGold);
    }

    // Print the right-side message only if it's not NULL
    if (message != NULL) {
        mvprintw(0, cols - strlen(message) - 2, "%s", message);
    }

    refresh(); // Update the display
}

/*
 * Display the game grid from a DISPLAY message string
 * @param client client state to update
 * @param displayString display message string
 */
void display_grid(client_t* client, const char* displayString)
{
    if (client == NULL || client->gameWindow == NULL || displayString == NULL) {
        return;
    }
    
    werase(client->gameWindow); //clear game window
    
    const char* gridDisplay = displayString + 8; //skip the "DISPLAY\n" prefix
    
    int row = 0;
    const char* lineStart = gridDisplay;
    const char* lineEnd;
    
    while ((lineEnd = strchr(lineStart, '\n')) != NULL) {
        size_t lineLength = lineEnd - lineStart;
        
        for (size_t col = 0; col < lineLength; col++) {
            char ch = lineStart[col];
            
            if (has_colors()) { //apply colors based on character type
                if (ch == '*') {
                    wattron(client->gameWindow, COLOR_PAIR(1)); //gold
                } else if (ch == '@') {
                    wattron(client->gameWindow, COLOR_PAIR(2)); //current player
                } else if (ch >= 'A' && ch <= 'Z') {
                    wattron(client->gameWindow, COLOR_PAIR(3)); //other players
                    wattron(client->gameWindow, COLOR_PAIR(3));
                } else if (ch == '+' || ch == '-' || ch == '|') {
                    wattron(client->gameWindow, COLOR_PAIR(4)); //walls and boundaries
                }
            }
            
            mvwaddch(client->gameWindow, row, col, ch); //print the character at the current pos
            
            if (has_colors()) {
                wattroff(client->gameWindow, COLOR_PAIR(1)); //turn off colors
                wattroff(client->gameWindow, COLOR_PAIR(2));
                wattroff(client->gameWindow, COLOR_PAIR(3));
                wattroff(client->gameWindow, COLOR_PAIR(4));
            }
        }
        
        lineStart = lineEnd + 1; //move to the next line
        row++;
    }
    
    wrefresh(client->gameWindow); //update the display
}