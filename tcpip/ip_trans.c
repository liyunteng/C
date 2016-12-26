/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/10/2015 05:33:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  liyunteng (), li_yunteng@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

/*******************************************************************************
* Author : liyunteng
* Email : li_yunteng@163.com
* Created Time : 2015-12-10 17:33
* Filename : test.c
* Description : 
* *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *ip;
	if (argc < 2) {
		ip = "172.172.16.1";
	} else {
		ip = argv[1];
	}

	char c;
	unsigned long val = 0;
	unsigned long addr = 0x0;
	unsigned long base = 10;
	while (c = *ip++) {
		switch (c) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				val = (val * base) + (c - '0');
				break;
			case '.':
				addr = addr << 8 | val;
				val = 0;
				break;
			default:
				break;
		}
	}
	addr = addr << 8 | val;
	printf("0x%lX\n", addr);

	unsigned long t;
	t = addr >> 24 & 0xFF;
	printf("%d.", (int)t);
	t = (addr >> 16) & 0xFF;
	printf("%d.", (int)t);
	t = (addr >> 8)& 0xFF; 
	printf("%d.", (int)t);
	t = (addr >> 0) & 0xFF;
	printf("%d\n", (int)t);
	
}
