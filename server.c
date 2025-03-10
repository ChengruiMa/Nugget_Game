/*
 * CS50 'Nuggets' Project — Server
 *
 * Zachary Rosca-Halmagean, March 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "message.h"

// function prototypes
static int parseArgs(int argc, char *argv[], int *storedSeed, char *map);
static game_t *initGame(FILE *map);
bool handleMessage(void *arg, const addr_t from, const char *message);
static void endGame(game_t *game);

// structs
typedef struct game
{
    grid_t *grid;           // master grid
    player_t **players;     // list of players
    spectator_t *spectator; // the single spectator
    int playersSeen;        // total number of players seen (i.e., both joined and left)
    gold_t *gold;
} game_t;

typedef struct gold
{
    int *piles;
    int numPiles; // num of piles
    int total; // total gold
} gold_t;

// global variables
const int maxPlayers = 4;
const int maxSpectators = 1;
const int maxRealNameLength = 20;

static int parseArgs(int argc, char *argv[], int *storedSeed, char *map)
{
    // check for exactly one or two parameters (2 or 3 args including the program name)
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Usage: ./server map.txt [seed]\n");
        return 1; // return 1 to indicate incorrect usage
    }

    if (argc == 3)
    { // optional parameter (seed) provided, make sure it's in correct format (unsigned integer) & store it
        int seed = atoi(argv[2]);
        if (seed < 0)
        {
            fprintf(stderr, "Incorrect seed format, must be unsigned integer: %d\n", seed);
            return 3; // return 3 to indicate incorrect seed format
        }

        // store seed
        *storedSeed = seed;
    }

    // check if map file exists
    FILE *mapFile = fopen(argv[1], "r");
    if (mapFile == NULL)
    {
        fprintf(stderr, "Incorrect map filepath, could not read file: %s\n", argv[1]);
        return 2; // return 2 to indicate map file not found
    }
    // close file
    fclose(mapFile);

    // store map file path
    strcpy(map, argv[1]); // TODO: check if this handles memory correctly

    return 0; // return 0 to indicate successful parsing
}

/**
 * Initializes game state
 *
 * pretty sure this should create/return a game state struct (or pointer to one) — need to define this struct in the gamestate module's header file
 *
 * Inputs:
 * @param map: a pointer to the opened map FILE with map information to be read
 */
game_t *
initGame(FILE *map)
{
    if (map == NULL)
    {
        fprintf(stderr, "Error reading map file\n");
        return 4; // return 4 to indicate error reading map file
    }

    // create game state here?

    // drop gold across map in various piles (at least GoldMinNumPiles and at most GoldMaxNumPiles); random number of nuggets in each (this is handled in the gold module i'm p sure)
}

static void endGame(game_t *game)
{
    if (game == NULL)
    {
        fprintf(stderr, "Error: Invalid game state provided in endGame\n");
        return;
    } // else do below

    game_delete(game); // free all memory allocated for game state
}

/**
 * Compares two players by score (i.e., number of nuggets collected)
 *
 * Inputs:
 * @param a: the first player to compare
 * @param b: the second player to compare
 */
static int comparePlayers(const void *a, const void *b)
{
    player_t *playerA = *(player_t **)a;
    player_t *playerB = *(player_t **)b;

    return playerB->purse - playerA->purse; // sort in descending order
}

/**
 * Handles game over operations (i.e., leaderboard, final score, etc.)
 *
 * Inputs:
 * @param game: the game state to handle game over operations for
 */
static void handleGameOver(game_t *game)
{
    if (game == NULL)
    {
        fprintf(stderr, "Error Invalid game state provided in handleGameOver\n");
        return;
    }

    player_t** players = game->players;
    int playersSeen = game->playersSeen;

    // sort players by score (i.e., number of nuggets collected)
    qsort(players, playersSeen, sizeof(player_t*), comparePlayers); // need to define comparePlayers function

    // create leaderboard end game message
    char* leaderboard = malloc(((playersSeen + 1) * (sizeof(char)*maxRealNameLength)) + 50); // should be enough for message (add 1 to players seen to account for spectator)
    if (leaderboard == NULL)
    {
        fprintf(stderr, "Error creating leaderboard message\n");
        return;
    }

    strcpy(leaderboard, "QUIT GAME OVER:\n");
    for (int i = 0; i < playersSeen; i++)
    {
        player_t* player = players[i];
        char* playerScore = malloc(sizeof(char) * maxRealNameLength + 10); // should be enough for message
        if (playerScore == NULL)
        {
            fprintf(stderr, "Error creating player score message\n");
            return;
        }

        sprintf(playerScore, "%c %10d %s\n", player->playerLetter, player->purse, player->realName); // format message as per REQUIREMENTS spec
        strcat(leaderboard, playerScore);
        free(playerScore); // free player score message
    }

    // send leaderboard message to all players and spectators
    for (int i = 0; i < playersSeen; i++)
    {
        player_t* player = players[i];
        message_send(player->address, leaderboard);
    }

    if (game->spectator != NULL)
    {
        message_send(game->spectator->address, leaderboard);
    }

    free(leaderboard); // free leaderboard message
}

