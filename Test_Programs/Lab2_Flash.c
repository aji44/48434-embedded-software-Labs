#include <stdio.h>

#define FLASH_DATA_START 0x00080000LU

int phrase = 0xFF; /* represents the 8bytes of space in flash memory - Find better name for this...*/

int Flash_AllocateVar1(); /* 8bytes on any address */
int Flash_AllocateVar2(); /* 16bytes only on even address */
int Flash_AllocateVar4(); /* 16bytes only on address divisible by 4 i.e. 80000 or 80004 */

void test_1();
void test_2();
void test_3();
void test_4();
void test_5();

int main(void)
{
	test_5();

	return 0;
}

int Flash_AllocateVar1()
{
	/*
	Mask values: 
	0080 (initial)
	0040
	0020
	0010
	0008
	0004
	0002
	0001 (final)
	0000 (invalid)
	*/
	int mask = 0x80; 
	printf("Initial Mask - %04x\n", mask);
	int pos;
	int variable;

	for(pos = 0x00; pos < 0x08; pos += 0x01) {
		if(mask == (phrase & mask)) {
			variable = FLASH_DATA_START + pos; 
			/* printf("Variable Address = %d\n", variable); */
			phrase = (phrase ^ mask);
			return variable;
		}
		mask = mask >> 1;
		printf("Mask - %04x\n", mask);
	}
	return -1;
}

int Flash_AllocateVar2()
{
	/*
	Mask values: 
	00c0
	0030
	000c
	0003 (final)
	0000 (invalid)
	*/

	int mask = 0xC0;
	printf("Initial Mask - %04x\n", mask);
	int pos;
	int variable;

	for(pos = 0x00; pos < 0x08; pos += 0x02) {
		if(mask == (phrase & mask)) {
			variable = FLASH_DATA_START + pos; 
			/* printf("Variable Address = %d\n", variable); */
			phrase = (phrase ^ mask);
			return variable;
		}

		/*mask = mask >> 2;  I.E 11000000 -> 00110000*/

		mask = mask >> 2; 
		printf("Mask - %04x\n", mask);
	}
	return -1;
}

int Flash_AllocateVar4()
{
	/*
	Mask values: 
	00f0 (initial)
	000f (final)
	0000 (invalid)
	*/

	int mask = 0xF0; /* 11110000 */
	printf("Initial Mask - %04x\n", mask);
	int pos;
	int variable;

	for(pos = 0x00; pos < 0x08; pos += 0x04) {
		if(mask == (phrase & mask)) {
			variable = FLASH_DATA_START + pos; 
			/* printf("Variable Address = %d\n", variable); */
			phrase = (phrase ^ mask);
			return variable;
		}

		mask = mask >> 4; /* 11110000 -> 00001111*/
		printf("Mask - %04x\n", mask);
	}
	return -1;
}

/*****************************************TESTS*****************************************/


void test_1()
{
	int out;
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*1 should succeed */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*2 should succeed */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*3 should succeed */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*4 should succeed */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*5 should succeed */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*6 should succeed */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*7 should succeed */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*8 should succeed */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*9 should fail, no more room  */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out);  /*10 should fail, no more room */
}

void test_2()
{
	int out;
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*1 should succeed */
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*2 should succeed */
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*3 should succeed */
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*4 should succeed */
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*5 should fail, no more room  */
}

void test_3()
{
	int out;
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*1 should succeed */
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out); /*2 should succeed */
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*3 should succeed */
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*4 should succeed */
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*4 should fail, no more room */
	out = Flash_AllocateVar2();
	printf("Allocated? (%04x,  %04x) \n", out, (1+out)); /*5 should fail, no more room  */
}

void test_4()
{
	int out;
	out = Flash_AllocateVar4();
	printf("Allocated? (%04x - %04x) \n", out, (3+out)); /*1 should succeed*/
	out = Flash_AllocateVar4();
	printf("Allocated? (%04x - %04x) \n", out, (3+out)); /*2 should succeed*/
	out = Flash_AllocateVar4();
	printf("Allocated? (%04x - %04x) \n", out, (3+out)); /*3 should fail, no more room*/
}

void test_5()
{
	int out;
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out); /*1 should succeed*/
	out = Flash_AllocateVar4();
	printf("Allocated? (%04x - %04x) \n", out, (3+out)); /*2 should succeed*/
	out = Flash_AllocateVar4();
	printf("Allocated? (%04x - %04x) \n", out, (3+out)); /*3 should fail no more room*/
	out = Flash_AllocateVar4();
	printf("Allocated? (%04x - %04x) \n", out, (3+out)); /*4 should fail, no more room*/
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out); /*5 should succeed*/
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out); /*6 should succeed*/
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out); /*7 should succeed*/
	out = Flash_AllocateVar1();
	printf("Allocated? (%04x) \n", out); /*8 should fail, no more room*/
}