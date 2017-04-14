/*! @file
 *
 *  @brief Routines to implement packet encoding and decoding for the serial port.
 *
 *  This contains the functions for implementing the "Tower to PC Protocol" 5-byte packets.
 *
 *  @author PMcL
 *  @date 2015-07-23
 */
/*!
 ** @file packet.h
 ** @project Lab1
 ** @version 1.0
 ** @compiler GNU C Compiler
 ** @processor MK70FN1M0VMJ12
 ** @authors
 ** 	   Corey Stidston 98119910
 ** 	   Menka Mehta 12195032
 ** @brief
 **         packet module.
 **         This module contains the code for managing incoming and outgoing packets
 ** @date 29th March 2017
 */
/*!
 **  @addtogroup packet_module packet documentation
 **  @{
 */
/* MODULE packet */

#ifndef PACKET_H
#define PACKET_H

// new types
#include "types.h"

// Packet structure
extern uint8_t 	Packet_Command,		/*!< The packet's command */
				Packet_Parameter1, 	/*!< The packet's 1st parameter */
				Packet_Parameter2, 	/*!< The packet's 2nd parameter */
				Packet_Parameter3,	/*!< The packet's 3rd parameter */
				Packet_Checksum;	/*!< The packet's checksum */

// Acknowledgment bit mask
extern const uint8_t PACKET_ACK_MASK;

extern uint8_t towerNumberLsb, towerNumberMsb;

/*************************************************PC TO TOWER COMMANDS*************************************************/

//The PC will issue this command upon startup to retrieve the state of the Tower to update the interface application.
#define GET_STARTUP_VAL 0x04

//Get the version of the tower software
#define GET_VERSION 0x09

//Get or set the tower number
#define TOWER_NUMBER 0x0B

//Packet Parameter 1 for getting the tower number
#define TOWER_NUMBER_GET 1

//Packet Parameter 1 for setting the tower number
#define TOWER_NUMBER_SET 2

//Least significant byte of Student ID
#define SID 0x13A8

/*************************************************TOWER TO PC COMMANDS*************************************************/

/*
 * The tower will issue this command upon startup to allow the PC to update
 * the interface application and the Tower.
 * Typically setup data will also be sent from the Tower to the PC.
 */
#define TOWER_STARTUP_COMM 0x04
#define TOWER_STARTUP_PAR1 0x0
#define TOWER_STARTUP_PAR2 0x0
#define TOWER_STARTUP_PAR3 0x0

//Get the tower version command
#define TOWER_VERSION_COMM 0x09
#define TOWER_VERSION_V 'v'
#define TOWER_VERSION_MAJ 1
#define TOWER_VERSION_MIN 0

//Get the tower number command
#define TOWER_NUMBER_COMM 0x0B
#define TOWER_NUMBER_PAR1 1

/*************************************************PUBLIC FUNCTION DECLARATION*******************************************/

/*! @brief Initializes the packets by calling the initialization routines of the supporting software modules.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz
 *  @return bool - TRUE if the packet module was successfully initialized.
 */
bool Packet_Init(const uint32_t baudRate, const uint32_t moduleClk);

/*! @brief Attempts to get a packet from the received data.
 *
 *  @return bool - TRUE if a valid packet was received.
 */
bool Packet_Get(void);

/*! @brief Builds a packet and places it in the transmit FIFO buffer.
 *
 *  @return bool - TRUE if a valid packet was sent.
 */
bool Packet_Put(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3);

/*! @brief Handles a packet once it has been validated by Packet_Get
 *
 *  @return void
 */
void Packet_Handle(void);

#endif

/* END packet */
/*!
 ** @}
 */
