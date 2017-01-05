#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include "gpio_lib.h"
 
#define LSBFIRST 0
#define MSBFIRST 1
#define DISPBUF_LEN 8
 
typedef unsigned char byte;
 
// w1-thermal(DS18B20) device file, change it in your case.
const char* DS18B20_DEVICE="/sys/bus/w1/devices/28-000004f0230d/w1_slave";
 
/*
 * 74HC595 relative stuff
 */
// Cubieboard pin connected to ST_CP of 74HC595 (RCK)
unsigned int latchPin = SUNXI_GPG(0);
// Cubieboard pin connected to SH_CP of 74HC595 (SCK)
unsigned int clockPin = SUNXI_GPG(0);
// Cubieboard pin connected to DS of 74HC595 (DIN)
unsigned int dataPin = SUNXI_GPG(2);
 
/*
 * Display relative stuff
 */
// Digits: "0,1,2,....,9" for common anode 8-segment-LED
unsigned int digit_tab[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92,
                        0x82, 0xf8, 0x80, 0x90};
// Digits with a 'dot' at the right-bottom corner
unsigned int digitdot_tab[] = { 0xc0&0x7f, 0xf9&0x7f, 0xa4&0x7f,
                        0xb0&0x7f, 0x99&0x7f, 0x92&0x7f, 0x82&0x7f,
                        0xf8&0x7f, 0x80&0x7f, 0x90&0x7f};
// Symbols: 'clear', 'dot', '-'
unsigned int symbol_tab[]={ 0xff, 0x7f, 0xbf};
// LED display buffer
static char dispbuf[DISPBUF_LEN];
 
 
/**
 * Set Cubieboard's GPIO port-D pin I/O mode: INPUT/OUTPUT
 */
void pinMode(unsigned int pin, unsigned int io_mode)
{
    if (SETUP_OK != sunxi_gpio_set_cfgpin(pin, io_mode))
    {
        printf("Failed to config GPIO pin\n");
    }
}
 
/**
 * Set Cubieboard's GPIO port-D pin value(LOW/HIGH)
 */
void digitalWrite(int pin, int hl)
{
    if (sunxi_gpio_output(pin, hl))
    {
        printf("Failed to set GPIO pin value\n");
    }
}
 
/**
 * Arduino shiftOut:
 * https://github.com/arduino/Arduino/blob/master/hardware/arduino/cores/arduino/wiring_shift.c
 */
void shiftOut(unsigned int dataPin, unsigned int clockPin, int bitOrder, byte val)
{
    byte i;
    for (i = 0; i < 8; i++)
    {
        if (bitOrder == LSBFIRST)
            digitalWrite(dataPin, ! !(val & (1 << i)));
        else
            digitalWrite(dataPin, ! !(val & (1 << (7 - i))));
 
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
}
 
/**
 * Initialize the GPIO & relative pins
 */
void init_gpio()
{
    if (SETUP_OK != sunxi_gpio_init())
    {
        printf("Failed to initialize GPIO\n");
    }
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
}
 
/**
 * Get current temperature from the w1-thermal device
 */
void get_temperature(char* tempbuf, int len)
{
 
    char* line=NULL;
    char* temperature_tok=NULL;
    int temperature=0;
    int n;
 
    // open the thermal sensor's device
    FILE* fp=fopen(DS18B20_DEVICE,"r");
    if(!fp){
        fprintf(stderr,"Failed to open device(%s) file!\n", DS18B20_DEVICE);
        return;
    }
 
    fseek(fp,0,SEEK_SET);
    // skip the first line
    getline(&line, &n, fp);
    free(line);
    line=NULL;
 
    // get the temperature line
    getline(&line, &n, fp);
    printf("line:%s\n",line);
    strtok(line,"=");
    temperature_tok=strtok(NULL,"=");
 
    if(temperature_tok){
        strncpy(tempbuf, temperature_tok, len);
    }
    free(line);
    fclose(fp);
}
 
/**
 * Thread of filling the time infomation into display buffer
 */
void* time_to_dispbuf()
{
    time_t timep;
    struct tm *p;
    char timebuf[4];
    int interval=1; // in seconds
 
    while(1){
        // get localtime
        time(&timep);
        p = localtime(&timep);
        sprintf(timebuf, "%02d%02d", p->tm_hour, p->tm_min);
 
        dispbuf[0]=digit_tab[timebuf[0] - '0'];
        dispbuf[1]=digitdot_tab[timebuf[1] - '0'];
        dispbuf[2]=digit_tab[timebuf[2] - '0'];
        dispbuf[3]=digit_tab[timebuf[3] - '0'];
        dispbuf[4]=symbol_tab[2]; // '-'
 
        sleep(interval);
    }
}
 
/**
 * Thread of filling the temperature into display buffer
 */
void* temp_to_dispbuf()
{
    char tempbuf[3];
    int interval=5; // in seconds;
 
    while(1){
        memset(tempbuf,0, sizeof tempbuf);
        get_temperature(tempbuf, sizeof tempbuf);
        printf("temp:%c%c%c\n", tempbuf[0], tempbuf[1], tempbuf[2]);
        // check temperature validity
        if(isdigit(tempbuf[0]) && isdigit(tempbuf[1]) && isdigit(tempbuf[2])){
            dispbuf[5]=digit_tab[tempbuf[0]-'0'];
            dispbuf[6]=digitdot_tab[tempbuf[1]-'0'];
            dispbuf[7]=digit_tab[tempbuf[2]-'0'];
        }
        sleep(interval);
    }
 
}
 
int main(int argc, char **argv)
{
 
    pthread_t get_time_thread, get_temp_thread;
    void * thread_ret;
 
    init_gpio();
 
 
   // pthread_create( &get_time_thread, NULL, time_to_dispbuf, NULL);
   // pthread_create( &get_temp_thread, NULL, temp_to_dispbuf, NULL);
 
    while (1)
    {
 
        int i;
        //for(i=0;i<DISPBUF_LEN;i++){
		//usleep(1000);
            	digitalWrite(clockPin, 0);
		//digitalWrite(latchPin, 0);
		//digitalWrite(dataPin, 0);
            //shiftOut(dataPin, clockPin, MSBFIRST, 1<<i);
            //shiftOut(dataPin, clockPin, MSBFIRST, dispbuf[i]);
		//usleep(1000);
		digitalWrite(clockPin, 1);
		//digitalWrite(latchPin, 1);
		//digitalWrite(dataPin, 1);
           // digitalWrite(latchPin, 1);
           // usleep(1000);
       // }
 
    }
 
   // pthread_join(get_time_thread,&thread_ret);
  //  pthread_join(get_temp_thread,&thread_ret);
 
    return 0;
}
