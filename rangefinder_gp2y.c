
/*******************************************************************************
 * Copyright (c) 2022 Gerasimenko Evgeniy (evgenyger.work@gmail.com)                      *
 *                                                                             *
 * The MIT License (MIT):                                                      *
 * Permission is hereby granted, free of charge, to any person obtaining a     *
 * copy of this software and associated documentation files (the "Software"),  *
 * to deal in the Software without restriction, including without limitation   *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell   *
 * copies of the Software, and to permit persons to whom the Software is       *
 * furnished to do so, subject to the following conditions:                    *
 * The above copyright notice and this permission notice shall be included     *
 * in all copies or substantial portions of the Software.                      *
 *                                                                             *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,             *
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR       *
 * OTHER DEALINGS IN THE SOFTWARE.                                             *
 ******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>
#include <ads1115.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#define AD_BASE 122
#define AD_ADDR 0x48 //i2c address

#define ADC_PIN 2

void help()
{
	printf("    Use this application for reading from rangefinder\n");
	printf("    execute format: ./range TIME \n");
	printf("    return: length in cm\n");
	printf("    TIME - pause between writing in ms\n");
	printf("    -h - help\n");
	printf("    -q - quiet\n");
}

int clamp(int x, int min, int max)
{
	return (x < min) ? min : ((x > max) ? max : x);
}

int main(int argc, char *argv[])
{ 
	int quiet = 0;
	if (argc > 1) {
		if ((strcmp(argv[1], "-h") == 0)) {
			help();
			return 0;
		} else {
			if ((strcmp(argv[1], "-q") == 0)) {
				quiet = 1;
			}
		}
	}
	char fifo_name[100];
	if ((quiet) && (argc == 4))
	{
        strcpy(fifo_name,argv[3]);
 	} else
	{
	if((!quiet) && (argc == 3))
	strcpy(fifo_name,argv[2]);
	};

	if ((quiet && argv[3] &&  argc != 4) || (!quiet && argv[2] && argc != 3) || (quiet && !argv[3] &&  argc != 3) || (!quiet && !argv[3] &&  argc != 2)) {
		help();
		return 0;
	}

	if (!quiet)
		printf("\nThe rangefinder application was started\n\n");

	int argument = 1;
	if (quiet)
		argument++;
	int delay_ms = atoi(argv[argument]);

	wiringPiSetup();
	ads1115Setup(AD_BASE, AD_ADDR);
	digitalWrite(AD_BASE, 0);

        struct timespec time;
	if(!(((quiet) && (argc == 4)) || ((!quiet) && (argc == 3))))

         while (1) {
               clock_gettime(CLOCK_REALTIME, &time);
               int sec = time.tv_sec;
               int min = (sec/60) % 60;
               int hours = (sec/3600)% 24;

		int ADC_VAL = analogRead(AD_BASE + ADC_PIN);
		if (!quiet)
			printf("ADC: %d; TIME: %d : %d : %d \n", ADC_VAL,  hours, min, (sec% 60));
		else
			printf("%d ;  %d : %d : %d\n", ADC_VAL, hours, min,(sec % 60));

		fflush(stdout);
		usleep(1000 * delay_ms);
	} else

	{     
                  mkfifo(fifo_name, 0666);
                 int fifo = open(fifo_name, O_WRONLY);
                 while (1) {
		clock_gettime(CLOCK_REALTIME, &time);

                int sec = time.tv_sec;
                int min = (sec/60) % 60;
                int hours = (sec/3600)% 24;

		int ADC_VAL = analogRead(AD_BASE + ADC_PIN);
		if (!quiet) 
			{
			char str[80];
			sprintf(str,"ADC: %d; TIME: %d : %d : %d\n", ADC_VAL, hours, min, (sec% 60)); 
			write(fifo, str, strlen(str) + 1);
			}
			else
			{
			char str[80];
			sprintf(str, "%d ; %d : %d : %d\n", ADC_VAL, hours, min,(sec % 60));
			write(fifo, str, strlen(str) + 1);  
                         }


		fflush(stdout);
		usleep(1000 * delay_ms);
	}
       }
	return 0;
}

