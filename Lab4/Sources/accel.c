/*! @file
 *
 *  @brief Implementation of the HAL for the accelerometer.
 *
 *  @author Corey Stidston and Menka Mehta
 *  @date 2017-05-17
 */

/*!
 *  @addtogroup accel_module Accel module documentation
 *  @{
*/

// Accelerometer functions
#include "accel.h"

// Inter-Integrated Circuit
#include "I2C.h"

// Median filter
#include "median.h"

// K70 module registers
#include "MK70F12.h"

// CPU and PE_types are needed for critical section variables and the defintion of NULL pointer
#include "CPU.h"
#include "PE_types.h"
#include "types.h"


// Accelerometer registers
#define ADDRESS_OUT_X_MSB 0x01

#define ADDRESS_INT_SOURCE 0x0C

//function prototype
void actualAccelReadDataBack(void * nothing);

static const TI2CModule I2C_ACCEL_MODULE = {
		.primarySlaveAddress = 0x1D,
		.baudRate = 100000,
		.readCompleteCallbackFunction = actualAccelReadDataBack,
		.readCompleteCallbackArguments = 0
};


static TAccelSetup AccelModuleSetup;
static TAccelMode CurrentMode;

static union
{
  uint8_t byte;			/*!< The INT_SOURCE bits accessed as a byte. */
  struct
  {
    uint8_t SRC_DRDY   : 1;	/*!< Data ready interrupt status. */
    uint8_t               : 1;
    uint8_t SRC_FF_MT  : 1;	/*!< Freefall/motion interrupt status. */
    uint8_t SRC_PULSE  : 1;	/*!< Pulse detection interrupt status. */
    uint8_t SRC_LNDPRT : 1;	/*!< Orientation interrupt status. */
    uint8_t SRC_TRANS  : 1;	/*!< Transient interrupt status. */
    uint8_t SRC_FIFO   : 1;	/*!< FIFO interrupt status. */
    uint8_t SRC_ASLP   : 1;	/*!< Auto-SLEEP/WAKE interrupt status. */
  } bits;			/*!< The INT_SOURCE bits accessed individually. */
} INT_SOURCE_Union;

#define INT_SOURCE     		INT_SOURCE_Union.byte
#define INT_SOURCE_SRC_DRDY	INT_SOURCE_Union.bits.SRC_DRDY
#define INT_SOURCE_SRC_FF_MT	CTRL_REG4_Union.bits.SRC_FF_MT
#define INT_SOURCE_SRC_PULSE	CTRL_REG4_Union.bits.SRC_PULSE
#define INT_SOURCE_SRC_LNDPRT	CTRL_REG4_Union.bits.SRC_LNDPRT
#define INT_SOURCE_SRC_TRANS	CTRL_REG4_Union.bits.SRC_TRANS
#define INT_SOURCE_SRC_FIFO	CTRL_REG4_Union.bits.SRC_FIFO
#define INT_SOURCE_SRC_ASLP	CTRL_REG4_Union.bits.SRC_ASLP

#define ADDRESS_CTRL_REG1 0x2A

typedef enum
{
  DATE_RATE_800_HZ,
  DATE_RATE_400_HZ,
  DATE_RATE_200_HZ,
  DATE_RATE_100_HZ,
  DATE_RATE_50_HZ,
  DATE_RATE_12_5_HZ,
  DATE_RATE_6_25_HZ,
  DATE_RATE_1_56_HZ
} TOutputDataRate;

typedef enum
{
  SLEEP_MODE_RATE_50_HZ,
  SLEEP_MODE_RATE_12_5_HZ,
  SLEEP_MODE_RATE_6_25_HZ,
  SLEEP_MODE_RATE_1_56_HZ
} TSLEEPModeRate;

static union
{
  uint8_t byte;			/*!< The CTRL_REG1 bits accessed as a byte. */
  struct
  {
    uint8_t ACTIVE    : 1;	/*!< Mode selection. */
    uint8_t F_READ    : 1;	/*!< Fast read mode. */
    uint8_t LNOISE    : 1;	/*!< Reduced noise mode. */
    uint8_t DR        : 3;	/*!< Data rate selection. */
    uint8_t ASLP_RATE : 2;	/*!< Auto-WAKE sample frequency. */
  } bits;			/*!< The CTRL_REG1 bits accessed individually. */
} CTRL_REG1_Union;

