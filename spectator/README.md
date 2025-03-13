# Spectator Module - Nuggets

This directory contains the implementation for the spectator module for the Nuggets game.

## Overview

The spectator module manages the functionality about the spectators - users who can watch the game but do not actively participate as players. Only one spectator can be connected at a time.

## Files

* `spectator.c` - Implementation of the spectator module
* `spectator.h` - Header file with function prototypes and type definitions

## Data Structure

The `spectator_t` structure contains:
* Network address for communication with the spectator client

```
typedef struct spectator {
    addr_t address;     // network address for sending messages
} spectator_t;
```

## Functions

The module provides the following functions:
* `spectator_new()` - Create a new spectator with the provided network address
* `spectator_delete()` - Free memory allocated for a spectator
* `spectator_sendMessage()` - Send a network message to the spectator

## Dependencies

The spectator module depends on:
* `../support/` - For network message handling

## Usage

The spectator module is used by the game module to:
* Create and manage spectator connections
* Replace an existing spectator when a new one connects
* Send game updates to the spectator client
* Provide a complete view of the game map

To build, simply
```
make
```