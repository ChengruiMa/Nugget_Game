# CS50 Nuggets
## Design Spec
### Team 11, Winter, 2025

## User Interface

### Server

The server's only interface with the user is on the command-line; it must always have two arguments.

```
server map [seed]
```

where **map** is the filepath to the file that holds a string representation of the game map and **seed** (optional) is the seed integer used for the random number generator.

For example, if `map.txt` is a file in `../data`,

```console
user@plank:~$ ./server ../data/map.txt 50
```

### Client

The client's interface with the user is two-fold, both through the command-line: one when the user starts the client and another when the user interacts with the client.

The client must always have two arguments.

```
client hostname port [playername]
```

where **hostname** is the IP address of the server, **port** is the port number of the server, and **playername** (optional) is the name of the player.

For example, if the server is running on `127.0.0.1` and port `8000`,

```console
user@plank:~$ ./client 127.0.0.1 8000 zac
```

Once the client is running, the user can interact with the client through the command-line. The client must support the following commands, passed via keystrokes:

- `Q`: quit the game
- `h`: move left, if possible
- `l`: move right, if possible
- `j`: move down, if possible
- `k`: move up, if possible
- `y`: move diagonally up and left, if possible
- `u`: move diagonally up and right, if possible
- `b`: move diagonally down and left, if possible
- `n`: move diagonally down and right, if possible

where 'if possible' means the adjacent gridpoint in the given direction is either an empty spot, a pile of gold, or another player. For each move command, the capitalized keystroke of itself will continue moving the player in that direction until no longer possible (essentially a toggle for each move).

Note: the move commands are limited to *players*; spectators are view-only and cannot move, with their only valid keystroke being `Q`.

## Inputs and Outputs

### Server

1. **Command-Line Arguments**  
   - **Usage:** `./server mapfile [seed]`  
   - **Purpose:**
     - `mapfile` (required): Path to a map file describing the game layout.
     - `seed` (optional): Integer seed for randomizing gold placement (if omitted, a pseudo-random seed is used).
   - **Validation:**  
     The server verifies that the map file is valid (no disallowed characters, rectangular shape, etc.). If invalid, the server reports an error and exits.

2. **Incoming Messages (from Client)**
   - `PLAY <name>`: Request from a new player to join the game.
   - `KEY <char>`: Keystroke from an existing player.
   - `SPECTATE`: Request from a user to join as a spectator.
   - **Malformed or unknown**: Server handles by ignoring or sending a `QUIT` message.

3. **Outgoing Messages (to Client)**
   - `OK`: Confirms acceptance of a new player or spectator.
   - `GRID <rows> <cols>`: Dimensions of the game grid.
   - `GOLD <p1> <p2> <p3>`: Updates on collected gold.
   - `DISPLAY <mapString>`: Updated view of the game map.
   - `QUIT <reason>`: Notification that the game is ending.

4. **Server Logging (Optional)**
   - Diagnostic logs may appear on `stderr` or a log file for debugging (e.g., connection events, gold pickups, errors).

---

### Client

1. **Command-Line Arguments**  
   - **Usage:** `./client hostname port [playerName]`  
   - **Purpose:**
     - `hostname`: The server hostname or IP.
     - `port`: The port on which the server is listening.
     - `playerName` (optional): The name used when joining. If omitted, the client may prompt the user.

2. **User Keystrokes**  
   - **Movement keys**: Immediately sent to the server as `KEY <char>`, where `char` is a valid keystroke described above.
   - **Q**: Client quits (sends `KEY Q` to the server).
   - **Window resizing**: If using `ncurses`, the client may detect when the window is too small and ask the user to resize.

3. **Incoming Messages (from Server)**
   - May include `GRID`, `DISPLAY`, `GOLD`, `QUIT`, etc.
   - The client parses these and updates the user’s display.

4. **Client Output**
   - Real-time visual map display via `ncurses`.
   - Potential debugging or error messages on `stderr`.

---

## Functional Decomposition into Modules

### Common components
1. Grid module

Purpose — to represent and manage the game grid

Key functions:

