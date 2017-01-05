#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio_lib.h"
#include "ds18b20.h"


#define Delay_ms(a) usleep(a*1000)
#define Delay_us(a) usleep(a)
//extern void Delay_ms(__IO uint32_t nTime);
//extern void Delay_us(__IO uint32_t nTime);


void Ds18b20_Mode(ds18b20_TypeDef ds,int mode);
void Ds18b20_Reset(ds18b20_TypeDef ds);
void Ds18b20_WriteByte(ds18b20_TypeDef ds, int data);
void Ds18b20_WriteBit(ds18b20_TypeDef ds, int bit);
int Ds18b20_ReadByte(ds18b20_TypeDef ds);
int Ds18b20_ReadBit(ds18b20_TypeDef ds);
int Ds18b20_TouchByte(ds18b20_TypeDef ds, int data);
void Ds18b20_Block(ds18b20_TypeDef ds, unsigned char *data, int data_len);

#define DS18B20_MODE_READ INPUT
#define DS18B20_MODE_WRITE OUTPUT

void Ds18b20_Init(ds18b20_TypeDef *ds,int gpio,int *addr){
	(*ds).gpio=gpio;
	(*ds).flag=0;
	(*ds).addr=addr;
	}
void Ds18b20_Mode(ds18b20_TypeDef ds,int mode){
	pinMode(ds.gpio,mode);
	}

	
void Ds18b20_Reset(ds18b20_TypeDef ds){
	Ds18b20_Mode(ds,OUTPUT);
	digitalWrite(ds.gpio,1);
	digitalWrite(ds.gpio,0);
	Delay_us(480);
	digitalWrite(ds.gpio,1);
	Ds18b20_Mode(ds,INPUT);
	Delay_us(10);
	while(sunxi_gpio_input(ds.gpio));
	//GPIO_ReadInputDataBit(ds.GPIO,ds.GPIO_conf.GPIO_Pin);
	Ds18b20_Mode(ds,OUTPUT);
	digitalWrite(ds.gpio,1);
	Delay_us(100);
	}

void Ds18b20_WriteByte(ds18b20_TypeDef ds, int data){
	int i;
	for(i=0;i<8;i++){
		digitalWrite(ds.gpio,1);
		digitalWrite(ds.gpio,0);
		if(data & 0x01)
			digitalWrite(ds.gpio,1);
		else
			digitalWrite(ds.gpio,0);
		data>>=1;
		Delay_us(60);
		digitalWrite(ds.gpio,1);
		}
	digitalWrite(ds.gpio,1);
	}
		
int Ds18b20_ReadByte(ds18b20_TypeDef ds){
	int i,x=0;
	Ds18b20_Mode(ds,DS18B20_MODE_WRITE);
	for(i=0;i<8;i++){
		digitalWrite(ds.gpio,0);
		x>>=1;
		digitalWrite(ds.gpio,1);
		Ds18b20_Mode(ds,DS18B20_MODE_READ);
		//Delay_us(15);
		if(sunxi_gpio_input(ds.gpio))
			x|=0x80;
		Delay_us(100);
		Ds18b20_Mode(ds,DS18B20_MODE_WRITE);
		}
	return x;
	}
		
void Ds18b20_ConvertTemperature(ds18b20_TypeDef ds)
{
int i;

Ds18b20_Reset(ds); // Perform Master Reset of OneWire Bus
Ds18b20_WriteByte(ds, 0x55);
for(i=0;i<8;i++) Ds18b20_WriteByte(ds, ds.addr[i]);
//Ds18b20_WriteByte(ds, 0xCC);	 // skip ROM
Ds18b20_WriteByte(ds, 0x44);	 // convert temperature
//Ds18b20_Reset(ds);	 // Perform Master Reset of OneWire Bus
}

void Ds18b20_ReadTemperature(ds18b20_TypeDef *ds)
{
int ls,ms;
int temp;
int sig=0;
int i;
Ds18b20_Reset(*ds);
//Ds18b20_WriteByte(*ds, 0xCC);	 // skip ROM
Ds18b20_WriteByte(*ds, 0x55);
for(i=0;i<8;i++) Ds18b20_WriteByte(*ds, (*ds).addr[i]);
Ds18b20_WriteByte(*ds, 0xBE);	 // read scratch pad

ls=Ds18b20_ReadByte(*ds);
ms=Ds18b20_ReadByte(*ds);
//printf("LS 0x%X MS 0x%X",ls,ms);
if(ms & 0xF0){
	ms&=~(ms & 0xFF);
	sig=1;
//	printf(" MS1 0x%X ",ms);
}
temp= ls | ms<<8;
if(!sig) (*ds).temp=(float)temp/16.0;
else  (*ds).temp=(float)temp/-16.0;
//printf("0x%X\n",temp);
//temp=temp/16;


}
void Ds18b20_ReadROM(ds18b20_TypeDef ds){
	int i;

	Ds18b20_Reset(ds);
	Ds18b20_WriteByte(ds, 0x33);
	for(i=0;i<8;i++){
		printf("0x%X ",Ds18b20_ReadByte(ds));
	}

}



