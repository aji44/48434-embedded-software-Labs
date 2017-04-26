  /*! @file
   *
   *  @brief Routines for controlling the Real Time Clock (RTC) on the TWR-K70F120M.
   *
   *  This contains the functions for operating the real time clock (RTC).
   *
   *  @author Corey Stidston & Menka Mehta
   *  @date 2017-04-26
   */
  /*!
  **  @addtogroup RTC_module RTC module documentation
  **  @{
  */
  #include "RTC.h"
  #include <types.h>

  static void *RTC_Arguments; //pointer to userArguments funtion
  static void (*RTC_Callback)(void *); //pointer to userCallback function
  /*! @brief Initializes the RTC before first use.
   *
   *  Sets up the control register for the RTC and locks it.
   *  Enables the RTC and sets an interrupt every second.
   *  @param userFunction is a pointer to a user callback function.
   *  @param userArguments is a pointer to the user arguments to use with the user callback function.
   *  @return bool - TRUE if the RTC was successfully initialized.
   */
  bool RTC_Init(void (*userFunction)(void*), void* userArguments){
   RTC_Arguments = userArguments; //Globally accessible (userArguments)
   RTC_Callback = userFunction; //Globally accessible (userFunction)

   SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;  	// Enable clock gate RTC
   //lab3 note hint 7 -RTC | pg 1394/2275 k70 manual
   RTC_CR |= RTC_CR_OSCE_MASK;	//Enables the 32.768kHz Oscillator
   //Enable 18pF load - pg 1394/2275 k70 manual
   RTC_CR |= RTC_CR_SC2P_MASK;
   RTC_CR |= RTC_CR_SC16P_MASK;

   //RTC-Interrupt Enable Register|PG 1399/2275 K70 Manual
   //TSIE -Time Seconds Interrupt Enable
    RTC_IER |= RTC_IER_TSIE_MASK;				// Enables seconds enable interrupt (on by default)
    //TAIE - Time Alarm Interrupt Enable
    RTC_IER &= ~RTC_IER_TAIE_MASK;			// Disables Time Alarm Interrupt
    //TOIE - Time Overflow Interrupt Enable
    RTC_IER &= ~RTC_IER_TOIE_MASK;			// Disables Overflow Interrupt
    //TIIE - Time Invalid Interrupt Enable
    RTC_IER &= ~RTC_IER_TIIE_MASK;			// Disables Time Invalid Interrupt


    RTC_LR &= ~RTC_LR_CRL_MASK;				// Locks the control register
    RTC_SR |= RTC_SR_TCE_MASK;				// Initialises the timer control

// Initialise NVICs for RTC | pg 97/2275 k70 manual
 NVICICPR2 =  (1 << 3);		//IRQ 67(seconds interrupt) mod 32
 NVICISER2 =  (1 << 3);

  return true;
  }

  /*! @brief Sets the value of the real time clock.
   *
   *  @param hours The desired value of the real time clock hours (0-23).
   *  @param minutes The desired value of the real time clock minutes (0-59).
   *  @param seconds The desired value of the real time clock seconds (0-59).
   *  @note Assumes that the RTC module has been initialized and all input parameters are in range.
   */
  void RTC_Set(const uint8_t hours, const uint8_t minutes, const uint8_t seconds){

}

  /*! @brief Gets the value of the real time clock.
   *
   *  @param hours The address of a variable to store the real time clock hours.
   *  @param minutes The address of a variable to store the real time clock minutes.
   *  @param seconds The address of a variable to store the real time clock seconds.
   *  @note Assumes that the RTC module has been initialized.
   */
  void RTC_Get(uint8_t* const hours, uint8_t* const minutes, uint8_t* const seconds){

  }

  /*! @brief Interrupt service routine for the RTC.
   *
   *  The RTC has incremented one second.
   *  The user callback function will be called.
   *  @note Assumes the RTC has been initialized.
   */
  void __attribute__ ((interrupt)) RTC_ISR(void){
   if(RTC_Callback)
   (*RTC_Callback)(RTC_Arguments);
}



















  /*!
  ** @}
  */