- `grid_new()`: Create a new grid from map file
- `grid_delete()`: Free memory associated with grid
- `grid_load()`: Load map from file into grid
- `grid_get()`: Get character at specific position
- `grid_set()`: Set character at specific position
- `grid_isVisible()`: Determine if one point is visible from another
- `grid_toString()`: Convert grid to string representation
- `grid_getSize()`: Get dimensions of grid



2. Message module (provided)

Purpose — to handle network communication

Will use the provided functions to send/receive messages and handle network events

### Server components

1. Main server component

Purpose — to initialize server and manage game loop

Key functions:

- `parseArgs()`: Parse command-line arguments
- `initializeGame()`: Set up game state
- `handleMessage()`: Dispatch incoming messages to appropriate handlers
- `gameOver()`: Handle end-game logic and summary



2. Game State Module

Purpose — to manage the overall game state

Key functions:

- `game_new()`: Initialize game state
- `game_delete()`: Clean up game state
- `game_addPlayer()`: Add a new player to the game
- `game_removePlayer()`: Remove a player
- `game_setSpectator()`: Set/replace the spectator
- `game_movePlayer()`: Process player movement
- `game_collectGold()`: Handle gold collection logic
- `game_isOver()`: Check if all gold has been collected
- `game_getSummary()`: Generate end-game summary

3. Player module

Purpose — to manage player information

Key functions:

- `player_new()`: Create a new player
- `player_delete()`: Remove a player
- `player_getPosition()`: Get current position
- `player_setPosition()`: Set position
- `player_addGold()`: Add gold to player's purse
- `player_getName()`: Get player's name
- `player_getLetter()`: Get player's letter

4. Gold module

Purpose — to manage gold piles

Key functions:

- `gold_initialize()`: Create and distribute gold piles
- `gold_collect()`: Handle collection of gold pile
- `gold_getRemainingCount()`: Get count of uncollected gold
- `gold_getPileCount()`: Get number of piles

5. Visibility module

Purpose — to calculate visibility between points

Key functions:

- `visibility_calculate()`: Determine visible spots from position
- `visibility_isSpotVisible()`: Check if spot is visible


6. Network handler module

Purpose — to process network messages

Key functions:

- `handlePlayMessage()`: Process PLAY message
- `handleSpectateMessage()`: Process SPECTATE message
- `handleKeyMessage()`: Process KEY message
- `sendDisplayUpdate()`: Send updated display to clients
- `sendGoldUpdate()`: Send gold updates to clients
- `sendQuitMessage()`: Send QUIT message


### Client components

1. Main client component

Purpose — to initialize client and manage input/display loop

Key functions:

- `parseArgs()`: Parse command-line arguments
- `initializeClient()`: Set up client state
- `handleMessage()`: Process incoming messages
- `handleInput()`: Handle keyboard input
- `cleanup()`: Perform exit cleanup

2. Display module

Purpose — to manage ncurses display

Key functions:

- `display_initialize()`: Set up ncurses
- `display_cleanup()`: Clean up ncurses
- `display_grid()`: Display the game grid
- `display_status()`: Update status line
- `display_checkSize()`: Check if window is large enough


3. Client state module

Purpose — to maintain client's view of game state

Key functions:

- `client_new()`: Create new client state
- `client_delete()`: Clean up client state
- `client_updateGrid()`: Update grid from server message
- `client_updateGold()`: Update gold information
- `client_handleQuit()`: Process quit message


4. Network handler module

Purpose — to manage network communication

Key functions:

- `handleOkMessage()`: Process OK message
- `handleGridMessage()`: Process GRID message
- `handleGoldMessage()`: Process GOLD message
- `handleDisplayMessage()`: Process DISPLAY message
- `handleQuitMessage()`: Process QUIT message
- `handleErrorMessage()`: Process ERROR message
- `sendPlayMessage()`: Send PLAY message
- `sendSpectateMessage()`: Send SPECTATE message
- `sendKeyMessage()`: Send KEY message

## Pseudo Code for Logic/Algorithmic Flow

## Server 

### Initialization