#define CTRL_REG1     		    CTRL_REG1_Union.byte
#define CTRL_REG1_ACTIVE	    CTRL_REG1_Union.bits.ACTIVE
#define CTRL_REG1_F_READ  	  CTRL_REG1_Union.bits.F_READ
#define CTRL_REG1_LNOISE  	  CTRL_REG1_Union.bits.LNOISE
#define CTRL_REG1_DR	    	  CTRL_REG1_Union.bits.DR
#define CTRL_REG1_ASLP_RATE	  CTRL_REG1_Union.bits.ASLP_RATE

#define ADDRESS_CTRL_REG2 0x2B

#define ADDRESS_CTRL_REG3 0x2C

static union
{
  uint8_t byte;			/*!< The CTRL_REG3 bits accessed as a byte. */
  struct
  {
    uint8_t PP_OD       : 1;	/*!< Push-pull/open drain selection. */
    uint8_t IPOL        : 1;	/*!< Interrupt polarity. */
    uint8_t WAKE_FF_MT  : 1;	/*!< Freefall/motion function in SLEEP mode. */
    uint8_t WAKE_PULSE  : 1;	/*!< Pulse function in SLEEP mode. */
    uint8_t WAKE_LNDPRT : 1;	/*!< Orientation function in SLEEP mode. */
    uint8_t WAKE_TRANS  : 1;	/*!< Transient function in SLEEP mode. */
    uint8_t FIFO_GATE   : 1;	/*!< FIFO gate bypass. */
  } bits;			/*!< The CTRL_REG3 bits accessed individually. */
} CTRL_REG3_Union;

#define CTRL_REG3     		    CTRL_REG3_Union.byte
#define CTRL_REG3_PP_OD		    CTRL_REG3_Union.bits.PP_OD
#define CTRL_REG3_IPOL		    CTRL_REG3_Union.bits.IPOL
#define CTRL_REG3_WAKE_FF_MT	CTRL_REG3_Union.bits.WAKE_FF_MT
#define CTRL_REG3_WAKE_PULSE	CTRL_REG3_Union.bits.WAKE_PULSE
#define CTRL_REG3_WAKE_LNDPRT	CTRL_REG3_Union.bits.WAKE_LNDPRT
#define CTRL_REG3_WAKE_TRANS	CTRL_REG3_Union.bits.WAKE_TRANS
#define CTRL_REG3_FIFO_GATE	  CTRL_REG3_Union.bits.FIFO_GATE

#define ADDRESS_CTRL_REG4 0x2D

static union
{
  uint8_t byte;			/*!< The CTRL_REG4 bits accessed as a byte. */
  struct
  {
    uint8_t INT_EN_DRDY   : 1;	/*!< Data ready interrupt enable. */
    uint8_t               : 1;
    uint8_t INT_EN_FF_MT  : 1;	/*!< Freefall/motion interrupt enable. */
    uint8_t INT_EN_PULSE  : 1;	/*!< Pulse detection interrupt enable. */
    uint8_t INT_EN_LNDPRT : 1;	/*!< Orientation interrupt enable. */
    uint8_t INT_EN_TRANS  : 1;	/*!< Transient interrupt enable. */
    uint8_t INT_EN_FIFO   : 1;	/*!< FIFO interrupt enable. */
    uint8_t INT_EN_ASLP   : 1;	/*!< Auto-SLEEP/WAKE interrupt enable. */
  } bits;			/*!< The CTRL_REG4 bits accessed individually. */
} CTRL_REG4_Union;

