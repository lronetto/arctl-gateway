#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include "gpio_lib.h"
#include "ds18b20.h"


#define Pin_temp SUNXI_GPG(3)
#define Pin_clock SUNXI_GPG(2)
#define Pin_latch SUNXI_GPG(1)
#define Pin_data SUNXI_GPG(0)

// ds18b20_TypeDef temp1;
int addr3[]={0x28,0x9D,0xFC,0x98,0x6,0x0,0x0,0x4E};
void shiftOut(unsigned int dataPin, unsigned int clockPin, int bitOrder, int val)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        if (bitOrder)
            digitalWrite(dataPin, ! !(val & (1 << i)));
        else
            digitalWrite(dataPin, ! !(val & (1 << (7 - i))));
 
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
}
void init_gpio()
{
    if (SETUP_OK != sunxi_gpio_init())
    {
        printf("Failed to initialize GPIO\n");
    }
    pinMode(Pin_latch, OUTPUT);
    pinMode(Pin_clock, OUTPUT);
    pinMode(Pin_data, OUTPUT);
pinMode(Pin_temp, OUTPUT);
}
int main(){
	init_gpio();
//	Ds18b20_Init(&temp1,Pin_temp,addr3);
	digitalWrite(Pin_latch, 0);
	shiftOut(Pin_data,Pin_clock,0,0xFF);
	digitalWrite(Pin_latch, 1);
	usleep(1000);
	while(1){
//		Ds18b20_ReadTemperature(&temp1);
//    		Ds18b20_ConvertTemperature(temp1);
//		printf("temp:%f\n",temp1.temp);
		usleep(1000000);
		}
	}