/**
 * Converts a message string into an array of strings (each serving a diff purpose)
 *
 * Inputs:
 * @param message: the message string to be parsed
 */

char **
parseMessage(char *message)
{
    // parse message into an array of strings
    // return array of strings

    if (message == NULL)
    {
        fprintf(stderr, "Error parsing message\n");
        return NULL;
    }

    // split message into array of strings
    char **parsed = malloc(sizeof(char *) * strlen(message)); // p sure this will allocate length of msg and not num of words — don't think it rlly matters tho

    if (parsed == NULL)
    {
        fprintf(stderr, "Error parsing message\n");
        return NULL;
    }

    char *token = strtok(message, " ");
    int i = 0;

    while (token != NULL)
    {
        parsed[i] = malloc(sizeof(token) + 1); // +1 for null terminator, or malloc(strlen(token) + 1);
        if (parsed[i] == NULL)
        {
            fprintf(stderr, "Error parsing message\n");
            return NULL;
        }
        strcpy(parsed[i], token); // copy token into array

        token = strtok(NULL, " "); // get next token
        i++;
    }

    parsed[i] = NULL; // set last element to NULL to indicate end of array

    return parsed;
}

/**
 * Frees memory allocated for a message array
 *
 * Inputs:
 * @param message: the message array to be freed
 */
void freeMessage(char **message)
{
    if (message == NULL)
    {
        fprintf(stderr, "Error freeing message\n");
        return;
    }

    for (int i = 0; message[i] != NULL; i++)
    {
        free(message[i]);
    }

    free(message); // free array
}

void sendOK(player_t *player)
{
    char *message = malloc(sizeof("OK ") + sizeof(player->letter) + 2); // +1 for null terminator, +1 for new line
    if (message == NULL)
    {
        fprintf(stderr, "Error sending OK message to player\n");
        return;
    }

    strcpy(message, "OK ");
    strcat(message, player->letter);

    // send OK w/ player letter to player
    message_send(player->address, message);

    free(message);
}

static void
handleNewSpectator(game_t* game, addr_t from)
{
    if (game == NULL) {
        fprintf(stderr, "Invalid game state passed to handleNewSpectator\n");
        return;
    }
    // add to game state (THE BELOW SHOULD BE A FUNCTION IN THE GAMESTATE MODULE @ARAL — as in, game_addSpectator(game, from) or something of the nature)
    if (game->spectator != NULL) {
        message_send(game->spectator->address, "You have been replaced by a new spectator.");
        spectator_delete(game->spectator); // should free spectator memory (THIS SHOULD BE A FUNCTION IN SPECTATOR MODULE THAT SHOULD EXIST @NEAL — must also create the spectator module @NEAL)
    }

    spectator_t* spectator = spectator_new(from); // create new spectator (SHOULD BE A FUNCTION IN SPECTATOR MODULE THAT SHOULD EXIST @NEAL)
    game->spectator = spectator; // set spectator in game state
    // DONE WITH ADD TO GAME STATE LOGIC — ABOVE (after game null check) SHOULD BE HANDLED IN GAME STATE MODULE @ARAL

    // send initial grid message to spectator
    int nrows = game->grid->nrows;
    int ncols = game->grid->ncols;

    char* message = malloc(sizeof(char) * 100); // should be enough for message
    if (message == NULL) {
        fprintf(stderr, "Error sending initial grid message to spectator\n");
        return;
    }

    sprintf(message, "GRID %d %d\n", nrows, ncols); // format message as per REQUIREMENTS spec
    message_send(from, message); // send grid message to spectator
    free(message); // free message
}

