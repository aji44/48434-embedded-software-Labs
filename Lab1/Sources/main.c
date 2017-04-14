/* ###################################################################
 **     Filename    : main.c
 **     Project     : Lab1
 **     Processor   : MK70FN1M0VMJ12
 **     Version     : Driver 01.01
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2015-07-20, 13:27, # CodeGen: 0
 **     Abstract    :
 **         Main module.
 **         This module contains user's application code.
 **     Settings    :
 **     Contents    :
 **         No public methods
 **
 ** ###################################################################*/
/*!
 ** @file main.c
 ** @project Lab1
 ** @version 1.0
 ** @compiler GNU C Compiler
 ** @processor MK70FN1M0VMJ12
 ** @authors
 ** 	   Corey Stidston 98119910
 ** 	   Menka Mehta 12195032
 ** @brief
 **         Main module for Lab1.
 ** @date 29th March 2017
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */

/****************************************HEADER FILES****************************************************/

// CPU module - contains low level hardware initialization routines
#include "Cpu.h"
#include "Events.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "packet.h"
#include "UART.h"

/****************************************MAIN MODULE****************************************************/

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */
	const uint32_t baudRate = 38400;
	const uint32_t moduleClock = CPU_BUS_CLK_HZ;

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */

	//Initialize the packet module and consequently the UART
	bool valid = Packet_Init(BAUDRATE, MODULE_CLOCK);

	if (valid)
	{
		//Startup packets
		Packet_Put(TOWER_STARTUP_COMM, TOWER_STARTUP_PAR1, TOWER_STARTUP_PAR2, TOWER_STARTUP_PAR3);

		//Tower Version Packet
		Packet_Put(TOWER_VERSION_COMM, TOWER_VERSION_V, TOWER_VERSION_MAJ, TOWER_VERSION_MIN);

		//Tower Number Packet
		Packet_Put(TOWER_NUMBER_COMM, TOWER_NUMBER_PAR1, towerNumberLsb, towerNumberMsb);
	}
	for (;;)
	{
		if (valid)
		{
			//Check if there is a packet in the retrieved data
			if (Packet_Get())
			{
				Packet_Handle();
			}

			//Checks whether the RDRF or TDRE flags are set and retrieves/transmits data
			UART_Poll();
		}
	}

	/*** Don't write any code pass this line, or it will be deleted during code generation. ***/
	/*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
#ifdef PEX_RTOS_START
	PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
#endif
	/*** End of RTOS startup code.  ***/
	/*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
	for(;;){}
	/*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.5 [05.21]
 **     for the Freescale Kinetis series of microcontrollers.
 **
 ** ###################################################################
 */
