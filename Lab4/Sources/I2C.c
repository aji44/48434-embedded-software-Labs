/*! @file
 *
 *  @brief I/O routines for the K70 I2C interface.
 *
 *  This contains the functions for operating the I2C (inter-integrated circuit) module.
 *
 *  @author Corey Stidston and Menka Mehta
 *  @date 2017-05-08
 */

#include "I2C.h"
#include "MK70F12.h"
#include "PE_Types.h"
#include "Cpu.h"
#include "stdlib.h"
#include "types.h"

//Definitions
#define I2C_D_READ  0x01 //from datasheet figure 11
#define I2C_D_WRITE 0x00 //from datasheet figure 11

static void* ReadCompleteUserArgumentsGlobal;
/*!< Private global pointer to the user arguments to use with the user callback function */
static void (*ReadCompleteCallbackGlobal)(void *);
/*!< Private global pointer to data ready user callback function */

static uint8_t SlaveAddress;    /*!< Private global variable to store 8-bit slave address */
static uint8_t RegisterAddress; /*!< Register to read and write from */
static bool waitForAck(void);
static bool start(void);
static bool stop(bool invokeCallback);
static void sendDeviceAddress(void);
static void sendRegisterAddress(const uint8_t registerAddress);

bool waitForAck(void)
{
	//Wait for ack
	while(!((I2C0_S & I2C_S_IICIF_MASK) == 0)) //I2C_S_IICIF_MASK
	{

	}
	I2C0_S |= I2C_S_IICIF_MASK;
	return true;
}

bool start(void)
{
	//start
	I2C0_S |= I2C_S_IICIF_MASK; //Clear interrupts pending
	I2C0_C1 |= I2C_C1_IICEN_MASK | I2C_C1_IICIE_MASK; //Enable I2c and interrupts
	I2C0_C1 |= I2C_C1_MST_MASK; //Master mode selected, start signal
	I2C0_C1 |= I2C_C1_TX_MASK; //Transmit mode selected

	return true;
}

bool stop(bool invokeCallback)
{
	//stop //disable the following.
	//disable TX would set it to the receive mode
	I2C0_C1 &= ~(I2C_C1_IICIE_MASK | I2C_C1_MST_MASK | I2C_C1_TX_MASK);

	if(invokeCallback)
	{
		(*ReadCompleteCallbackGlobal)(ReadCompleteUserArgumentsGlobal);
	}
	return true;
}

void sendDeviceAddress(void)
{
	//slave addresses I2C Data I/O register (i2Cx_D) pg 1875/2275 k70 manual
	I2C0_D = (SlaveAddress << 1)  | I2C_D_WRITE; // Send slave address with write bit

	waitForAck();
}

void sendRegisterAddress(const uint8_t registerAddress)
{
	I2C0_D = registerAddress; //send slave register address

	waitForAck();
}

void waitTilIdle(void)
{
	//page 1872 k70 manual
	while((I2C0_S & I2C_S_BUSY_MASK) == I2C_S_BUSY_MASK) // Wait till bus is idle
	{

	}
}

/*! @brief Sets up the I2C before first use.
 *
 *  @param aI2CModule is a structure containing the operating conditions for the module.
 *  @param moduleClk The module clock in Hz.
 *  @return BOOL - TRUE if the I2C module was successfully initialized.
 */
bool I2C_Init(const TI2CModule* const aI2CModule, const uint32_t moduleClk)
{
	ReadCompleteUserArgumentsGlobal = aI2CModule->readCompleteCallbackArguments;
	// userArguments made globally(private) accessible
	ReadCompleteCallbackGlobal = aI2CModule->readCompleteCallbackFunction;
	// userFunction made globally(private) accessible

	//arrays store multiplier and scl divider values
	const static uint8_t mult[] = { 1, 2, 4 };
	const static uint16_t scl[] = { 20, 22, 24, 26, 28, 30, 34, 40, 28, 32, 36, 40, 44, 48,
			56, 68, 48, 56, 64, 72, 80, 88, 104, 128, 80, 96, 112, 128, 144, 160, 192,
			240, 160, 192, 224, 256, 288, 320, 384, 480, 320, 384, 448, 512, 576, 640,
			768, 960, 640, 768, 896, 1024, 1152, 1280, 1536, 1920, 1280, 1536, 1792,
			2048, 2304, 2560, 3072, 3840 };

	uint8_t i, //counter for looping through scl dividerarray -scldivcount
	j, // counter for looping through multplier array - mulcount
	multiplier, //selected count value of multiplier
	sclDivider; //selected count value of scl divider
	uint32_t baudRateError = 100000; // baudrate should be close to 100kbps lab4 requirement
	uint32_t selectedBaudRate;   //baudRate of current calculation

	//enables clocks
	SIM_SCGC4 |= SIM_SCGC4_IIC0_MASK; //pg 352/2275 k70 manual
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK; // enable pin routing port E

	//Schematics-  accelerometer pg9/11 on the right PTE18 and 19
	// mux page 286/2275 k70 and ODE is open drain enable pg 325/2275
	PORTE_PCR18 = PORT_PCR_MUX(0x4) | PORT_PCR_ODE_MASK;
	PORTE_PCR19 = PORT_PCR_MUX(0x4) | PORT_PCR_ODE_MASK;

	//loop to find baudrate pg 1870
	for (i=0; i <(sizeof(scl)/sizeof(uint16_t))-1; i++)
	{
		for (j=0; j< (sizeof(mult)/sizeof(uint8_t))-1; j++)
		{
			if (abs(selectedBaudRate -  aI2CModule ->baudRate) < baudRateError) //check if the baudRate is closer to 100kbps
			{
				selectedBaudRate =moduleClk / mult[j] * scl[i]; //calculate baudRate
				baudRateError = abs(selectedBaudRate - aI2CModule->baudRate); //calculate difference between baudrates
				multiplier = j;
				sclDivider = i;
			}
		}
	}
	//set baudrate pg1870 k70
	I2C0_F |= I2C_F_ICR(sclDivider);
	I2C0_F |= I2C_F_MULT(mult[multiplier]);

	// Clear the control register
	I2C0_C1 = 0;

	// enable I2C register pg 1871/2275
	I2C0_C1 |= I2C_C1_IICEN_MASK;

	// I have to enable interrupt as well
	// Enable interrupt
	I2C0_C1 |= I2C_C1_IICIE_MASK;

	//12c programmable input glitch filter registers
	I2C0_FLT = I2C_FLT_FLT(0x00);
	//clear the control register c2
	//I2C0_C2 = 0; //not sure


	//NVICS page 91/2275 k70 manual
	// IRQ = 24 mod 32 = 24
	// NVICICPR0 = NVIC_ICPR_CLRPEND(1 << 24); // Clear any pending interrupts on I2C0
	// NVICISER0 = NVIC_ISER_SETENA(1 << 24); // Enable interrupts on I2C0
	NVICICPR1 = (1<<24);                   	// Clears pending interrupts on I2C0 module
	NVICISER1 = (1<<24);                   	// Enables interrupts on I2C0 module

	return true;
}