#define CTRL_REG4            		CTRL_REG4_Union.byte
#define CTRL_REG4_INT_EN_DRDY	  CTRL_REG4_Union.bits.INT_EN_DRDY
#define CTRL_REG4_INT_EN_FF_MT	CTRL_REG4_Union.bits.INT_EN_FF_MT
#define CTRL_REG4_INT_EN_PULSE	CTRL_REG4_Union.bits.INT_EN_PULSE
#define CTRL_REG4_INT_EN_LNDPRT	CTRL_REG4_Union.bits.INT_EN_LNDPRT
#define CTRL_REG4_INT_EN_TRANS	CTRL_REG4_Union.bits.INT_EN_TRANS
#define CTRL_REG4_INT_EN_FIFO	  CTRL_REG4_Union.bits.INT_EN_FIFO
#define CTRL_REG4_INT_EN_ASLP	  CTRL_REG4_Union.bits.INT_EN_ASLP

#define ADDRESS_CTRL_REG5 0x2E

static union
{
  uint8_t byte;			/*!< The CTRL_REG5 bits accessed as a byte. */
  struct
  {
    uint8_t INT_CFG_DRDY   : 1;	/*!< Data ready interrupt enable. */
    uint8_t                : 1;
    uint8_t INT_CFG_FF_MT  : 1;	/*!< Freefall/motion interrupt enable. */
    uint8_t INT_CFG_PULSE  : 1;	/*!< Pulse detection interrupt enable. */
    uint8_t INT_CFG_LNDPRT : 1;	/*!< Orientation interrupt enable. */
    uint8_t INT_CFG_TRANS  : 1;	/*!< Transient interrupt enable. */
    uint8_t INT_CFG_FIFO   : 1;	/*!< FIFO interrupt enable. */
    uint8_t INT_CFG_ASLP   : 1;	/*!< Auto-SLEEP/WAKE interrupt enable. */
  } bits;			/*!< The CTRL_REG5 bits accessed individually. */
} CTRL_REG5_Union;

#define CTRL_REG5     		      	CTRL_REG5_Union.byte
#define CTRL_REG5_INT_CFG_DRDY		CTRL_REG5_Union.bits.INT_CFG_DRDY
#define CTRL_REG5_INT_CFG_FF_MT		CTRL_REG5_Union.bits.INT_CFG_FF_MT
#define CTRL_REG5_INT_CFG_PULSE		CTRL_REG5_Union.bits.INT_CFG_PULSE
#define CTRL_REG5_INT_CFG_LNDPRT	CTRL_REG5_Union.bits.INT_CFG_LNDPRT
#define CTRL_REG5_INT_CFG_TRANS		CTRL_REG5_Union.bits.INT_CFG_TRANS
#define CTRL_REG5_INT_CFG_FIFO		CTRL_REG5_Union.bits.INT_CFG_FIFO
#define CTRL_REG5_INT_CFG_ASLP		CTRL_REG5_Union.bits.INT_CFG_ASLP

/*!
 * @brief Callback once data is available.
 */
static void (*DataCallback)(void *);

/*!
 * @brief Argument to pass to data callback.
 */
static void *DataCallbackArgument;

/*!
 * @brief Callback once read is complete.
 */
static void (*ReadCallback)(void *);

/*!
 * @brief Argument passed to read callback.
 */
static void *ReadCallbackArgument;

static TAccelMode CurrentMode;

/*!
 * @brief Used for controlling Standby mode for REG1
 * @param standby
 */
void standbyMode(bool standby)
{
	if (standby)
	{
		CTRL_REG1_ACTIVE = 0;
		I2C_Write(ADDRESS_CTRL_REG1, CTRL_REG1);
	}
	else
	{
		CTRL_REG1_ACTIVE = 1;
		I2C_Write(ADDRESS_CTRL_REG1, CTRL_REG1);
	}
}


/*! @brief Initializes the accelerometer by calling the initialization routines of the supporting software modules.
 *
 *  @param accelSetup is a pointer to an accelerometer setup structure.
 *  @return bool - TRUE if the accelerometer module was successfully initialized.
 */

