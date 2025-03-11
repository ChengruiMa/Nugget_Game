/*
 * test_server.c
 *
 * Updated unit tests for the new server.c.
 *
 * This file tests:
 *   - parseArgs (valid, usage error, invalid seed)
 *   - parseMessage and freeMessage
 *   - sendOK (that an OK message is constructed and sent)
 *   - handleMessage for the "QUIT" command (which should trigger handleQuit)
 *   - handleMessage for an unknown command (which should do nothing)
 *
 * Note:
 *   For testing purposes, we assume that functions formerly declared static
 *   (parseArgs, parseMessage, freeMessage, sendOK, handleMessage) are now exposed.
 *   Also, we override message_send with a dummy implementation to capture sent messages.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

/* --- Dummy Definitions & Stubs --- */

/* Define a dummy addr_t type. In real code this may be a struct or pointer. */
typedef char* addr_t;

/* Dummy implementation of message_send.
   Instead of sending data over the network, it copies the message into a global buffer. */
char dummy_sent_message[256];
addr_t dummy_sent_addr = NULL;
int message_send(addr_t addr, const char* msg) {
    if (msg == NULL) return 0;
    strncpy(dummy_sent_message, msg, sizeof(dummy_sent_message) - 1);
    dummy_sent_message[sizeof(dummy_sent_message) - 1] = '\0';
    dummy_sent_addr = addr;
    return 1; // simulate success
}

/* Minimal dummy structure for player_t needed by sendOK and handleMessage */
typedef struct player {
    char letter;   // assigned letter
    addr_t address;
    int row;
    int col;
} player_t;

/* Dummy spectator_t and gold_t (not used in these tests) */
typedef struct spectator {
    addr_t address;
} spectator_t;

typedef struct gold {
    int* counter;
    int piles;
    int index;
} gold_t;

/* 
 * Dummy grid structure.
 * Note: server.c expects game->grid->rows and ->cols.
 * In our map module, grid_t might have nrows and ncols.
 * For testing, we define a dummy grid_t that matches the server's expectation.
 */
typedef struct dummy_grid {
    int rows;
    int cols;
} dummy_grid_t;
typedef dummy_grid_t grid_t;

/* --- Dummy implementations for functions called in handleMessage --- */
void handleNewSpectator(void* game, addr_t from) {
    // For testing, simply simulate by sending a dummy message.
    message_send(from, "SPECTATE OK");
}
void handleQuit(void* game, addr_t from) {
    // For testing, simulate quit by sending a dummy message.
    message_send(from, "QUIT OK");
}
void handleKeyPress(void* game, addr_t from, char* keyStr) {
    // For testing, assume keyStr is a valid string.
    if (keyStr == NULL || strlen(keyStr) == 0) return;
    char key = keyStr[0];
    char buf[50];
    snprintf(buf, sizeof(buf), "KEY %c HANDLED", key);
    message_send(from, buf);
}
void handleMalformedMessage(addr_t from, char* error, char* msg) {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s:\n\t%s", error, msg);
    message_send(from, buf);
}

/* --- Extern declarations for functions from server.c ---
   We assume these are now accessible (i.e. no longer static). */
extern int parseArgs(int argc, char* argv[], int* storedSeed, char* map);
extern char** parseMessage(char* message);
extern void freeMessage(char** message);
extern void sendOK(player_t* player);
extern bool handleMessage(void* arg, const addr_t from, const char* message);

/* --- Define a minimal game_t structure as expected by server.c --- */
typedef struct game {
    grid_t* grid;           // dummy grid with fields rows and cols
    player_t** players;     // array of players (not used in these tests)
    spectator_t* spectator;
    int playersSeen;
    gold_t* gold;
} game_t;

/* --- Test Functions --- */