1. Parse command-line arguments and validate them
2. Initialize the random number generator with the given seed or process ID
3. Load the map file with the map module
4. Initialize the game state by randomly placing gold piles
5. Initialize the network and announce the port number
6. Enter the main loop

### Main Loop

1. Wait for messages from clients
2. Parse and validate incoming messages
3. Process messages according to their type:
   - PLAY: Add a new player to the game
   - SPECTATE: Add or replace the spectator
   - KEY: Process a player's keystroke by Player Movement
4. Update the game state if needed
5. Send updated information to all clients
6. Check if the game is over (all gold collected)
7. If game is over, send QUIT messages to all clients with the final summary
8. Clean up and exit

### Player Movement

1. Validate the requested movement direction
2. Calculate the new position based on the direction
3. Check if the new position is valid (empty spot, gold pile, or another player)
4. If the new position contains gold, collect it
5. If the new position contains another player, swap positions
6. Update the player's position
7. Update the player's visibility and memory
8. Send updated DISPLAY messages to all clients

### Map Module's Pseudocode and High-Level Logic
    Function loadMap:
        Read map file line by line
        Determine dimensions (rows and columns)
        Create internal representation of the map
        Validate map structure (enough room for players and gold)
        Return the map structure
    
    Function isSpotType:
        Determine if a given position is:
            - A room spot (can hold players or gold)
            - A passage spot (can hold players)
            - A boundary (horizontal, vertical, or corner)
            - Outside the map boundaries
        Return the appropriate boolean result
    
    Function findEmptySpot:
        Randomly select positions on the map
        Check if the position is an empty room spot
        Return the first valid empty position found

    Function visibilityCalculation:
        For each gridpoint on the map:
            Draw a line from the player's position to the gridpoint
            Check if the line passes through any blocking gridpoints
            If not, mark the gridpoint as visible to the player
            Update the player's visibility and memory maps

## Client

### Initialization

1. Parse command-line arguments and validate them
2. Initialize the display
3. Initialize the network
4. Send PLAY or SPECTATE message to the server
5. Wait for initial messages from the server (OK, GRID, GOLD, DISPLAY)
6. If allowed in, initialize and update the display
7. Enter the main loop

### Main Loop

1. Check for user input
2. If input is valid (if spectator, then only Q), send KEY message to server
3. Check for messages from server
4. If message is valid, process it according to its type (GRID, GOLD, DISPLAY, QUIT, ERROR), as described below
5. Update the display if needed
6. Repeat until told to quit

### Handling Server Messages

1. Parse the message to determine its type
2. For GRID messages: store the grid dimensions
3. For GOLD messages: update gold information and display status
4. For DISPLAY messages: update the grid display
5. For QUIT messages: display the summary, clean up, and exit
6. For ERROR messages: display the error on the status line

## Major Data Structures

### 1. Game State (`game_t`)

**Description**  
A single structure holding top-level information about the Nuggets game.

**Key Fields**
- `grid`: Reference to the map layout (see `grid_t`).
- `goldRemaining`: The total amount of uncollected gold in the game.
- `players[]`: Array or list of active player states.
- `spectator`: Pointer or reference to the spectator’s data (if any).
- `nPlayers`: Current number of active players.

<pre>
typedef struct game {
    struct grid* grid;         // Pointer to the game map structure
    int goldRemaining;         // Uncollected gold
    player_t* players;         // Dynamic array of player_t
    spectator_t* spectator;    // Single spectator data
    int nPlayers;              // Count of active players
} game_t;
</pre>

### 2. Grid / Map Representation (`grid_t`)

**Description**  
Holds the layout of tunnels, walls, and rooms with a 2D array or array-of-strings.

**Usage**  
- Server checks valid movement, gold location, etc.  
- Each cell can be updated to reflect gold status or player presence.

<pre>
typedef struct grid {
    int nrows;       // number of rows
    int ncols;       // number of columns
    char** cells;    // 2D array of characters
} grid_t;

</pre>

### 3. Player State (`player_t`)

**Description**  
Tracks the state of a single player: their position, gold collected, and network address.

**Key Fields**

