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
#include "../support/message.h" // ../support/message.h

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

/**
 * Get the network address of a spectator.
 * 
 * @param spectator: the spectator to get the address of
 * 
 * @return the network address of the spectator
 */
addr_t spectator_getAddress(spectator_t* spectator);

#endif // SPECTATOR_H
