/* ###################################################################
 **     Filename    : main.c
 **     Project     : Lab2
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
 ** @version 2.0
 ** @brief
 **         Main module.
 **         This module contains user's application code.
 */
/*!
 **  @addtogroup main_module main module documentation
 **  @{
 */
/* MODULE main */


// CPU module - contains low level hardware initialization routines
#include "Cpu.h"
#include "Events.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Flash.h"
#include "LEDs.h"
#include "packet.h"
#include "types.h"
#include "UART.h"
#include "RTC.h"
#include "PIT.h"
#include "FTM.h"

const static uint32_t BAUD_RATE = 115200;
const static uint32_t MODULE_CLOCK = CPU_BUS_CLK_HZ;

void FTM0Callback(void *arg);
void RTCCallback(void *arg);
void PITCallback(void *arg);


//TFTMChannel configuration for FTM timer
TFTMChannel packetTimer = {
		0, 															//channel
		CPU_MCGFF_CLK_HZ_CONFIG_0,			//delay count
		TIMER_FUNCTION_OUTPUT_COMPARE,	//timer function
		TIMER_OUTPUT_HIGH,							//ioType
		FTM0Callback,										//User function
		(void*) 0												//User arguments
};

void TowerInit(void)
{
	bool packetStatus = Packet_Init(BAUD_RATE, MODULE_CLOCK);
	bool flashStatus  = Flash_Init();
	bool ledStatus = LEDs_Init();
	bool PITStatus = PIT_Init(MODULE_CLOCK, &PITCallback, (void *)0);
	PIT_Set(500e6, false);

	bool FTMStatus = FTM_Init();
	FTM_Set(&packetTimer);

	bool RTCStatus = RTC_Init(&RTCCallback, (void *)0);

	if(packetStatus && flashStatus && ledStatus && PITStatus && RTCStatus && FTMStatus)
	{
		LEDs_On(LED_ORANGE);	//Tower was initialized correctly
	}
}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
	/* Write your local variable definition here */

	/*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
	/*** End of Processor Expert internal initialization.                    ***/

	/* Write your code here */
	__DI(); 		//Disable interrupts before peripheral module initialization
	TowerInit();	//Initialize tower peripheral modules
	__EI(); 		//Enable interrupts

	Packet_Put(TOWER_STARTUP_COMM, TOWER_STARTUP_PAR1, TOWER_STARTUP_PAR2, TOWER_STARTUP_PAR3);
	Packet_Put(TOWER_NUMBER_COMM, TOWER_NUMBER_PAR1, TowerNumber->s.Lo, TowerNumber->s.Hi);
	Packet_Put(TOWER_VERSION_COMM, TOWER_VERSION_V, TOWER_VERSION_MAJ, TOWER_VERSION_MIN);
	Packet_Put(TOWER_MODE_COMM, TOWER_MODE_PAR1, TowerMode->s.Lo, TowerMode->s.Hi);

	for (;;)
	{
		if (Packet_Get())	//Check if there is a packet in the retrieved data
		{
			LEDs_On(LED_BLUE);
			FTM_StartTimer(&packetTimer);
			Packet_Handle();
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

//RTCCallback function from RTC_ISR
void RTCCallback(void *arg)
{
	uint8_t h, m ,s;
	RTC_Get(&h, &m, &s);			//Get hours, mins, secs
	Packet_Put(0x0c, h, m, s);//Send to PC
	LEDs_Toggle(LED_YELLOW);	//Toggle Yellow LED
}

//PITCallback function from PIT_ISR
void PITCallback(void *arg)
{
	LEDs_Toggle(LED_GREEN);
}

//FTM0Callback function from FTM_ISR
void FTM0Callback(void *arg)
{
	LEDs_Off(LED_BLUE);
}

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
