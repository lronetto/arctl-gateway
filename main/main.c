#include "main.h"

void *ThreadXbee(void *vargp);
void *ThreadSerial(void *vargp);

int main(){
	int i,id;
	thread_args args;
	pthread_t IdSerial,IdXbee,IdPost;
	/*iolib_init();
	iolib_setdir(8,25, BBBIO_DIR_IN);
	iolib_setdir(8,27, BBBIO_DIR_IN);
	iolib_setdir(8,29, BBBIO_DIR_IN);
	iolib_setdir(8,31, BBBIO_DIR_IN);
	usleep(500);
	if(is_high(8,25)) sys_id=1;
	//if(is_high(8,27)) sys_id|=2;
	//if(is_high(8,29)) sys_id|=4;
	//if(is_high(8,31)) sys_id|=8;
	iolib_free();*/
	printf("inicio \n");
	fflush(stdout);
	pthread_create(&(IdSerial), NULL, ThreadSerial, (void *)&(args));
	usleep(10000000);
	pthread_create(&(IdXbee), NULL, ThreadXbee, (void *)&(args));
	//pthread_create(&(IdPost), NULL, ThreadPost, (void *)&(args));

	while(1){
		usleep(10000000);
	}
}
