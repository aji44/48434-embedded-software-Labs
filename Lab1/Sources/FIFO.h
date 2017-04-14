/*! @file
 *
 *  @brief Routines to implement a FIFO buffer.
 *
 *  This contains the structure and "methods" for accessing a byte-wide FIFO.
 *
 *  @author PMcL
 *  @date 2015-07-23
 */
/*!
 ** @file fifo.h
 ** @project Lab1
 ** @version 1.0
 ** @compiler GNU C Compiler
 ** @processor MK70FN1M0VMJ12
 ** @authors
 ** 	   Corey Stidston 98119910
 ** 	   Menka Mehta 12195032
 ** @brief
 **         FIFO module.
 **         This module contains the code for the First in First Out
 ** @date 29th March 2017
 */
/*!
 **  @addtogroup FIFO_module fifo documentation
 **  @{
 */
/* MODULE FIFO */

#ifndef FIFO_H
#define FIFO_H


#include "types.h"
#define FIFO_SIZE 256 // Number of bytes in a FIFO

/*!
 * @struct TFIFO
 */
typedef struct
{
	uint16_t Start;		/*!< The index of the position of the oldest data in the FIFO */
	uint16_t End; 		/*!< The index of the next available empty position in the FIFO */
	uint16_t volatile NbBytes;	/*!< The number of bytes currently stored in the FIFO */
	uint8_t Buffer[FIFO_SIZE];	/*!< The actual array of bytes to store the data */
} TFIFO;

/*************************************************PUBLIC FUNCTION DECLARATION*******************************************/

/*! @brief Initialize the FIFO before first use.
 *
 *  @param FIFO A pointer to the FIFO that needs initializing.
 *  @return void
 */
void FIFO_Init(TFIFO * const FIFO);

/*! @brief Put one character into the FIFO.
 *
 *  @param FIFO A pointer to a FIFO struct where data is to be stored.
 *  @param data A byte of data to store in the FIFO buffer.
 *  @return bool - TRUE if data is successfully stored in the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
bool FIFO_Put(TFIFO * const FIFO, const uint8_t data);

/*! @brief Get one character from the FIFO.
 *
 *  @param FIFO A pointer to a FIFO struct with data to be retrieved.
 *  @param dataPtr A pointer to a memory location to place the retrieved byte.
 *  @return bool - TRUE if data is successfully retrieved from the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
bool FIFO_Get(TFIFO * const FIFO, uint8_t * const dataPtr);

#endif

/* END FIFO */
/*!
 ** @}
 */
