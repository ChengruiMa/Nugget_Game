/**
 * @file spectator.h
 * 
 * @brief This file provides the interface for the spectator module.
 * 
 * The spectator module is responsible for managing plspectatorayer state and operations.
 * 
 * @author Team 11 (11xers)
 */

#ifndef SPECTATOR_H
#define SPECTATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// include modules
#include "message.h" // ../support/message.h

typedef struct spectator {
    addr_t address; // network address for sending messages
} spectator_t;


/**
 * Create a new spectator.
 * 
 * @param address: the spectator's network address
 * 
 * @return a pointer to the new spectator
 */
spectator_t* spectator_new(addr_t address);

/**
 * Delete a spectator.
 * 
 * @param spectator: the spectator to delete
 */
void spectator_delete(spectator_t* spectator);

/**
 * Send a message to a spectator.
 * 
 * @param spectator: the spectator to send the message to
 * @param message: the message to send
 */
void spectator_sendMessage(spectator_t* spectator, char* message);

#endif // SPECTATOR_H