void handleQuit(game_t *game, addr_t from)
{
    if (game == NULL)
    {
        fprintf(stderr, "Invalid parameters passed to handleQuit\n");
        return;
    }

    // get player from address (THIS SHOULD GO IN THE GAMESTATE MODULE @ARAL)
    player_t *player = NULL;
    for (int i = 0; i < game->playersSeen; i++)
    {
        player_t *player = game->players[i];
        if (message_eqAddr(player->address, from))
        {
            break; // here you would return the player (AS A METHOD IN GAMESTATE MDOULE @ARAL)
        }
    }

    // handle spectator quit
    if (player == NULL)
    {
        spectator_t *spectator = game->spectator;

        if (spectator == NULL)
        {
            fprintf(stderr, "Error handling quit: could not find player or spectator with matching address in game\n");
            return;
        }

        message_send(from, "QUIT Thanks for watching!");

        spectator_delete(spectator); // should free spectator memory (THIS SHOULD BE A FUNCTION IN SPECTATOR MODULE THAT SHOULD EXIST @NEAL — important for memory management)
        game->spectator = NULL;      // set spectator pointer to NULL (p sure this is best practice)
    }
    else
    {
        player->left = true; // set player left to true (THIS SHOULD EXIST IN PLAYER STRUCT @NEAL (either 'left', 'isActive', 'active', or something of the nature))
        // don't remove from game state since we'll want to keep player in game state for game over operations (i.e., leaderboard, final score, etc.)
        message_send(from, "QUIT Thanks for playing!"); // send quit msg as defined in REQUIREMENTS spec
    }
}

static void updateSpectatorDisplay(game_t *game, spectator_t *spectator)
{
    if (game == NULL || spectator == NULL)
    {
        fprintf(stderr, "Invalid parameters passed to updateSpectatorDisplay\n");
        return;
    }

    grid_t *grid = game->grid;
    // char* display = grid_toString(grid); // get grid as string
    // NOTE: BELOW IS SUBJECT TO CHANGE DEPENDING ON MARTIN'S IMPLEMENTATION — LIKELY WILL NEED TO CHANGE/TWEAK
    char *display = grid_buildDisplayString(game, grid); // build display string (THIS SHOULD BE A FUNCTION IN THE GRID MODULE @MARTIN)

    char *message = malloc((sizeof(char) * strlen(display)) + 10); // +10 for "DISPLAY \n" and null terminator — should fit in message_MaxBytes according to REQUIREMENTS spec
    if (message == NULL)
    {
        fprintf(stderr, "Error updating spectator display\n");
        return;
    }

    strcpy(message, "DISPLAY \n");
    strcat(message, display);

    message_send(spectator->address, message); // send display message to spectator

    free(message);
    free(display);
}

static void updatePlayerDisplay(game_t *game, player_t *player)
{
    if (game == NULL || player == NULL)
    {
        fprintf(stderr, "Invalid parameters passed to updatePlayerDisplay\n");
        return;
    }

    grid_t *grid = game->grid;
    memory_t *visibility = visibility_new(grid);              // create memory_t struct for player visibility, used in calculate step
    point_t *playerPos = point_new(player->row, player->col); // create point_t struct for player position, used in calculate step
    visibility_calculate(visibility, grid, playerPos);

    // char* display = player_createDisplayString(player, grid, game->gold);
    // NOTE: BELOW IS SUBJECT TO CHANGE DEPENDING ON MARTIN'S IMPLEMENTATION — LIKELY WILL NEED TO CHANGE/TWEAK
    char *display = grid_buildPlayerDisplayString(game, player); // build player display string (THIS SHOULD BE A FUNCTION IN THE GRID MODULE @MARTIN)

    char *message = malloc((sizeof(char) * strlen(display)) + 10); // +10 for "DISPLAY \n" and null terminator — should fit in message_MaxBytes according to REQUIREMENTS spec
    if (message == NULL)
    {
        fprintf(stderr, "Error updating player display\n");
        return;
    }

    strcpy(message, "DISPLAY \n");
    strcat(message, display);

    message_send(player->address, message); // send display message to player

    free(message);
    free(display);
    visibility_delete(visibility); // remove once grid/visibility is refactored to keep visibility contained in grid/visibility operations — should not be exposed to users of the grid module
    point_delete(playerPos);       // remove once grid/visibility is refactored to keep visibility contained in grid/visibility operations — should not be exposed to users of the grid module
}

