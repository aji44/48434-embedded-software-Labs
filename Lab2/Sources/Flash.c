/*
 * Flash.c
 *
 *  Created on: 12 Apr 2017
 *      Author: 98119910
 */

#include "types.h"
#include "Flash.h"
#include "MK70F12.h"

static bool LaunchCommand(TFCCOB* commonCommandObject);
static bool WritePhrase(const uint32_t address, const uint64union_t phrase);
static bool ReadPhrase(uint64_t * const phrase);
static void WaitCCIF(void);
static void SetCCIF(void);

uint8_t phrase = 0xFF; //11111111

/*! @brief Enables the Flash module.
 *
 *  @return bool - TRUE if the Flash was setup successfully.
 */
bool Flash_Init(void)
{
  return true;
}

/*! @brief Allocates space for a non-volatile variable in the Flash memory.
 *
 *  @param variable is the address of a pointer to a variable that is to be allocated space in Flash memory.
 *         The pointer will be allocated to a relevant address:
 *         If the variable is a byte, then any address.
 *         If the variable is a half-word, then an even address.
 *         If the variable is a word, then an address divisible by 4.
 *         This allows the resulting variable to be used with the relevant Flash_Write function which assumes a certain memory address.
 *         e.g. a 16-bit variable will be on an even address
 *  @param size The size, in bytes, of the variable that is to be allocated space in the Flash memory. Valid values are 1, 2 and 4.
 *  @return bool - TRUE if the variable was allocated space in the Flash memory.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_AllocateVar(volatile void** variable, const uint8_t size)
{
	int mask;
	int addressPos;

	switch(size)
	{
		case 1:
		mask = 0x80; /* 10000000 */
		break;
		case 2:
		mask = 0xC0; /* 11000000 */
		break;
		case 4:
		mask = 0xF0; /* 11110000 */
		break;
	}

	for(addressPos = FLASH_DATA_START; addressPos < (FLASH_DATA_END+1); addressPos += size) {
		if(mask == (phrase & mask)) {
			*variable = addressPos; 
			phrase = (phrase ^ mask);
			return true;
		}
		mask = mask >> size;
	}
	return false;
}

/*! @brief Writes a 32-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 32-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 4-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write32(volatile uint32_t* const address, const uint32_t data)
{
	uint8_t index = address - FLASH_DATA_START;
	uint64_t mask = 0xFFFFFFFF00000000U
	uint64_t newPhrase = 0x0000000000000000U

	uint64_t currentPhrase;
	ReadPhrase(&currentPhrase);

	uint16_t tempPhrase[2];

	uint8_t i;
	for(i = 0; i < 8; i += 4) {
		if(i = index) {
			tempPhrase[i] = data;
		} else {
			tempPhrase[i] = ((currentPhrase & data) >> (64-(32*(i+1))));
		}
		mask = mask >> 32;
	}

	for(i = 0; i < 8; i += 4) {
		newPhrase = newPhrase | tempPhrase[i];
	}
	
	WritePhrase(newPhrase);
	return true;
}

/*! @brief Writes a 16-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 16-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if address is not aligned to a 2-byte boundary or if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write16(volatile uint16_t* const address, const uint16_t data)
{
	uint8_t index = address - FLASH_DATA_START;
	uint64_t mask = 0xFFFF000000000000U
	uint64_t newPhrase = 0x0000000000000000U

	uint64_t currentPhrase;
	ReadPhrase(&currentPhrase);

	uint16_t tempPhrase[4];

	uint8_t i;
	for(i = 0; i < 8; i += 2) {
		if(i = index) {
			tempPhrase[i] = data;
		} else {
			tempPhrase[i] = ((currentPhrase & data) >> (64-(16*(i+1))));
		}
		mask = mask >> 16;
	}

	for(i = 0; i < 8; i += 2) {
		newPhrase = newPhrase | tempPhrase[i];
	}
	
	WritePhrase(newPhrase);
	return true;
}

/*! @brief Writes an 8-bit number to Flash.
 *
 *  @param address The address of the data.
 *  @param data The 8-bit data to write.
 *  @return bool - TRUE if Flash was written successfully, FALSE if there is a programming error.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Write8(volatile uint8_t* const address, const uint8_t data)
{
	uint8_t index = address - FLASH_DATA_START;
	uint64_t mask = 0xFF00000000000000U
	uint64_t newPhrase = 0x0000000000000000U

	uint64_t currentPhrase;
	ReadPhrase(&currentPhrase);

	uint8_t tempPhrase[8];

	uint8_t i;
	for(i = 0; i < 8; i++) {
		if(i = index) {
			tempPhrase[i] = data;
		} else {
			tempPhrase[i] = ((currentPhrase & data) >> (64-(8*(i+1))));
		}
		mask = mask >> 8;
	}

	for(i = 0; i < 8; i++) {
		newPhrase = newPhrase | tempPhrase[i];
	}
	
	WritePhrase(newPhrase);
	return true;
}

//P 789 and P806
bool WritePhrase(const uint64union_t phrase) //const uint32_t address, 
{
	uint32_8union_t flashStart;

	uint32union_t word_Hi;
	uint32union_t word_Lo;

	uint16union_t halfword_1; //16 bytes
	uint16union_t halfword_2;
	uint16union_t halfword_3;
	uint16union_t halfword_4;

	WaitCCIFReady();

	if(!Flash_Erase())
	{
		return false;
	}

	flashStart.l = FLASH_DATA_START;
	FTFE_FCCOB0 = FLASH_CMD_PGM8; // defines the FTFE command to write
	FTFE_FCCOB1 = flashStart.s.b; // sets flash address[23:16] to 128
	FTFE_FCCOB2 = flashStart.s.c; // sets flash address[15:8] to 0
	FTFE_FCCOB3 = (flashStart.s.d & 0xF8);

	word_Hi = phrase.Hi;
	word_Lo = phrase.Lo;

	halfword_1 = word_Hi.Hi
	halfword_2 = word_Hi.Lo
	halfword_3 = word_Lo.Hi
	halfword_4 = word_Lo.Lo

	//Big Endian Sorted- is this Correct?
	FTFE_FCCOB4 = halfword_3.Hi
	FTFE_FCCOB5 = halfword_3.Lo
	FTFE_FCCOB6 = halfword_4.Hi 
	FTFE_FCCOB7 = halfword_4.Lo 
	FTFE_FCCOB8 = halfword_2.Hi
	FTFE_FCCOB9 = halfword_2.Lo
	FTFE_FCCOBA = halfword_1.Hi
	FTFE_FCCOBB = halfword_1.Lo

	SetCCIF(); //Initiates the command

	return true;
}

/*! @brief Reads the phrase starting from FLASH_DATA_START
 *	
 *	@param returns pointer to phrase
 *  @return bool - TRUE if the the phrase was read
 *  @note Assumes Flash has been initialized.
 */
bool ReadPhrase(uint64_t * const phrase)
{
	WaitCCIFReady();
	*phrase = _FP(FLASH_DATA_START);
	return true;
}

/*! @brief Erases the entire Flash sector.
 *
 *  @return bool - TRUE if the Flash "data" sector was erased successfully.
 *  @note Assumes Flash has been initialized.
 */
bool Flash_Erase(void)
{
	WaitCCIFReady();
	FTFE_FCCOB0 = 0x09; //Command to erase flash sector
	return true; //Later on, need to check error flags
}

void WaitCCIF(void)
{
	while (!(FTFE_FSTAT & FTFE_FSTAT_CCIF_MASK));
}

void SetCCIF(void)
{
	FTFE_FSTAT |= FTFE_FSTAT_CCIF_MASK;
}