/*! @brief Selects the current slave device
 *
 * @param slaveAddress The slave device address.
 */
void I2C_SelectSlaveDevice(const uint8_t slaveAddress)
{
	SlaveAddress = slaveAddress; //store the slave address globally(private)
}



/*! @brief Write a byte of data to a specified register
 *
 * @param registerAddress The register address.
 * @param data The 8-bit data to write.
 */
void I2C_Write(const uint8_t registerAddress, const uint8_t data)
{
	waitTilIdle();

	//TXAK Transmit Acknowledge Enable ?????
	//RSTA Repeat Start. ????
	I2C0_C1 |= (I2C_C1_TX_MASK); //Set to transmit mode
	start();

	sendDeviceAddress();
	sendRegisterAddress(registerAddress);

	I2C0_C1 &= ~(I2C_C1_TX_MASK); //Receive mode selected

	I2C0_D = data; // Send data

	waitForAck();

	stop(false);
}

/*! @brief Reads data of a specified length starting from a specified register
 *
 * Uses polling as the method of data reception.
 * @param registerAddress The register address.
 * @param data A pointer to store the bytes that are read.
 * @param nbBytes The number of bytes to read.
 */
void I2C_PollRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes)
{
	waitTilIdle();

	start();

	sendDeviceAddress();
	sendRegisterAddress(registerAddress);

	I2C0_C1 &= ~(I2C_C1_TX_MASK); //Receive mode selected

	I2C0_D = (SlaveAddress << 1)  | I2C_D_READ; // Send slave address with read bit
	waitForAck();

	for (int i = 0; i < nbBytes; i++)
	{
		if (i == nbBytes) //last byte
		{
			//I2C0_S |=
		}
		data[i] = (uint8_t) I2C0_D;

	}
	I2C0_C1 |= (I2C_C1_TX_MASK); //Set back to transmit mode
	stop(false);
}

/*! @brief Reads data of a specified length starting from a specified register
 *
 * Uses interrupts as the method of data reception.
 * @param registerAddress The register address.
 * @param data A pointer to store the bytes that are read.
 * @param nbBytes The number of bytes to read.
 */
void I2C_IntRead(const uint8_t registerAddress, uint8_t* const data, const uint8_t nbBytes)
{
	waitTilIdle();

	start();

	sendDeviceAddress();
	sendRegisterAddress(registerAddress);

	I2C0_C1 &= ~(I2C_C1_TX_MASK); //Receive mode selected

	I2C0_D = (SlaveAddress << 1)  | I2C_D_READ; // Send slave address with read bit
	waitForAck();

	for (int i = 0; i < nbBytes; i++)
	{
		data[i] = (uint8_t) I2C0_D;
	}
	I2C0_C1 |= (I2C_C1_TX_MASK); //Set back to transmit mode
	stop(true);
}


/*! @brief Interrupt service routine for the I2C.
 *
 *  Only used for reading data.
 *  At the end of reception, the user callback function will be called.
 *  @note Assumes the I2C module has been initialized.
 */
void __attribute__ ((interrupt)) I2C_ISR(void)
{
	uint8_t status = I2C0_S;

<<<<<<< HEAD
	if (!(status & I2C_S_IICIF_MASK))
=======
	//STARTF, STOPF?

	if (!(status & I2C_S_IICIF_MASK)) //?
>>>>>>> e64d040108e36d12d6e774224d127d2b11213d67
	{
		return;
	} 
	else 
	{
		//acknowledge interrupt
		I2C0_S |= I2C_S_IICIF_MASK;
	}


	//Therefore, it is a read operation
	// Do we need to check if it is a read operation? how?

}