static void updateSpectatorGold(game_t* game)
{
    spectator_t* spectator = game->spectator;
    if (spectator == NULL)
    {
        fprintf(stderr, "No spectator currently active in game.\n");
        return;
    }

    int remainingGold = getRemainingGold(game->gold);
    // int totalGold = game->gold->total;

    char* message = malloc(sizeof(char) * 50); // should be enough for message
    if (message == NULL)
    {
        fprintf(stderr, "Error updating spectator gold\n");
        return;
    }

    // sprintf(message, "Spectator: %d/%d nuggets unclaimed", remainingGold, totalGold); // need to format / add colors / add bold/emphasis to message later
    
    sprintf(message, "GOLD %d %d %d", 0, 0, remainingGold); // format message as per REQUIREMENTS spec
    message_send(spectator->address, message); // send gold message to spectator
    free(message); // free message
}

static void updateAllPlayerGold(game_t* game)
{
    player_t** players = game->players;
    int playersSeen = game->playersSeen;

    for (int i = 0; i < playersSeen; i++)
    {
        player_t* player = players[i];
        if (player == NULL)
        {
            fprintf(stderr, "Error updating player gold: player is NULL\n");
            continue;
        }

        int currentGold = player->purse;
        int remainingGold = getRemainingGold(game->gold);

        char* message = malloc(sizeof(char) * 50); // should be enough for message
        if (message == NULL)
        {
            fprintf(stderr, "Error updating player gold\n");
            return;
        }

        sprintf(message, "GOLD %d %d %d", 0, currentGold, remainingGold); // format message as per REQUIREMENTS spec
        message_send(player->address, message); // send gold message to player
        free(message); // free message
    }
}

static int getRemainingGold(gold_t* gold)
{
    if (gold == NULL)
    {
        fprintf(stderr, "Error getting remaining gold\n");
        return -1;
    }

    int totalRemaining = 0;
    for (int i = 0; i < gold->numPiles; i++)
    {
        totalRemaining += gold->piles[i];
    }

    return totalRemaining;
}

static bool isGameOver(game_t* game)
{
    if (game == NULL)
    {
        fprintf(stderr, "Error checking if game is over\n");
        return false;
    }

    int remainingGold = getRemainingGold(game->gold);
    if (remainingGold == 0)
    {
        return true;
    }

    return false;
}

static void handleKeyPress(game_t *game, const addr_t from, char key)
{
    // handle key press from player
    // check if key is valid (i.e., one of the arrow keys or 'Q')
    // if valid, move player in that direction
    // if 'Q', remove player from game

    if (game == NULL || key == NULL)
    {
        fprintf(stderr, "Error handling key press\n");
        return;
    }

    // get player from address (THIS SHOULD GO IN THE GAMESTATE MODULE @ARAL)
    player_t *player = NULL;
    for (int i = 0; i < game->playersSeen; i++)
    {
        player_t *player = game->players[i];
        if (message_eqAddr(player->address, from))
        {
            break; // here you would return the player (AS A METHOD IN GAMESTATE MDOULE @ARAL)
        }
    }

    if (player == NULL)
    {
        fprintf(stderr, "Error handling key press: could not find player with matching address in game\n");

        // since no player, must be spectator
        // handle spectator quit if key is Q
        if (key == 'Q')
        {
            handleQuit(game, from); // handleQuit should handle both player and spectator quitting
        }
        else
        {
            handleMalformedMessage(from, "Message Error: Invalid key message provided", key);
        }
        return;
    }

    // handle key press for player
    switch (key)
    {
    case 'h':
        // move player left, if possible
        // TODO: players might want to be updated to playerList, as outline in implementation spec
        player_move(player, game->grid, player->row - 1, player->col, game->players); // assuming row is x and col is y (TODO: THE NAMING SHOULD BE CHANGED TO X AND Y @NEAL)
        break;
    case 'l':
        // move player right, if possible
        player_move(player, game->grid, player->row + 1, player->col, game->players);
        break;
    case 'j':
        // move player up, if possible
        player_move(player, game->grid, player->row, player->col + 1, game->players);
        break;
    case 'k':
        // move player down, if possible
        player_move(player, game->grid, player->row, player->col - 1, game->players);
        break;
    case 'y':
        // move player up and left, if possible
        player_move(player, game->grid, player->row - 1, player->col + 1, game->players);
        break;
    case 'u':
        // move player up and right, if possible
        player_move(player, game->grid, player->row + 1, player->col + 1, game->players);
        break;
    case 'b':
        // move player down and left, if possible
        player_move(player, game->grid, player->row - 1, player->col - 1, game->players);
        break;
    case 'n':
        // move player down and right, if possible
        player_move(player, game->grid, player->row + 1, player->col - 1, game->players);
        break;
    // now the uppercase versions of above, which act as a toggle (i.e., while loop to move in that direction until player runs into wall)
    case 'H':
        while (grid_isRoom(game->grid, player->row - 1, player->col))
        { // pretty sure `grid_isRoom` is basically isEmpty (anything that isn't a wall)
            player_move(player, game->grid, player->row - 1, player->col, game->players);
        }
        break;
    case 'L':
        while (grid_isRoom(game->grid, player->row + 1, player->col))
        {
            player_move(player, game->grid, player->row + 1, player->col, game->players);
        }
        break;
    case 'J':
        while (grid_isRoom(game->grid, player->row, player->col + 1))
        {
            player_move(player, game->grid, player->row, player->col + 1, game->players);
        }
        break;
    case 'K':
        while (grid_isRoom(game->grid, player->row, player->col - 1))
        {
            player_move(player, game->grid, player->row, player->col - 1, game->players);
        }
        break;
    case 'Y':
        while (grid_isRoom(game->grid, player->row - 1, player->col + 1))
        {
            player_move(player, game->grid, player->row - 1, player->col + 1, game->players);
        }
        break;
    case 'U':
        while (grid_isRoom(game->grid, player->row + 1, player->col + 1))
        {
            player_move(player, game->grid, player->row + 1, player->col + 1, game->players);
        }
        break;
    case 'B':
        while (grid_isRoom(game->grid, player->row - 1, player->col - 1))
        {
            player_move(player, game->grid, player->row - 1, player->col - 1, game->players);
        }
        break;
    case 'N':
        while (grid_isRoom(game->grid, player->row + 1, player->col - 1))
        {
            player_move(player, game->grid, player->row + 1, player->col - 1, game->players);
        }
        break;
    case 'Q':
        // remove player from game
        handleQuit(game, from);
        break;
    default:
        break; // not sending error messages for invalid keys, just ignoring them (think of behavior in a normal video game)
    }
}

