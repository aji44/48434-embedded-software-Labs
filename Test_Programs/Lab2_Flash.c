#include <stdio.h>

#define FLASH_DATA_START 0x00080000LU

int phrase = 0xFF; /* represents the 8bytes of space in flash memory - Find better name for this...*/

int Flash_AllocateVar1(); /* 8bytes on any address */
int Flash_AllocateVar2(); /* 16bytes only on even address */

void test_1();
void test_2();
void test_3();

int main(void)
{
	test_2();

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
	/*
	Mask values: 
	192
	48
	12
	3
	*/

	int mask1 = 0x80; /* 	10000000 */
	int mask2 = 0x40;  /* 	01000000 */
	int pos;
	int variable;

	for(pos = 0x00; pos < 0x08; pos += 0x02) {
		/* if(mask == (phrase & mask)) */
		if((phrase & mask1) & mask2) {
			variable = FLASH_DATA_START + pos; 
			printf("Variable Address = %d\n", variable);
			phrase = (phrase ^ (mask1 | mask2));

			return 1;
		}

		/*mask = mask >> 2;  I.E 11000000 -> 00110000*/

		mask1 = mask1 >> 2; 
		mask2 = mask2 >> 2;
		printf("Mask1 - %d\n", mask1);
		printf("Mask2 - %d\n", mask2);
	}
	return 0;
}



/*****************************************TESTS*****************************************/


void test_1()
{
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*1 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*2 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*3 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*4 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*5 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*6 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*7 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*8 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*9 should fail, no more room  */
	printf("Allocated? = %d\n",Flash_AllocateVar1());  /*10 should fail, no more room */
}

void test_2()
{
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*1 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*2 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*3 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*4 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*5 should fail, no more room  */
}

void test_3()
{
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*1 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar1()); /*2 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*3 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*4 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*4 should succeed */
	printf("Allocated? = %d\n",Flash_AllocateVar2()); /*5 should fail, no more room  */
}