void test_parseArgs_valid() {
    // Create a temporary map file.
    const char* tempMapFile = "temp_map.txt";
    FILE* fp = fopen(tempMapFile, "w");
    assert(fp != NULL);
    fprintf(fp, "dummy map content\n");
    fclose(fp);

    char* argv[] = { "server", (char*)tempMapFile, "12345" };
    int seed = -1;
    char map[100] = {0};
    int ret = parseArgs(3, argv, &seed, map);
    assert(ret == 0);
    assert(seed == 12345);
    assert(strcmp(map, tempMapFile) == 0);

    remove(tempMapFile);
    printf("test_parseArgs_valid passed\n");
}

void test_parseArgs_invalid_usage() {
    char* argv[] = { "server" };
    int seed = -1;
    char map[100] = {0};
    int ret = parseArgs(1, argv, &seed, map);
    assert(ret == 1);
    printf("test_parseArgs_invalid_usage passed\n");
}

void test_parseArgs_invalid_seed() {
    // Create a temporary map file.
    const char* tempMapFile = "temp_map.txt";
    FILE* fp = fopen(tempMapFile, "w");
    assert(fp != NULL);
    fprintf(fp, "dummy map\n");
    fclose(fp);

    char* argv[] = { "server", (char*)tempMapFile, "-5" };
    int seed = -1;
    char map[100] = {0};
    int ret = parseArgs(3, argv, &seed, map);
    assert(ret == 3);
    remove(tempMapFile);
    printf("test_parseArgs_invalid_seed passed\n");
}

void test_parseMessage_and_freeMessage() {
    char msg[] = "PLAY JohnDoe";
    char** tokens = parseMessage(msg);
    assert(tokens != NULL);
    // Expect tokens[0] == "PLAY", tokens[1] == "JohnDoe"
    assert(strcmp(tokens[0], "PLAY") == 0);
    assert(strcmp(tokens[1], "JohnDoe") == 0);
    freeMessage(tokens);
    printf("test_parseMessage_and_freeMessage passed\n");
}

void test_sendOK() {
    player_t dummyPlayer;
    dummyPlayer.letter = 'A';
    dummyPlayer.address = "dummy_addr";
    memset(dummy_sent_message, 0, sizeof(dummy_sent_message));
    sendOK(&dummyPlayer);
    // Verify that the sent message starts with "OK " and contains 'A'
    assert(strncmp(dummy_sent_message, "OK ", 3) == 0);
    assert(strchr(dummy_sent_message, 'A') != NULL);
    printf("test_sendOK passed\n");
}

void test_handleMessage_quit() {
    // Prepare a dummy game state with a dummy grid.
    game_t game;
    dummy_grid_t dgrid;
    dgrid.rows = 10;
    dgrid.cols = 20;
    game.grid = (grid_t*)&dgrid;
    game.playersSeen = 0;
    game.players = NULL;
    game.spectator = NULL;
    game.gold = NULL;

    memset(dummy_sent_message, 0, sizeof(dummy_sent_message));
    // Send a "QUIT" command.
    bool cont = handleMessage(&game, "dummy_addr", "QUIT");
    // Expect that handleQuit is called and dummy_sent_message contains "QUIT OK"
    assert(strncmp(dummy_sent_message, "QUIT OK", 7) == 0);
    printf("test_handleMessage_quit passed\n");
}

void test_handleMessage_unknown() {
    game_t game;
    dummy_grid_t dgrid;
    dgrid.rows = 10;
    dgrid.cols = 20;
    game.grid = (grid_t*)&dgrid;
    game.playersSeen = 0;
    game.players = NULL;
    game.spectator = NULL;
    game.gold = NULL;

    memset(dummy_sent_message, 0, sizeof(dummy_sent_message));
    // Send an unknown command "FOO".
    bool cont = handleMessage(&game, "dummy_addr", "FOO");
    // For unknown commands, the code does nothing. Check that dummy_sent_message remains empty.
    assert(strlen(dummy_sent_message) == 0);
    printf("test_handleMessage_unknown passed\n");
}

int main() {
    test_parseArgs_valid();
    test_parseArgs_invalid_usage();
    test_parseArgs_invalid_seed();
    test_parseMessage_and_freeMessage();
    test_sendOK();
    test_handleMessage_quit();
    test_handleMessage_unknown();
    printf("All updated server tests passed!\n");
    return 0;
}