bool handleMessage(void *arg, const addr_t from, const char *message)
{
    if (arg == NULL || message == NULL)
    {
        fprintf(stderr, "\033[0;31mFATAL Error handling message: client or game does not exist (probably game, make sure it is initialized!) \033[0m\n");
        return true; // true ends the message loop, as defined in the message module (false does not)
    }

    game_t *game = (game_t *)arg; // cast arg to game state

    char **parsed = parseMessage(message);

    if (parsed == NULL)
    {
        fprintf(stderr, "Error handling message\n");
        return true; // true ends the message loop
    }

    // handle message based on parsed message
    // check if message is from a player or spectator
    // if player, check if they are joining, moving, or leaving
    // if spectator, check if they are joining or leaving

    // get args from parsed message
    int args = 0;
    while (parsed[args] != NULL)
    {
        args++;
    }

    if (args == 0)
    {
        // client error, send error message to client rather than stderr
        message_send(from, "Message Error: no arguments provided (malformed message)\n");
        // fprintf(stderr, "Message Error: no arguments provided (malformed message)\n");
        free((char *)message); // need to cast message to free it since it's a const char*
        freeMessage(parsed);   // should take care of everything?
        return false;          // not fatal, continue message loop
    }

    // switch for commands
    // switch (parsed[0]) { // first argument, command passed
    if (strcmp(parsed[0], "PLAY") == 0)
    {
        // handle request from new player to join game

        if (args >= 2)
        {
            // handleNewPlayer(game, from, parsed[1]);

            // do we need to reset/initialize a grid for the player? I think so since player_new needs a grid

            int rows = game->grid->rows;
            int cols = game->grid->cols;

            // BELOW CREATES PLAYER LETTER (UNCOMMENT IF PLAYER NEW DOESN'T HANDLE THIS, BUT INSTEAD TAKES ONE AS ARGUMENT @NEAL)
            // char playerLetter = 'A' + game->playersSeen; // get player letter based on number of players seen (alphabet is contiguous with ASCII character set, so we can do this)

            // make player name
            char realName[maxRealNameLength];
            strcpy(realName, parsed[1]); // copy player name from parsed message

            // generate random starting position for player (handled by player_new according to IMPLEMENTATION SPEC, so commented out for now)
            // point_t* start = grid_findEmptySpot(game->grid); // find empty spot on grid for player to start
            // int x = start->x;
            // int y = start->y;

            // create new player
            player_t *newPlayer = player_new(realName, from, game->grid); // didn't see playerLetter being created in the spec's pseudocode?

            if (newPlayer == NULL)
            {
                // delete grid created for player if we created a grid above
                // free(start); // free start point
                // free(realName); // free real name

                handleMalformedMessage(from, "Message Error: Invalid player message provided", message);
            }
            else
            {
                // add player to game state (THIS SHOULD BE A FUNCTION IN THE GAMESTATE MODULE @ARAL)
                // game->players[game->playersSeen] = newPlayer;
                // game->playersSeen++;

                // immediately send new client the size of the grid in the format "GRID rows cols"
                char *gridMessage = malloc(sizeof("GRID ") + sizeof(rows) + sizeof(cols) + 2); // +1 for null terminator, +1 for space
                if (gridMessage == NULL)
                {
                    fprintf(stderr, "Error sending grid size to new player\n");
                    return true; // true ends the message loop
                }

                strcpy(gridMessage, "GRID ");
                strcat(gridMessage, rows);
                strcat(gridMessage, " ");
                strcat(gridMessage, cols);

                message_send(from, gridMessage);
                sendOK(newPlayer); // send OK message to new player
            }
        }
        else
        {
            handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'PLAY'", message);
        }
    }
    else if (strcmp(parsed[0], "KEY") == 0)
    {
        // handle keystroke from existing player (move or quit)

        if (args != 2)
        {
            handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'KEY'", message);
        }
        else
        {
            handleKeyPress(game, from, parsed[1]);
        }
    }
    else if (strcmp(parsed[0], "SPECTATE") == 0)
    {
        // handle request from new spectator to join game

        if (args != 1)
        {
            handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'SPECTATE'", message);
        }
        else
        {
            handleNewSpectator(game, from);
        }
    }
    else if (strcmp(parsed[0], "QUIT") == 0)
    {
        // handle request from player or spectator to leave game

        if (args != 1)
        {
            handleMalformedMessage(from, "Message Error: Incorrect number of arguments for command, 'QUIT'", message);
        }
        else
        {
            handleQuit(game, from); // handleQuit should handle both player and spectator quitting
        }
    } // ignore any invalid command (beyond above commands, as per REQUIREMENTS spec)

    // free parsed message
    freeMessage(parsed);

    // update game state for spectator
    updateSpectatorDisplay(game, game->spectator);

    // send updated game state to all players and spectators
    int playersSeen = game->playersSeen;
    for (int i = 0; i < playersSeen; i++)
    {
        updatePlayerDisplay(game, game->players[i]);
    }

    // send updated golds to all players and spectators
    updateSpectatorGold(game); // maybe condense these two functions into one? thought they'd be more distinct, but maybe not after writing them
    updateAllPlayerGold(game);

    // check if game is over (i.e., all gold has been collected)
    if (isGameOver(game))
    {
        // handle game over (leaderboards, end game state, etc.)
        handleGameOver(game);
        return true; // true ends the message loop
    } else {
        return false; // false continues the message loop
    }
}

