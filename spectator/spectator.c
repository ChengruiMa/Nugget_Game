/**
 * spectator.c — implementation of the spectator module
 * 
 * The spectator module is responsible for managing spectator state and operations.
 * 
 * @author Team 11 (11xers)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "spectator.h"
#include "../support/message.h"

/**
 * Create a new spectator.
 * 
 * @param address: the spectator's network address
 * 
 * @return a pointer to the new spectator
 */
spectator_t* spectator_new(addr_t address)
{
    spectator_t* spectator = malloc(sizeof(spectator_t));
    if (spectator == NULL)
    {
        fprintf(stderr, "Error creating new spectator\n");
        return NULL;
    }

    spectator->address = address;

    return spectator;
}

/**
 * Delete a spectator.
 * 
 * @param spectator: the spectator to delete
 */
void spectator_delete(spectator_t* spectator)
{
    if (spectator == NULL)
    {
        fprintf(stderr, "Error deleting spectator: spectator is NULL\n");
        return;
    }

    free(spectator);
}

/**
 * Send a message to a spectator.
 * 
 * @param spectator: the spectator to send the message to
 * @param message: the message to send
 */
void spectator_sendMessage(spectator_t* spectator, char* message)
{
    if (spectator == NULL || message == NULL)
    {
        fprintf(stderr, "Error sending message to spectator: spectator or message is NULL\n");
        return;
    }

    message_send(spectator->address, message);
}

/**
 * Get the network address of a spectator.
 * 
 * @param spectator: the spectator to get the address of
 * 
 * @return the network address of the spectator
 */
addr_t spectator_getAddress(spectator_t* spectator)
{
    if (spectator == NULL)
    {
        fprintf(stderr, "Error getting address of spectator: spectator is NULL\n");
        return (addr_t){0}; // Return an empty address — this is not a valid address, but it is a valid return value
    }

    return spectator->address;
}
