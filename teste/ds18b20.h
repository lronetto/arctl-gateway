#ifndef _DS18B20_H_
#define _DS18B20_H_


typedef struct {
	int gpio;
	int flag;
	float temp;
	int *addr;
	}ds18b20_TypeDef;



void Ds18b20_Init(ds18b20_TypeDef *ds,int gpio,int *addr);
void Ds18b20_ConvertTemperature(ds18b20_TypeDef ds);
void Ds18b20_ReadTemperature(ds18b20_TypeDef *ds);
int Ds18b20_ReadPower(ds18b20_TypeDef ds);
void Ds18b20_ReadROM(ds18b20_TypeDef ds);

#endif
