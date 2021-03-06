/*! @file
 *
 *  @brief I/O routines for UART communications on the TWR-K70F120M.
 *
 *  This contains the functions for operating the UART (serial port).
 *
 *  @author Corey Stidston & Menka Mehta
 *  @date 2017-04-18
 */
/*!
 * @addtogroup UART_module UART documentation
 * @{
 */
/* MODULE UART */

/****************************************HEADER FILES****************************************************/
#include "FIFO.h"
#include "types.h"
#include "MK70F12.h"

/****************************************GLOBAL VARS*****************************************************/
TFIFO RxFIFO;
TFIFO TxFIFO;

/****************************************PUBLIC FUNCTION DEFINITION***************************************/

/*! @brief Sets up the UART interface before first use.
 *
 *  @param baudRate The desired baud rate in bits/sec.
 *  @param moduleClk The module clock rate in Hz
 *  @return bool - TRUE if the UART was successfully initialized.
 */
bool UART_Init(const uint32_t baudRate, const uint32_t moduleClk)
{
	uint8_t brfa;						//Baud rate fine adjustment variable
	uint16union_t sbr;					//Variable used to hold baud rate value

	if (baudRate == 0) return false;		//Check whether the BaudRate is not set to zero to avoid a runtime error

	SIM_SCGC4 |= SIM_SCGC4_UART2_MASK; 	//Enable UART module in SIM_SCGC4
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK; 	//Enable Pin routing for Port E

	PORTE_PCR16 |= PORT_PCR_MUX(3);		//Set Pin16 to MUX mode Alternate 3
	PORTE_PCR17 |= PORT_PCR_MUX(3);		//Set Pin17 to MUX mode Alternate 3

	UART2_C2 &= ~UART_C2_TE_MASK;		//Disable UART transmitter
	UART2_C2 &= ~UART_C2_RE_MASK;		//Disable UART receiver

	FIFO_Init(&RxFIFO);					//Initialize the Receiving FIFO for usage
	FIFO_Init(&TxFIFO);					//Initialize the Transmitting FIFO for usage

	sbr.l = moduleClk / (baudRate * 16);//Set the baud rate

	if (sbr.l > 0x1FFF) return false;	//Check that the BaudRate is valid

	UART2_BDH = sbr.s.Hi;				//Set the high part of the BaudRate
	UART2_BDL = sbr.s.Lo;				//Set the low part of the BaudRate

	brfa = (uint8_t) ((moduleClk*2) / baudRate) % 32; //Calculate the BaudRate Find Adjust Value
	UART2_C4 |= UART_C4_BRFA_MASK;		//Prepare the register
	UART2_C4 &= brfa;					//Set the BaudRate Fine Adjust value

	UART2_C2 |= UART_C2_TE_MASK;		//Enables UART transmitter
	UART2_C2 |= UART_C2_RE_MASK;		//Enables UART receiver

	return true;
}

/*! @brief Get a character from the receive FIFO if it is not empty.
 *
 *  @param dataPtr A pointer to memory to store the retrieved byte.
 *  @return bool - TRUE if the receive FIFO returned a character.
 *  @note Assumes that UART_Init has been called.
 */
bool UART_InChar(uint8_t * const dataPtr)
{
	//Get the data stored in RxFIFO and store it within the address given by dataPtr
	return FIFO_Get(&RxFIFO, dataPtr);
}

/*! @brief Put a byte in the transmit FIFO if it is not full.
 *
 *  @param data The byte to be placed in the transmit FIFO.
 *  @return bool - TRUE if the data was placed in the transmit FIFO.
 *  @note Assumes that UART_Init has been called.
 */
bool UART_OutChar(const uint8_t data)
{
	//Place the value stored in data into the TxFIFO
	return FIFO_Put(&TxFIFO, data);
}

/*! @brief Poll the UART status register to try and receive and/or transmit one character.
 *
 *  @return void
 *  @note Assumes that UART_Init has been called.
 */
void UART_Poll(void)
{
	//If TDRE is set, there is data to transmit, place this in the data register
	if (UART2_S1 & UART_S1_TDRE_MASK)
	{
		FIFO_Get(&TxFIFO, (uint8_t *) &UART2_D);
	}

	//If RDRF is set, there is data to receive, retrieve this from the data register
	if (UART2_S1 & UART_S1_RDRF_MASK)
	{
		FIFO_Put(&RxFIFO, UART2_D);
	}
}


/*!
 * @}
 */
