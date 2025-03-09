/*
 * display.h - header file for ncurses display module
 *
 */

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdbool.h>
#include "client_state.h"

/*
 * Initialize the ncurses display
 * @param client client state to initialize
 * @return true if successful, false if error
 */
bool display_initialize(client_t* client);

/*
 * Clean up ncurses display resources
 * @param client client state to cleanup
 */
void display_cleanup(client_t* client);

/*
 * Check if the terminal window is large enough for the game grid
 * @param client client state to check
 * @return true if window is big enough, false if not
 */
bool display_check_size(client_t* client);

/*
 * Update the status line with information from the client state
 * If message is not NULL, display it as well
 * @param client client state to update
 * @param message message to display
 */
void display_status(client_t* client, const char* message);

/*
 * Display the game grid from a DISPLAY message string
 * @param client client state to update
 * @param displayString display message string
 */
void display_grid(client_t* client, const char* displayString);

/*
 * Print a message to the user and wait for confirmation
 * @param format format string for printf
 * @param ... arguments for printf
 */
void display_message(const char* format, ...);

#endif // __DISPLAY_H