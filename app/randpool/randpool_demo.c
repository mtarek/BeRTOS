/**
 * \file
 * <!--
 * Copyright 2007 Develer S.r.l. (http://www.develer.com/)
 * This file is part of DevLib - See README.devlib for information.
 * -->
 *
 * \brief Test function for randpool API.
 *
 * This program return a rand number generate from randpool function.
 * For use randpool function you need a souce of entropy. In this
 * program you can choose from:
 * 	- /dev/random
 * 	- /dev/urandom
 * 	- /dev/input/mouse0
 * 	
 * There are 3 mode of output:
 *     - binmode: program generate a sequenze of random byte.
 *     - Matrix of random number for octave program.
 *     - Vector of random number for octave program.
 * 
 * \version $Id$
 * \author Daniele Basile <asterix@develer.com>
 */

/*#*
 *#* $Log$
 *#* Revision 1.1  2007/02/15 18:17:51  asterix
 *#* Add randpool test program.
 *#*
 *#*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cfg/compiler.h>
#include <drv/timer.h>
#include <algos/md2.h> 
#include <algos/randpool.h>
#include <unistd.h>

#define LEN 256      //Size of buffer containing a random number.
#define SAMPLE 1000  //Defoult number of byte that put in entropy pool.

int main (int argc, char *argv[])
{
	EntropyPool pool;
	uint8_t ch;
	uint8_t buff[LEN];
	FILE *pdev;
	int opt = getopt (argc, argv, "murh");
	int samp = SAMPLE;
	int round = 10;
	int mode = 0;
	int  pass = 0;

	timer_init();

	randpool_init(&pool, NULL, 0); //Init a entropy pool.

	/*
	 * Chose a source of entropy.
	 */
	switch(opt)
	{
		case 'm':
		{
			pdev = fopen("/dev/input/mouse0", "r");
			break;
		}
		case 'u':
		{
			pdev = fopen("/dev/urandom", "r");
			break;
		}
		case 'r':
		{
			pdev = fopen("/dev/random", "r");
			break;
		}
		case 'h':
		{
		}
		default:
		{
			printf("\n");	
			printf("\n");	
			printf("randpool_demo [OPTION] [SAMPLE] [ROUND] [MODE]\n");	
			printf("\n");	
			printf("OPTION:\n");	
			printf("  -r  /dev/random\n");	
			printf("  -u  /dev/urandom\n");	
			printf("  -m  /dev/input/mouse0\n");	
			printf("\n");	
			printf("SAMPLE:\n");	
			printf("  num  number of entropy byte to put in etropy pool.\n");	
			printf("\n");	
			printf("ROUND:\n");	
			printf("  num  number call of randpool_get function.\n");	
			printf("\n");
			printf("MODE:\n");	
			printf("  0  binmode\n");	
			printf("  1  vector mode\n");	
			printf("  2  matrix mode\n");	
			printf("\n");
			printf("Test program of randpool API.\n");	
			printf("This program create an entropy pool of 256byte, and fill it\n");	
			printf("with entropy from a exsternal source. This source can be:\n");	
			printf("  - /dev/random (option: -r)\n");	
			printf("  - /dev/urandom (option: -u)\n");	
			printf("  - /dev/input/mouse0 (option: -m)\n");	
			printf("\n");	
			printf("Once the pool is full, program call a randpool_get ROUND time,\n");	
			printf("printing on STDOUT random value, in accord with a select mode.\n");	
			printf("The mode are:\n");	
			printf("  - binmode: program print on STDOUT random byte. (option: 0 (defaul mode)\n");	
			printf("  - vector mode: program print on STDOUT a ROUND vector of random value.\n");	
			printf("                 The output is format for octave program. (option: 1)\n");	
			printf("  - matrix mode: program print on STDOUT a matrix of random value.\n");	
			printf("                 The output is format for octave program. (option: 2)\n");	
			printf("\n");
			exit(1);
		}

	}

	/*
	 *  
	 */
	if(argc > 2)
	{
		if(argv[2])
			samp = atoi(argv[2]);  //Number of byte take from entropy source
		if(argv[3])
			round = atoi(argv[3]); //Number of time we call randpool_get.
		if(argv[4])
		{	
			switch(atoi(argv[4]))
			{
				case 1:
				{
					mode = 1;
					printf("# Created by Asterix.\n");
					printf("# name: __nargin__\n");
					printf("# type: scalar\n");
					printf("0\n");
					break;
				}
				case 2:
				{
					mode = 2;
					printf("# Created by Asterix.\n");
					printf("# name: __nargin__\n");
					printf("# type: scalar\n");
					printf("0\n");
					break;
				}
				default:
				{
					break;
				}

			}

		}

	}

	/*
	 * Add byte to entropy pool from a source of entropy.
	 */
	for(int i = 0; i < samp; i++)
	{

		ch = fgetc(pdev);
		randpool_add(&pool, &ch, sizeof(ch));
		
	}

	fclose(pdev);

	
	for(int k = 0; k < round; k++)
	{	
		switch(mode)
		{
			case 1:
			{
				printf("\n");
				printf("\n# name: vet%d",k);
				printf("\n# type: matrix");
				printf("\n# rows: 1");
				printf("\n# columns: %d\n", LEN);
				pass = 1;
				break;
			}
			case 2:
			{
				printf("\n");
				printf("\n# name: randMatrix");
				printf("\n# type: matrix");
				printf("\n# rows: %d",round);
				printf("\n# columns: %d\n", LEN);
				pass = 1;
				mode = 0;
				break;
			}
		}

		randpool_get(&pool, buff, LEN);

		if(pass)
		{
			for(int j = 0; j < LEN; j++)
			{
				printf("%d ", buff[j]);
			}
			printf("\n");
		}
		else
			fwrite(buff, sizeof(uint8_t), LEN, stdout);
	}
	

}