bool Accel_Init(const TAccelSetup* const accelSetup)
{
	AccelModuleSetup = *accelSetup;
	CurrentMode = ACCEL_POLL; //By default

	DataCallback = accelSetup-> dataReadyCallbackFunction;
	DataCallbackArgument=accelSetup->dataReadyCallbackArguments;

	ReadCallback = accelSetup->readCompleteCallbackFunction;
	ReadCallbackArgument = accelSetup->readCompleteCallbackArguments;

	//Initialize I2C
	I2C_Init(&I2C_ACCEL_MODULE, accelSetup->moduleClk);

	standbyMode(true); //Standby Mode Activate

	CTRL_REG1_DR = DATE_RATE_1_56_HZ;
	CTRL_REG1_F_READ = 1; 										//8 bit precision
	CTRL_REG1_LNOISE = 0; 										//Set to full dynamic range mode
	CTRL_REG1_ASLP_RATE = SLEEP_MODE_RATE_1_56_HZ;
	I2C_Write(ADDRESS_CTRL_REG1, CTRL_REG1); 	//Write to the register

	CTRL_REG3_PP_OD = 0;
	CTRL_REG3_IPOL = 1;
	I2C_Write(ADDRESS_CTRL_REG3, CTRL_REG3); //Write to the register

	CTRL_REG4_INT_EN_DRDY = 0;
	I2C_Write(ADDRESS_CTRL_REG4, CTRL_REG4); //Write to the register

	CTRL_REG5_INT_CFG_DRDY = 1;
	I2C_Write(ADDRESS_CTRL_REG5, CTRL_REG5);

	standbyMode(false); //Standby Mode Deactivate

	SIM_SCGC5 |=  SIM_SCGC5_PORTB_MASK; //set portB as per lab requirements

	//set pins use pin 4
	PORTB_PCR4 &= ~PORT_PCR_MUX_MASK; //clear any previously set bits for the PCR_MUX
	PORTB_PCR4 |= PORT_PCR_MUX(1); //set pin 4
	PORTB_PCR4 |= PORT_PCR_ISF_MASK; //set interrupt status flag pg 323
	PORTB_PCR4 |= PORT_PCR_IRQC(10); //set interrupt configuration

	//NVICS: IQR- Pin detect portB (88mod32) pg98 k70 manual
	NVICICPR2 = (1<<24);
	NVICISER2 = (1<<24);

	return true;
}

/*!
 *  @brief Used for accessing the current mode
 */
TAccelMode Accel_GetMode()
{
	return CurrentMode;
}

/*! @brief Reads X, Y and Z accelerations.
 *  @param data is a an array of 3 bytes where the X, Y and Z data are stored.
 */
void Accel_ReadXYZ(uint8_t data[3])
{
	//I2C_SelectSlaveDevice(0x1d);

	if (CurrentMode == ACCEL_POLL)
	{
		I2C_PollRead(ADDRESS_OUT_X_MSB, data, (sizeof(data)/sizeof(data[0])));
	}
	else if (CurrentMode == ACCEL_INT)
	{
		I2C_IntRead(ADDRESS_OUT_X_MSB, data,  (sizeof(data)/sizeof(data[0])));
	}
}

/*! @brief actualAccelReadDataBack
 *  @param nothing
 */

void actualAccelReadDataBack(void * nothing)
{
	if(AccelModuleSetup.readCompleteCallbackFunction != 0)
	{
		AccelModuleSetup.readCompleteCallbackFunction(AccelModuleSetup.readCompleteCallbackArguments);
	}
}

/*! @brief Set the mode of the accelerometer.
 *  @param mode specifies either polled or interrupt driven operation.
 */
void Accel_SetMode(const TAccelMode mode)
{
	CurrentMode = mode;

	if (mode == ACCEL_POLL)
	{
		CTRL_REG4_INT_EN_DRDY = 0; //Data ready interrupt disabled
		I2C_Write(ADDRESS_CTRL_REG4, CTRL_REG4);
	}
	else
	{
		CTRL_REG4_INT_EN_DRDY = 1; //Data ready interrupt enabled
		I2C_Write(ADDRESS_CTRL_REG4, CTRL_REG4);
	}
}


void __attribute__ ((interrupt)) AccelDataReady_ISR(void)
{
	if (!(PORTB_PCR7 & PORT_PCR_ISF_MASK))
	{
		return;
	}

	PORTB_PCR4 |= PORT_PCR_ISF_MASK; 			//Clear interrupt

	if (CurrentMode == ACCEL_INT)
	{
		(DataCallback)(DataCallbackArgument);	//Initiate Callback
	}
}
/*!
 * @}
*/
