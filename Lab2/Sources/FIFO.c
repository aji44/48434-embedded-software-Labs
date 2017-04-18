/* ###################################################################
 **     Filename    : FIFO.c
 **     Project     : Lab1
 **     Processor   : MK70FN1M0VMJ12
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2015-07-20, 13:27, # CodeGen: 0
 **     Abstract    :
 **         FIFO module.
 **         This module contains the code for the First in First Out
 **         Circular queue.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file fifo.c
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

/****************************************HEADER FILES****************************************************/
#include "FIFO.h"

/****************************************PUBLIC FUNCTION DEFINITION***************************************/

/*! @brief Initialize the FIFO before first use.
 *
 *  @param FIFO A pointer to the FIFO that needs initializing.
 *  @return void
 */
void FIFO_Init(TFIFO * const FIFO)
{
	FIFO->Start = 0;
	FIFO->End = 0;
	FIFO->NbBytes = 0;
}

/*! @brief Put one character into the FIFO.
 *
 *  @param FIFO A pointer to a FIFO struct where data is to be stored.
 *  @param data A byte of data to store in the FIFO buffer.
 *  @return bool - TRUE if data is successfully stored in the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
bool FIFO_Put(TFIFO * const FIFO, const uint8_t data)
{
	if (FIFO->NbBytes < FIFO_SIZE) //Check there is space in the buffer
	{
		FIFO->Buffer[FIFO->End] = data; 	//Put data into FIFO buffer
		FIFO->NbBytes++; 			//Number of bytes in FIFO increases
		FIFO->End++; 			//Next available position iterates
		if (FIFO->End == FIFO_SIZE-1) FIFO->End = 0; //Check whether the FIFO is full, reset
		return true;
	}

	else
	{
		return false; //Buffer Overflow
	}
}

/*! @brief Get one character from the FIFO.
 *
 *  @param FIFO A pointer to a FIFO struct with data to be retrieved.
 *  @param dataPtr A pointer to a memory location to place the retrieved byte.
 *  @return bool - TRUE if data is successfully retrieved from the FIFO.
 *  @note Assumes that FIFO_Init has been called.
 */
bool FIFO_Get(TFIFO * const FIFO, uint8_t * const dataPtr)
{
	if (FIFO->NbBytes > 0)
	{ //If there is data in the buffer..
		*dataPtr = FIFO->Buffer[FIFO->Start]; //Data = Array[Start]
		FIFO->Start++; //Moves to the next element in the array
		FIFO->NbBytes--;
		if (FIFO->Start == FIFO_SIZE-1) FIFO->Start = 0;
		return true;
	}
	return false;
}

/* END FIFO */
/*!
 ** @}
 */