static void handleMalformedMessage(const addr_t from, char *error, char *message)
{
    // build error message (error + message)
    char *errorMessage = malloc(sizeof(error) + sizeof(message) + 4); // +1 for null terminator, +1 for new line + 1 for colon + 1 for tab
    if (errorMessage == NULL)
    {
        fprintf(stderr, "Error handling malformed message\n");
        return;
    }

    strcpy(errorMessage, error);
    strcat(errorMessage, ":\n\t");
    strcat(errorMessage, message);

    // send error message to client
    message_send(from, errorMessage);

    // free error message
    free(errorMessage);
}

int main(int argc, char *argv[])
{
    // call parseArgs
    int seed = -1;
    char map[100];

    int parseArgsReturnCode = parseArgs(argc, argv, &seed, map);

    if (parseArgsReturnCode != 0)
    {
        fprintf(stderr, "Exiting server with return code: %d\n", parseArgsReturnCode);
        return parseArgsReturnCode;
    }

    // generate random map using seed phrase if provided
    if (seed != -1)
    {
        srand(seed);
    }
    else
    {
        srand(getpid());
    }

    // load map file
    FILE *mapFile = fopen(map, "r"); // already checked if file is able to be read in parseArgs, but check again
    if (mapFile == NULL)
    {
        fprintf(stderr, "Error reading map file: %s\n", mapFile);
        return 4; // return 4 to indicate error reading map file
    }

    // initialize game state

    // close map file
    fclose(mapFile);

    // initialize network and announce port number (handled by message module)
    int port = message_init(stderr);
    if (port == 0)
    {
        fprintf(stderr, "Failed to initialize server. Possibly no open ports?\n");
        return 5; // return 5 to indicate error initializing server
    }

    // wait for messages from clients (start a loop listening for messages)
}