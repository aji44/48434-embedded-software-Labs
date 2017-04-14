#include <stdio.h>

#define FLASH_DATA_START 0x00080000LU

int phrase = 0xFF;

int Flash_AllocateVar1();
int Flash_AllocateVar2();

int main(void)
{
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 1 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 2 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 3 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 4 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 5 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 6 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 7 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 8 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 9 should fail, not enough room */
	printf("Allocated? = %d\n",Flash_AllocateVar1(1)); /* 10 should fail, not enough room */

	
	return 0;
}

int Flash_AllocateVar1()
{
	int mask = 0x80; 
	int pos;
	int variable;

	for(pos = 0x00; pos < 0x08; pos += 0x01) {
		if(phrase & mask) {
			variable = FLASH_DATA_START + pos; 
			printf("Variable Address = %d\n", variable);
			phrase = (phrase ^ mask);
			return 1;
		}
		mask = mask >> 1;
		printf("Mask - %d\n", mask);
	}
	return 0;
}

int Flash_AllocateVar2()
{
	int mask = 0xC0; /* 11000000 */
	int pos;
	int variable;

	for(pos = 0x00; pos < 0x08; pos += 0x02) {
		if(phrase & mask) {
			variable = FLASH_DATA_START + pos; 
			printf("Variable Address = %d\n", variable);
			phrase = (phrase ^ mask);
			return 1;
		}
		mask = mask >> 2; /* I.E 11000000 -> 00110000*/
		printf("Mask - %d\n", mask);
	}
	return 0;
}