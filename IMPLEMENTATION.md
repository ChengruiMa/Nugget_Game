# CS50 Nuggets
## Implementation Spec
### Team name, term, year

> This **template** includes some gray text meant to explain how to use the template; delete all of them in your document!

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

> Update your plan for distributing the project work among your 3(4) team members.
> Who writes the client program, the server program, each module?
> Who is responsible for various aspects of testing, for documentation, etc?

## Player

### Data structures

The player module maintains information about each player in the game. Each player is represented by a player struct.

```c
typedef struct player {
  char playerLetter;           // A-Z identifying this player
  char* realName;             // player's real name
  int purse;                  // how much gold this player has collected
  int row;                    // current row position in the grid
  int col;                    // current column position in the grid
  addr_t address;             // network address for sending messages
  gridpoint_t* visiblePoints; // dynamically allocated array of visible points
  int numVisiblePoints;       // count of visible points
  grid_t* knownGrid;          // grid showing what this player has seen
} player_t;
```

We also maintain a list of all active players:

```c
typedef struct playerList {
  player_t** players;        // array of pointers to player structs
  int numPlayers;            // number of players currently active
  int maxPlayers;            // maximum allowed players (26)
} playerList_t;
```

### Definition of function prototypes

A function to create a new player structure.

```c
player_t* player_new(const char* name, const addr_t address, grid_t* masterGrid);
```

A function to update a player's position on the grid.

```c
bool player_move(player_t* player, grid_t* masterGrid, int newRow, int newCol, playerList_t* playerList);
```

A function to update the player's visibility based on current position.

```c
void player_updateVisibility(player_t* player, grid_t* masterGrid);
```

A function to add gold to a player's purse.

```c
void player_addGold(player_t* player, int goldAmount);
```

A function to create a display string for this player based on what they can see.

```c
char* player_createDisplayString(player_t* player, grid_t* masterGrid, gold_t* goldPiles);
```

A function to free all memory associated with a player.

```c
void player_delete(player_t* player);
```

### Detailed pseudo code

#### `player_new`:

    validate parameters
    allocate memory for new player_t
    initialize all fields to default values
    copy and store the player's name
    store the network address
    assign next available player letter
    allocate empty knownGrid
    randomly place player in an empty room spot on the masterGrid
    update player's visibility based on initial position
    return the new player structure

#### `player_move`:

    validate parameters
    check if the new position is a valid move
    if there's a player at the destination
        swap positions with that player
        update both players' visibility
        return true
    if there's gold at the destination
        collect the gold
        update player's purse
        mark the spot as empty
    update player's position to new coordinates
    update player's visibility based on new position
    update player's knownGrid with newly visible areas
    return true if move was successful, false otherwise

#### `player_updateVisibility`:

    validate parameters
    free previous visiblePoints array
    for each point on the grid
        check if it's visible from the player's position using visibility algorithm
        if visible, add to visiblePoints array
    update numVisiblePoints
    update player's knownGrid with newly visible areas

---

## Server

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()

---

## XYZ module

> For each module, repeat the same framework above.

### Data structures

### Definition of function prototypes

### Detailed pseudo code

---

## Testing plan

### unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

### integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### system testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.

## Client

### Data structures

The client program maintains a representation of the game state as known to the client.

```c
typedef struct client {
  char* hostname;          // server hostname
  int port;                // server port
  char* playerName;        // player's name or NULL if spectator
  bool isSpectator;        // whether this client is a spectator
  char playerLetter;       // if player, the assigned letter
  int purse;               // player's gold collected
  int remainingGold;       // gold remaining in the game
  addr_t serverAddress;    // server's network address
  grid_t* grid;           // local representation of the game grid
  WINDOW* gameWindow;      // ncurses window for game display
} client_t;
```

### Definition of function prototypes

A function to parse the command-line arguments and initialize the client.

```c
static int parseArgs(const int argc, char* argv[], client_t* client);
```

A function to initialize ncurses and set up the display.

```c
static void initializeDisplay(client_t* client);
```

A function to handle incoming messages from the server.

```c
static bool handleMessage(void* arg, const addr_t from, const char* message);
```

A function to handle user keystrokes.

```c
static bool handleInput(void* arg);
```

A function to update the status line at the top of the display.

```c
static void updateStatusLine(client_t* client, const char* message);
```

A function to update the display based on the current game state.

```c
static void updateDisplay(client_t* client, const char* displayString);
```

A function for graceful shutdown of the client.

```c
static void shutdownClient(client_t* client, const char* message);
```

### Detailed pseudo code

#### `parseArgs`:

    validate commandline parameters
    if fewer than 3 arguments or more than 4 arguments
        print usage message and return error
    extract hostname from argv[1]
    extract port from argv[2] and validate it's a number
    initialize message module
    if 4 arguments
        set isSpectator to false
        extract playerName from argv[3]
        send PLAY message to server
    else
        set isSpectator to true
        send SPECTATE message to server
    return success

#### `handleMessage`:

    validate parameters
    extract message type from first word
    if message is OK
        extract player letter and store in client
        return true
    if message is GRID
        extract grid dimensions
        initialize client's grid structure
        initialize ncurses display if not already done
        return true
    if message is GOLD
        extract collected gold, purse amount, and remaining gold
        update client stats
        update status line with gold information
        return true
    if message is DISPLAY
        extract display string
        update game display with new information
        refresh ncurses windows
        return true
    if message is QUIT
        end ncurses mode
        display quit message
        exit program
        return false
    if message is ERROR
        display error message on status line
        return true
    log unknown message type
    return true

#### `handleInput`:

    get keystroke from user
    if keystroke is EOF
        send KEY Q to server
        return false
    if valid game keystroke
        format and send KEY message to server
    else
        display "unknown keystroke" on status line
    return true

#### `updateStatusLine`:

    clear status line
    if client is spectator
        display "Spectator: X nuggets unclaimed."
    else
        display "Player X has Y nuggets (Z nuggets unclaimed)."
    if additional message provided
        display it on right side of status line
    refresh window

#### `initializeDisplay`:

    initialize ncurses
    check terminal size
    while terminal is too small for grid
        display message asking user to resize terminal
        wait for user to resize
    create game window
    set up colors if terminal supports them
    disable cursor
    enable non-blocking input
    enable immediate key input (no Enter required)