<pre>
typedef struct player {
    char letter;       // 'A'...'Z'
    int row;           // current row position
    int col;           // current column position
    int gold;          // total gold collected
    addr_t address;    // network address for sending updates
} player_t;
</pre>

### 4. Spectator State (`spectator_t`)

**Description**  
Represents the single spectator’s network address and connection status.

<pre>
typedef struct spectator {
    addr_t address;   // network address
    bool active;      // whether spectator is currently active
} spectator_t;
</pre>

## Testing Plan

The complete game can be tested by initializing a server and multiple clients, with each client connecting to the server and interacting with the game. Users can then play the game, ensuring everything is working as expected. Beyond this, the game should then be tested by a variety of unit and integration tests.

### Unit Testing

- Maps
    - Test the map module with a variety of maps, including edge cases (e.g. empty maps, maps with only walls, maps with only gold, etc.) to ensure rendering is as expected.
    - Test the map module with a variety of seed phrases, ensuring the random generation of maps is consistent (i.e., the same seed phrase should always generate the same map).
- Players
    - Test the player module with a variety of player names, ensuring the player is created with the correct name.
    - Test the player module with a variety of different movements and keystrokes, ensuring the player always moves as expected (without exceptions or leaving the map).
    - Test the player module with different `DISPLAY` and visibility settings.
- Spectators
    - Test the spectator module with keystokes beyond the allowed `Q` to ensure the spectator cannot move.
    - Test the spectator module's `DISPLAY` and ensure it *knows all* and *sees all* — the map rendering must not be the same as the player's view.
- Message Communication
    - Ensure messages being sent between the server and client are not malformed by outputting the messages to the console or some log file for inspection (comparison with expected format).
    - Ensure messages are being sent and received in the correct order by the correct clients.
- Message Parsing
    - Ensure messages are being parsed and interpreted correctly by the server and client. This includes ensuring the server can parse messages from multiple clients and the client can parse messages from the server.
    - Ensure malformed messages do not crash the server or client.

### Integration Testing

- Test `server` with a variety of invalid arguments:
    - No arguments,
    - Three or more arguments,
    - An invalid *map* file (i.e., a nonexistent path),
    - An invalid *seed* (i.e., a non-integer).
- Test `client` with a variety of invalid arguments:
    - No arguments,
    - One argument,
    - Four or more arguments,
    - An invalid *hostname* (i.e., a malformed IP address),
    - An invalid *port* (e.g., a non-integer, or one that is not exposed).
    - An invalid *playername* (e.g., a name that is too long, or just whitespace characters).
- Test aspects of the game logic, such as:
    - Clients being able to connect to the server and interact with the game; this expects that each player and spectator is receiving the proper messages, and that the client module can properly display the map and game state.
    - The game ending when all gold nuggets are collected, which causes the program to exit gracefully.
- Test for memory leaks and resource management:
    - Run `valgrind` on the server and client processes, as well as each module in isolation.
- Test for compile-time errors by running the program with the provided `Makefile` and ensuring no errors are thrown after `make all` is run at the top-level directory (assuming `make clean` was run beforehand).

## Division of Labor

### Zac: Server Core & Network Communication

- Implement the server's main functionality
- Parse command-line arguments and map files
- Set up and manage network communication using the message module
- Implement the message handling for server-client communication
- Handle spectator connections and replacements

### Martin: Game Logic, Map & Visibility

- Design and implement the grid data structure
- Implement player and gold placement logic
- Design game state representation
- Implement player movement and collision logic
- Implement visibility algorithms
- Manage gold collection and game end conditions

### Neal: Client Core & Display

- Implement the client's main functionality
- Parse command-line arguments
- Set up and manage network communication on the client side
- Implement ncurses display management
- Handle window sizing and user interface elements
- Implement the message handling for client-server communication

### Aral: Testing & Integration

- Create test maps and test cases
- Implement unit tests for server and client components
- Create integration tests for full system testing
- Manage documentation (DESIGN.md, IMPLEMENTATION.md)
- Create custom map designs
- Handle build system (Makefiles) and ensure clean compilation