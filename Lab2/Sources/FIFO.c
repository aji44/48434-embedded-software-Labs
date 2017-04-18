/*! @file
 *
 *  @brief FIFO buffer function implementations
 *
 *  @author Corey Stidston and Menka Mehta
 *  @date 2017-04-18
 */
/*!
 * @addtogroup fifo_module FIFO module documentation
 * @{
 */

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


/*!
 * @}
 */
